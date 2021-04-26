import subprocess
from subprocess import check_output
import sys
import re
import pprint
import copy
import os
from .arch_properties import get_arch
from .files import *
from .roofline import kunpeng_characteristics


def code(event_code):
    codes = {"MEM_STALL_ANY_LOAD": "r7004",
             "MEM_STALL_ANY_STORE": "r7005",
             "EXEC_STALL_CYCLE": "r7001",
             "MEM_STALL_L1MISS": "r7006",
             "MEM_STALL_L2MISS": "r7007",
             "REMOTE_ACCESS": "r0031",
             "MEM_ACCESS_LD": "r0066",
             "MEM_ACCESS_ST": "r0067",
             "LL_CACHE": "r0032",
             "LL_CACHE_MISS": "r0033",
             "fetch_bubble": "r2014",
             "L1D_CACHE": "r0004",
             "L2D_CACHE": "r0016",
             "rd_spipe": "r20",
             "flux_rd": "r01",
             "flux_wr": "r00"
    }
    return codes[event_code]


def get_no_conflict_events_list(architecture):
    if architecture == "kunpeng":
        events = ["r7004", # MEM_STALL_ANY_LOAD
                  "r7005", # MEM_STALL_ANY_STORE
                  "r7001", # exec_stall_cycle
                  "r7006", # MEM_STALL_L1MISS
                  "r7007", # MEM_STALL_L2MISS
                  "r0031", # REMOTE_ACCESS"
                  "r0066", # MEM_ACCESS_LD
                  "r0067", # MEM_ACCESS_ST
                  "r0032", # LL_CACHE
                  "r0033",  # LL_CACHE_MISS
                  "duration_time"
                  ]
        return events
    if architecture == "intel_xeon":
        events = ["L1-dcache-loads",
                  "LLC-loads",
                  "LLC-stores",
                  "LLC-store-misses",
                  "LLC-load-misses"]
        return events
    return []


def get_conflicted_events_list(architecture):
    if architecture == "kunpeng":
        events = ["r2014", #"fetch_bubble"
                  "CPU_CYCLES",
                  "INST_SPEC",
                  "INST_RETIRED",
                  "r0004", #"L1D_CACHE":
                  "r0016", #"L2D_CACHE":
                  "r20", #"rd_spipe":
                  "r01", #"flux_rd":
                  "r00" #"flux_wr"
                  ]
        return events
    return []


def get_event_value_from_file_line(line, event_list):
    for event_name in event_list:
        if event_name in line:
            event_value = int(re.search(r'\d+', line).group())
            return {event_name: event_value}
    return None


def merge_two_dicts(x, y):
    """Given two dictionaries, merge them into a new dict as a shallow copy."""
    z = {**x, **y}
    return z


def collect_list_of_events(prog_name, prog_args, event_list): # can collect groups of events or single events
    result = {}
    prog_name_arg = ["--prog=" + prog_name]
    profiling_args = ["--compiler=g++", "--no_run=false", "--metrics=true", "--output="+tmp_perf_metrics_file_name]
    events_args = ["--events=" + ','.join(event_list)]
    all_args = prog_name_arg  + profiling_args + events_args + prog_args

    subprocess.check_call(["bash"] + ['./make_omp.sh'] + all_args)

    a_file = open("./"+prog_name+"/"+tmp_perf_metrics_file_name)

    lines = a_file.readlines()
    for line in lines:
        new_metric = get_event_value_from_file_line(line, event_list)
        if new_metric is not None:
            result = merge_two_dicts(result, new_metric)

    return result


def analyse_events(architecture, hardware_events):
    all = copy.deepcopy(hardware_events)

    if architecture == "kunpeng":
        all["Frontend_Bound"] = 100.0* (all[code("fetch_bubble")]/(4.0 * all["CPU_CYCLES"]))
        all["Bad_Speculation"] = 100.0* ((all["INST_SPEC"] - all["INST_RETIRED"])/(4.0 * all["CPU_CYCLES"]))
        all["Retiring"] = 100.0* (all["INST_RETIRED"] / (4.0 * all["CPU_CYCLES"]))
        all["Backend_Bound"] = (100.0 - (all["Frontend_Bound"] + all["Bad_Speculation"] + all["Retiring"]))
        all["Memory_Bound"] = 100.0* ((all[code("MEM_STALL_ANY_LOAD")] + all[code("MEM_STALL_ANY_STORE")])/all[code("EXEC_STALL_CYCLE")])
        all["L1_Bound"] = 100.0* ((all[code("MEM_STALL_ANY_LOAD")] - all[code("MEM_STALL_L1MISS")])/all[code("EXEC_STALL_CYCLE")])
        all["L2_Bound"] = 100.0* ((all[code("MEM_STALL_L1MISS")] - all[code("MEM_STALL_L2MISS")])/all[code("EXEC_STALL_CYCLE")])
        all["L3_Bound_or_DRAM"] = 100.0* (all[code("MEM_STALL_L2MISS")]/all[code("EXEC_STALL_CYCLE")])
        all["Store_Bound"] = 100.0* (all[code("MEM_STALL_ANY_STORE")]/all[code("EXEC_STALL_CYCLE")])
        all["Core_Bound"] = 100.0* ((all[code("EXEC_STALL_CYCLE")] - all[code("MEM_STALL_ANY_LOAD")] - all[code("MEM_STALL_ANY_STORE")])/all[code("EXEC_STALL_CYCLE")])

        all["LL_hit_rate"] = 100.0* (1.0 - all[code("LL_CACHE_MISS")]/all[code("LL_CACHE")])
        all["Remote_accesses"] = 100.0* (all[code("REMOTE_ACCESS")]/(all[code("MEM_ACCESS_LD")] + all[code("MEM_ACCESS_ST")]))

        all["L1_SBW"] = all[code("L1D_CACHE")] * 16 / (all["duration_time"])
        all["L1_SBW_percent"] = 100.0 * all["L1_SBW"] / kunpeng_characteristics["bandwidths"]["L1"]

        all["L2_SBW"] = all[code("L2D_CACHE")] * 64 / (all["duration_time"])
        all["L2_SBW_percent"] = 100.0 * all["L2_SBW"] / kunpeng_characteristics["bandwidths"]["L2"]

        all["L3_SBW"] = all[code("rd_spipe")] * 64 / (all["duration_time"])
        all["L3_SBW_percent"] = 100.0 * all["L3_SBW"] * 64 / kunpeng_characteristics["bandwidths"]["L3"]

        all["DRAM_SBW"] = (all[code("flux_rd")] + all[code("flux_wr")]) * 32 / (all["duration_time"])
        all["DRAM_SBW_percent"] = 100.0 * all["DRAM_SBW"] / kunpeng_characteristics["bandwidths"]["DRAM"]

    if architecture == "intel_xeon":
        all["LLC_hit_rate"] = 1.0 - (all["LLC-store-misses"] + all["LLC-load-misses"])/(all["LLC-stores"] + all["LLC-loads"])

    # remove hardware events from resulting dict
    for key in all.copy():
        if key in hardware_events:
            del all[key]

    return all


def init_table(output_file_name, metrics): # add header if file does not exist
    if not os.path.exists(output_file_name):
        with open(output_file_name, 'w') as output_file:
            output_file.write("test_name,")
            for key in metrics:
                output_file.write(str(key) + ",")
            output_file.write("\n")


def add_metrics_to_file(output_file_name, test_name, metrics):
    with open(output_file_name, 'a') as output_file:
        output_file.write(test_name + ",")
        for key in metrics:
            output_file.write(str(metrics[key]) + ",")
        output_file.write("\n")


def profiling_add_separator():
    arch = get_arch()
    file_name = "./output/" + arch + "_profile_metrics.csv"
    with open(file_name, 'a') as output_file:
        output_file.write("\n")


def run_profiling(prog_name, test_name, prog_args):
    arch = get_arch()
    output_file_name = "./output/" + arch + "_profile_metrics.csv"

    no_conflict_event_list = get_no_conflict_events_list(arch)
    conflicted_event_list = get_conflicted_events_list(arch)

    hardware_events = {}
    hardware_events = merge_two_dicts(hardware_events, collect_list_of_events(prog_name, prog_args, no_conflict_event_list))
    for conflicted_event in conflicted_event_list:
        hardware_events = merge_two_dicts(hardware_events, collect_list_of_events(prog_name, prog_args, [conflicted_event]))

    app_metrics = analyse_events(arch, hardware_events)
    init_table(output_file_name, app_metrics)
    add_metrics_to_file(output_file_name, test_name, app_metrics)