#include "cblas.h"
#include "knn.h"
#include <mpi.h>

//The memory assumed is n * d for X, n * d for the incoming points Y 

knnresult distrAllkNN(double * X, int n, int d, int k){
    
    int pid, nproc;
    knnresult myknn;
    MPI_Request mpireq;
    MPI_Status mpistat;

    MPI_Comm_rank( MPI_COMM_WORLD , &pid);
    MPI_Comm_size( MPI_COMM_WORLD , &nproc);
    int src = (pid == 0) ? (nproc - 1) : pid - 1;
    int dest = (pid + 1) % nproc;

    double *Z = (double *) malloc(sizeof(double) * (n * d) );
    double *Y = (double *) malloc(sizeof(double) * (n * d) );
    double *temp;

    Y = X; //first iteration

    MPI_Irecv(Z ,n * d , MPI_DOUBLE ,src , 100 + 10 * src, MPI_COMM_WORLD , &mpireq);
    
    myknn = knn(X, Y, n, n, d, k);
    MPI_send(Y ,n * d , MPI_DOUBLE, dest , 100 + 10 * dest, MPI_COMM_WORLD , &mpistat);

    MPI_Wait( &mpireq , &mpistat);
    
    //swap pointers
    temp = Y;
    Y = Z;
    Z = temp;
    
}

int main(int argc, char *argv[]){
    MPI_Init( &argc , &argv);

    int n, d, k, pid, nproc;
    const int NITERS;
    double *X;

    MPI_Comm_rank( MPI_COMM_WORLD , &pid);
    MPI_Comm_size( MPI_COMM_WORLD , &nproc);

    //Maybe to read
    knnresult finalknn = distrAllkNN(double * X, int n, int d, int k);



    MPI_Finalize();
}