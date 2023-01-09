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

    if(argc < 2){
        n = 17;
        m = 17; 
        k = 3; 
        d = 1;
        
        X = (double *) malloc(sizeof(double) * m * d);
        Y = (double *) malloc(sizeof(double) * n * d);
        
        FILE *f = fopen("input.txt", "r");
        for(int i = 0; i < n; i++){
            fscanf(f, "%lf\n", &X[i]);
            Y[i] = X[i];
        }
        fclose(f);

        //randarr(X, m * d, 5.0, -5.0);
        //randarr(Y, n * d, 2.0, -2.0);
    }
    else if(argc == 2){
        int rows, cols;
        k = 5;
        X = read_MNIST_images(argv[1], &m, &rows, &cols);

        d = rows * cols;
        n = m;
        Y = X;
        yfree = 0;
    }
    else if(argc == 3){
        int rows, cols;
        k = 5;
        X = read_MNIST_images(argv[1], &m, &rows, &cols);
        d = rows * cols;
        Y = read_MNIST_images(argv[2], &n, &rows, &cols);
        yfree = 1;
    }
    else{
        fprintf(stderr, "Wrong number of inputs, expected 0 or 1 but got %d.\nInput a filename containing the data for X to be copied to Y or 2 filenames for X and Y or nothing for a random example.\n", argc - 1);
        exit(1);
    }
    
    //print_arrd(X, m, d);
    //print_arrd(Y, n, d);

    struct timeval start_time, end_time;
    double elapsed_time;
    
    gettimeofday(&start_time, NULL);
        knnresult knn = kNN(X, Y, n, m, d, k);
        
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    //print_arri(knn.nidx, m, k);
    //print_arrd(knn.ndist, m, k);
    printf("Execution Time, %lf\n", elapsed_time);

    free_knnresult(knn);
    free(X);
    if(yfree)
        free(Y);
    return 0;
}