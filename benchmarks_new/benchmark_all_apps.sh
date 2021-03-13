#!/bin/bash

rm -rf ./output/
mkdir ./output/

LINEAR_SIZE="800000000" # 6.4 GB in double, 3.2 GB in float
MIN_MTX_SIZE="128"
MAX_MTX_SIZE="1024"
MIN_MTX_TRANSPOSE_SIZE="256"
MAX_MTX_TRANSPOSE_SIZE="32768"
GRAPH_MIN_SIZE="6"
GRAPH_MAX_SIZE="12"
LC_MIN_SIZE="16"
LC_MAX_SIZE="128"
FFT_SIZE="8192"

# stencil params
STENCIL_1D_MIN_RAD="1"
STENCIL_1D_MAX_RAD="12"

MIN_2D_GRID_SIZE="256"
MAX_2D_GRID_SIZE="32768"

MIN_3D_GRID_SIZE="16"
MAX_3D_GRID_SIZE="512"

#sizes in KB
RA_RADIUS="2" # 2 KB
RA_MAX_RAD="2097152" # 2 GB

rm -rf ./output/metrics.csv
bash ./collect_common_stats.sh init
bash ./collect_roofline.sh init "single_socket"

function add_separator {
    bash ./collect_common_stats.sh add_separating_line
}

benchmarks_list=$1
profile=$2

#################### TRIAD ########################

for ((mode=0;mode<10;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name" $profile
done
add_separator

exit 0

##################### STENCIL 1D ########################

mode=0
for ((radius=STENCIL_1D_MIN_RAD;radius<=STENCIL_1D_MAX_RAD;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius" --lower_bound="$mode" --higher_bound="$mode
    name="stencil_1D|S="$LINEAR_SIZE"|R="$radius"|M="$mode"|"
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name" $profile
done

add_separator

##################### STENCIL 2D ########################

mode=0 # rectangle
for ((radius=1;radius<=3;radius++)); do
    for (( GRID_SIZE=MIN_2D_GRID_SIZE; GRID_SIZE<=MAX_2D_GRID_SIZE; GRID_SIZE*=2 )); do
        args="--length="$GRID_SIZE" --lower_bound="$mode" --higher_bound="$mode" --radius="$radius
        name="stencil_2D|S="$GRID_SIZE"|R="$radius"|""|M="$mode"|"
        bash ./benchmark_specific_app.sh "stencil_2D" "$args" "$name" $profile
    done
    add_separator
done

add_separator

mode=1 # cross
for ((radius=1;radius<=3;radius++)); do
    for (( GRID_SIZE=MIN_2D_GRID_SIZE; GRID_SIZE<=MAX_2D_GRID_SIZE; GRID_SIZE*=2 )); do
        args="--length="$GRID_SIZE" --lower_bound="$mode" --higher_bound="$mode" --radius="$radius
        name="stencil_2D|S="$GRID_SIZE"|R="$radius"|""|M="$mode"|"
        bash ./benchmark_specific_app.sh "stencil_2D" "$args" "$name" $profile
    done
    add_separator
done

add_separator

##################### 3D STENCIL ########################

mode=0 # rectangle
for ((radius=1;radius<=3;radius++)); do
    for (( GRID_SIZE=MIN_3D_GRID_SIZE; GRID_SIZE<=MAX_3D_GRID_SIZE; GRID_SIZE*=2 )); do
        args="--length="$GRID_SIZE" --lower_bound="$mode" --higher_bound="$mode" --radius="$radius
        name="stencil_3D|S="$GRID_SIZE"|R="$radius"|""|M="$mode"|"
        bash ./benchmark_specific_app.sh "stencil_3D" "$args" "$name" $profile
    done
    add_separator
done

add_separator

mode=1 # cross
for ((radius=1;radius<=3;radius++)); do
    for (( GRID_SIZE=MIN_3D_GRID_SIZE; GRID_SIZE<=MAX_3D_GRID_SIZE; GRID_SIZE*=2 )); do
        args="--length="$GRID_SIZE" --lower_bound="$mode" --higher_bound="$mode" --radius="$radius
        name="stencil_3D|S="$GRID_SIZE"|R="$radius"|""|M="$mode"|"
        bash ./benchmark_specific_app.sh "stencil_3D" "$args" "$name" $profile
    done
    add_separator
done

add_separator

##################### MAT_MUL ########################

for (( MTX_SIZE=MIN_MTX_SIZE; MTX_SIZE<=MAX_MTX_SIZE; MTX_SIZE*=2 )); do
    for ((mode=0;mode<=5;mode++)); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="matrix_mult|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name" $profile
    done

    add_separator
done

add_separator

##################### MAT_TRANSPOSAL ########################
for ((mode=0;mode<=3;mode++)); do
    for (( MTX_SIZE=MIN_MTX_TRANSPOSE_SIZE; MTX_SIZE<=MAX_MTX_TRANSPOSE_SIZE; MTX_SIZE*=2 )); do
        args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
        name="matrix_transp|S="$MTX_SIZE"|M="$mode"|"
        bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name" $profile
    done

    add_separator
done

add_separator

#####################RAND_GENERATOR ########################
mode=0 #storage
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name" $profile

mode=1 #reduction
args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
name="rand_generator|S="$LINEAR_SIZE"|M="$mode
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name" $profile

add_separator

##################### LC ########################
for ((size=$LC_MIN_SIZE;size<=$LC_MAX_SIZE;size*=2 )); do
    args="--length="$size
    name="lc_kernel|S="$size
    bash ./benchmark_specific_app.sh "lc_kernel" "$args" "$name" $profile
done

add_separator

##################### FFT ########################
args="--length="$FFT_SIZE
name="rec_fft|S="$FFT_SIZE
bash ./benchmark_specific_app.sh "rec_fft" "$args" "$name" $profile

add_separator

##################### RANDOM_ACCESS ########################

mode=0 #load
for ((size=$RA_RADIUS;size<=$RA_MAX_RAD;size*=2 )); do
  args="--length="$LINEAR_SIZE" --radius="$size" --lower_bound="$mode" --higher_bound="$mode
  name="rec_fft|R="$size"|mode=load"
  bash ./benchmark_specific_app.sh "random_access" "$args" "$name" $profile
done

add_separator

mode=1 #store
for ((size=$RA_RADIUS;size<=$RA_MAX_RAD;size*=2 )); do
  args="--length="$LINEAR_SIZE" --radius="$size" --lower_bound="$mode" --higher_bound="$mode
  name="rec_fft|R="$size"|mode=store"
  bash ./benchmark_specific_app.sh "random_access" "$args" "$name" $profile
done

add_separator

#####################BFS##############################

#mode=0 #sequential
#for ((size=$GRAPH_MIN_SIZE;size<=$GRAPH_MAX_SIZE;size++)); do
#    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
#    name="bfs|S="$size"|M="$mode"|"
#    bash ./benchmark_specific_app.sh "bfs" "$args" "$name"
#done

#add_separator

#mode=1 #parallel
#for ((size=$GRAPH_MIN_SIZE;size<=$GRAPH_MAX_SIZE;size++)); do
#    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
#    name="bfs|S="$size"|M="$mode"|"
#    bash ./benchmark_specific_app.sh "bfs" "$args" "$name"
#done

#add_separator


##################### Roofline ########################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_single_socket.txt
#python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline_dual_socket.txt


