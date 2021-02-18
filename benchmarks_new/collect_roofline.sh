#!/bin/bash

file_name_prefix="./output/roofline"
PERF_PATTERN="avg_flops:"
ARITHMETIC_INTENSITY_PATTERN="flops/byte"

function init {
    SOCKETS=$1
    file_name=$file_name_prefix"_"$SOCKETS".txt"
    printf $SOCKETS"\n" >> $file_name
}

function collect_stats {
    PROG_NAME=$1
    PROG_ARGS=$2
    TEST_NAME=$3
    THREADS=$4
    SOCKETS=$5
    COMMON_ARGS=" --compiler=g++ --no_run=false --metrics=false "

    file_name=$file_name_prefix"_"$SOCKETS".txt"

    rm "./"$PROG_NAME"/results.txt"
    bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS
    printf $TEST_NAME"," >> $file_name

    search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"," >> $file_name

    search_result=$(grep -R "$ARITHMETIC_INTENSITY_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"," >> $file_name
    printf "\n" >> $file_name
}

"$@"