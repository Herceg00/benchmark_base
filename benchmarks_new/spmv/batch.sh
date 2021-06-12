#!/bin/bash
export OMP_NUM_THREADS=64
export OMP_PLACES=cores
export OMP_PROC_BIND=close
./omp_triada_np_STD