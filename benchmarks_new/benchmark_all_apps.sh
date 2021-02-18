#!/bin/bash

rm -rf ./output/
mkdir ./output/
file_name="./output/performance_stats.csv"
roof_name="./output/roofline.txt"

#LINEAR_SIZE="2000000"
#MTX_SIZE="1024"
#GRAPH_MIN_SIZE="5"
#GRAPH_MAX_SIZE="8"

LINEAR_SIZE="2000"
MTX_SIZE="128"
GRAPH_MIN_SIZE="5"
GRAPH_MAX_SIZE="8"

##################### names init ########################

declare -a column_names=("algorithm"
			                   "Time"
                         "Performance"
                         "Bandwidth" 
		                     ""
                         "Time"
                         "Performance"
                         "Bandwidth")

printf ",one_socket,,,,duo_socket," >> $file_name
printf " " >> $file_name
printf "\n" >> $file_name

it="1"
for name in "${column_names[@]}"
do
    it=$((it+1))
    printf $name"," >> $file_name
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### TRIAD ########################

for ((mode=0;mode<=15;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### STENCIL ########################

for ((radius=3;radius<=10;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius
    name="stencil_1D|S="$LINEAR_SIZE"|R="$radius"|"
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_MUL ########################

for ((mode=0;mode<=5;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_mult|S="$MTX_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_TRANSPOSAL ########################

for ((mode=0;mode<=3;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_transp|S="$MTX_SIZE"|M="$mode"|"
    bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### BELLMAN_FORD ########################
mode=0
for ((size=$GRAPH_MIN_SIZE;size<=$GRAPH_MAX_SIZE;size++)); do
    args="--length="$size
    name="bellman_ford|S="$size"|"
    bash ./benchmark_specific_app.sh "bellman_ford" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

#####################GAUSS ########################
mode=0

args="--length="$MTX_SIZE
name="gauss|S="$MTX_SIZE"|"
bash ./benchmark_specific_app.sh "gauss" "$args" "$name"

printf " " >> $file_name
printf "\n" >> $file_name

#####################RAND_GENERATOR ########################

args="--length="$LINEAR_SIZE
name="rand_generator|S="$LINEAR_SIZE"|"
bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"

printf " " >> $file_name
printf "\n" >> $file_name

################### generate roofline ######################

python3 ./../roofline/roofline.py ./../benchmarks_new/output/roofline.txt


