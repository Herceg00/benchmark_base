#!/bin/bash

file_name="./performance_stats.csv"
rm $file_name

roof_name="./roofline.txt"

LINEAR_SIZE="2000"
MTX_SIZE="100"

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
    name="triada(S="$LINEAR_SIZE",M="$mode")"
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### STENCIL ########################

for ((radius=3;radius<=10;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius
    name="stencil_1D(S="$LINEAR_SIZE",R="$radius")"
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_MUL ########################

for ((mode=0;mode<=5;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_mult(S="$MTX_SIZE",M="$mode")"
    bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_TRANSPOSAL ########################
MTX_SIZE=1024

for ((mode=0;mode<=3;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_transp(S="$MTX_SIZE",M="$mode")"
    bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### BELLMAN_FORD ########################
mode=0
for ((size=8;size<=12;size++)); do
    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
    name="bellman_ford(S="$size",M="$mode")"
    bash ./benchmark_specific_app.sh "bellman_ford" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name


#####################GAUSS ########################
mode=0
for ((size=1024;size<=4000;size+=512)); do
    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
    name="gauss(S="$size",M="$mode")"
    bash ./benchmark_specific_app.sh "gauss" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name

#####################RAND_GENERATOR ########################
mode=0
for ((size=1048576 ;size<=16777216; size*=2)); do
    args="--length="$size" --lower_bound="$mode" --higher_bound="$mode
    name="rand_generator(S="$size",M="$mode")"
    bash ./benchmark_specific_app.sh "rand_generator" "$args" "$name"
    printf "\n" >> $roof_name
done

printf " " >> $file_name
printf "\n" >> $file_name


