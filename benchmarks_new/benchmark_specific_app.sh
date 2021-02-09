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

##################### test name print #########################

printf $TEST_NAME"," >> $file_name

##################### single socket test ########################
rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=48"
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
rm "./"$PROG_NAME"/results.txt"
THREADS=" --threads=96"
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

##################### saving data ########################

for ((row=1;row<=num_rows;row++)) do
    for ((col=1;col<=num_columns;col++)) do
        printf ${matrix[$row,$col]}"," >> $file_name
    done
    printf " " >> $file_name
    printf "\n" >> $file_name
done

rm tmp_file.txt