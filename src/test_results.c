#include <stdio.h>
#include <math.h>
#include <stdlib.h>

int test_result(FILE *fin, FILE *fout){
    int iscorrect = 0;
    
    //scan the size and dimension
    int N, d;
    fscanf("%d %d\n", &N, &d);

    //stores the points
    double *p = (double *)malloc(d * sizeof(double));

    int k = pow(3, d);

    double *D = (double *)malloc(k * sizeof(double));
    int *idx = (int *)malloc(k * sizeof(int));


    double *X = (double *)malloc(d * sizeof(double));
    
    return iscorrect;
}

int main(int argc, char *argv[]){

    if(argc != 3){
        printf("Wrong usage. Input the filenames as arguments and then the number of total points and the number of dimensions e.g. : inputs/rg3d1000.txt results/res.txt\n");
        exit(1);
    }

    FILE *fin, *fout;
    if( (fin = fopen(argv[1], "r")) == NULL){
        printf("ERROR: Could not open file %s\n", argv[1]);
        exit(1);
    }
    if( (fout = fopen(argv[2], "r")) == NULL){
        printf("ERROR: Could not open file %s\n", argv[2]);
        exit(1);
    }

    if(test_result(fin, fout))
        printf("File %s contains the right knn for file %s\n", argv[2], argv[1]);
    else
        printf("File %s does NOT contain the right knn for file %s\n", argv[2], argv[1]);

    return 0;
}