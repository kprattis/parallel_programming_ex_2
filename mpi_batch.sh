#!/bin/bash
#SBATCH --partition=batch
#SBATCH --ntasks-per-node=1
#SBATCH --nodes=1
#SBATCH --time=0:01:00
#SBATCH --cpus-per-task=12 

srun bin/knn_mpi