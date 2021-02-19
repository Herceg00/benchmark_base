#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="2000000"
MTX_SIZE="512"
GRAPH_MIN_SIZE="6"
GRAPH_MAX_SIZE="12"

bash ./collect_common_stats.sh init
bash ./collect_metrics.sh init "single_socket"
#bash ./collect_metrics.sh init "dual_socket"
bash ./collect_roofline.sh init "single_socket"
#bash ./collect_roofline.sh init "dual_socket"

function add_separator {
    bash ./collect_common_stats.sh add_separating_line
    bash ./collect_metrics.sh add_separating_line "single_socket"
}

##################### BELLMAN_FORD ########################
mode=0 # parallel
for ((size=$GRAPH_MIN_SIZE;size<=$GRAPH_MAX_SIZE;size++)); do
    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
    name="bellman_ford|S="$size"|M="$mode"|"
    bash ./benchmark_specific_app.sh "bellman_ford" "$args" "$name"
done

add_separator

mode=1 # sequential
for ((size=$GRAPH_MIN_SIZE;size<="9";size++)); do # upper bound is different since this algorithm is long
    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
    name="bellman_ford|S="$size"|M="$mode"|"
    bash ./benchmark_specific_app.sh "bellman_ford" "$args" "$name"
done

add_separator

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


