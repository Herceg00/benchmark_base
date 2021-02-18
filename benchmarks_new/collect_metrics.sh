#!/bin/bash

file_name="./output/event_counters.csv"
tmp_metrics_file_name="metrics.txt"

declare -a event_names=("instructions"
    "armv8_pmuv3_0/cpu_cycles/"
    "armv8_pmuv3_0/inst_retired/"
    "armv8_pmuv3_0/ll_cache_miss/"
    "armv8_pmuv3_0/ll_cache/"
    "armv8_pmuv3_0/mem_access/"
    "armv8_pmuv3_0/remote_access/"
)

function add_separator() {
    printf " " >> $file_name
    printf "\n" >> $file_name
}

function remove_spaces() {
    var=$1
    echo "${var//+([[:space:]])/}"
}

function replace_backslash() {
    echo "$1" | tr '/' '*'
}

function parse_events() {
    search_result=$(grep -R "$1" "./$PROG_NAME/$tmp_metrics_file_name")
    search_result=$(replace_backslash $search_result)
    loc_event_name=$(replace_backslash $1)

    parsed_number=`echo $search_result | sed -e "s/"$loc_event_name"//"`

    result=$(remove_spaces $parsed_number)
    echo $result
}

function join_by {
    local IFS="$1"; shift; echo "$*";
}

function init {
    printf "benchmark name," >> $file_name
    for event_name in "${event_names[@]}"
    do
        printf $event_name"," >> $file_name
    done
    printf "\n" >> $file_name
}

function collect_stats() {
    PROG_NAME=$1
    PROG_ARGS=$2
    TEST_NAME=$3
    THREADS=$4
    COMMON_ARGS="--compiler=g++ --no_run=false --metrics=true --output=$tmp_metrics_file_name"

    # get list of events as a param
    events_param="--events="$(join_by , "${event_names[@]}")

    # collect basic events
    bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS $events_param

    printf $TEST_NAME"," >> $file_name
    for current_name in "${event_names[@]}"
    do
      parsed_number=$(parse_events $current_name)
      #echo $parsed_number
      printf $parsed_number"," >> $file_name
    done

    add_separator

    # remove tmp metrics file
    rm ./$PROG_NAME/$tmp_metrics_file_name
}

"$@"


