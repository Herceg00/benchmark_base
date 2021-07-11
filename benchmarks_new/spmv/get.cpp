#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <climits>
#include <random>
#include <cstdio>
#include <cstdlib>
#include <set>
#include <cmath>
#include <parallel/algorithm>
#include <chrono>
using namespace std;
using namespace chrono;

#define base_type double
#define nz (unsigned long long) (LENGTH) * (unsigned long long) (RADIUS)

struct csr {
    vector<base_type> value;
    vector<unsigned long long> column_indexes;
    vector<unsigned long long> row_indexing;
    csr () {
        value.resize(nz);
        column_indexes.resize(nz);
        row_indexing.resize(LENGTH + 1);
    }
};
/*
void print_csr(csr &matrix)
{
    fprintf(stderr, "-------------------------------\n");
    for (int i = 0; i < nz; i++) {
        fprintf(stderr, "%d) row: %d column: %d value: %lf\n", i, matrix.row_indexes[i], matrix.column_indexes[i], matrix.value[i]);
    }
    fprintf(stderr, "-------------------------------\n");
}
*/

csr generate_csr_matrix_ud() // uniform distribution
{

    double start, end;
    fprintf(stderr, "Creating a matrix...\n\n");

    vector<pair<int, int>> element_location(nz);
    
    fprintf(stderr, "1)Generating rows and columns. START  ----> ");
    start = omp_get_wtime();
    #pragma omp parallel 
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (unsigned long long i = 0; i < nz; i++) {
                int column = rand_r(&myseed) % LENGTH;
                int row = rand_r(&myseed) % LENGTH;
                element_location[i] = {row, column};
        }

    }
    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);

    fprintf(stderr, "2)Sorting. START  ----> ");
    start = omp_get_wtime();

    __gnu_parallel::sort(element_location.begin(), element_location.end());

    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);

    csr matrix;
    vector<int> am_in_row(LENGTH);

    fprintf(stderr, "3)Making CSR. START  ----> ");
    start = omp_get_wtime();
    #pragma omp parallel 
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (unsigned long long i = 0; i < nz; i++) {
            matrix.value[i] = static_cast <base_type> (rand_r(&myseed)) / (static_cast <base_type> (RAND_MAX / SHRT_MAX)) - static_cast <base_type> (rand_r(&myseed)) / (static_cast <base_type> (RAND_MAX / SHRT_MAX));
            matrix.column_indexes[i] = element_location[i].second;
            #pragma omp atomic
            am_in_row[element_location[i].first]++;
        }
    }
    for (unsigned long long i = 1; i <= LENGTH; i++) {
        matrix.row_indexing[i] = matrix.row_indexing[i - 1] + am_in_row[i - 1];
    }

    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);
    fprintf(stderr, "The matrix has been created\n\n");

    return matrix;
}


base_type get_thread_safe_normal_distribution_number(const double & m, const double & d) { // !!!
    static thread_local mt19937 generator; // !!!
    normal_distribution<double> distribution(m, d); // !!!
    return distribution(generator); // !!!
} // !!!

csr generate_csr_matrix_nd() // normal distribution
{
    double start, end;
    fprintf(stderr, "Creating a matrix...\n\n");

    vector<pair<int, int>> element_location(nz);

    fprintf(stderr, "1)Generating rows and columns. START  ----> ");
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (unsigned long long i = 0; i < nz; i++) {
                int column = abs(int(get_thread_safe_normal_distribution_number(LENGTH / 2, LENGTH / 5))) % LENGTH; // !!!
                int row = abs(int(get_thread_safe_normal_distribution_number(LENGTH / 2, LENGTH / 5))) % LENGTH; // !!!
                element_location[i] = {row, column};
        }

    }
    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);

    fprintf(stderr, "2)Sorting. START  ----> ");
    start = omp_get_wtime();

    __gnu_parallel::sort(element_location.begin(), element_location.end());

    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);

    csr matrix;
    vector<int> am_in_row(LENGTH);

    fprintf(stderr, "3)Making CSR. START  ----> ");
    start = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp for schedule(static)
        for (unsigned long long i = 0; i < nz; i++) {
            matrix.value[i] = get_thread_safe_normal_distribution_number(RADIUS, 1e6); // !!!
            matrix.column_indexes[i] = element_location[i].second;
            #pragma omp atomic
            am_in_row[element_location[i].first]++;
        }
    }
    for (unsigned long long i = 1; i <= LENGTH; i++) {
        matrix.row_indexing[i] = matrix.row_indexing[i - 1] + am_in_row[i - 1];
    }

    fprintf(stderr, "FINISH  ");
    end = omp_get_wtime();
    fprintf(stderr, "Time taken: %.2fs\n\n", end - start);
    fprintf(stderr, "The matrix has been created\n\n");

    return matrix;
}

void get_vector(vector<base_type> &v)
{

    v.resize(LENGTH);
    #pragma omp parallel 
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule (static)
        for (unsigned long long i = 0; i < LENGTH; i++) {
            base_type val = static_cast <base_type> (rand_r(&myseed)) / (static_cast <base_type> (RAND_MAX / SHRT_MAX)) - static_cast <base_type> (rand_r(&myseed)) / (static_cast <base_type> (RAND_MAX / SHRT_MAX)); 
            v[i] = val;
        }
    }
}

