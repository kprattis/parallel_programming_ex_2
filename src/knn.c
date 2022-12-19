#include "knn.h"
#include <stdio.h>
#include <stdlib.h>
#include "cblas.h"

knnresult kNN(double *X, double *Y, int n, int m, int d, int k){
        
    double *dist = (double *) malloc(m * BLOCKSIZE * sizeof(double)); // [m by Bloocksize]

    knnresult knn = init_knnresult(k, m);

    int startid, endid, dist_size;
    //block matrix Y
    for(int b = 0; b < n; b += BLOCKSIZE){
        startid = b;
        endid = min(b + BLOCKSIZE, n);
        dist_size = endid - startid;

        //Create X o X vector
        for(int i = 1; i < m; i++){
            dist[i * dist_size] = euclidean_norm(X + i*d, d);
        }

        //Create Y o Y vector
        for(int i = 1; i < dist_size; i++){
            dist[i] = euclidean_norm(Y + (i + startid)*d, d);
        }
        //printf("distane matrix is: \n");
        //print_arr(dist, m, dist_size, DOUBLE);

        // Copy XoX to all columns and YoY to all rows of dist
        for(int i = 1; i < m; i++){
            for(int j = 1; j < dist_size; j++){
                dist[i * dist_size + j] = dist[j] + dist[i * dist_size];  
            }
        }

        //Fix first row and column
        dist[0] = euclidean_norm(Y + startid, d);
        
        for(int i = 1; i < m; i++){
            dist[i * dist_size] = dist[0]; 
        }

        double temp = dist[0];
        dist[0] = euclidean_norm(X, d);
        
        for(int i = 1; i < dist_size; i++){
            dist[i] += dist[0];
        }
        dist[0] += temp;
        // End of copy section

        printf("distane matrix is: \n");
        print_arr(dist, m, dist_size, DOUBLE);

        cblas_dgemm(CblasRowMajor, CblasNoTrans,
                    CblasTrans, m, dist_size,
                    d, -2.0, X,
                    d, Y + startid, d, 1.0 , dist, dist_size);

        //printf("distane matrix is: \n");
        //print_arr(dist, m, dist_size, DOUBLE);

        shift_select(&knn, dist, m, dist_size, k);
    }

    free(dist);

    return knn;
}