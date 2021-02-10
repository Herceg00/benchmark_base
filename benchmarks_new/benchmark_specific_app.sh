#!/bin/bash

declare -A matrix
PROG_NAME=$1
PROG_ARGS=$2
TEST_NAME=$3
COMMON_ARGS="--compiler=g++ --no_run=false"

file_name="./performance_stats.csv"


TIME_PATTERN="avg_time:"
BAND_PATTERN="avg_bw:"
PERF_PATTERN="avg_flops:"

rm info.txt
lscpu >> info.txt

lscpu_nodes=$(grep -R "NUMA node(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')

lscpu_cpus=$(grep -R -m 1 "CPU(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')

cpus_per_node=$(($lscpu_cpus/$lscpu_nodes))

rm info.txt


##################### test name print #########################

printf $TEST_NAME"," >> $file_name

##################### single socket test ########################
rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=$cpus_per_node"
echo "Single-socket test: " $THREADS
bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS

search_result=$(grep -R "$TIME_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\ts," >> $file_name

search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\tGFLOP/s," >> $file_name

search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\tGB/s," >> $file_name

printf "," >> $file_name

##################### dual socket test ########################
if [ $lscpu_nodes = "2" ]; then
rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=$lscpu_cpus"
echo "Dual-socket test: "$THREADS
bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS

search_result=$(grep -R "$TIME_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\ts," >> $file_name

search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\tGFLOP/s," >> $file_name

search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"\tGB/s," >> $file_name

printf " " >> $file_name
printf "\n" >> $file_name

fi
##################### saving data ########################

for ((row=1;row<=num_rows;row++)) do
    for ((col=1;col<=num_columns;col++)) do
        printf ${matrix[$row,$col]}"," >> $file_name
    done
    printf " " >> $file_name
    printf "\n" >> $file_name
done

rm tmp_file.txt