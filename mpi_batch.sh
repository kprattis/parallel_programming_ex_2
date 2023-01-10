#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=4
#SBATCH --time=0:01:00
#SBATCH --cpus-per-task=2 

srun -n 4 bin/knn_mpi