#!/bin/bash

file_name_prefix="./output/metrics"
tmp_metrics_file_name="metrics.txt"

declare -a kunpeng_event_names=("instructions"
        "armv8_pmuv3_0/cpu_cycles/"
        "armv8_pmuv3_0/inst_retired/"
        "armv8_pmuv3_0/ll_cache_miss/"
        "armv8_pmuv3_0/ll_cache/"
        "armv8_pmuv3_0/mem_access/"
        "armv8_pmuv3_0/remote_access/"
        "duration_time"
        "armv8_pmuv3_0/stall_backend/"
        "armv8_pmuv3_0/stall_frontend/"
)

function add_separator() {
    SOCKETS=$1
    file_name=$file_name_prefix"_"$SOCKETS".csv"
    printf " " >> $file_name
    printf "\n" >> $file_name
}

function remove_spaces() {
    var=$1
    echo $var | sed 's/[^0-9]*//g'
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
    SOCKETS=$1
    file_name=$file_name_prefix"_"$SOCKETS".csv"

    printf "benchmark name," >> $file_name

    arch=$(bash ./cpu_info.sh get_arch)
    if [ $arch = "aarch64" ]; then
        for event_name in "${kunpeng_event_names[@]}"
        do
            printf $event_name"," >> $file_name
        done
    fi

    printf "\n" >> $file_name
}

function compute_additional_data {
    echo "hehe"
}

function collect_stats {
    PROG_NAME=$1
    PROG_ARGS=$2
    TEST_NAME=$3
    THREADS=$4
    SOCKETS=$5
    COMMON_ARGS="--compiler=g++ --no_run=false --metrics=true --output=$tmp_metrics_file_name"

    file_name=$file_name_prefix"_"$SOCKETS".csv"

    # get list of events as a param
    arch=$(bash ./cpu_info.sh get_arch)
    if [ $arch = "aarch64" ]; then
        events_param="--events="$(join_by , "${kunpeng_event_names[@]}")
    fi

    # collect basic events
    bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS $events_param

    printf $TEST_NAME"," >> $file_name

    declare -A event_values
    arch=$(bash ./cpu_info.sh get_arch)
    if [ $arch = "aarch64" ]; then
        for current_name in "${kunpeng_event_names[@]}"
        do
            parsed_number=$(parse_events $current_name)
            printf $parsed_number"," >> $file_name
            event_values["$current_name"]="$parsed_number"
        done
    fi

    echo "${event_values["instructions"]}"

    add_separator $SOCKETS

    # remove tmp metrics file
    rm ./$PROG_NAME/$tmp_metrics_file_name
}

"$@"

