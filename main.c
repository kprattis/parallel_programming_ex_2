#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int main(int argc, char *argv[]){

    int m = 2;
    int n = 5;
    int d = 1;
    
    int k = 3;

    double *X = malloc(m * d * sizeof(double));
    double *Y = malloc(n * d * sizeof(double));
    
    //init X, Y
    srand(time(NULL));
    for(int i = 0; i < m; i++){
        for(int j = 0; j < d; j++){
            X[i * d + j] = ((int) rand()) % 10 * 1.0;
        }
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < d; j++){
            Y[i * d + j] = ((int) rand()) % 10 * 1.0;
        }
    }

    printf("Query X:\n");
    print_arr(X, m, d, DOUBLE);

    printf("Corpus Y:\n");
    print_arr(Y, n, d, DOUBLE);

    knnresult knn = kNN(X, Y, n, m, d, k);

    printf("The %d nn ids for each query point are:\n", k);
    print_arr(knn.nidx, m, k, INT);

    printf("With distances:\n");
    print_arr(knn.ndist, m, k, DOUBLE);
    

    free_knnresult(knn);
    free(X);
    free(Y);
    return 0;
}