#include "knn.h"

#include <stdio.h> //print, File
#include <cblas.h>// cblas_dgemm
#include <mpi.h> //MPI_Send ..
#include <stdlib.h> //malloc ..
#include <sys/time.h> //Execution time
#include <cilk/cilk.h> //cilk_for
#include <cilk/cilk_api.h> //number of cilk workers
#include <string.h> //memcpy

//Max size of array D (same as V0)
#define MAXSIZE 1024 * 1024 * 128l

knnresult distrAllkNN(double * X, int n, int d, int k, int N){
    
    //Init MPI parameters
    int tid, numtasks;
    MPI_Comm_rank(MPI_COMM_WORLD, &tid);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    
    MPI_Request mpireq;
    MPI_Status mpistat;

    //round will count the number of iterations in the processes ring
    int round = 0;

    //the max size of points - the points of the numtasks - 1 process
    int m = N - (numtasks - 1) * (N/numtasks);
    
    ///Here set the blocksize, which is the reduced size of the dimension of corpus n of the distance matrix
    //to make sure even the last points fit in memory we init it at m.
    long BLOCKSIZE = m;
    if ( (long) BLOCKSIZE * BLOCKSIZE > MAXSIZE){
        BLOCKSIZE = min( (long)BLOCKSIZE * BLOCKSIZE / 4, MAXSIZE) / BLOCKSIZE;   
    }
    printf("I am proccess %d and my distance matrix will have size: %d x %ld\n", tid , n, BLOCKSIZE);
    if(BLOCKSIZE < k || n < k){
        printf("too large sizes or too small k, n = %d, d = %d\n", n, d);
        MPI_Abort(MPI_COMM_WORLD , 1);
    }
    //-------------------------------End of Blocksize init---------------------

    //------------------Memory Allocation----------------------------
    //Distance array
    double *D = (double *) malloc(n * BLOCKSIZE * sizeof(double));
    
    //Here store the norms of each process' own points
    double *normx = (double *) malloc(n * sizeof(double));
    cilk_for(int i = 0; i < n; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    //memory to store the norms of the receiveing points
    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));
    
    //the result
    knnresult knn = init_knnresult(n, k);

    //the corpus points
    double *Y = (double *) malloc(sizeof(double) * m * d);

    //the points that will be received
    double *Z = (double *) malloc(sizeof(double) * m * d);
    //------------------End of Memory Allocation----------------------------

    //Send to the next in the ring (dest) and receive from the previous (src)
    int dest = (tid + 1) % numtasks;
    int src = (tid == 0) ? numtasks - 1 : tid - 1;
    
    int start, end, size;

    //First iteration corpus is my own points X (in V1 m->Y and n->X)
    m = n;
    memcpy(Y, X, m * d * sizeof(double));

    //tid of the process whose points currently are the corpus
    //at the start Y = X so originID = tid
    int originID = tid;

    //It will take numtasks - 1 for any process' points to reach every other process
    // Add the first iteration of the initial points : total of numtasks calculations
    while(round < numtasks){

        //Non Blocking send before calculation
        if(round < numtasks - 1)
            MPI_Isend(Y, m * d, MPI_DOUBLE , dest, 1000 , MPI_COMM_WORLD, &mpireq);
        
        //Code same as V0
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
        
        //In the below section, adjust if needed to receive the points of numtasks - 1 or
        //if those points were the last corpus adjust to receive the standard length of points
        if(tid != numtasks - 1){
            //set m to be the length of the next points to receive
            //every process receives the points of (numtasks - 1) at round tid
            if(round == tid){
                m = N - (numtasks - 1) * (N/numtasks);
            }
            if(round == (tid + 1) % numtasks){
                m = n;
            }
        }
        else{
            //If I am the last process, adjust to receive from the others
            if(round == 0){
                m = N/numtasks;
            }   
        }
        
        //Receive the points
        if(round < numtasks - 1)
            MPI_Recv(Z, m * d, MPI_DOUBLE , src , 1000 , MPI_COMM_WORLD, &mpistat);
        
        //set the originid of the points
        originID = (originID == 0) ? numtasks - 1 : originID - 1;

        //wait for my corpus to be sent, so that I do not overwrite it
        MPI_Wait(&mpireq, &mpistat);
        
        //swap Y and Z pointers
        double *temp = Z;
        Z = Y;
        Y = temp;    

        //increment round
        round ++;  
    }

    //free variables
    free(D);
    free(normx);
    free(normy);
    
    return knn;
}

int main(int argc, char *argv[]){
        
    MPI_Init(&argc, &argv);

    MPI_Status mpistat;
    MPI_Request mpireq;

    int tid, numtasks;
    MPI_Comm_rank( MPI_COMM_WORLD , &tid);
    MPI_Comm_size( MPI_COMM_WORLD , &numtasks);

    printf("Cilk workers used for process %d : %d\n", tid,  __cilkrts_get_nworkers());

    //-----------------Input Control-----------------------------------
    if(tid == 0){
        if(argc != 3){
            printf("Wrong input: %d arguments. Right argument usage: inputs/infile.txt outputs/outfile.txt\n", argc);
            MPI_Abort( MPI_COMM_WORLD, 1);
        }
    }
    //--------------------End of input control-------------------------

    FILE *f;
    int N, d, k, n;

    //the process 0 will read the file and distribute it with MPI_Send to the other processes. Then it reopens the file and reads the first points as its own.

    //==========First read and send the size N and the dimension d and k ============
    if(tid == 0){
        f = fopen(argv[1], "r");
        if(f == NULL){
            printf("Could not open file %s\n", argv[1]);
            MPI_Abort( MPI_COMM_WORLD, 1);
        }
        fscanf(f, "%d %d %d\n", &N, &d, &k);
        fclose(f);

        for(int i = 1; i < numtasks; i++){
            MPI_Send(&N, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            MPI_Send(&d, 1, MPI_INT, i, 1, MPI_COMM_WORLD);
            MPI_Send(&k, 1, MPI_INT, i, 2, MPI_COMM_WORLD);
        }
    }
    else{
        //Receive the sizes
        MPI_Recv(&N, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &mpistat);
        MPI_Recv(&d, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &mpistat);
        MPI_Recv(&k, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, &mpistat);
    }
    //======================================================================
    
    //Wait for everyone to reach here
    MPI_Barrier(MPI_COMM_WORLD);

    //Initialize according to id
    int chunk = N / numtasks;
    int start = tid * chunk;
    int end = (tid == numtasks - 1) ? N  : (tid + 1) * chunk;
    n = end - start;

    double *X = (double *) malloc(n * d * sizeof(double));
    double temp;

    //------------------------------Start of read and distribute file------------------------------ 
    if(tid == 0){

        f = fopen(argv[1], "r");
        if(f == NULL){
            printf("Could not open file %s\n", argv[1]);
            MPI_Abort( MPI_COMM_WORLD, 1);
        }

        fscanf(f, "%d %d\n", &N, &d);

        //Read and send the corresponding points to each task id
        for(int p = 0; p < numtasks - 1; p++){
            for(int i = start; i < end; i ++){
                for(int j = 0; j < d; j++){
                    fscanf(f, "%lf ", X + (i - start) * d + j);
                }
                fscanf(f, "\n");
            }
            if(p > 0)
                MPI_Send(X, n * d, MPI_DOUBLE , p , 100 , MPI_COMM_WORLD);
        }

        //adjust-realloc to send to the last task
        X = realloc(X, (N - (numtasks - 1) * chunk) * d * sizeof(double));
        for(int i = (numtasks - 1) * chunk; i < N; i ++){
            for(int j = 0; j < d; j++){
                fscanf(f, "%lf ", X + (i - (numtasks - 1) * chunk) * d + j);
            }
            fscanf(f, "\n");
        }
        
        //send to the last task
        MPI_Send(X, (N - (numtasks - 1) * chunk) * d, MPI_DOUBLE , numtasks - 1 , 100 , MPI_COMM_WORLD);
        
        fclose(f);
        
        //Read 0's own points
        X = realloc(X, n * d * sizeof(double));
        f = fopen(argv[1],"r");
        fscanf(f, "%d %d\n", &N, &d);
        for(int i = start; i < end; i ++){
            for(int j = 0; j < d; j++){
                fscanf(f, "%lf ", X + (i - start) * d + j);
            }
            fscanf(f, "\n");
        }
        fclose(f);

    }
    else{
        //receive if I am not the task 0
        MPI_Recv(X, n * d, MPI_DOUBLE , 0 , 100 , MPI_COMM_WORLD, &mpistat);         
    }
    //------------------------------End of read and distribute file------------------------------
    
    //Wait for everyone
    MPI_Barrier(MPI_COMM_WORLD);


    //Calculate knn
    struct timeval start_time, end_time;
    double elapsed_time;
    gettimeofday(&start_time, NULL);
        knnresult knn = distrAllkNN(X, n, d, k, N);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    //wait for all processes to finish
    MPI_Barrier(MPI_COMM_WORLD);

    double othertime;

    //--------------------------------------Start of writing results--------------------------------------------------------------
    if(tid == 0){
        
        //Receive all execution times and keep the longest. That is the duration of the knn algorithm
        for(int i = 1; i < numtasks; i++){
            MPI_Recv(&othertime, 1, MPI_DOUBLE , i , i * 100 + 3 , MPI_COMM_WORLD, &mpistat);
            elapsed_time = (elapsed_time < othertime) ? othertime : elapsed_time;
        }
        printf("Execution Time, %lf\n", elapsed_time);


        f = fopen(argv[2], "w");
        if(f == NULL){
            printf("Could not open file %s\n", argv[2]);
            MPI_Abort( MPI_COMM_WORLD, 1);
        }
        //Wite Info
        fprintf(f, "Execution Time, %lf, Cilk workers %d, Number of tasks %d\n", elapsed_time,  __cilkrts_get_nworkers(), numtasks);

        //Write 0's knn.ndist
        fprint_arrd(f, knn.ndist, n, k);

        //Receive and write from the others
        for(int i = 1; i < numtasks - 1; i++){
            MPI_Recv(knn.ndist, n * k, MPI_DOUBLE , i , i * 100 + 1 , MPI_COMM_WORLD, &mpistat);
            fprint_arrd(f, knn.ndist, n, k);
        }

        //adjust for last task
        knn.ndist = realloc(knn.ndist, (N - (numtasks - 1) * chunk) * k * sizeof(double));
        MPI_Recv(knn.ndist, (N - (numtasks - 1) * chunk) * k, MPI_DOUBLE , numtasks - 1 , (numtasks - 1) * 100 + 1 , MPI_COMM_WORLD, &mpistat);
        fprint_arrd(f, knn.ndist, (N - (numtasks - 1) * chunk), k);

        fprintf(f, "\n");

        //Similar for ids
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
    //--------------------------------------End of writing results----------------------------------------------------------------
    
    MPI_Finalize();
    return 0;
}
