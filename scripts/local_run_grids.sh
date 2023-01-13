#!/bin/bash

for f in $(ls inputs/ | grep ".txt")
do

make mpirun IFILE=inputs/$f OFILE=results/V1_$f
bin/knn inputs/$f results/V0_$f


done

