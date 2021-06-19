from scripts.get_profile_metrics import run_profiling, profiling_add_separator
from scripts.get_timings import run_timings, timings_add_separator
from scripts.arch_properties import get_threads_count, get_cores_count, get_sockets_count
import os
import optparse
import shutil
from scripts.roofline import generate_roofline_from_profiling_data
from scripts.arch_properties import get_arch

linear_length = 800000000
max_threads = 18


# all upper borders are inclusive
all_tests_data = {"triada": {"radius": 256,
                             "length": linear_length,
                             "mode": {"min": 0, "max": 9, "step": 1}},
                  "stencil_1D": {"mode": {"min": 0, "max": 0, "step": 1},
                                 "length": 10000000,
                                 "radius": {"min": 17, "max": 17, "step": 1},
                                 "threads": {"min": 1, "max": max_threads, "step": 1}},
                  "stencil_2D": {"mode": {"min": 0, "max": 3, "step": 1},
                                 "radius": {"min": 1, "max": 3, "step": 1},
                                 "length": {"min": 256, "max": 131072, "step": "mult", "step_val": 2}},
                  "stencil_3D": {"mode": {"min": 0, "max": 1, "step": 1},
                                 "radius": {"min": 3, "max": 3, "step": 1},
                                 "length": {"min": 64, "max": 1800, "step": "mult", "step_val": 1.2}},
                  "matrix_transp": {"radius": {"min": 16, "max": 16, "step": 1},
                                    "mode": {"min": 0, "max": 3, "step": 1},
                                    "length": {"min": 256, "max": 131072, "step": "mult", "step_val": 2}},
                  "matrix_mult": {"mode": {"min": 0, "max": 5, "step": 1},
                                 "length": {"min": 256, "max": 2048, "step": "mult", "step_val": 2}},
                  "lc_kernel_arcavgxyz": {"mode": {"min": 0, "max": 1, "step": 1},
                                          "length": {"min": 64, "max": 512, "step": "mult", "step_val": 1.2}},
                  "lc_kernel_generic": {"radius": 8,
                                        "mode": {"min": 0, "max": 3, "step": 1},
                                        "length": {"min": 64, "max": 512, "step": "mult", "step_val": 1.2}},
                  "random_access": {"length": linear_length,
                                    "mode": 0,
                                    "radius": {"min": 256, "max": 262144, "step": "mult", "step_val": 2}},
                                    #"radius": {"min": 256, "max": 1024, "step": "mult", "step_val": 1.2}}, # from 256 KB to 64 MB in details
                                    #"radius": {"min": 2, "max": 2097152, "step": "mult", "step_val": 2}}, # from 2 KB to 2 GB
                  "cache_bandwidths": {"length": 1024*1024*2,
                                       "mode": 0},
                  "rand_generator": {"length": linear_length,
                                     "mode": 0},
                  "bfs": {"mode": 0,
                          "length": {"min": 12, "max": 23, "step": 1}
                          },
                  "bellman_ford": {"mode": 0,
                          "length": 18,
                          "threads": {"min": 1, "max": max_threads, "step": 1}
                          },
                  "page_rank": {"mode": {"min": 0, "max": 1, "step": 1},
                                "length": {"min": 12, "max": 23, "step": 1},
                                "threads": {"min": 1, "max": max_threads, "step": 1}
                                },
                  "n_body": {"mode": 0, # non-vector mode
                             "length": 4096,
                             #"length": {"min": 1024, "max": 32768, "step": "mult", "step_val": 2},
                             "threads": {"min": 1, "max": max_threads, "step": 1}},
                  "atomic_graphs": {"mode": {"min": 0, "max": 1, "step": 1},
                                    "length": {"min": 12, "max": 18, "step": 1}},
                  "strided_walks": {"length": linear_length,
                                    "mode": {"min": 0, "max": 1, "step": 1},
                                    "radius": {"min": 1, "max": 250000000, "step": "mult", "step_val": 2}},
                  "prefix_sum": {"mode": {"min": 0, "max": 0, "step": 1},
                                 "length": {"min": 100000, "max": linear_length, "step": "mult", "step_val": 2}},
                  "saxpy_satis": {"length": linear_length,
                                  "threads": {"min": 1, "max": max_threads, "step": 1}}, #actually R = num_threads
                  "spmv": {"length": {"min": 10000, "max": 10000, "step": 1000},
                           "radius": 10,
                           "mode": 0,
                           "rand_mode": 0,
                           "threads": {"min": 1, "max": max_threads, "step": 1}},
                    #length - matrix and vector dimension
                    #radius - the degree of sparsity of the matrix (in % of all matrix elements)
                    #mode - type of parallel for schedule (0 - static, 1 - guided, 2 - dynamic)
                    #rand_mode - the way to get random numbers (0 - normal distribution, 1 - gaussian)
                    #threads - amount of threads used in program
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
        if parameter_info["step"] == "mult":
            mult_val = parameter_info["step_val"]
            return int(i*mult_val)
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

            run_timings(bench_name, bench_table_name, parameters_string.split(" "), options)
            if options.profile:
                run_profiling(bench_name, bench_table_name, parameters_string.split(" "))
        else:
            run_tests_across_specific_parameter(bench_name, next_parameter, all_parameters_data, parameters_string, options)

        i = get_step(parameter_info, i)

        if i > get_max(parameter_info):
            timings_add_separator()
            profiling_add_separator()


def run_benchmark(bench_name, bench_params, options):  # benchmarks a specified application
    # get first parameter (usually mode)
    first_parameter = next(iter(bench_params))

    if "threads" in all_tests_data[bench_name].keys():
        print("Threads in param")
        list_of_params = "--redundant=" + "0"
    else:
        # generate list of parameters to be passed into benchmark (currently only threads)
        print("NO Threads in param")
        threads = get_cores_count()
        if int(options.sockets) > 1:
            threads = int(options.sockets) * threads
        list_of_params = "--threads=" + str(threads)


    # recursively run benchmark for all combinations of input params
    run_tests_across_specific_parameter(bench_name, first_parameter, bench_params, list_of_params, options)


def run_single_benchmark(options):
    if options.thread_num is None:
        threads = get_cores_count()
        if options.sockets > 1:
            threads = options.sockets * threads
    else:
        threads = options.thread_num
    print("using " + str(threads) + " thread(s)")
    parameters_string = "--threads=" + str(threads) + " " + options.force

    bench_table_name = get_bench_table_name(options.bench, parameters_string)

    run_timings(options.bench, bench_table_name, parameters_string.split(" "), options)
    if options.profile:
        run_profiling(options.bench, bench_table_name, parameters_string.split(" "))


def init():
    if os.path.exists("./output/"):
        shutil.rmtree("./output/")
    os.makedirs("./output/", exist_ok=True)


def generate_roofline():
    arch = get_arch()
    roofline_file_name = "./output/" + arch + "_roofline.csv"
    generate_roofline_from_profiling_data(roofline_file_name, "Kunpeng Roofline Model")


def check_target_bench_correctness(bench_name):
    subfolders = [ f.name for f in os.scandir("./") if f.is_dir() ]
    if not (bench_name in subfolders):
        raise ValueError('Incorrect benchmark name: {}!'.format(bench_name))


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
    parser.add_option('-c', '--compiler',
                      action="store", dest="compiler",
                      help="specify compiler used", default="g++")
    parser.add_option('-t', '--threads-num',
                      action="store", dest="thread_num",
                      help="specify thread number", default=None)

    options, args = parser.parse_args()

    if options.force != "":  # run single test
        print("FORCE RUN")
        check_target_bench_correctness(options.bench)
        run_single_benchmark(options)
    else:  # run tests
        for current_test, test_parameters in all_tests_data.items():
            if options.bench == "all" or current_test in options.bench:
                check_target_bench_correctness(current_test)
                run_benchmark(current_test, test_parameters, options)

    # generate roofline model
    # generate_roofline()
