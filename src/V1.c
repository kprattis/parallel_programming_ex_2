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

knnresult distrAllkNN(double * X, int n, int d, int k, int N){
    int tid, numtasks;

    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);
    
    MPI_Request mpireq, mpisizereq[numtasks + 2];
    MPI_Status mpistat, mpisizestat;

    int round = 0;
    
    int m = n;



    long BLOCKSIZE = N - (numtasks - 1) * N / numtasks;
    if ( (long) BLOCKSIZE * BLOCKSIZE > MAXSIZE){
        BLOCKSIZE = min( (long)BLOCKSIZE * BLOCKSIZE / 4, MAXSIZE) / BLOCKSIZE;   
    }


    printf("I am proccess %d and my distance matrix will have size: %d x %ld\n", tid , n, BLOCKSIZE);
    if(BLOCKSIZE < k){
        printf("too large sizes, n = %d, d = %d\n", n, d);
        exit(1);
    }

    //Distance array
    double *D = (double *) malloc(m * BLOCKSIZE * sizeof(double));
    
    //Here store the norms
    double *normx = (double *) malloc(n * sizeof(double));
    cilk_for(int i = 0; i < n; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));
    
    //the result
    knnresult knn = init_knnresult(n, k);

    double *Y = (double *) malloc(sizeof(double) * m * d);
    memcpy(Y, X, m * d * sizeof(double));

    double *Z = (double *) malloc(sizeof(double) * m * d);
    
    int dest = (tid + 1) % numtasks;
    int src = (tid == 0) ? numtasks - 1 : tid - 1;
    
    int start, end, size;
    int kval;

    //It will take numtasks - 1 interchnges of points before the initial points of every process return.
    // Add the first iteration of the initial points : total of numtasks calculations
    while(round < numtasks){
        
        printf("Here I stuck before %d\n", tid);
        MPI_Isend(Y, m * d, MPI_DOUBLE , dest, n , MPI_COMM_WORLD, &mpireq);
        printf("Here I stuck after %d\n", tid);
        //------------------Calculate the knn result--------------------------------
        for(int b = 0; b < m; b += BLOCKSIZE){
            start = b;
            end = min(m, b + BLOCKSIZE);
            size = end - start;

            cilk_for(int i = 0; i < BLOCKSIZE; i++)
                normy[i] = euclidean_norm(Y + i * d, d);

            cilk_for(int i = 0; i < n; i++)
                cilk_for(int j = start; j < end; j++){
                    D[i * BLOCKSIZE + j - start] = normx[i] + normy[j - start];
                }

            cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, n, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

            kval = min(k, size);
            cilk_for(int i = 0; i < n; i++)
                kselect(D + i * BLOCKSIZE, 0, size - 1, kval, knn.ndist + i * k, knn.nidx + i * k, (b == 0) && (round == 0), start + ((tid + round) % numtasks) * n);
        }
        //--------------------End of own points Calculation----------------------------
        
        MPI_Wait(&mpireq, &mpistat);
        
        if(tid != numtasks - 1){
            if(round == tid){
                m = N - (numtasks - 1) * N / numtasks;
                Y = realloc(Y, m * d * sizeof(double));
                Z = realloc(Z, m * d * sizeof(double));
            }

            if((round + 1) % numtasks == tid){
                m = n;
                Y = realloc(Y, m * d * sizeof(double));
                Z = realloc(Z, m * d * sizeof(double));
            }
            MPI_Recv(Z, m * d, MPI_DOUBLE , src , MPI_ANY_TAG , MPI_COMM_WORLD, &mpistat);
        }
        else{
            if(round == 0){
                m = N/numtasks;
                Y = realloc(Y, m * d * sizeof(double));
                Z = realloc(Z, m * d * sizeof(double));
            }   
            
            MPI_Recv(Z, m * d, MPI_DOUBLE, src, MPI_ANY_TAG , MPI_COMM_WORLD, &mpistat);
            
        }

        
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
    //char infile[] = "input.txt";

    FILE *f;
    
    MPI_Init(&argc, &argv);

    MPI_Status mpistat;

    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);

    int N = 14, d = 2, k = 2, n;
    int chunk = N / numtasks;
    int start = tid * chunk;
    int end = (tid == numtasks - 1) ? N  : (tid + 1) * chunk;
    n = end - start;
    double *X = (double *) malloc(sizeof(double) * n * d);

    srand(start * rand());
    for(int i = start; i < end; i++){
        X[i - start] = rand() * 1.0 / RAND_MAX * 10 + -5;
    }
    
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

        for(int i = 1; i < numtasks; i++){
            MPI_Recv(knn.ndist, n * k, MPI_DOUBLE , i , i * 100 + 1 , MPI_COMM_WORLD, &mpistat);
            fprint_arrd(f, knn.ndist, n, k);
        }

        fprintf(f, "\n");

        fprint_arri(f, knn.nidx, n, k);

        for(int i = 1; i < numtasks; i++){
            MPI_Recv(knn.nidx, n * k, MPI_INT , i , i * 100 + 2 , MPI_COMM_WORLD, &mpistat);
            fprint_arri(f, knn.nidx, n, k);
        }


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