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

void fprint_arrd(FILE *f, double *arr, int n, int k);
void fprint_arri(FILE *f, int *arr, int n, int k);

knnresult distrAllkNN(double * X, int n, int d, int k){
    int tid, numtasks;

    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    
    MPI_Request mpireq;
    MPI_Status mpistat;

    int round = 0;

    long BLOCKSIZE = n;
    if ( (long) n * n > MAXSIZE){
        BLOCKSIZE = min( (long) n * n / 4, MAXSIZE);
        BLOCKSIZE = (BLOCKSIZE < n) ? 0 : BLOCKSIZE / n;   
    }

    printf("I am proccess %d and my distance matrix will have size: %d x %ld\n", tid , n, BLOCKSIZE);
    if(BLOCKSIZE < k){
        printf("too large sizes, n = %d, d = %d\n", n, d);
        exit(1);
    }

    //Distance array
    double *D = (double *) malloc(n * BLOCKSIZE * sizeof(double));
    
    //the result
    knnresult knn = init_knnresult(n, k);

    double *Y = (double *) malloc(sizeof(double) * n * d);
    memcpy(Y, X, n * d * sizeof(double));

    double *Z = (double *) malloc(sizeof(double) * n * d);
    
    int dest = (tid + 1) % numtasks;
    int src = (tid == 0) ? numtasks - 1 : tid - 1;
    
    int start, end, size;
    int kval;

    while(round < numtasks - 1){
        
        MPI_Isend(Y, n * d, MPI_DOUBLE , dest, 1000 + 100 * rounds + tid , MPI_COMM_WORLD, &mpireq);
        
        //------------------Calculate the knn result--------------------------------
        for(int b = 0; b < n; b += BLOCKSIZE){
            start = b;
            end = min(n, b + BLOCKSIZE);
            size = end - start;

            cilk_for(int i = 0; i < BLOCKSIZE; i++)
                normy[i] = euclidean_norm(Y + i * d, d);

            cilk_for(int i = 0; i < m; i++)
                cilk_for(int j = start; j < end; j++){
                    D[i * BLOCKSIZE + j - start] = normx[i] + normy[j - start];
                }

            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

            kval = min(k, size);
            cilk_for(int i = 0; i < m; i++)
                kselect(D + i * BLOCKSIZE, 0, size - 1, kval, knn.ndist + i * k, knn.nidx + i * k, (b == 0) && (round == 0), start);
        }
        //--------------------End of own points Calculation----------------------------
        printf("I am %d at round %d\n", tid, rounds);
        print_arrd(Y, n , d);

        MPI_Recv(Z, n * d, MPI_DOUBLE , src , 1000 + 100*rounds + src , MPI_COMM_WORLD, &mpistat);

        double **temp = &Z;
        &Z = &Y;
        &Y = temp;    

        round ++;  
        MPI_Wait(&mpireq, &mpistat);
    }

    free(D);
    free(normx);
    free(normy);

    return knn;
}

int main(int argc, char *argv[]){
    char resultsfile[] = "results.txt";
    char infile[] = "input.txt";

    FILE *fresults, *f;
    
    MPI_Init(&argc, &argv);

    MPI_Status mpistat;

    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    
    int N = 12, d = 1, k = 2, n;

    int IhaveWritten = 1;
    int WritingDone = 1;

    int chunk = N / numtasks;

    int start = tid * chunk;
    int end = (tid == numtasks - 1) ? N : (tid + 1) * chunk;

    n = end - start;

    double *X = (double *) malloc(sizeof(double) * n * d);

    srand(start * rand());
    for(int i = start; i < end; i++){
        X[i - start] = rand() * 1.0 / RAND_MAX * 10 + -5;
    }
    
    printf("%d X is:\n", tid);
    print_arrd(X, n, d);
    
    struct timeval start_time, end_time;
    double elapsed_time;
    
    

    gettimeofday(&start_time, NULL);
        knnresult knn = distrAllkNN(X, n, d, k);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    
    //print_arri(knn.nidx, m, k);
    //print_arrd(knn.ndist, m, k);
    printf("%d Execution Time, %lf\n", tid, elapsed_time);

    printf("%d finished\n", tid);

    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            knn.nidx[i * k + j]; 
        }
    }

    //print distances
    if(tid == 0){
        fresults = fopen(resultsfile, "w");
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);

        MPI_Send(&IhaveWritten, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);

    }
    else if(tid == (numtasks - 1)){

        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);

        fresults = fopen(resultsfile, "a");
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);
        
        MPI_Send(&IhaveWritten, 1, MPI_INT , 0, 10000 , MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);
        printf("I have permission %d\n", tid);

        fresults = fopen(resultsfile, "a");
        
        fprint_arrd(fresults, knn.ndist, n, k);
        fclose(fresults);

        MPI_Send(&WritingDone, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);
    }

    //print ids
    if(tid == 0){
        MPI_Recv(&WritingDone, 1, MPI_INT , numtasks - 1, 10000 , MPI_COMM_WORLD, &mpistat);

            
        fresults = fopen(resultsfile, "a");
        fprintf(fresults, "\n\n");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);

        MPI_Send(&IhaveWritten, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);

    }
    else if(tid == (numtasks - 1)){
        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);

        fresults = fopen(resultsfile, "a");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);

        
    }
    else{
        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);
        fresults = fopen(resultsfile, "a");
        fprint_arri(fresults, knn.nidx, n, k);
        fclose(fresults);
        MPI_Send(&IhaveWritten, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);
    }

 if(tid == 0){
        f = fopen(infile, "w");
        fprint_arrd(f, X, n, d);
        fclose(f);

        MPI_Send(&IhaveWritten, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);

    }
    else if(tid == (numtasks - 1)){

        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);

        f = fopen(infile, "a");
        fprint_arrd(f, X, n, d);
        fclose(f);
        
        MPI_Send(&IhaveWritten, 1, MPI_INT , 0, 10000 , MPI_COMM_WORLD);
    }
    else{
        MPI_Recv(&IhaveWritten, 1, MPI_INT , tid - 1, tid - 1 , MPI_COMM_WORLD, &mpistat);
        printf("I have permission %d\n", tid);

        f = fopen(infile, "a");
        fprint_arrd(f, X, n, d);
        fclose(f);

        MPI_Send(&WritingDone, 1, MPI_INT , tid + 1, tid , MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}