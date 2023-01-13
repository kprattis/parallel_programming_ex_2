#!/bin/bash

dims=( 2 3 )
sizes=(10 25 50 75 100 )

for d in "${dims[@]}"
do
    for s in "${sizes[@]}"
    do
        
        echo bin/reg_grid inputs/"rg${d}d${s}".txt $s $d

        bin/reg_grid inputs/"rg${d}d${s}".txt ${s} ${d}

        echo created inputs/"rg${d}d${s}".txt
    done
done
