import subprocess
from subprocess import check_output
import sys
import re
import pprint
import copy
import os
from .arch_properties import get_arch
from .files import *


def get_timing_from_file_line(line, timings):
    for key, val in timings.items():
        if key in line:
            timings[key] = float(line.split(" ")[1])
            print(timings[key])
    return timings


def run_test_and_parse_timings(prog_name, prog_args): # collect time, perf and BW values
    result = {}
    prog_name_arg = ["--prog=" + prog_name]
    profiling_args = ["--compiler=g++", "--no_run=false", "--metrics=false", "--output="+tmp_timings_file_name]
    all_args = prog_name_arg + profiling_args + prog_args

    subprocess.check_call(["bash"] + ['./make_omp.sh'] + all_args)
    a_file = open("./"+prog_name+"/"+tmp_timings_file_name)

    lines = a_file.readlines()
    timings = {"avg_time": 0, "avg_bw": 0, "avg_flops": 0}
    for line in lines:
        timings = get_timing_from_file_line(line, timings)
    return timings


def init_table(output_file_name, timings): # add header if file does not exist
    if not os.path.exists(output_file_name):
        with open(output_file_name, 'w') as output_file:
            output_file.write("test_name,")
            output_file.write("test_name" + ",")
            for key in timings:
                output_file.write(str(key) + ",")
            output_file.write("\n")


def add_metrics_to_file(output_file_name, test_name, metrics):
    with open(output_file_name, 'a') as output_file:
        output_file.write(test_name + ",")
        for key in metrics:
            output_file.write(str(metrics[key]) + ",")
        output_file.write("\n")
        output_file.write("\n")


def run_timings(prog_name, test_name, prog_args):
    arch = get_arch()
    output_file_name = "./output/" + arch + "_timings.csv"

    timings = run_test_and_parse_timings(prog_name, prog_args)

    init_table(output_file_name, timings)
    add_metrics_to_file(output_file_name, test_name, timings)
