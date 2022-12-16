#include "knn.h"

#define MAXBLOCKSIZE sizeof(int) * 64 

knnresult kNN(double * X, double * Y, int n, int m, int d, int k){
    
    int size = n * m ;
    int bsize = (size < MAXBLOCKSIZE) ? size : MAXBLOCKSIZE

    
    double **dist; //!< Distances for every query point-corpus point combination [m-by-n]



    knnresult knn;

    
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n;j++)
            dist[i][j] = euclidean_dist(X[i], Y[j], d);

    for(int i = 0; i < m; i++)
        for(int j = 0; j < m; j++){}
            //....            

}