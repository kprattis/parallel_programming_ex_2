#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=3
#SBATCH --nodes=1
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=5

srun -n 4 bin/knn_mpi