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
            X[i * b + j] = ((int) rand()) % 1000 * 1.0;
        }
    }
}

void print_results(knnresult knn){
    printf("The %d nearest neighbors id's for each query point are:\n", knn.k);
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

double * regular_grid(int dim, double start[], double end[], double step[], int *s){
    
    int *dim_size = (int *) malloc(dim * sizeof(int));
    int total_size = 1;

    for(int d = 0; d < dim; d++){
        dim_size[d] = (int) (end[d] - start[d])/step[d] + 1;
        total_size = dim_size[d] * total_size;
    }

    *s = total_size;

    double *X = (double *) malloc(total_size * dim * sizeof(double));
    int index;
    
    for(int i = 0; i < total_size; i++){
        index = i;
        for(int d = 0; d < dim; d++){
            X[i * dim + d] =  (index % dim_size[d]) * step[d] + start[d];
            index = index / dim_size[d];
        }
    }

    free(dim_size);

    return X;
}