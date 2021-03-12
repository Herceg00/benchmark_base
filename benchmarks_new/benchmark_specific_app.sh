#!/bin/bash

PROG_NAME=$1
PROG_ARGS=$2
TEST_NAME=$3
PROFILE=$4

#################### obtain CPU info ##########################
cores_num=$(bash ./cpu_info.sh get_cores_count)
sockets_num=$(bash ./cpu_info.sh get_sockets_count)
echo "CORES NUMBER=$cores_num"
echo "SOCKETS NUMBER=$sockets_num"

##################### single socket test ########################
THREADS=" --threads=$cores_num"
bash ./collect_common_stats.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS"
if [[ $PROFILE == "profile" ]]; then
    echo "profiling run"
    bash ./collect_roofline.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS" "single_socket"
    python3 ./collect_metrics.py "$PROG_NAME" "$TEST_NAME" "$PROG_ARGS" "$THREADS"
fi

##################### dual socket test ########################
if [ $sockets_num = "2" ]; then
    cores_num=$(( 2*cores_num))
    THREADS="--threads=$cores_num"
    bash ./collect_common_stats.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS"
fi

if [ $sockets_num = "4" ]; then
    cores_num=$(bash ./cpu_info.sh get_cores_count)
    cores_num=$(( 2*cores_num))
    THREADS="--threads=$cores_num"
    bash ./collect_common_stats.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS"

    cores_num=$(bash ./cpu_info.sh get_cores_count)
    cores_num=$(( 3*cores_num))
    THREADS="--threads=$cores_num"
    bash ./collect_common_stats.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS"

    cores_num=$(bash ./cpu_info.sh get_cores_count)
    cores_num=$(( 4*cores_num))
    THREADS="--threads=$cores_num"
    bash ./collect_common_stats.sh collect_stats "$PROG_NAME" "$PROG_ARGS" "$TEST_NAME" "$THREADS"
fi

bash ./collect_common_stats.sh add_separator