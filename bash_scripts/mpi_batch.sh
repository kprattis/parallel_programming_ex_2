#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=2
#SBATCH --nodes=2
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=5

module load gcc/9.4.0 openmpi/4.1.2
module load openblas
module load OpenCilk

srun bin/knn_mpi inputs/rg3d500.txt results/V1_rg3d500.txt