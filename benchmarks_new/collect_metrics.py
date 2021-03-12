import subprocess
from subprocess import check_output
import sys
import re
import pprint
import copy
import os


metrics_file_name = "metrics.txt"
output_file_name = "./output/metrics.csv"


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
             "fetch_bubble": "r2014"
    }
    return codes[event_code]


def get_arch():
    architecture = "unknown"
    output = subprocess.check_output(["lscpu"])
    for item in output.decode().split("\n"):
        if "Architecture:" in item:
            arch_line = item.strip()
            arch_str = arch_line.split(":")[1]
            if "aarch64" in arch_str:
                architecture = "kunpeng920"
            if "x86_64" in arch_str:
                architecture = "intel"
    return architecture


def get_no_conflict_events_list(architecture):
    if architecture == "kunpeng920":
        events = ["r7004", # MEM_STALL_ANY_LOAD
                  "r7005", # MEM_STALL_ANY_STORE
                  "r7001", # exec_stall_cycle
                  "r7006", # MEM_STALL_L1MISS
                  "r7007", # MEM_STALL_L2MISS
                  "r0031", # REMOTE_ACCESS"
                  "r0066", # MEM_ACCESS_LD
                  "r0067", # MEM_ACCESS_ST
                  "r0032", # LL_CACHE
                  "r0033"  # LL_CACHE_MISS
                  ]
        return events
    return []


def get_conflicted_events_list(architecture):
    if architecture == "kunpeng920":
        events = ["r2014", #"fetch_bubble"
                  "CPU_CYCLES",
                  "INST_SPEC",
                  "INST_RETIRED"
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
    profiling_args = ["--compiler=g++", "--no_run=false", "--metrics=true", "--output="+metrics_file_name]
    events_args = ["--events=" + ','.join(event_list)]
    all_args = prog_name_arg  + profiling_args + events_args + prog_args

    subprocess.check_call(["bash"] + ['./make_omp.sh'] + all_args)

    a_file = open("./"+prog_name+"/"+metrics_file_name)

    lines = a_file.readlines()
    for line in lines:
        new_metric = get_event_value_from_file_line(line, event_list)
        if new_metric is not None:
            result = merge_two_dicts(result, new_metric)

    return result


def analyse_events(architecture, hardware_events):
    all = copy.deepcopy(hardware_events)

    if architecture == "kunpeng920":
        all["Frontend_Bound"] = all[code("fetch_bubble")]/(4.0 * all["CPU_CYCLES"])
        all["Bad_Speculation"] = (all["INST_SPEC"] - all["INST_RETIRED"])/(4.0 * all["CPU_CYCLES"])
        all["Retiring"] = all["INST_RETIRED"] / (4.0 * all["CPU_CYCLES"])
        all["Backend_Bound"] = 1.0 - (all["Frontend_Bound"] + all["Bad_Speculation"] + all["Retiring"])
        all["Memory_Bound"] = (all[code("MEM_STALL_ANY_LOAD")] + all[code("MEM_STALL_ANY_STORE")])/all[code("EXEC_STALL_CYCLE")]
        all["L1_Bound"] = (all[code("MEM_STALL_ANY_LOAD")] - all[code("MEM_STALL_L1MISS")])/all[code("EXEC_STALL_CYCLE")]
        all["L2_Bound"] = (all[code("MEM_STALL_L1MISS")] - all[code("MEM_STALL_L2MISS")])/all[code("EXEC_STALL_CYCLE")]
        all["L3_Bound_or_DRAM"] = all[code("MEM_STALL_L2MISS")]/all[code("EXEC_STALL_CYCLE")]
        all["Store_Bound"] = all[code("MEM_STALL_ANY_STORE")]/all[code("EXEC_STALL_CYCLE")]
        all["Core_Bound"] = (all[code("EXEC_STALL_CYCLE")] - all[code("MEM_STALL_ANY_LOAD")] - all[code("MEM_STALL_ANY_STORE")])/all[code("EXEC_STALL_CYCLE")]

        all["LL_hit_rate"] = 1.0 - all[code("LL_CACHE_MISS")]/all[code("LL_CACHE")]
        all["Remote_accesses"] = all[code("REMOTE_ACCESS")]/(all[code("MEM_ACCESS_LD")] + all[code("MEM_ACCESS_ST")])

    # remove hardware events from resulting dict
    for key in all.copy():
        if key in hardware_events:
            del all[key]

    return all


def add_metrics_to_file(test_name, metrics):
    need_header = False
    if os.path.exists(output_file_name):
        append_write = 'a' # append if already exists
    else:
        append_write = 'w' # make a new file if not
        need_header = True

    with open(output_file_name, append_write) as output_file:
        if need_header:
            output_file.write("test_name,")
            for key in metrics:
                output_file.write(str(key) + ",")
            output_file.write("\n")

        output_file.write(test_name + ",")
        for key in metrics:
            output_file.write(str(metrics[key]) + ",")
        output_file.write("\n")
        output_file.write("\n")


if __name__ == "__main__":
    pos = 0
    prog_name = ""
    test_name = ""
    prog_args = []
    for arg in sys.argv:
        if pos == 1:
            prog_name = str(arg)
        elif pos == 2:
            test_name = str(arg)
        elif pos > 0: # if not a script name
            tmp_list = str(arg).split(" ")
            if '' in tmp_list:
                tmp_list.remove('')
            prog_args += tmp_list
        pos += 1

    arch = get_arch()

    no_conflict_event_list = get_no_conflict_events_list(arch)
    conflicted_event_list = get_conflicted_events_list(arch)

    hardware_events = {}
    hardware_events = merge_two_dicts(hardware_events, collect_list_of_events(prog_name, prog_args, no_conflict_event_list))
    for conflicted_event in conflicted_event_list:
        hardware_events = merge_two_dicts(hardware_events, collect_list_of_events(prog_name, prog_args, [conflicted_event]))

    app_metrics = analyse_events(arch, hardware_events)
    add_metrics_to_file(test_name, app_metrics)


