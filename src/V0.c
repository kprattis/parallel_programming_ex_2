#include "knn.h"

#include <stdio.h> //File handling, print
#include <cblas.h> //cblas_dgemm
#include <stdlib.h> //malloc, free
#include <sys/time.h> //execution time
#include <cilk/cilk_api.h> //number of cilk workers
#include <cilk/cilk.h> //cilk for

//Max size of array D
#define MAXSIZE 1024 * 1024 * 128l

knnresult kNN(double *X, double *Y, int n, int m, int d, int k){

    //Here set the blocksize, which is the reduced size of the dimension n of the distance matrix
    long BLOCKSIZE = n;
    if ( (long) m * n > MAXSIZE){
      BLOCKSIZE = min( (long) m * n / 4, MAXSIZE);
      BLOCKSIZE /= m;   
    }

    printf("Distance Matrix Allocation => corpus Blocksize = %ld. Calculating %d nearest neighbors...\n", BLOCKSIZE, k);
    if(BLOCKSIZE < k){
        printf("Error: too large sizes m = %d, n = %d or Blocksize < k.\n", m, n);
        exit(1);
    }
    //-------------------------End of Blocksize set-------------------------------

    //-------------------Initialize memory----------------------------------
    //Distance array
    double *D = (double *) malloc(m * BLOCKSIZE * sizeof(double));

    //store the norm of each query point
    double *normx = (double *) malloc(m * sizeof(double));
    cilk_for(int i = 0; i < m; i++)
        normx[i] = euclidean_norm(X + i * d, d);

    //here store the norm of the current corpus block
    double *normy = (double *) malloc(BLOCKSIZE * sizeof(double));

    //the knnresult
    knnresult knn = init_knnresult(m, k);
    //-------------------End of Memory Init--------------------------------

    int start, end, size;
    for(int b = 0; b < n; b += BLOCKSIZE){
        
        //Current blocks' corpus points start, end and size
        start = b;
        end = min(n, b + BLOCKSIZE);
        size = end - start;

        //calculate euclidean norm
        cilk_for(int i = 0; i < size; i++)
            normy[i] = euclidean_norm(Y + (i + start) * d, d);

        //init the array with the sum of the appropriate norms in each cell
        cilk_for(int i = 0; i < m; i++)
            cilk_for(int j = 0; j < size; j++){
                D[i * size + j] = normx[i] + normy[j];
            }

        //Calculate and add to D the product -2X*Y(t)
        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasTrans, m, size, d, -2.0, X, d, Y + start * d, d, 1.0 , D, size);

        //select the k nearest neighbors for every query point. If b == 0 => first iteration choose from scratch, else renew the ndist and nidx.
        cilk_for(int i = 0; i < m; i++)
            kselect(D + i * size, 0, size - 1, k, knn.ndist + i * k, knn.nidx + i * k, (b == 0), start);
    
    }

    //free the allocated memory
    free(normx);
    free(normy);
    free(D);

    return knn;
}

int main(int argc, char *argv[]){
    
    int n, m, k, d;
    double *X, *Y;

    printf("Cilk workers used : %d\n", __cilkrts_get_nworkers());

    //---------------------------Argument Control Start------------------------
    if(argc == 3){
        FILE *f = fopen(argv[1], "r");
        if(f == NULL){
            printf("Error: Could not open file %s\n", argv[1]);
        }
        printf("Processing file %s\n", argv[1]);
        
        //--------------------------Read FILE-----------------
        fscanf(f, "%d %d %d\n", &n, &d, &k);
        m = n;

        X = (double *) malloc(sizeof(double) * m * d);
        
        for(int i = 0; i < m; i++){
            for(int j = 0; j < d; j++){
                fscanf(f, "%lf ", X + i * d + j);
            }
            fscanf(f, "\n");
        }
        
        fclose(f);
        //--------------------------Read FILE------------------

        //for the grids example that we test, set Y to point to X
        Y = X;
    }
    else{
        printf("Wrong input: Given %d arguments. Right usage: bin/knn inputs/infile.txt outputs/outfile.txt\n", argc);
        exit(1);
    }
    //---------------------------Argument Control End------------------------
    
    //Calculate knn and the execution time
    struct timeval start_time, end_time;
    double elapsed_time;
    
    gettimeofday(&start_time, NULL);
        knnresult knn = kNN(X, Y, n, m, d, k);
        
    gettimeofday(&end_time, NULL);
    elapsed_time = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_usec - start_time.tv_usec) / 1000000.0;
    
    printf("Execution Time, %lf\n", elapsed_time);

    //-----------------------------Write Results-------------------------------
    FILE *f = fopen(argv[2], "w");
    if(f == NULL){
        printf("Error, could not open file %s\n", argv[2]);
        exit(1);
    }

    fprintf(f, "Execution Time : %lf, cilk workers: %d\n", elapsed_time, __cilkrts_get_nworkers());

    fprint_arrd(f, knn.ndist, m, k);

    fprintf(f, "\n");

    fprint_arri(f, knn.nidx, m, k);

    fclose(f);
    //--------------------------End of Write Results-----------------------------------


    //free the allocated memory
    free_knnresult(knn);
    free(X);
    
    return 0;
}