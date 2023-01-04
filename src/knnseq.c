#include <stdio.h>
#include "knn.h"
#include <cblas.h>
#include <stdlib.h>
#include <sys/time.h>
#include <omp.h>

//The assumption is that the memory of our computer is enough for 3 n*d double arrays. 
//Therefore we will compute block by block the distance matrix D, each block with enough
//size to fit in memory.

    /*
    double *normx = (double *) malloc(m * sizeof(double));
    double *normy = (double *) malloc(n * sizeof(double));

    for(int j = 0; j < n; i++)
        normy[j] = euclidean_norm(Y + j * d, d);
    
    for(int i = 0; i < m; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            D[i * n + j] = normy[j] + normx[i];
    
    free(normx);
    free(normy);
    */

knnresult kNN(double *X, double *Y, int n, int m, int d, int k){
    
    knnresult knn = init_knnresult(m, k);
    double *D = (double *) malloc(m * n * sizeof(double));

    //#pragma omp parallel for collapse(2)
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            D[i * n + j] = euclidean_norm(Y + j * d, d) + euclidean_norm(X + i * d, d);
    
    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, n, d, -2.0, X, d, Y, d, 1.0 , D, n);

    print_arrd(X, m, d);
    print_arrd(Y, n, d);
    print_arrd(D, m, n);

    for(int i = 0; i < m; i++){
        kselect(D + i * n, 0, n - 1, k, knn.ndist + i * k, knn.nidx + i * k);
    
        //modify indexes to be global
        for(int j = 0; j < k; j++)
            knn.nidx[i * k + j] *= d;
            // this now shows to the start of the corresponding corpus point
    }

    free(D);
    return knn;
}

int main(int argc, char *argv[]){

    
    int n = 3, m = 2, k = 2, d = 2;
    double Y[] = {2.0, 3.0, 4.0, 8.0 ,9.0, 11.0};
    double X[] = {1.0, 4.0, 2.0, 6.0};

    struct timeval start_time, end_time;
    double elapsed_time;
    
    gettimeofday(&start_time, NULL);
    knnresult knn = kNN(X, Y, n, m, d, k);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    printf("Time is %lf\n", elapsed_time);
    print_arri(knn.nidx, m, k);
    print_arrd(knn.ndist, m, k);

    free_knnresult(knn);
    //free(X);
    //free(Y);
    return 0;
}