#!/bin/bash

file_name_prefix="./output/metrics"
tmp_metrics_file_name="metrics.txt"

declare -A event_values

declare -a arm_event_names=("instructions"
        "armv8_pmuv3_0/cpu_cycles/"
        "armv8_pmuv3_0/inst_retired/"
        "armv8_pmuv3_0/ll_cache_miss/"
        "armv8_pmuv3_0/ll_cache/"
        "armv8_pmuv3_0/mem_access/"
        "armv8_pmuv3_0/remote_access/"
        "armv8_pmuv3_0/stall_backend/"
        "armv8_pmuv3_0/stall_frontend/"
        "branch-misses"
        "cpu-clock"
        "duration_time"
)

declare -a intel_event_names=("instructions"
        "branch-misses"
        "LLC-load-misses"
        "LLC-loads"
        "LLC-store-misses"
        "LLC-stores"
        "L1-dcache-load-misses"
        "L1-dcache-loads"
        "L1-dcache-stores"
        "cpu-cycles"
        "mem-loads"
        "mem-stores"
        "cache-misses"
        "cpu-clock"
)

function add_separator() {
    SOCKETS=$1
    file_name=$file_name_prefix"_"$SOCKETS".csv"
    printf " " >> $file_name
    printf "\n" >> $file_name
}

function add_separating_line {
    SOCKETS=$1
    file_name=$file_name_prefix"_"$SOCKETS".csv"
    printf ",,,,,,,,,,,,,,,,,,,," >> $file_name
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
    if [[ $arch = "aarch64" ]]; then
        for event_name in "${arm_event_names[@]}"
        do
            printf $event_name"," >> $file_name
        done
    fi

    if [[ $arch = "intel" ]]; then
        for event_name in "${intel_event_names[@]}"
        do
            printf $event_name"," >> $file_name
        done
    fi

    printf "\n" >> $file_name
}

function analyse_events {
    declare -A analyse_values

    arch=$(bash ./cpu_info.sh get_arch)
    if [[ $arch = "aarch64" ]]; then
        analyse_values['ipc']=$(echo "scale=4; ${event_values['instructions']}/${event_values['armv8_pmuv3_0/cpu_cycles/']}" | bc -l)
        analyse_values['branch_misses_percent']=$(echo "scale=4; ${event_values['branch-misses']}/${event_values['instructions']}" | bc -l)
        analyse_values['L1_hit_rate']=$(echo "scale=4; 100.0*(${event_values['armv8_pmuv3_0/ll_cache/']} - ${event_values['armv8_pmuv3_0/ll_cache_miss/']})/${event_values['armv8_pmuv3_0/ll_cache/']}" | bc -l)

        analyse_values['mem_accesses_per_instruction']=$(echo "scale=4; ${event_values['armv8_pmuv3_0/ll_cache/']}/${event_values['instructions']}" | bc -l)
        analyse_values['mem_instructions_per_sec']=$(echo "scale=4; ${event_values['armv8_pmuv3_0/ll_cache/']}/${event_values['cpu-clock']}" | bc -l)
    fi

    if [[ $arch = "intel" ]]; then
        analyse_values['ipc']=$(echo "scale=4; ${event_values['instructions']}/${event_values['cpu-cycles']}" | bc -l)
        analyse_values['branch_misses_percent']=$(echo "scale=4; ${event_values['branch-misses']}/${event_values['instructions']}" | bc -l)
        analyse_values['L1_hit_rate']=$(echo "scale=4; 100.0*(${event_values['L1-dcache-loads']})/(${event_values['L1-dcache-loads']} + ${event_values['L1-dcache-load-misses']})" | bc -l)
        analyse_values['LLC_hit_rate']=$(echo "scale=4; 100.0*(${event_values['LLC-loads']})/(${event_values['LLC-loads']} + ${event_values['LLC-load-misses']})" | bc -l)

        analyse_values['mem_accesses_per_instruction']=$(echo "scale=4; (${event_values['L1-dcache-loads']} + ${event_values['L1-dcache-load-misses']})/${event_values['instructions']}" | bc -l)
        analyse_values['mem_instructions_per_sec']=$(echo "scale=4; (${event_values['L1-dcache-loads']} + ${event_values['L1-dcache-load-misses']})/${event_values['cpu-clock']}" | bc -l)
    fi

    printf "," >> $file_name

    for key in "${!analyse_values[@]}"; do
        echo "$key -- ""${analyse_values[$key]}";
        printf "$key = ""${analyse_values[$key]}," >> $file_name
    done
}

function collect_stats {
    PROG_NAME=$1
    PROG_ARGS=$2
    TEST_NAME=$3
    THREADS=$4
    SOCKETS=$5
    COMMON_ARGS="--compiler=g++ --no_run=false --metrics=true --output=$tmp_metrics_file_name"

    file_name=$file_name_prefix"_"$SOCKETS".csv"

    rm ./$PROG_NAME/$tmp_metrics_file_name # TODO remove maybe

    # get list of events as a param
    arch=$(bash ./cpu_info.sh get_arch)
    echo $arch
    if [[ $arch = "aarch64" ]]; then
        events_param="--events="$(join_by , "${arm_event_names[@]}")
    fi

    if [[ $arch = "intel" ]]; then
        events_param="--events="$(join_by , "${intel_event_names[@]}")
    fi

    # collect basic events
    bash make_omp.sh --prog=$PROG_NAME $PROG_ARGS $COMMON_ARGS $THREADS $events_param

    printf $TEST_NAME"," >> $file_name

    arch=$(bash ./cpu_info.sh get_arch)
    if [[ $arch = "aarch64" ]]; then
        for current_name in "${arm_event_names[@]}"
        do
            parsed_number=$(parse_events $current_name)
            printf $parsed_number"," >> $file_name
            event_values["$current_name"]="$parsed_number"
        done
    fi

    if [[ $arch = "intel" ]]; then
        for current_name in "${intel_event_names[@]}"
        do
            parsed_number=$(parse_events $current_name)
            printf $parsed_number"," >> $file_name
            event_values["$current_name"]="$parsed_number"
        done
    fi
    
    # analyse events
    analyse_events

    add_separator $SOCKETS

    # remove tmp metrics file
    #rm ./$PROG_NAME/$tmp_metrics_file_name
}

"$@"


