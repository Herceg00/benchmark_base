#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="2000000"
MTX_SIZE="512"
GRAPH_MIN_SIZE="6"
GRAPH_MAX_SIZE="12"
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

##################### LC ########################
for ((size=$LC_MIN_SIZE;size<=$LC_MAX_SIZE;size*=2 )); do
    args="--length="$size
    name="lc_kernel|S="$size"|"
    bash ./benchmark_specific_app.sh "lc_kernel" "$args" "$name"
done

add_separator

##################### FFT ########################

args="--length="$FFT_SIZE
name="rec_fft|S="$FFT_SIZE"|"
bash ./benchmark_specific_app.sh "rec_fft" "$args" "$name"

add_separator

##################### Roofline ########################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


