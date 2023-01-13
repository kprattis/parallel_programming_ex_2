#include <stdio.h>
#include <math.h>
#include <stdlib.h>


/*
This program performs a test for the knn results of V0 and V1. 
It is designed to verify that in a 2d 3d (and 4d) regular grid 
the inside points have been calculated correctly.
*/

int factorial(int a){
    if(a == 0)
        return 1;
    int f = 1;
    for(int i = 1; i <= a; i++){
        f = f * i;
    }
    return f;
}

int *id2coords(int id, int d, int N){
    int *dimvals = (int *) malloc(sizeof(int) * d);

    for(int i = 0; i < d; i++){
        dimvals[i] = (id % N) + 1;
        id = id / N;
    }

    return dimvals;
}

int distance(int p, int q, int N, int d){
    int n = round(pow(N, 1.0/d));
    int *coordsp = id2coords(p, d, n);
    int *coordsq = id2coords(q, d, n);

    int dist = 0;
    for(int i = 0; i < d; i++){
        dist += (coordsp[i] - coordsq[i]) * (coordsp[i] - coordsq[i]);
    }

    free(coordsp);
    free(coordsq);
    return dist;
}

int isexternal(int id, int N, int d){
    
    int n = round(pow(N, 1.0/d));
    int isexternal = 0;
    int *dimvals = id2coords(id, d, n);

    for(int i = 0; i < d; i ++){
        if(dimvals[i] == n || dimvals[i] == 1)
            isexternal = 1;
    }

    free(dimvals);
    return isexternal;
}

int test_result(FILE *fin, FILE *fout){
    int iscorrect = 1;
    
    //scan the size and dimension
    int N, d;
    int idx, count;

    double temp;
    fscanf(fin, "%d %d\n", &N, &d);
    char c = ' ';
    while(c != '\n'){
        fscanf(fout, "%c", &c);
    }

    //stores the points
    int *fact = (int *)malloc((d + 1) * sizeof(int));
    int *comb = (int *)malloc((d + 1) * sizeof(int));

    for(int i = 0; i <= d; i++)
        fact[i] = factorial(i);
    for(int i = 0; i <=d; i++)
        comb[i] = fact[d] / (fact[d - i] * fact[i]);

    free(fact);

    int k = pow(3, d);

    double *D = (double *)malloc(k * sizeof(double));

    for(int p = 0; p < N; p++){
        
        for(int i = 0; i < k; i++){
            fscanf(fout, "%lf ", D + i);
        }
        fscanf(fout, "\n");

        if(isexternal(p, N, d))
            continue;

        //sort distances
        for(int i = 0; i < k - 1; i++){
            for(int j = i + 1; j < k; j++){
                if(D[j] < D[i]){
                    temp = D[j]; 
                    D[j] = D[i];
                    D[i] = temp;
                }
            }
        }

        //check
        idx = 0;
        for(int i = 0; i <= d; i++){
            count = 0;
            while(  (idx < k) && (((int) D[idx]) == i)){
                count ++;
                idx ++;
            }
            if(count != (int) (pow(2, i) * comb[i])){
                printf("Found %d of distance %d instead of %d at point %d\n", count, i, (int) pow(2, i) * comb[i], p);
                return 0;
            }

        }
    }
    printf("Distances checked ok\n");

    free(comb);
    free(D);  

    int *idxs = (int *) malloc(sizeof(int) * k);
    int dist;

    fscanf(fout, "\n");
    for(int p = 0; p < N; p++){
        for(int i = 0; i < k; i++){
            fscanf(fout, "%d ", idxs + i);
        }
        fscanf(fout, "\n");

        if(isexternal(p, N, d))
            continue;

        for(int i = 0; i < k; i++){
            dist = distance(idxs[i], p, N, d);
            if(dist > d){
                printf("Found that %d does not have %d as a knn since their distance squared is %d\n", p, idxs[i], dist);
                return 0;
            }
        }

    }
    
    printf("Indexes checked ok\n");
    free(idxs);
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

    printf("-----------------------------------START OF TEST--------------------------------------------------\n");
    printf("Testing only the inside points of the grid\n");
    if(test_result(fin, fout))
        printf("File %s contains the right knn for file %s\n", argv[2], argv[1]);
    else
       printf("File %s does NOT contain the right knn for file %s\n", argv[2], argv[1]);
    printf("-------------------------------------END OF TEST--------------------------------------------------\n");

    fclose(fin);
    fclose(fout);
    return 0;
}