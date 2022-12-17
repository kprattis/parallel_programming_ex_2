#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include "cblas.h"

knnresult *init_knnresult(int k, int m){
    
    knnresult *knn = malloc(sizeof(knnresult));

    knn->k = k;
    knn->m = m;
    knn->ndist = (double *) malloc(m * k * sizeof(double));
    knn->nidx = (int *) malloc(m * k * sizeof(int));
    
    return knn;
}

void free_knnresult(knnresult *knn){
    free(knn->ndist);
    free(knn->nidx);
    free(knn);
}


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){
        
    double *dist = (double *) calloc(m * n, sizeof(double)); // [m by n]

    knnresult *knn = init_knnresult(k, m);

    //calculate distances matrix dist

    //Create X o X vector
    for(int i = 1; i < m; i++){
        for(int j = 0; j < d; j++){
            dist[i * n] += X[i * d + j] * X[i * d + j];
        }
    }
    
    //Create Y o Y vector
    for(int i = 1; i < n; i++){
        for(int j = 0; j < d; j++){
            dist[i] += Y[i * d + j] * Y[i * d + j];
        }
    }

    // Copy XoX to all columns and YoY to all rows of dist
    for(int i = 1; i < m; i++){
        for(int j = 1; j < n; j++){
            dist[i * n + j] = dist[j] + dist[i * n];  
        }
    }

    cblas_dgemm(CblasRowMajor, CblasNoTrans,
                 CblasTrans, m, n,
                 d, -2.0, X,
                 m, Y, d, 1.0 , dist, m);

    printf("distane matrix is: \n");
    print_arr(dist, m, n);

    free(dist);
}

void print_arr(double *arr, int a, int b){
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            printf("%.3lf ", arr[i * b + j]);
        }
        printf("\n");
    }
}