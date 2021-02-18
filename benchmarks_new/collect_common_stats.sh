#!/bin/bash

file_name="./output/performance_stats.csv"
TIME_PATTERN="avg_time:"
BAND_PATTERN="avg_bw:"
PERF_PATTERN="avg_flops:"

function add_separator() {
    printf " " >> $file_name
    printf "\n" >> $file_name
}

function init {
    declare -a column_names=("algorithm"
			                   "Time"
                         "Performance"
                         "Bandwidth"
		                     "algorithm"
                         "Time"
                         "Performance"
                         "Bandwidth")

    printf ",one_socket,,,,dual_socket," >> $file_name

    add_separator

    it="1"
    for name in "${column_names[@]}"
    do
        it=$((it+1))
        printf $name"," >> $file_name
    done

    add_separator
}

function collect_stats {
    PROG_NAME=$1
    PROG_ARGS=$2
    TEST_NAME=$3
    THREADS=$4
    COMMON_ARGS=" --compiler=g++ --no_run=false --metrics=false "

    rm "./"$PROG_NAME"/results.txt"

    bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS

    printf $TEST_NAME"," >> $file_name

    search_result=$(grep -R "$TIME_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"\ts," >> $file_name

    search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"\tGFLOP/s," >> $file_name

    search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"\tGB/s," >> $file_name
}

"$@"