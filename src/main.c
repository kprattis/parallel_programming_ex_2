#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int BLOCKSIZE;

int main(int argc, char *argv[]){

    srand(time(NULL));

    int m = 2;
    int n = 4;
    int d = 1;

    int k = 1;

    BLOCKSIZE = n;


    double *X = malloc(m * d * sizeof(double));
    double *Y = malloc(n * d * sizeof(double));
    
    //init X, Y
    randarr(X, m, d);
    randarr(Y, n, d);

    
    //print arrays
    printf("Query X:\n");
    print_arr(X, m, d, DOUBLE);

    printf("Corpus Y:\n");
    print_arr(Y, n, d, DOUBLE);
    
    knnresult knn = kNN(X, Y, n, m, d, k);

    //print results
    //print_results(knn);
    
    free_knnresult(knn);
    free(X);
    free(Y);
    return 0;
}