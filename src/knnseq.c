#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){
    
    double *normx = (double *) malloc(m * sizeof(double));
    double *normy = (double *) malloc(n * sizeof(double));

    cilk_for(int j = 0; j < n; j++)
        normy[j] = euclidean_norm(Y + j * d, d);
    
    cilk_for(int i = 0; i < m; i++)
        normx[i] = euclidean_norm(X + i * d, d);


    double *D = (double *) malloc(m * n * sizeof(double));

    cilk_for(int i = 0; i < m; i++)
        cilk_for(int j = 0; j < n; j++)
            D[i * n + j] = normx[i] + normy[j];

    free(normx);
    free(normy);

    cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, n, d, -2.0, X, d, Y, d, 1.0 , D, n);

    print_arrd(X, m, d);
    print_arrd(Y, n, d);
    print_arrd(D, m, n);
    
    knnresult knn = init_knnresult(m, k);

    cilk_for(int i = 0; i < m; i++){
        kselect(D + i * n, 0, n - 1, k, knn.ndist + i * k, knn.nidx + i * k); 
        //modify indexes to be global
        cilk_for(int j = 0; j < k; j++)
            knn.nidx[i * k + j] *= d;
            // this now shows to the start of the corresponding corpus point
    }

    free(D);
    return knn;
}

int main(int argc, char *argv[]){

    int n = 5, m = 6, k = 2, d = 3;
    double *X = (double *) malloc(sizeof(double) * m * d);
    double *Y = (double *) malloc(sizeof(double) * n * d);

    if(argc < 2){
        randarr(X, m * d, 5.0, -5.0);
        randarr(Y, n * d, 2.0, -2.0);
    }
    else if(argc == 2){
        randarr(X, m * d, 5.0, -5.0);
        randarr(Y, n * d, 2.0, -2.0);
    }
    else{
        fprintf(stderr, "Wrong number of inputs, expected 0 or 1 but got %d.\nInput a filename containing the data or nothing for a random example.\n", argc - 1);
        exit(1);
    }


    struct timeval start_time, end_time;
    double elapsed_time;
    
    gettimeofday(&start_time, NULL);
        knnresult knn = kNN(X, Y, n, m, d, k);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    
    print_arri(knn.nidx, m, k);
    print_arrd(knn.ndist, m, k);
    printf("Time is %lf\n", elapsed_time);

    free_knnresult(knn);
    free(X);
    free(Y);
    return 0;
}