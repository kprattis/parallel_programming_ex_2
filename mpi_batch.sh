#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=4
#SBATCH --nodes=1
#SBATCH --time=00:01:00
#SBATCH --cpus-per-task=5

srun -n 4 bin/knn_mpi