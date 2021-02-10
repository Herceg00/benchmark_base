#!/bin/bash

file_name="./performance_stats.csv"
rm $file_name

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

for ((mode=1;mode<=5;mode++)); do
    args="--length="$LINEAR_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="triada|S="$LINEAR_SIZE"|M="$mode
    bash ./benchmark_specific_app.sh "triada" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### STENCIL ########################

for ((radius=3;radius<=7;radius++)); do
    args="--length="$LINEAR_SIZE" --radius="$radius
    name="stencil1D|S="$LINEAR_SIZE"|R="$radius
    bash ./benchmark_specific_app.sh "stencil_1D" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_MUL ########################

for ((mode=1;mode<=6;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_mult|S="$MTX_SIZE"|M="$mode
    bash ./benchmark_specific_app.sh "matrix_mult" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name

##################### MAT_TRANSPOSAL ########################

for ((mode=0;mode<=3;mode++)); do
    args="--length="$MTX_SIZE" --lower_bound="$mode" --higher_bound="$mode
    name="matrix_transp|S="$MTX_SIZE"|M="$mode
    bash ./benchmark_specific_app.sh "matrix_transp" "$args" "$name"
done

printf " " >> $file_name
printf "\n" >> $file_name


