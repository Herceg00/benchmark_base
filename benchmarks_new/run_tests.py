from scripts.get_profile_metrics import run_profiling, profiling_add_separator
from scripts.get_timings import run_timings, timings_add_separator
from scripts.arch_properties import get_threads_count, get_cores_count, get_sockets_count
import os
import optparse
import shutil
from scripts.roofline import generate_roofline_from_profiling_data
from scripts.arch_properties import get_arch

linear_length = 800000000

all_tests_data = {"triada": {"mode": {"min": 0, "max": 9, "step": 1},
                             "length": linear_length},
                  "stencil_1D": {"mode": 0,
                                 "length": linear_length,
                                 "radius": {"min": 1, "max": 12, "step": 1}},
                  "stencil_2D": {"mode": {"min": 0, "max": 1, "step": 1},
                                 "radius": {"min": 1, "max": 3, "step": 1},
                                 "length": {"min": 256, "max": 32768, "step": "2_pow"}},
                  "stencil_3D": {"mode": {"min": 0, "max": 1, "step": 1},
                                 "radius": {"min": 1, "max": 3, "step": 1},
                                 "length": {"min": 64, "max": 512, "step": "2_pow"}},
                  "matrix_transp": {"mode": {"min": 0, "max": 3, "step": 1},
                                    "length": {"min": 256, "max": 32768, "step": "2_pow"}},
                  "matrix_mul": {"mode": {"min": 0, "max": 6, "step": 1},
                                 "length": {"min": 256, "max": 2048, "step": "2_pow"}},
                  "lc_kernel": {"length": {"min": 16, "max": 256, "step": "2_pow"}},
                  "random_access": {"length": linear_length,
                                    "mode": {"min": 0, "max": 1, "step": 1},
                                    "radius": {"min": 2, "max": 2097152, "step": "2_pow"}}, # from 2 KB to 2 GB
                  "cache_bandwidths": {"length": 1024*1024*2,
                                       "mode": 0},
                  "rand_generator": {"length": linear_length,
                                     "mode": 0}
                  }

RA_RADIUS="2" # 2 KB
RA_MAX_RAD="2097152" # 2 GB


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
        return parameter_info


def get_step(parameter_info, i):
    if isinstance(parameter_info, dict):
        if parameter_info["step"] == "2_pow":
            return i*2
        else:
            return i + parameter_info["step"]
    else:
        return i + 1


def run_tests_across_specific_parameter(bench_name, parameter_name, all_parameters_data, prev_params, options):
    parameter_info = all_parameters_data[parameter_name]

    i = get_min(parameter_info)
    while i <= get_max(parameter_info):
        parameters_string = prev_params + " " + "--"+parameter_name+"="+str(i)

        next_parameter = next_key(all_parameters_data, parameter_name)
        if not next_parameter:
            bench_table_name = get_bench_table_name(bench_name, parameters_string)

            run_timings(bench_name, bench_table_name, parameters_string.split(" "))
            if options.profile:
                run_profiling(bench_name, bench_table_name, parameters_string.split(" "))
        else:
            run_tests_across_specific_parameter(bench_name, next_parameter, all_parameters_data, parameters_string, options)

        if i == get_max(parameter_info):
            timings_add_separator()
            profiling_add_separator()

        i = get_step(parameter_info, i)


def run_benchmark(bench_name, bench_params, options):  # benchmarks a specified application
    # get first parameter (usually mode)
    first_parameter = next(iter(bench_params))

    # generate list of parameters to be passed into benchmark (currently only threads)
    threads = get_cores_count()
    if options.sockets > 1:
        threads = options.sockets * threads
    list_of_params = "--threads=" + str(threads)

    # recursively run benchmark for all combinations of input params
    run_tests_across_specific_parameter(bench_name, first_parameter, bench_params, list_of_params, options)


def run_single_benchmark(bench_name, options):
    threads = get_cores_count()
    if options.sockets > 1:
        threads = options.sockets * threads
    print("using " + str(threads) + " threads")
    parameters_string = "--threads=" + str(threads) + " " + options.force

    bench_table_name = get_bench_table_name(bench_name, parameters_string)

    run_timings(bench_name, bench_table_name, parameters_string.split(" "))
    if options.profile:
        run_profiling(bench_name, bench_table_name, parameters_string.split(" "))


def init():
    if os.path.exists("./output/"):
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
                      help="specify benchmark to test (or all for testing all available benchmarks)", default="all")
    parser.add_option('-f', '--force',
                      action="store", dest="force",
                      help="specify parameters for running a single benchmark, for example --mode=0 --length=1000 "
                           "--radius=3 for 1D stencil", default="")
    parser.add_option('-p', '--profile',
                      action="store_true", dest="profile",
                      help="use to collect hardware events available (including top-down analysis)", default=False)
    parser.add_option('-s', '--sockets',
                      action="store", dest="sockets",
                      help="set number of sockets used", default=1)

    options, args = parser.parse_args()

    if options.force != "":  # run single test
        print("FORCE RUN")
        run_single_benchmark(options.bench, options)
    else:  # run tests
        for current_test, test_parameters in all_tests_data.items():
            if options.bench == "all" or current_test in options.bench:
                run_benchmark(current_test, test_parameters, options)

    # generate roofline model
    # generate_roofline()
