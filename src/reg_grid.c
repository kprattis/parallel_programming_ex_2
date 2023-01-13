#include <stdio.h>
#include <math.h>
#include <stdlib.h>

/*
Create a (square) regular grid in d dimensions of size N per dimension 
*/

void regular_grid(FILE *f, int dim, double start, double end, double step, int stop){
    
    int total_size = 1;
    int dim_size = (int) (end - start)/step + 1;

    if(stop != 0){
        total_size = stop;
    }
    else{
        total_size = pow(dim_size, dim);
    }


    int index;
    
    for(int i = 0; i < total_size; i++){
        index = i;
        for(int d = 0; d < dim; d++){
            fprintf(f, "%lf ",(index % dim_size) * step + start);
            index = index / dim_size;
        }
        fprintf(f, "\n");
        if(i > stop){
            break;
        }
    }

}

int main(int argc, char *argv[]){
    

    if(argc < 4 || argc > 5){
        printf("Right Usage: bin/reg_grid filename N d [maxnumpoints]\n");
        printf("E.g. to make a 10x10x10 regular grid use: bin/reg_grid inputs/reg3d10.txt 10 3\n");
        exit(1);
    }
    
    FILE *f = fopen(argv[1], "w");
    int stop = 0;
    int k;
    int d = atoi(argv[3]);
    int dimsize = atoi(argv[2]);
    int n;
    
    if(argc > 4){
        stop = atoi(argv[4]);
        k = 5;
        n = stop;
    }
    else{
        k = pow(3, d);
        n = pow(dimsize, d);
    }

    fprintf(f, "%d %d %d\n", n, d, k);
    regular_grid(f, d, 1.0, dimsize, 1.0, stop);

    fclose(f);
    return 0;
}