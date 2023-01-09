#include <stdio.h>

void regular_grid(FILE *f, int dim, double start[], double end[], double step[], int *n_points){
    
    int *dim_size = (int *) malloc(dim * sizeof(int));
    int total_size = 1;

    for(int d = 0; d < dim; d++){
        dim_size[d] = (int) (end[d] - start[d])/step[d] + 1;
        total_size = dim_size[d] * total_size;
    }

    *n_points = total_size;

    int index;
    
    for(int i = 0; i < total_size; i++){
        index = i;
        for(int d = 0; d < dim; d++){
            fprintf(f, "%lf ",(index % dim_size[d]) * step[d] + start[d]);
            index = index / dim_size[d];
        }
        fprintf(f, "\n");
    }

    free(dim_size);
}

int main(int argc, char *argv[]){
    FILE * f = fopen("input.txt", "w");
    
    double start[] = {1, 1, 1};
    double end[] = {10, 10, 10};
    double step[] = {1, 1, 1};

    int n;

    regular_grid(f, 3, start, end, step, &n);

    fclose(f);
    return 0;
}