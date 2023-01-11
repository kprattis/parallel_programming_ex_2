#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk_api.h>
#include <cilk/cilk.h>
#include <string.h>

#define MAXSIZE 1024 * 1024 * 256l


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){

    //Here set the blocksize, which is the reduced size of the dimension n of the distance matrix
    long BLOCKSIZE = n;

    if ( (long) m * n > MAXSIZE){
      BLOCKSIZE = min( (long) m * n / 4, MAXSIZE);
      BLOCKSIZE /= m;   
    }

    printf("The distance matrix will have size: %d x %ld. Calculating %d nearest neighbors\n", m, BLOCKSIZE, k);
    if(BLOCKSIZE < k){
        printf("Error: too large sizes m = %d, n = %d\n", m, n);
        exit(1);
    }

    //Distance array
    double *D = (double *) malloc(m * BLOCKSIZE * sizeof(double));

    double *normx = (double *) malloc(m * sizeof(double));
    cilk_for(int i = 0; i < m; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));

    //the result
    knnresult knn = init_knnresult(m, k);
    
    int start, end, size;

    for(int b = 0; b < n; b += BLOCKSIZE){
        start = b;
        end = min(n, b + BLOCKSIZE);
        size = end - start;

        cilk_for(int i = 0; i < size; i++)
            normy[i] = euclidean_norm(Y + (i + start) * d, d);

        cilk_for(int i = 0; i < m; i++)
            cilk_for(int j = 0; j < size; j++){
                D[i * size + j] = normx[i] + normy[j];
            }

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

        //printf("%d \n", b);
        cilk_for(int i = 0; i < m; i++)
            kselect(D + i * size, 0, size - 1, k, knn.ndist + i * k, knn.nidx + i * k, (b == 0), start);
    
    }

    free(normx);
    free(normy);
    free(D);

    return knn;
}

int main(int argc, char *argv[]){
    int n, m, k, d;
    double *X, *Y;
    printf("Cilk workers used : %d\n", __cilkrts_get_nworkers());
    int yfree = 1;

    if(argc == 3){
                
        FILE *f = fopen(argv[1], "r");
        if(f == NULL){
            printf("Error: Could not open file %s\n", argv[1]);
        }
        printf("Processing file %s\n", argv[1]);
        fscanf(f, "%d %d\n", &n, &d);
        k = pow(3, d);
        m = n;

        X = (double *) malloc(sizeof(double) * m * d);
        

        for(int i = 0; i < m; i++){
            for(int j = 0; j < d; j++){
                fscanf(f, "%lf ", X + i * d + j);
            }
            fscanf(f, "\n");
        }
        
        fclose(f);

        Y = X;
        yfree = 0;
    }
    else{
        printf("Wrong input: %d arguments. Right usage: bin/knn inputs/infile.txt outputs/outfile.txt\n", argc);
        exit(1);
    }
    
    struct timeval start_time, end_time;
    double elapsed_time;
    
    gettimeofday(&start_time, NULL);
        knnresult knn = kNN(X, Y, n, m, d, k);
        
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    printf("Execution Time, %lf\n", elapsed_time);

    FILE *f = fopen(argv[2], "w");
    if(f == NULL){
        printf("Error, could not open file %s\n", argv[2]);
        exit(1);
    }

    fprintf(f, "Execution Time : %lf, cilk workers: %d\n", elapsed_time, __cilkrts_get_nworkers());

    for(int i = 0; i < m; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%d ", knn.nidx[i * k + j]);
        }
        fprintf(f, "\n");
    }

    fprintf(f, "\n");

    for(int i = 0; i < m; i++){
        for(int j = 0; j < k; j++){
            fprintf(f, "%.3lf ", knn.ndist[i * k + j]);
        }
        fprintf(f, "\n");
    }

    fclose(f);

    free_knnresult(knn);
    
    free(X);
    if(yfree)
        free(Y);
    return 0;
}

/*
else if(argc == 3){
        int rows, cols;
        k = 5;
        X = read_MNIST_images(argv[1], &m, &rows, &cols);
        d = rows * cols;
        Y = read_MNIST_images(argv[2], &n, &rows, &cols);
        yfree = 1;
    }
    */