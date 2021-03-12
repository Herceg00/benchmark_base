#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="800000000" # 6.4 GB in double, 3.2 GB in float

rm -rf ./output/metrics.csv
bash ./collect_common_stats.sh init
bash ./collect_roofline.sh init "single_socket"

##################### cache benchmarks ########################

length=80000000
radius=5
args="--length="$LINEAR_SIZE" --radius="$radius
name="stencil_1D|S="$LINEAR_SIZE"|R="$radius"|"
bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name" "profile"


length=80000000
mode=0
args="--length="$length" --lower_bound="$mode" --higher_bound="$mode
name="triada|S="$length"|M="$mode"|"
bash ./benchmark_specific_app.sh "triada" "$args" "$name" "profile"


mode=0
mtx_size=1024
args="--length="$mtx_size" --lower_bound="$mode" --higher_bound="$mode
name="matrix_transp|S="$mtx_size"|M="$mode"|"
bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name" "profile"


##################### Roofline ########################
python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt