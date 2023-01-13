#!/bin/bash

mkdir -p inputs
mkdir -p results

dims=(1024 860 500 250 50 )
sizes=(100 )
N=60000

for d in "${dims[@]}"
do
    for s in "${sizes[@]}"
    do
        
        echo bin/reg_grid inputs/"bigrid${d}d${N}".txt $s $d ${N}

        bin/reg_grid inputs/"bigrid${d}d${N}".txt ${s} ${d} ${N}

        echo created inputs/"rg${d}d${N}".txt
    done
done
