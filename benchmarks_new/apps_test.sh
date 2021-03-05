#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="800000000" # 6.4 GB in double, 3.2 GB in float
MIN_MTX_SIZE="2048"
MAX_MTX_SIZE="8192"
MIN_MTX_TRANSPOSE_SIZE="512"
MAX_MTX_TRANSPOSE_SIZE="32768"
GRAPH_MIN_SIZE="6"
GRAPH_MAX_SIZE="12"
STENCIL_MIN_RAD="3"
STENCIL_MAX_RAD="10"
LC_MIN_SIZE="32"
LC_MAX_SIZE="128"
FFT_SIZE="8192"

#sizes in KB
RA_RADIUS="2" # 2 KB
RA_MAX_RAD="2097152" # 2 GB

bash ./collect_common_stats.sh init
bash ./collect_metrics.sh init "single_socket"
#bash ./collect_metrics.sh init "dual_socket"
bash ./collect_roofline.sh init "single_socket"
#bash ./collect_roofline.sh init "dual_socket"

function add_separator {
    bash ./collect_common_stats.sh add_separating_line
    bash ./collect_metrics.sh add_separating_line "single_socket"
}

##################### RANDOM_ACCESS ########################

mode=0 #storage
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

mode=1 #reduction
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

for ((mode=0;mode<=1;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
done

#for ((size=$RA_RADIUS;size<=$RA_MAX_RAD;size*=2 )); do
#  args="--length="$LINEAR_SIZE" --radius="$size
#  name="rec_fft|R="$size
#  bash ./benchmark_specific_app.sh "random_access" "$args" "$name"
#done

add_separator

##################### Roofline ########################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


