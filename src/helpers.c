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

void print_results(knnresult knn, bool print_dist){
    printf("The %d nearest neighbors id's for each query point are:\n", knn.k);
    print_arr(knn.nidx, knn.m, knn.k, INT);

    if(print_dist){
        printf("With distances:\n");
        print_arr(knn.ndist, knn.m, knn.k, DOUBLE);
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

double euclidean_norm(double *vec, int d){
    double norm = 0.0;
    for(int i = 0; i < d; i++)
        norm += vec[i] * vec[i];
    return norm;
}

int min(int a, int b){
    return (a < b) ? a : b;
}

void save_image(double *Image, int rows, int cols, char *filename){
    FILE *f = fopen(filename, "w");

    if(f == NULL){
        fprintf(stderr, "Could not open the file %s\n", filename);
        exit(1);
    }

    u_int8_t pixel; 
    for(int r = 0; r < rows; r++){
        fprintf(f, "%u", (u_int8_t) (Image[r * cols] * 255) );
        for(int c = 1; c < cols; c++){
            fprintf(f, " %u", (u_int8_t) (Image[r * cols + c] * 255) );
        }
        fprintf(f, "\n");
    }

    fclose(f);
}