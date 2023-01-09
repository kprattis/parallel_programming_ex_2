#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <math.h>
#include <mpi.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <string.h>

#define MAXSIZE 1024 * 1024 * 256l

void fprint_arrd(FILE *f, double *arr, int n, int k);
void fprint_arri(FILE *f, int *arr, int n, int k);

knnresult distrAllkNN(double * X, int n, int d, int k, int N){
    int tid, numtasks;

    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    
    printf("Cilk workers used for process %d : %d\n", tid,  __cilkrts_get_nworkers());

    MPI_Request mpireq;
    MPI_Status mpistat;

    int round = 0;
    
    int m = N - (numtasks - 1) * (N/numtasks);
    

    long BLOCKSIZE = m;
    if ( (long) BLOCKSIZE * BLOCKSIZE > MAXSIZE){
        BLOCKSIZE = min( (long)BLOCKSIZE * BLOCKSIZE / 4, MAXSIZE) / BLOCKSIZE;   
    }


    printf("I am proccess %d and my distance matrix will have size: %d x %ld\n", tid , n, BLOCKSIZE);
    if(BLOCKSIZE < k){
        printf("too large sizes or too small k, n = %d, d = %d\n", n, d);
        exit(1);
    }

    //Distance array
    double *D = (double *) malloc(n * BLOCKSIZE * sizeof(double));
    
    //Here store the norms
    double *normx = (double *) malloc(n * sizeof(double));
    cilk_for(int i = 0; i < n; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));
    
    //the result
    knnresult knn = init_knnresult(n, k);

    double *Y = (double *) malloc(sizeof(double) * m * d);
    

    double *Z = (double *) malloc(sizeof(double) * m * d);
    
    int dest = (tid + 1) % numtasks;
    int src = (tid == 0) ? numtasks - 1 : tid - 1;
    
    int start, end, size;

    //It will take numtasks - 1 for any process' points to reach every other process
    // Add the first iteration of the initial points : total of numtasks calculations
    m = n;
    memcpy(Y, X, m * d * sizeof(double));

    //tid of the points currently processing
    int originID = tid;

    while(round < numtasks){
        if(round < numtasks - 1)
            MPI_Isend(Y, m * d, MPI_DOUBLE , dest, 1000 , MPI_COMM_WORLD, &mpireq);
        
        //------------------Calculate the knn result--------------------------------
        for(int b = 0; b < m; b += BLOCKSIZE){
            start = b;
            end = min(m, b + BLOCKSIZE);
            size = end - start;

            cilk_for(int i = 0; i < size; i++)
                normy[i] = euclidean_norm(Y + (i + start) * d, d);
            
            cilk_for(int i = 0; i < n; i++)
                cilk_for(int j = 0; j < size; j++){
                    D[i * size + j] = normx[i] + normy[j];
                }


            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);       

            cilk_for(int i = 0; i < n; i++)
                kselect(D + i * size, 0, size - 1, k, knn.ndist + i * k, knn.nidx + i * k, (b == 0) && (round == 0), start + originID * (N / numtasks));
        }
        //--------------------End of own points Calculation----------------------------
        
        //
        if(tid != numtasks - 1){
            if(round == tid){
                m = N - (numtasks - 1) * (N/numtasks);
            }
            if(round == (tid + 1) % numtasks){
                m = n;
            }
        }
        else{
            if(round == 0){
                m = N/numtasks;
            }   
        }
        
        if(round < numtasks - 1)
            MPI_Recv(Z, m * d, MPI_DOUBLE , src , 1000 , MPI_COMM_WORLD, &mpistat);

        originID = (originID == 0) ? numtasks - 1 : originID - 1;

        MPI_Wait(&mpireq, &mpistat);
        
        double *temp = Z;
        Z = Y;
        Y = temp;    

        round ++;  
       
    }

    free(D);
    free(normx);
    free(normy);

    return knn;
}

int main(int argc, char *argv[]){
    char resultsfile[] = "results.txt";
    char inputfile[] = "input.txt";

    FILE *f;
    
    MPI_Init(&argc, &argv);

    MPI_Status mpistat;

    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);

    int N = 17, d = 1, k = 3, n;
    int chunk = N / numtasks;
    int start = tid * chunk;
    int end = (tid == numtasks - 1) ? N  : (tid + 1) * chunk;
    n = end - start;
    double *X = (double *) malloc(sizeof(double) * n * d);


    f = fopen(inputfile, "r");
    
    double temp;
    for(int i = 0; i < start; i++){
        fscanf(f, "%lf\n", &temp);
    }
    for(int i = start; i < end; i++){
        fscanf(f, "%lf\n", &X[i - start]);
    }

    fclose(f);
    
    struct timeval start_time, end_time;
    double elapsed_time;
    gettimeofday(&start_time, NULL);
        knnresult knn = distrAllkNN(X, n, d, k, N);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    double othertime;

    //wait for all processes to finish
    MPI_Barrier(MPI_COMM_WORLD);

    //print results in file. Send all results to the proccess 0.
    if(tid == 0){
        
        for(int i = 1; i < numtasks; i++){
            MPI_Recv(&othertime, 1, MPI_DOUBLE , i , i * 100 + 3 , MPI_COMM_WORLD, &mpistat);
            elapsed_time = (elapsed_time < othertime) ? othertime : elapsed_time;
        }
        printf("Execution Time, %lf\n", elapsed_time);


        f = fopen(resultsfile, "w");
        
        fprint_arrd(f, knn.ndist, n, k);

        for(int i = 1; i < numtasks - 1; i++){
            MPI_Recv(knn.ndist, n * k, MPI_DOUBLE , i , i * 100 + 1 , MPI_COMM_WORLD, &mpistat);
            fprint_arrd(f, knn.ndist, n, k);
        }

        knn.ndist = realloc(knn.ndist, (N - (numtasks - 1) * chunk) * k * sizeof(double));
        MPI_Recv(knn.ndist, (N - (numtasks - 1) * chunk) * k, MPI_DOUBLE , numtasks - 1 , (numtasks - 1) * 100 + 1 , MPI_COMM_WORLD, &mpistat);
        fprint_arrd(f, knn.ndist, (N - (numtasks - 1) * chunk), k);

        fprintf(f, "\n");

        fprint_arri(f, knn.nidx, n, k);

        for(int i = 1; i < numtasks - 1; i++){
            MPI_Recv(knn.nidx, n * k, MPI_INT , i , i * 100 + 2 , MPI_COMM_WORLD, &mpistat);
            fprint_arri(f, knn.nidx, n, k);
        }

        knn.nidx = realloc(knn.nidx, (N - (numtasks - 1) * chunk) * k * sizeof(int));
        MPI_Recv(knn.nidx, (N - (numtasks - 1) * chunk) * k, MPI_INT , numtasks - 1 , (numtasks - 1) * 100 + 2 , MPI_COMM_WORLD, &mpistat);
        fprint_arri(f, knn.nidx, (N - (numtasks - 1) * chunk), k);


        fclose(f);
    }
    else{
        MPI_Send(&elapsed_time, 1, MPI_DOUBLE , 0 , tid * 100 + 3 , MPI_COMM_WORLD);
        MPI_Send(knn.ndist, k * n, MPI_DOUBLE , 0 , tid * 100 + 1 , MPI_COMM_WORLD);
        MPI_Send(knn.nidx, k * n, MPI_INT , 0 , tid * 100 + 2 , MPI_COMM_WORLD);
    }
    
    MPI_Finalize();
    return 0;
}