#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=20
#SBATCH --nodes=3
#SBATCH --time=00:10:00
#SBATCH --cpus-per-task=2

srun bin/knn_mpi $1 $2

