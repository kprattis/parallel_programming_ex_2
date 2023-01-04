#include "knn.h"
#include <stdlib.h>
#include <stdio.h>
#include <endian.h>
#include <time.h>

void randarr(double *X, int size, double max, double min){
   
    srand(time(NULL));
    for(int j = 0; j < size; j++)
        X[j] = (rand() * 1.0 / RAND_MAX) * (max - min) + min;

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

double * read_MNIST_images(char * filename, int *num_images, int *rows, int *cols){
    
    FILE *f;
    if((f = fopen(filename, "r")) == NULL){
        fprintf(stderr, "Error. Could not open file: %s.\n",filename);
        exit(1);
    }

    unsigned int res, magic, n_img, n_rows, n_cols;
    
    u_int8_t pixel;

    res = fread(&magic, sizeof(unsigned int), 1, f);
    res = fread(&n_img, sizeof(unsigned int), 1, f);
    res = fread(&n_rows, sizeof(unsigned int), 1, f);
    res = fread(&n_cols, sizeof(unsigned int), 1, f);
    
    if(magic > 3000){
        magic = be32toh(magic);
        n_img = be32toh(n_img);
        n_rows = be32toh(n_rows);
        n_cols = be32toh(n_cols);
    }
    else{
        magic = le32toh(magic);
        n_img = le32toh(n_img);
        n_rows = le32toh(n_rows);
        n_cols = le32toh(n_cols);
    }

    int m = n_img;
    int d = n_cols * n_rows;

    *num_images = m;
    *rows = n_rows;
    *cols = n_cols;

    double *X = (double *) malloc(m * d * sizeof(double));

    for(int i = 0; i < m; i++){
        for(int j = 0; j < d; j++){
            res = fread(&pixel, sizeof(u_int8_t), 1, f);
            X[i * d + j] = (double) (pixel / 255.0);
        }
    }
    
    fclose(f);
    return X;
}
