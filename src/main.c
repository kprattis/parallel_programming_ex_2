#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

int BLOCKSIZE;

int main(int argc, char *argv[]){

    srand(time(NULL));

    int m = 5;
    int n = 10000;
    int d = 3;

    int k = 5;

    BLOCKSIZE = n/10;

    double start[] = {-2.0, -2.0, -2.0};
    double end[] = {2.0, 2.0, 2.0};
    double step[] = {1.0, 1.0, 0.5};

    double *X = regular_grid(d, start, end, step, &m);//malloc(m * d * sizeof(double));
    
    n = m;
    printf("%d %d %d\n",m, n, d);
    double *Y = X;
    
    //print_arr(X, m, d, DOUBLE);
    //init X, Y
    //randarr(X, m, d);
    //randarr(Y, n, d);

    
    //print arrays
    /*
    printf("Query X:\n");
    print_arr(X, m, d, DOUBLE);

    printf("Corpus Y:\n");
    print_arr(Y, n, d, DOUBLE);
    */
    knnresult knn;
    
    struct timeval start_time, end_time;
    double elapsed_time;

    gettimeofday(&start_time, NULL);
    knn = kNN(X, Y, n, m, d, k);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    printf("time: %lf\n", elapsed_time);
    //print results
    print_results(knn);
    
    free_knnresult(knn);
    free(X);
    //free(Y);
    return 0;
}