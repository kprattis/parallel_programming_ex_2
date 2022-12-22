#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

int BLOCKSIZE;

int main(int argc, char *argv[]){

    int m = 5;
    int n = 10000;
    int d = 3;

    int k = 5;
    int rows, cols;

    BLOCKSIZE = n;
    char filename[] = "assets/t10k-images.idx3-ubyte";

    double *X = read_MNIST_images(filename, &m, &rows, &cols);
    d = rows * cols;

    printf("%d %d %d \n", m, d, m*d);
    
    double start[] = {-2.0, -2.0, -2.0};
    double end[] = {2.0, 2.0, 2.0};
    double step[] = {0.1, 0.1, 0.1};
    
    //double *X = regular_grid(d, start, end, step, &m);//malloc(m * d * sizeof(double));
    n = m;
    printf("%d %d %d\n",m, n, d);
    double *Y = X;
    
    //print_arr(X, m, d, DOUBLE);
    //init X, Y 
    //randarr(X, m, d);
    //randarr(Y, n, d);

    //save_image(X, rows, cols, "../Images/img1.txt");
    
    //print arrays

    printf("Query X:\n");
    //print_arr(X, m, d, DOUBLE);

    printf("Corpus Y:\n");
    //print_arr(Y, n, d, DOUBLE);
    
    knnresult knn;
    
    struct timeval start_time, end_time;
    double elapsed_time;

    gettimeofday(&start_time, NULL);
    knn = kNN(X, Y, n, m, d, k);
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;

    char *name = (char *)malloc(sizeof(char) * 20);
    for(int i = 0; i < k; i++){
        sprintf(name, "../Images/img%d.txt", i+1);
        save_image(X + knn.nidx[i] * d, rows, cols, name);
    }


    //print results
    print_results(knn, false);
    printf("time: %lf\n", elapsed_time);

    free_knnresult(knn);
    
    free(X);
    //free(Y);
    return 0;
}