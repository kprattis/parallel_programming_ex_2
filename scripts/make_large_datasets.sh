#!/bin/bash

dims=(1024 860 500 250 50 )
sizes=(100 )

for d in "${dims[@]}"
do
    for s in "${sizes[@]}"
    do
        
        echo bin/reg_grid inputs/"rg${d}d${s}".txt $s $d 60000

        bin/reg_grid inputs/"biggrid${d}d600000".txt ${s} ${d} 60000

        echo created inputs/"rg${d}d${s}".txt
    done
done
