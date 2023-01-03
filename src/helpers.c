#include "knn.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

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

int quickselect(double *D, int n, int k, double *dist, int *idx){

    if(n == 0)
        return 0;

    srand(time(NULL));
    int pivot = (int) (rand() * (n - 1)), l = 0;
    double temp;

    for(int i = 0; i < n; i++)
        if(D[i] < D[pivot]){
            temp = D[l];
            D[l] = D[i];
            D[i] = temp;

            l++;
        }
    
    printf("%d %lf\n", l, D[pivot]);
    
    if(k < l)
        return quickselect(D, l, k, dist, idx);
    
    for(int i = 0; i < l; i++){
        dist[i] =  D[i];
        idx[i] = i;
    }

    int ret = l + quickselect(D + l, n - l, k - l, dist + l, idx + l);

    for(int i = l; i < k; i++)
        idx[i] += i;

    return ret;

}

//print helpers

void print_arr(double *arr, int r, int c){
    for(int i = 0; i < r; i++){
        for(int j = 0; j < c; j++)
            printf("%.3lf ", arr[i* c + j]);
        printf("\n");
    }
    printf("\n");
}