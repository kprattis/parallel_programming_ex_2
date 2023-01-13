#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=4
#SBATCH --nodes=3
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=12

srun bin/knn_mpi $1 $2

