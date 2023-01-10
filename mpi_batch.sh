#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --time=0:01:00
#SBATCH --cpus-per-task=12 

module load gcc/9.4.0 openmpi/4.1.2
module load openblas
module load OpenCilk

export OMPI_CC=gcc

git clone https://github.com/kprattis/parallel_programming_ex_2
cd parallel_programming_ex_2

make mpi CC=clang HPCFLAGS=-Ι$OPENBLAS_ROOT/include

srun bin/knn_mpi

cd ..

#rm -r parallel_programming_ex_2
#rmdir parallel_programming_ex_2