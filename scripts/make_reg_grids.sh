#!/bin/bash

dims=(1 2 3 4)
sizes=(10 50 100 500)

for d in "${dims[@]}"
do
    for s in "${sizes[@]}"
    do
        
        bin/reg_grid inputs/"rg${d}d${s}".txt $s $d

        echo created inputs/"rg${d}d${s}".txt
    done
done
