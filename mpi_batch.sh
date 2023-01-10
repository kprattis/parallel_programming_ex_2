#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=3
#SBATCH --nodes=4
#SBATCH --time=0:01:00
#SBATCH --cpus-per-task=5

srun -n 4 bin/knn_mpi