#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include "cblas.h"

void print_arr(void *arr, int a, int b, type t){
    
    switch (t){
        case INT:
            
            for(int i = 0; i < a; i++){
                for(int j = 0; j < b; j++){
                    printf("%d ", ((int *)arr)[i * b + j]);
                }
                printf("\n");
            }
            break;
        
        case DOUBLE:

            for(int i = 0; i < a; i++){
                for(int j = 0; j < b; j++){
                    printf("%.3lf ", ((double *)arr)[i * b + j]);
                }
                printf("\n");
            }
            break;
    }
    
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


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){
        
    double *dist = (double *) calloc(m * n, sizeof(double)); // [m by n]

    knnresult knn = init_knnresult(k, m);

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

    for(int i = 0; i < d; i++)
        dist[0] += Y[i] * Y[i];

    for(int i = 1; i < m; i++){
        dist[i * n] += dist[0]; 
    }

    double temp = dist[0];
    dist[0] = 0;
    for(int i = 0; i < d; i++)
        dist[0] += X[i] * X[i];

    for(int i = 1; i < n; i++){
        dist[i] += dist[0]; 
    }

    dist[0] += temp;
    // End of copy section

    //printf("distane matrix is: \n");
    //print_arr(dist, m, n, DOUBLE);

    cblas_dgemm(CblasRowMajor, CblasNoTrans,
                 CblasTrans, m, n,
                 d, -2.0, X,
                 d, Y, d, 1.0 , dist, n);

    //printf("distane matrix is: \n");
    //print_arr(dist, m, n, DOUBLE);

    k_select(&knn, dist, m, n, k);

    free(dist);

    return knn;
}

void k_select(knnresult *knn, double *dist, int m, int n, int k){
    int taken_num, pos;
    bool pos_found;
    for(int i = 0; i < m; i++){
        knn->nidx[i * k] = 0;
        knn->ndist[i * k] = dist[i * n];
        taken_num = 1;
        for(int j = 1; j < n; j++){       
            pos = 0;
            pos_found = false;
            while(pos < taken_num){
                    
                if(dist[i * n + j] < knn->ndist[i * k + pos]){
                    shift(knn->ndist + i*k, k, pos, taken_num, DOUBLE);
                    shift(knn->nidx + i*k, k, pos, taken_num, INT); 

                    knn->nidx[i * k + pos] = j;
                    knn->ndist[i * k + pos] = dist[i * n + j];
                    
                    pos_found = true;
                    taken_num = (taken_num + 1 < k) ? taken_num + 1 : k;
                    break;
                }
                pos ++;
            }

            if(!pos_found && taken_num < k){
                knn->nidx[i * k + taken_num] = j;
                knn->ndist[i * k + taken_num] = dist[i * n + j];
                taken_num = (taken_num + 1 < k) ? taken_num + 1 : k;
            }
            
        }
    }
}

int shift(void *arr, int size, int start, int end, type t){
    if(start < 0 || end > size) return 1;
    switch (t){
        case INT :
            for(int i = end; i > start; i--){
                if(i == size) continue;
                ((int *)arr)[i] = ((int *)arr)[i - 1];
            }
            break;
        case DOUBLE :
            for(int i = end; i > start; i--){
                if(i == size) continue;
                ((double *) arr)[i] = ((double *) arr)[i - 1]; 
            }
            break;
    }
    return 0;

}