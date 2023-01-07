#include "knn.h"
#include <stdio.h>
#include <cblas.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <cilk/cilk.h>

#define MAXSIZE 1024


knnresult kNN(double *X, double *Y, int n, int m, int d, int k){

    int BLOCKSIZE = n;
    if (n * m > MAXSIZE)
        BLOCKSIZE = MAXSIZE;   

    printf("%d\n", BLOCKSIZE);

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
    
    int start, end, size, id = 0;

    for(int b = 0; b < n; b += BLOCKSIZE){
        start = b;
        end = min(n, b + BLOCKSIZE);
        size = end - start;

        cilk_for(int j = start; j < end; j++)
            normy[j - start] = euclidean_norm(Y + j * d, d);

        cilk_for(int i = 0; i < m; i++)
            cilk_for(int j = start; j < end; j++)
                D[i * n + j] = normx[i] + normy[j - start];

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

        //print_arrd(X, m, d);
        //print_arrd(Y + start * d, size, d);
        //print_arrd(D, m, size);

        int nfound = min(k, size);
        cilk_for(int i = 0; i < m; i++){
            printf("%d %d\n", i, id);
            kselect(Dcopy, D + i * BLOCKSIZE, 0, size - 1, nfound, shortest_distances +  3 * i * k + k * (id != 0), nearest_idxs + 3 * i * k + k * (id != 0)); 
            printf("%d %d\n", i, id);
            //modify indexes to be global
            cilk_for(int j = 0; j < k; j++){
                nearest_idxs[3 * i * k + k * (id != 0) + j] += start; 
                nearest_idxs[3 * i * k + k * (id != 0) + j] *= d;
            }
            
            if(id != 0){
                printf("%d %d\n", i, id);
                kselect(shortest_distances + 3 * i * k, 0, k + nfound - 1, k, shortest_distances + 3 * i * k + 2 * k, nearest_idxs + 3 * i * k + 2 * k);
                printf("%d %d\n", i, id);
                cilk_for(int j = 0; j < k; j++){
                    nearest_idxs[3 * i * k + j] = nearest_idxs[3 * i * k + 2 * k + j];
                    shortest_distances[3 * i * k + j] = shortest_distances[3 * i * k + 2 * k + j];
                }
                
            }
        }
        id ++;
    }

    free(normx);
    free(normy);

    free(D);
    

    knn = init_knnresult(m, k);
    for(int i = 0; i < m; i++){
        for(int j = 0; j < k; j++){
            knn.nidx[i * k + j] = nearest_idxs[3 * i * k + j];
            knn.ndist[i * k + j] = shortest_distances[3 * i * k + j];
        }
    }
    printf("%d\n", id);    
    free(nearest_idxs);
    free(shortest_distances);

    return knn;
}

int main(int argc, char *argv[]){
    
    int n = 1024 * 2, m = 1, k = 1, d = 1;

    double *X = (double *) malloc(sizeof(double) * m * d);
    double *Y = (double *) malloc(sizeof(double) * n * d);

    if(argc < 2){
        randarr(X, m * d, 5.0, -5.0);
        randarr(Y, n * d, 2.0, -2.0);
    }
    else if(argc == 2){
        randarr(X, m * d, 5.0, -5.0);
        randarr(Y, n * d, 2.0, -2.0);
    }
    else{
        fprintf(stderr, "Wrong number of inputs, expected 0 or 1 but got %d.\nInput a filename containing the data or nothing for a random example.\n", argc - 1);
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
    
    
    print_arri(knn.nidx, m, k);
    print_arrd(knn.ndist, m, k);
    printf("Time is %lf\n", elapsed_time);

    free_knnresult(knn);
    free(X);
    free(Y);
    return 0;
}