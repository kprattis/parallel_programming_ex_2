#!/bin/bash

dims=(1 2 3 4 5 )
sizes=(10 50 100 200 )

for d in "${dims[@]}"
do
    for s in "${sizes[@]}"
    do
        
        echo bin/reg_grid inputs/"rg${d}d${s}".txt $s $d

        bin/reg_grid inputs/"rg${d}d${s}".txt ${s} ${d}

        echo created inputs/"rg${d}d${s}".txt
    done
done
