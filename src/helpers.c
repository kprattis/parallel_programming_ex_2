#include "knn.h"
#include <stdlib.h>
#include <stdio.h>

//Init-free functions

knnresult init_knnresult(int k, int m){
    
    knnresult knn;

    knn.k = k;
    knn.m = m;
    knn.ndist = (double *) malloc(m * k * sizeof(double));
    knn.nidx = (int *) malloc(m * k * sizeof(int));
    
    return knn;
}

void free_knnresult(knnresult knn){
    free(knn.ndist);
    free(knn.nidx);
}

// Helping functions

double euclidean_norm(double *vec, int d){
    double norm = 0.0;
    for(int i = 0; i < d; i++)
        norm += vec[i] * vec[i];
    return norm;
}

int min(int a, int b){
    return (a < b) ? a : b;
}

//print helpers

void print_arrd(double *arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            printf("%.3lf ", arr[i* c + j]);
        printf("\n");
    }
    printf("\n");
}

void print_arri(int *arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            printf("%d ", arr[i* c + j]);
        printf("\n");
    }
    printf("\n");
}