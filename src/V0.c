#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>
#include <string.h>

#define MAXSIZE  5 * 1024 * 1024


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){

    int BLOCKSIZE = n;
    if (n * m > MAXSIZE){
      BLOCKSIZE = min( m * n / 4, MAXSIZE);
      BLOCKSIZE = MAXSIZE / m;   
    }


    printf("%d reduced from %d\n", BLOCKSIZE * m, n * m);

    if(BLOCKSIZE < 10){
        printf("too large sizes m = %d, n = %d, d = %d\n", m, n, d);
        exit(1);
    }

    //here store norms of Y points
    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));  
    
    //calculate norms of X points
    double *normx = (double *) malloc(m * sizeof(double));
    cilk_for(int i = 0; i < m; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    //Distance array
    double *D = (double *) malloc(m * BLOCKSIZE * sizeof(double));

    //the result
    knnresult knn;
    
    double *shortest_distances = (double *) malloc(sizeof(double) * 3 * k * m);
    int *nearest_idxs = (int *) malloc(sizeof(int) * 3 * k * m);
    
    int start, end, size;

    for(int b = 0; b < n; b += BLOCKSIZE){
        start = b;
        end = min(n, b + BLOCKSIZE);
        size = end - start;

        cilk_for(int j = start; j < end; j++)
            normy[j - start] = euclidean_norm(Y + j * d, d);

        cilk_for(int i = 0; i < m; i++)
            cilk_for(int j = start; j < end; j++)
                D[i * BLOCKSIZE + j - start] = normx[i] + normy[j - start];

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);


        int nfound = min(k, size);
        cilk_for(int i = 0; i < m; i++){

            kselect(D + i * BLOCKSIZE, 0, size - 1, nfound, shortest_distances +  3 * i * k + k * (b != 0), nearest_idxs + 3 * i * k + k * (b != 0)); 

            //modify indexes to be global
            cilk_for(int j = 0; j < k; j++){
                nearest_idxs[3 * i * k + k * (b != 0) + j] += start; 
                nearest_idxs[3 * i * k + k * (b != 0) + j] *= d;
            }
        
            if(b != 0){

                kselect(shortest_distances + 3 * i * k, 0, k + nfound - 1, k, shortest_distances + 3 * i * k + 2 * k, nearest_idxs + 3 * i * k + 2 * k);

                cilk_for(int j = 0; j < k; j++){
                    nearest_idxs[3 * i * k + 2 * k + j] = nearest_idxs[3 * i * k + nearest_idxs[3 * i * k + 2 * k + j]];
                }

                cilk_for(int j = 0; j < k; j++){
                    nearest_idxs[3 * i * k + j] = nearest_idxs[3 * i * k + 2 * k + j];
                    shortest_distances[3 * i * k + j] = shortest_distances[3 * i * k + 2 * k + j];
                }
                
            }
        }
    }

    free(normx);
    free(normy);

    free(D);
    

    knn = init_knnresult(m, k);
    cilk_for(int i = 0; i < m; i++){
        cilk_for(int j = 0; j < k; j++){
            knn.nidx[i * k + j] = nearest_idxs[3 * i * k + j];
            knn.ndist[i * k + j] = shortest_distances[3 * i * k + j];
        }
    }
  
    free(nearest_idxs);
    free(shortest_distances);

    return knn;
}

int main(int argc, char *argv[]){
    int n, m, k = 3, d;
    double *X, *Y;

    if(argc < 2){
        n = 10000;
        m = 10000; 
        k = 3; 
        d = 1;
        
        X = (double *) malloc(sizeof(double) * m * d);
        Y = (double *) malloc(sizeof(double) * n * d);
        
        randarr(X, m * d, 5.0, -5.0);
        randarr(Y, n * d, 2.0, -2.0);
    }
    else if(argc == 2){
        int rows, cols;
        k = 5;
        X = read_MNIST_images(argv[1], &m, &rows, &cols);
        d = rows * cols;
        n = m;
        Y = X;
    }
    else if(argc == 3){
        int rows, cols;
        k = 5;
        X = read_MNIST_images(argv[1], &m, &rows, &cols);
        d = rows * cols;
        Y = read_MNIST_images(argv[2], &n, &rows, &cols);
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
    if(Y != NULL)
        free(Y);
    return 0;
}