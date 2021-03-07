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
STENCIL_MIN_RAD="1"
STENCIL_MAX_RAD="12"
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

#################### TRIAD ########################

for ((mode=0;mode<=15;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
done

add_separator

##################### STENCIL 1D ########################

for ((radius=STENCIL_MIN_RAD;radius<=STENCIL_MAX_RAD;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius
    name="stencil_1D|S="$LINEAR_SIZE"|R="$radius"|"
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
done

add_separator

##################### STENCIL 2D ########################

for ((mode=0;mode<=1;mode++)); do
    for (( MTX_SIZE=MIN_MTX_TRANSPOSE_SIZE; MTX_SIZE<=MAX_MTX_TRANSPOSE_SIZE; MTX_SIZE*=2 )); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="stencil_2D|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
    done

    add_separator
done

add_separator

##################### MAT_MUL ########################

for (( MTX_SIZE=MIN_MTX_SIZE; MTX_SIZE<=MAX_MTX_SIZE; MTX_SIZE*=2 )); do
    for ((mode=0;mode<=5;mode++)); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="matrix_mult|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name"
    done

    add_separator
done

add_separator

##################### MAT_TRANSPOSAL ########################
for ((mode=0;mode<=3;mode++)); do
    for (( MTX_SIZE=MIN_MTX_TRANSPOSE_SIZE; MTX_SIZE<=MAX_MTX_TRANSPOSE_SIZE; MTX_SIZE*=2 )); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="matrix_transp|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
    done

    add_separator
done

add_separator

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

#####################GAUSS ########################

for (( MTX_SIZE=MIN_MTX_SIZE; MTX_SIZE<=MAX_MTX_SIZE; MTX_SIZE*=2 )); do
    args="--length="$MTX_SIZE
    name="gauss|S="$MTX_SIZE"|"
    bash ./benchmark_specific_app.sh "gauss" "$args" "$name"
done

add_separator

#####################RAND_GENERATOR ########################
mode=0 #storage
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

mode=1 #reduction
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

add_separator

##################### LC ########################
for ((size=$LC_MIN_SIZE;size<=$LC_MAX_SIZE;size*=2 )); do
    args="--length="$size
    name="lc_kernel|S="$size
    bash ./benchmark_specific_app.sh "lc_kernel" "$args" "$name"
done

add_separator

##################### FFT ########################
args="--length="$FFT_SIZE
name="rec_fft|S="$FFT_SIZE
bash ./benchmark_specific_app.sh "rec_fft" "$args" "$name"

add_separator

##################### RANDOM_ACCESS ########################

mode=0 #load
for ((size=$RA_RADIUS;size<=$RA_MAX_RAD;size*=2 )); do
  args="--length="$LINEAR_SIZE" --radius="$size" --lower_bound="$mode" --higher_bound="$mode
  name="rec_fft|R="$size"|mode=load"
  bash ./benchmark_specific_app.sh "random_access" "$args" "$name"
done

add_separator

mode=1 #store
for ((size=$RA_RADIUS;size<=$RA_MAX_RAD;size*=2 )); do
  args="--length="$LINEAR_SIZE" --radius="$size" --lower_bound="$mode" --higher_bound="$mode
  name="rec_fft|R="$size"|mode=store"
  bash ./benchmark_specific_app.sh "random_access" "$args" "$name"
done

add_separator

##################### Roofline ########################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


