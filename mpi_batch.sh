#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=2
#SBATCH --nodes=2
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=5

srun -n 4 bin/knn_mpi