#include "knn.h"
#include <stdlib.h>
#include <time.h>

double * randarr(int size, double max, double min){
    double *X = (double *) malloc(size * sizeof(int)); 
    
    srand(time(NULL));

    for(int j = 0; j < size; j++){
        X[j] = (rand() * 1.0 / RAND_MAX) * (max - min) + min;
    }
    
    return X;
}

double * regular_grid(int dim, double start[], double end[], double step[], int *n_points){
    
    int *dim_size = (int *) malloc(dim * sizeof(int));
    int total_size = 1;

    for(int d = 0; d < dim; d++){
        dim_size[d] = (int) (end[d] - start[d])/step[d] + 1;
        total_size = dim_size[d] * total_size;
    }

    *n_points = total_size;

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
