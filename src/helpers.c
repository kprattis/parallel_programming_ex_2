#include "knn.h"
#include <stdio.h>
#include <stdlib.h>

void print_arr(void *arr, int a, int b, type t){
    
    switch (t){
        case INT:
            
            for(int i = 0; i < a; i++){
                for(int j = 0; j < b; j++){
                    printf("%6d ", ((int *)arr)[i * b + j]);
                }
                printf("\n");
            }
            break;
        
        case DOUBLE:

            for(int i = 0; i < a; i++){
                for(int j = 0; j < b; j++){
                    printf("%7.2lf ", ((double *)arr)[i * b + j]);
                }
                printf("\n");
            }
            break;
    }
    
}

void randarr(double *X, int a, int b){
    for(int i = 0; i < a; i++){
        for(int j = 0; j < b; j++){
            X[i * b + j] = ((int) rand()) % 10 * 1.0;
        }
    }
}

void print_results(knnresult knn){
    printf("The %d nn ids for each query point are:\n", knn.k);
    print_arr(knn.nidx, knn.m, knn.k, INT);

    printf("With distances:\n");
    print_arr(knn.ndist, knn.m, knn.k, DOUBLE);
}

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

double euclidean_norm(double *vec, int d){
    double norm = 0.0;
    for(int i = 0; i < d; i++)
        norm += vec[i] * vec[i];
    return norm;
}

int min(int a, int b){
    return (a < b) ? a : b;
}
