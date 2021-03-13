from scripts.get_profile_metrics import run_profiling
from scripts.get_timings import run_timings
from scripts.arch_properties import get_threads_count, get_cores_count, get_sockets_count
import os
import optparse


all_tests_data = {"triada" : {"mode": 0,
                              "length": 800000},
                  "stencil_1D" : {"mode": {"min": 0, "max": 4, "step": 1},
                                  "length": {"min": 1024, "max": 2048, "step": 256},
                                  "radius": 4}
                  }

common_params = ""


def get_bench_table_name(bench_name, parameters_string):
    formated_string = parameters_string
    formated_string.replace(" ", "|")
    return bench_name + "|" + formated_string


def next_key(dict, key):
    keys = iter(dict)
    key in keys
    return next(keys, False)


def get_min(parameter_info):
    if isinstance(parameter_info, dict):
        return parameter_info["min"]
    else:
        return parameter_info


def get_max(parameter_info):
    if isinstance(parameter_info, dict):
        return parameter_info["max"]
    else:
        return parameter_info + 1


def get_step(parameter_info):
    if isinstance(parameter_info, dict):
        return parameter_info["step"]
    else:
        return 1


def run_tests_across_specific_parameter(bench_name, parameter_name, all_parameters_data, prev_params):
    parameter_info = all_parameters_data[parameter_name]
    for i in range(get_min(parameter_info), get_max(parameter_info), get_step(parameter_info)):
        parameters_string = prev_params + " " + "--"+parameter_name+"="+str(i)

        next_parameter = next_key(all_parameters_data, parameter_name)
        if not next_parameter:
            bench_table_name = get_bench_table_name(bench_name, parameters_string)

            run_timings(bench_name, bench_table_name, parameters_string.split(" "))
            run_profiling(bench_name, bench_table_name, parameters_string.split(" "))
        else:
            run_tests_across_specific_parameter(bench_name, next_parameter, all_parameters_data, parameters_string)


def run_benchmark(bench_name, bench_params):  # benchmarks a specified application
    # get first parameter (usually mode)
    first_parameter = next(iter(bench_params))

    # generate list of parameters to be passed into benchmark
    list_of_params = "--threads=" + str(get_cores_count())

    # recursively run benchmark for all combinations of input params
    run_tests_across_specific_parameter(bench_name, first_parameter, bench_params, list_of_params)


def init():
    os.makedirs("./output/", exist_ok=True)


if __name__ == "__main__":
    # create .csv files
    init()

    # parse arguments
    parser = optparse.OptionParser()
    parser.add_option('-b', '--bench',
                      action="store", dest="bench",
                      help="specify benchmark to test (or all for testing all)", default="all")
    parser.add_option('-p', '--profile',
                      action="store", dest="profile",
                      help="set true if metrics profiling is needed", default="False")
    options, args = parser.parse_args()

    # run tests
    for current_test, test_parameters in all_tests_data.items():
        if options.bench == "all" or current_test in options.bench:
            run_benchmark(current_test, test_parameters)
