#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>
#include <string.h>

#define MAXSIZE 1024 * 1024 * 256l

void fprint_arrd(FILE* f, double *arr, int n, int k){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%.3lf ", arr[k * i + j]);
        }
         fprintf(f, "\n");
    }
}

void fprint_arri(FILE* f, int *arr, int n, int k){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%d ", arr[k * i + j]);
        }
         fprintf(f, "\n");
    }
}

knnresult distrAllkNN(double * X, int n, int d, int k){
    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    MPI_Request mpireq;
    MPI_Status mpistat;

    int rounds = 0;

    long BLOCKSIZE = n;

    if ( (long) n * n > MAXSIZE){
        BLOCKSIZE = min( (long) n * n / 4, MAXSIZE);
        BLOCKSIZE = (BLOCKSIZE < n) ? 0 : BLOCKSIZE / n;   
    }

    printf("The distance matrix will have size: %d x %ld\n", n, BLOCKSIZE);
    if(BLOCKSIZE < k){
        printf("too large sizes, n = %d, d = %d\n", n, d);
        exit(1);
    }

    double *normx = (double *) malloc(n * sizeof(double));
        //calculate norms of X points
    cilk_for(int i = 0; i < n; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    //Distance array
    double *D = (double *) malloc(n * BLOCKSIZE * sizeof(double));
    
    //the result
    knnresult knn = init_knnresult(n, k);

    double *shortest_distances = (double *) malloc(sizeof(double) * 3 * k * n);
    int *nearest_idxs = (int *) malloc(sizeof(int) * 3 * k * n);

    double *Y = (double *) malloc(sizeof(double) * n * d);
    memcpy(Y, X, n * d * sizeof(double));

    double *Z = (double *) malloc(sizeof(double) * n * d);
    
    int dest = (tid + 1) % numtasks;
    int src = (tid == 0) ? numtasks - 1 : tid - 1;
    
    

    while(rounds < numtasks - 1){

        MPI_Isend(Y, n * d * sizeof(double), MPI_DOUBLE , dest, 1000 + 100 * rounds + tid , MPI_COMM_WORLD, &mpireq);
        
        //------------------Calculate the knn result--------------------------------

        for(int b = 0; b < n; b += BLOCKSIZE){
            start = b;
            end = min(n, b + BLOCKSIZE);
            size = end - start;

            cilk_for(int i = 0; i < n; i++)
                cilk_for(int j = start; j < end; j++)
                    D[i * BLOCKSIZE + j - start] = normx[i] + euclidean_norm(Y + j * d, d);

            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

            int kval = min(k, size);
            cilk_for(int i = 0; i < n; i++){

                kselect(D + i * BLOCKSIZE, 0, size - 1, kval, shortest_distances +  3 * i * k + k * (b != 0), nearest_idxs + 3 * i * k + k * (b != 0)); 

                //modify indexes to be global
                cilk_for(int j = 0; j < k; j++){
                    nearest_idxs[3 * i * k + k * (b != 0) + j] += start; 
                    nearest_idxs[3 * i * k + k * (b != 0) + j] *= d;
                }
            
                if(b != 0){

                    kselect(shortest_distances + 3 * i * k, 0, k + kval - 1, k, shortest_distances + 3 * i * k + 2 * k, nearest_idxs + 3 * i * k + 2 * k);

                    cilk_for(int j = 0; j < k; j++){
                        nearest_idxs[3 * i * k + 2 * k + j] = nearest_idxs[3 * i * k + nearest_idxs[3 * i * k + 2 * k + j]];
                    }

                    cilk_for(int j = 0; j < k; j++){
                        nearest_idxs[3 * i * k + j] = nearest_idxs[3 * i * k + 2 * k + j];
                        shortest_distances[3 * i * k + j] = shortest_distances[3 * i * k + 2 * k + j];
                    }
                    
                }
            }
        }
        
        
        //--------------------End of own points Calculation----------------------------
        
        MPI_recv(Z, n * d * sizeof(double) , MPI_DOUBLE , src , 1000 + 100*rounds + src , MPI_COMM_WORLD);

        swap(Z, Y);
        
        rounds ++;  

        if(rounds == 0){
            cilk_for(int i = 0; i < m; i++){
                cilk_for(int j = 0; j < k; j++){
                    knn.nidx[i * k + j] = nearest_idxs[3 * i * k + j];
                    knn.ndist[i * k + j] = shortest_distances[3 * i * k + j];
                }
            }
        }
        else{
                
        }

    
    }

    free(nearest_idxs);
    free(shortest_distances);
    free(normx);
    free(normy);
    free(D);

    return knn;

}

int main(int argc, char *argv[]){
    char resultsfile[] = "results.txt";
    FILE *fresults;
    
    MPI_Init(argc, argv);

    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    
    int N, d, k, n;

    int IhaveWritten = 1;
    int WritingDone = 1;

    int chunk = N / numtasks;

    int start = tid * chunk;
    int end = min((tid + 1) * chunk, N);

    n = end - start;
    k = 5;
    d = 1;

    for(int i = start; i < end; i++){
        X[i] = rand() * 1.0 / RAND_MAX;
    }

    knnresult knn = distrAllkNN(X, n, d, k);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            knn.idxs[i * k + j] += start * d; 
        }
    }

    //print distances
    if(tid == 0){
        fresults = fopen(resultsfile, "w");
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);

        MPI_Send(IhaveWritten, sizeof(int), MPI_INT , tid + 1, tid , MPI_COMM_WORLD);

    }
    else if(tid == (numtasks - 1)){
        MPI_Recv(IhaveWritten, sizeof(int), MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD);

        fresults = fopen(resultsfile, "a");
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);
        
        MPI_Send(IhaveWritten, sizeof(int), MPI_INT , tid + 1, tid , MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(IhaveWritten, sizeof(int), MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD);
        
        fresults = fopen(resultsfile, "a");
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);

        MPI_Send(WritingDone, sizeof(int), MPI_INT , 0, 10000 , MPI_COMM_WORLD)
    }

    //print ids
    if(tid == 0){
        MPI_Recv(WritingDone, sizeof(int), MPI_INT , numtasks - 1, 10000 , MPI_COMM_WORLD);

            
        fresults = fopen(resultsfile, "a");
        fprintf(fresults, "\n\n");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);

        MPI_Send(IhaveWritten, sizeof(int), MPI_INT , tid + 1, tid , MPI_COMM_WORLD);

    }
    else if(tid == (numtasks - 1)){
        MPI_Recv(IhaveWritten, sizeof(int), MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD);

        fresults = fopen(resultsfile, "a");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);

        MPI_Send(IhaveWritten, sizeof(int), MPI_INT , tid + 1, tid , MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(IhaveWritten, sizeof(int), MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD);
        fresults = fopen(resultsfile, "a");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);
    }

    MPI_Finalize();
    return 0;
}