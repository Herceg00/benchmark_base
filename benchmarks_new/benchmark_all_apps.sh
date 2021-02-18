#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="2000000"
MTX_SIZE="512"
GRAPH_MIN_SIZE="5"
GRAPH_MAX_SIZE="8"

bash ./collect_common_stats.sh init
bash ./collect_metrics.sh init "single_socket"
#bash ./collect_metrics.sh init "dual_socket"
bash ./collect_roofline.sh init "single_socket"
#bash ./collect_roofline.sh init "dual_socket"

##################### TRIAD ########################

for ((mode=0;mode<=15;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
done

##################### STENCIL ########################

for ((radius=3;radius<=6;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius
    name="stencil_1D|S="$LINEAR_SIZE"|R="$radius"|"
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
done

##################### MAT_MUL ########################

for ((mode=0;mode<=5;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_mult|S="$MTX_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name"
done

##################### MAT_TRANSPOSAL ########################

for ((mode=0;mode<=3;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_transp|S="$MTX_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
done

##################### BELLMAN_FORD ########################
mode=0
for ((size=$GRAPH_MIN_SIZE;size<=$GRAPH_MAX_SIZE;size++)); do
    args="--length="$size
    name="bellman_ford|S="$size"|"
    bash ./benchmark_specific_app.sh "bellman_ford" "$args" "$name"
done

#####################GAUSS ########################
mode=0
args="--length="$MTX_SIZE
name="gauss|S="$MTX_SIZE"|"
bash ./benchmark_specific_app.sh "gauss" "$args" "$name"

#####################RAND_GENERATOR ########################

args="--length="$LINEAR_SIZE
name="rand_generator|S="$LINEAR_SIZE"|"
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

################### generate roofline ######################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


