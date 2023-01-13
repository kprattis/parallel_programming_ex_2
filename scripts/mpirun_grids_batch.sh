#!/bin/bash

mkdir -p inputs
mkdir -p results
mkdir -p slurm_out

for f in $(ls inputs/ | grep ".txt")
do

sbatch -o slurm_out/res_$f scripts/mpi_batch.sh inputs/$f results/V1_$f

done
