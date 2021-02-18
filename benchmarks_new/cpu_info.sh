#!/bin/bash

function get_sockets_count() {
    lscpu >> info.txt
    sockets=$(grep -R "NUMA node(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')
    rm info.txt
    echo $sockets
}

function get_cores_count() {
    lscpu >> info.txt
    lscpu_nodes=$(grep -R "NUMA node(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')
    lscpu_cpus=$(grep -R -m 1 "CPU(s):" "./info.txt" | grep -Eo '[0-9]{1,3}')
    cpus_per_node=$(($lscpu_cpus/$lscpu_nodes))
    rm info.txt
    echo $cpus_per_node
}

"$@"


