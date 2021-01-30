#include <iostream>
#include <chrono>
#define N 16000000

int main() {
    static double A[N];
    static double B[N];
    static double C[N];
    static double X[N];

    //Initialization
#pragma omp parallel for num_threads(96) schedule(static)
    for (int i = 0; i < int(N); i++){
        A[i] = 0;
        B[i] = std::rand();
        C[i] = std::rand();
        X[i] = std::rand();
    }
#pragma omp barrier

    auto time_start = std::chrono::steady_clock::now();

    //Kernel
#pragma omp parallel for num_threads(96) schedule(static)
    for (int j = 0; j < int(N); j++){
        A[j] = B[j] * X[j] + C[j];
    }
#pragma omp barrier

    auto time_end = std::chrono::steady_clock::now();


    std::chrono::duration<double> elapsed_seconds = time_end - time_start;

    std::cout << "elapsed time: " << elapsed_seconds.count() << "s\n";

    size_t bytes_requested = N * (4 * sizeof(double));

    printf("Memory bandwidth %lf GB/s", bytes_requested * 1e-9 / elapsed_seconds.count());

}