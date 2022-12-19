#include "knn.h"

void shift_select(knnresult *knn, double *dist, int m, int n, int k){
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