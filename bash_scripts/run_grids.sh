#!/bin/bash

make all

for f in $(ls inputs/ | grep ".txt")
do

bin/knn inputs/$f results/V0_$f

done

