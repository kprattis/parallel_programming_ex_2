#!/bin/bash

mkdir -p slurm_out

for f in $(ls inputs/ | grep ".txt")
do

sbatch -o slurm_out.txt scripts/mpi_batch.sh IFILE=inputs/$f OFILE=results/V1_$f

done
