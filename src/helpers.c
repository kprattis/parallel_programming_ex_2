#include "knn.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

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

long min(long a, long b){
    return (a < b) ? a : b;
}

//print helpers

void print_arrd(double *arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            printf("%+.2lf ", arr[i* c + j]);
        printf("\n");
    }
    printf("\n");
}

void print_arri(int *arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            printf("%2d ", arr[i* c + j]);
        printf("\n");
    }
    printf("\n");
}

void fprint_arrd(FILE *f, double *arr, int n, int k){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%.3lf ", arr[k * i + j]);
        }
         fprintf(f, "\n");
    }
}

void fprint_arri(FILE *f, int *arr, int n, int k){
    for(int i = 0; i < n; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%d ", arr[k * i + j]);
        }
         fprintf(f, "\n");
    }
}