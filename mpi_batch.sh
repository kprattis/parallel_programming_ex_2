#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=4
#SBATCH --time=0:01:00
#SBATCH --cpus-per-task=2 

module load gcc/9.4.0 openmpi/4.1.2
module load openblas
module load OpenCilk

export OMPI_CC=clang

make mpi CC=clang HPCBLASINC="$OPENBLAS_ROOT/include" LBLAS=$OPENBLAS_ROOT/lib

srun -n 4 bin/knn_mpi