#include "knn.h"
#include <stdlib.h>
#include <string.h>

void swap(double *a, double *b){
    double temp = *a;
    *a = *b;
    *b = temp;
}

double quickselect(double* D, int left, int right, int k){
    if(left == right)
        return D[left];
    
    int pivot_index = rand() % (right - left + 1);
    double pivot = D[pivot_index];
    pivot_index = partition(D, left, right, pivot_index);

    if(pivot_index == k)
        return pivot;
    else if(k < pivot_index)
        return quickselect(D, left, pivot_index - 1, k);
    
    return quickselect(D, pivot_index + 1, right, k);
    
}

int partition(double* D, int left, int right, int pivot_index){
    double pivot = D[pivot_index];
    int new_pivot_index = left;
    
    swap(D + pivot_index, D + right);

    for(int i = 0; i < right; i++){
        if(D[i] < pivot){
            swap( D + i, D + new_pivot_index);
            new_pivot_index ++;
        }    
    }
    
    swap(D + new_pivot_index, D + right);
    return new_pivot_index;
}

void kselect(double *D, int left, int right, int k, double *dist, int *idx){
    
    int n = right - left + 1;

    double *Dcopy = (double *) malloc(sizeof(double) * n);
    memcpy(Dcopy, D + left, sizeof(double) * n);

    double pivot = quickselect(Dcopy, 0, n - 1, k);

    free(Dcopy);



    int cnt = 0;

    for(int i = left; i <= right; i++)
        if(D[i] < pivot){
            dist[cnt] = D[i];
            idx[cnt] = i; //global index of corpus
            cnt ++;
        }

    if(cnt < k)
        for(int i = left; i <= right; i++)
            if(D[i] == pivot){
                dist[cnt] = D[i];
                idx[cnt] = i;
                cnt ++;
            }

}



