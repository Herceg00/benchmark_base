#!/bin/bash

for cores in {1..64};
do
    #echo "Used $cores cores"
    export OMP_NUM_THREADS=$cores
    export OMP_PROC_BIND=true
    export OMP_PROC_BIND=close
    ./latest.out
done