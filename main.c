#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[]){

    int m = 3;
    int n = 2;
    int d = 1;
    
    int k = 1;

    double *X = malloc(m * d * sizeof(double));
    double *Y = malloc(n * d * sizeof(double));
    
    srand(time(NULL));

    for(int i = 0; i < m; i++){
        for(int j = 0; j < d; j++){
            X[i * d + j] = ((int) rand()) % 10 * 0.2;
        }
    }

    for(int i = 0; i < n; i++){
        for(int j = 0; j < d; j++){
            Y[i * d + j] = ((int) rand()) % 10 * 0.2;
        }
    }

    printf("Query X:\n");
    print_arr(X, m, d);

    printf("Corpus Y:\n");
    print_arr(Y, n, d);

    knnresult knn = kNN(X, Y, n, m, d, k);

    free_knnresults(knn);
    free(X);
    free(Y);
    return 0;
}