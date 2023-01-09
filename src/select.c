#include "knn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <pthread.h>


void swap(double *a, double *b){
    double temp = *a;
    *a = *b;
    *b = temp;
}

void swapi(int *a, int *b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(double* D, int *indices, int left, int right, int pivot_index){
    double pivot = D[pivot_index];
    int new_pivot_index = left;
    
    swapi(indices + pivot_index, indices + right);
    swap(D + pivot_index, D + right);

    for(int i = left; i < right; i++){
        if(D[i] < pivot){
            swap( D + i, D + new_pivot_index);
            swapi(indices + i, indices + new_pivot_index);
            new_pivot_index ++;
        }    
    }
    
    swap(D + new_pivot_index, D + right);
    swapi(indices + new_pivot_index, indices + right);

    return new_pivot_index;
}

double quickselect(double* D, int *indices, int left, int right, int k){
    if(left == right)
        return D[left];
    
    int pivot_index = rand() % (right - left + 1) + left;
    
    double pivot = D[pivot_index];
    
    pivot_index = partition(D, indices, left, right, pivot_index);
    
    if(pivot_index == k - 1)
        return pivot;
    else if(k - 1 < pivot_index)
        return quickselect(D, indices, left, pivot_index - 1, k);
    
    return quickselect(D, indices, pivot_index + 1, right, k);
    
}

void kselect(double *D, int left, int right, int k, double *ndist, int *nidx, int isknnempty, int shift){
    
    int n = right - left + 1;
    int *indices = (int *) malloc(sizeof(int) * n);

    int kval = min(k, n);

    cilk_for(int i = 0; i < n; i++){
        indices[i] = shift + i;
    }

    quickselect(D, indices, 0, n - 1, kval);
    
    if(isknnempty){
        cilk_spawn memcpy(ndist, D, k * sizeof(double));
        memcpy(nidx, indices, k * sizeof(int));

        cilk_sync;
    }
    else{
        
        double *mergeddists = (double *) malloc(sizeof(double) * (k + kval));
        cilk_spawn memcpy(mergeddists, ndist, sizeof(double) * k);
        cilk_spawn memcpy(mergeddists + k, D, kval * sizeof(double));
        
        int *mergedidx = (int *) malloc(sizeof(int) * (k + kval));
        cilk_spawn memcpy(mergedidx, nidx, sizeof(int) * k);
        memcpy(mergedidx + k, indices, kval * sizeof(int));

        free(indices);

        cilk_sync;

        quickselect(mergeddists, mergedidx, 0, kval + k - 1, k);
        cilk_spawn memcpy(ndist, mergeddists, k * sizeof(double));
        memcpy(nidx, mergedidx, k * sizeof(int));
        
        cilk_sync;


        free(mergedidx);
        free(mergeddists);
    }


}


