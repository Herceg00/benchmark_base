#!/bin/bash

file_name="./output/performance_stats.csv"
TIME_PATTERN="avg_time:"
BAND_PATTERN="avg_bw:"
PERF_PATTERN="avg_flops:"

function add_separator {
    printf " " >> $file_name
    printf "\n" >> $file_name
}

function add_separating_line {
    printf ",,,,,,,,,,,,,,,,,,,," >> $file_name
    printf "\n" >> $file_name
}

function init {
    declare -a column_names=("algorithm"
			                   "Time"
                         "Performance"
                         "Bandwidth"
                         "")

    sockets_num=$(bash ./cpu_info.sh get_sockets_count)
    if [ $sockets_num = "1" ]; then
        printf ",one_socket," >> $file_name
    fi
    if [ $sockets_num = "2" ]; then
        printf ",one_socket,,,,,two sockets," >> $file_name
    fi
    if [ $sockets_num = "4" ]; then
        printf ",one_socket,,,,,two sockets,,,,,three_sockets,,,,,four_socket," >> $file_name
    fi

    add_separator

    for ((socket=0;socket<$sockets_num;socket++)); do
        it="1"
        for name in "${column_names[@]}"
        do
            it=$((it+1))
            printf $name"," >> $file_name
        done
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
    dat=$(echo "scale=4; 1000.0*$dat" | bc -l)
    printf $dat"," >> $file_name

    search_result=$(grep -R "$PERF_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"," >> $file_name

    search_result=$(grep -R "$BAND_PATTERN" "./"$PROG_NAME"/results.txt")
    dat=`echo $search_result | grep -Eo '[+-]?[0-9]+([.][0-9]+)?'`
    printf $dat"," >> $file_name
    printf "," >> $file_name
}

"$@"