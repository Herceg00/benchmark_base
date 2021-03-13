from scripts.get_profile_metrics import run_profiling
from scripts.get_timings import run_timings
from scripts.arch_properties import get_threads_count, get_cores_count, get_sockets_count
import os
import optparse
import shutil
from scripts.roofline import generate_roofline_from_profiling_data
from scripts.arch_properties import get_arch


all_tests_data = {"triada" : {"mode": 0,
                              "length": 800000},
                  "stencil_1D" : {"mode": {"min": 0, "max": 4, "step": 1},
                                  "length": {"min": 1024, "max": 2048, "step": 256},
                                  "radius": 4}
                  }


def get_bench_table_name(bench_name, parameters_string):
    return bench_name + "|" + parameters_string.replace(" ", "")


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


def run_tests_across_specific_parameter(bench_name, parameter_name, all_parameters_data, prev_params, options):
    parameter_info = all_parameters_data[parameter_name]
    for i in range(get_min(parameter_info), get_max(parameter_info), get_step(parameter_info)):
        parameters_string = prev_params + " " + "--"+parameter_name+"="+str(i)

        next_parameter = next_key(all_parameters_data, parameter_name)
        if not next_parameter:
            bench_table_name = get_bench_table_name(bench_name, parameters_string)

            run_timings(bench_name, bench_table_name, parameters_string.split(" "))
            if options.profile == "True":
                run_profiling(bench_name, bench_table_name, parameters_string.split(" "))
        else:
            run_tests_across_specific_parameter(bench_name, next_parameter, all_parameters_data, parameters_string, options)


def run_benchmark(bench_name, bench_params, options):  # benchmarks a specified application
    # get first parameter (usually mode)
    first_parameter = next(iter(bench_params))

    # generate list of parameters to be passed into benchmark
    list_of_params = "--threads=" + str(get_cores_count())

    # recursively run benchmark for all combinations of input params
    run_tests_across_specific_parameter(bench_name, first_parameter, bench_params, list_of_params, options)


def init():
    shutil.rmtree("./output/")
    os.makedirs("./output/", exist_ok=True)


def generate_roofline():
    arch = get_arch()
    roofline_file_name = "./output/" + arch + "_roofline.csv"
    generate_roofline_from_profiling_data(roofline_file_name, "Kunpeng Roofline Model")


if __name__ == "__main__":
    # create .csv files
    init()

    # parse arguments
    parser = optparse.OptionParser()
    parser.add_option('-b', '--bench',
                      action="store", dest="bench",
                      help="specify benchmark to test (or all for testing all)", default="all")
    parser.add_option('-p', '--profile',
                      action="store_false", dest="profile",
                      help="set true if metrics profiling is needed", default=True)

    options, args = parser.parse_args()

    # run tests
    for current_test, test_parameters in all_tests_data.items():
        if options.bench == "all" or current_test in options.bench:
            run_benchmark(current_test, test_parameters, options)

    # generate roofline model
    generate_roofline()
