#include "knn.h"

void shift_select(int *nidx, double *ndist, int n_found ,double *dist, int n, int k, int global_id_offset){
    int pos;
    bool pos_found;

    for(int j = 0; j < n; j++){       
        pos = 0;
        pos_found = false;
        
        while(pos < n_found){
                
            if(dist[j] < ndist[pos]){
                shift(ndist, k, pos, n_found, DOUBLE);
                shift(nidx, k, pos, n_found, INT); 

                nidx[pos] = j + global_id_offset;
                ndist[pos] = dist[j];
                
                pos_found = true;
                n_found = min(n_found + 1, k);
                break;
            }
            pos ++;
        }

        if(!pos_found && n_found < k){
            nidx[n_found] = j + global_id_offset;
            ndist[n_found] = dist[j];
            n_found = min(n_found + 1, k);
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