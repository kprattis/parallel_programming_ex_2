#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=2
#SBATCH --nodes=2
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=5

module load openblas
module load OpenCilk

export OMPI_CC=clang

make mpi CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib
make V0 CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib

srun bin/knn_mpi inputs/rg3d500.txt results/V1_rg3d500.txt

