#include "knn.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <cilk/cilk.h>
#include <pthread.h>

void swap(double *a, double *b){
    double temp = *a;
    *a = *b;
    *b = temp;
}

int partition(double* D, int left, int right, int pivot_index){
    double pivot = D[pivot_index];
    int new_pivot_index = left;
    
    swap(D + pivot_index, D + right);

    for(int i = left; i < right; i++){
        if(D[i] < pivot){
            swap( D + i, D + new_pivot_index);
            new_pivot_index ++;
        }    
    }
    
    swap(D + new_pivot_index, D + right);
    return new_pivot_index;
}

double quickselect(double* D, int left, int right, int k){
    if(left == right)
        return D[left];
    
    int pivot_index = rand() % (right - left + 1) + left;
    
    double pivot = D[pivot_index];
    
    pivot_index = partition(D, left, right, pivot_index);
    
    if(pivot_index == k - 1)
        return pivot;
    else if(k - 1 < pivot_index)
        return quickselect(D, left, pivot_index - 1, k);
    
    return quickselect(D, pivot_index + 1, right, k);
    
}


void kselect(double *D, int left, int right, int k, double *dist, int *idx){
    
    int n = right - left + 1;

    double *Dcopy = (double *) malloc(sizeof(double) * n);
    memcpy(Dcopy, D + left, sizeof(double) * n);

    double pivot = quickselect(Dcopy, 0, n - 1, k);

    free(Dcopy);

    int cnt = 0;
    pthread_mutex_t cnt_mutex;
    pthread_mutex_init(&cnt_mutex, NULL);

    cilk_for(int i = left; i <= right; i++)

        if(D[i] < pivot){
            pthread_mutex_lock(&cnt_mutex);
            dist[cnt] = D[i];
            idx[cnt] = i;
            cnt ++;
            pthread_mutex_unlock(&cnt_mutex);
        }

    if(cnt < k)
        cilk_for(int i = left; i <= right; i++){
            if(D[i] == pivot){
                pthread_mutex_lock(&cnt_mutex);
                dist[cnt] = D[i];
                idx[cnt] = i;
                cnt ++;
                pthread_mutex_unlock(&cnt_mutex);
            }
        }

    pthread_mutex_destroy(&cnt_mutex);
}



