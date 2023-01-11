#!/bin/bash

for f in $(ls inputs/ | grep ".txt")
do

sbatch scripts/mpi_batch.sh IFILE=inputs/$f OFILE=results/V1_$f

done