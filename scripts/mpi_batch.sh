#!/bin/bash
#SBATCH --partition=rome
#SBATCH --ntasks-per-node=2
#SBATCH --nodes=2
#SBATCH --time=00:00:30
#SBATCH --cpus-per-task=5

IFILE=inputs/rg3d500.txt
OFILE=results/V1_rg3d500.txt

srun bin/knn_mpi $IFILE $OFILE

