#ifndef TIMERS_H
#define TIMERS_H
#include <ios>

class PerformanceCounter {
    double total_time = 0;
    double total_bw = 0;
    double total_flops = 0;
    std::chrono::time_point<std::chrono::steady_clock> start_time = std::chrono::steady_clock::now();
    std::chrono::time_point<std::chrono::steady_clock> end_time = std::chrono::steady_clock::now();
    double local_bw = 0;
    double local_time = 0;
    double local_flops = 0;

public:


    void start_timing(void) {
        start_time = std::chrono::steady_clock::now();
    }

    void end_timing(void) {
        end_time = std::chrono::steady_clock::now();
    }

    void update_counters(size_t bytes_requested, size_t flops_executed) {
        std::chrono::duration<double> elapsed_seconds = end_time - start_time;
        local_time = elapsed_seconds.count();
        local_bw = bytes_requested * 1e-9 / local_time;
        local_flops = flops_executed * 1e-9 / local_time;
        total_bw += local_bw;
        total_time += local_time;
        total_flops += local_flops;
    }

    void print_local_counters(void) {
        std::cout << std::fixed;
        std::cout << "local_time: " << local_time << " s" << std::endl;
        std::cout << "local_bw: " << local_bw << " Gb/s" << std::endl;
        std::cout << "local_flops: " << local_flops << " GFlops" << std::endl;
    }

    void print_average_counters(bool flops_required) {
        std::cout << std::fixed;
        std::cout << "avg_time: " << total_time/LOC_REPEAT << " s" << std::endl;
        std::cout << "avg_bw: " << total_bw/LOC_REPEAT << " Gb/s" << std::endl;
        if (flops_required) {
            std::cout << "avg_flops: " << total_flops/LOC_REPEAT << " GFlops" << std::endl;
        }
    }
};

#endif
