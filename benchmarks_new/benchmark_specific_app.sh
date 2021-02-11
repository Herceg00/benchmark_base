#!/bin/bash

declare -A matrix
PROG_NAME=$1
PROG_ARGS=$2
TEST_NAME=$3
COMMON_ARGS="--compiler=g++ --no_run=false"

file_name="./output/performance_stats.csv"
roof_name="./output/roofline.txt"

TIME_PATTERN="avg_time:"
BAND_PATTERN="avg_bw:"
PERF_PATTERN="avg_flops:"
ARITHMETIC_INTENSITY_PATTERN="flops/byte"

lscpu >> info.txt

lscpu_nodes=$(grep -R "NUMA node(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')

lscpu_cpus=$(grep -R -m 1 "CPU(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')

cpus_per_node=$(($lscpu_cpus/$lscpu_nodes))

rm info.txt


##################### test name print #########################
echo $TEST_NAME
#########to_CSV##############
printf $TEST_NAME"," >> $file_name

########to_roofline##########
printf $TEST_NAME"," >> $roof_name

##################### single socket test ########################
rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=$cpus_per_node"
echo "Single-socket test: " $THREADS
bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS
search_result=$(grep -R "$TIME_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\ts," >> $file_name

search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\tGFLOP/s," >> $file_name
########to_roofline##########
printf $dat"," >> $roof_name

search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\tGB/s," >> $file_name

search_result=$(grep -R "$ARITHMETIC_INTENSITY_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
########to_roofline##########
printf $dat"," >> $roof_name
printf ",single_socket," >> $roof_name
printf "\n" >> $roof_name

printf "," >> $file_name

##################### dual socket test ########################
if [ $lscpu_nodes = "2" ]; then

printf $TEST_NAME"," >> $roof_name

rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=$lscpu_cpus"
echo "Dual-socket test: "$THREADS
bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS

search_result=$(grep -R "$TIME_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\ts," >> $file_name

search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\tGFLOP/s," >> $file_name
########to_roofline##########
printf $dat"," >> $roof_name

search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
#########to_CSV##############
printf $dat"\tGB/s," >> $file_name

search_result=$(grep -R "$ARITHMETIC_INTENSITY_PATTERN" "./"$PROG_NAME"/results.txt")
dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
printf $dat"," >> $roof_name

printf ",dual_socket," >> $roof_name
printf "\n" >> $roof_name

fi

#printf " " >> $file_name
printf "\n" >> $file_name