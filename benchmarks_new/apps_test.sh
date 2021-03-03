#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="2000000"
MIN_MTX_SIZE="1024"
MAX_MTX_SIZE="1024"
GRAPH_MIN_SIZE="6"
GRAPH_MAX_SIZE="12"
STENCIL_MIN_RAD="3"
STENCIL_MAX_RAD="10"
LC_MIN_SIZE="32"
LC_MAX_SIZE="128"
FFT_SIZE="8192"

bash ./collect_common_stats.sh init
bash ./collect_metrics.sh init "single_socket"
#bash ./collect_metrics.sh init "dual_socket"
bash ./collect_roofline.sh init "single_socket"
#bash ./collect_roofline.sh init "dual_socket"

function add_separator {
    bash ./collect_common_stats.sh add_separating_line
    bash ./collect_metrics.sh add_separating_line "single_socket"
}

for (( MTX_SIZE=MIN_MTX_SIZE; MTX_SIZE<=MAX_MTX_SIZE; MTX_SIZE*=2 )); do
    for ((mode=0;mode<=3;mode++)); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="matrix_transp|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
    done

    add_separator
done

add_separator

##################### Roofline ########################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


