#include <iostream>
#include <string>
#include <vector>
#include <set> 
#include <omp.h>
#include <sched.h>

double *res;

void print_stats(csr &matrix)
{
    omp_set_num_threads(omp_get_max_threads());
    
    vector<unsigned long> iter_per_thread(omp_get_max_threads());

    FILE *fres = fopen("spmv_stats.txt", "a");

    fprintf(fres, "N: %d\nNZ_IN_A_ROW: %d\nMODE(0 - static, 1 - guided, 2 - static): %d\nRAND_MODE(0 - norm. dist./1 - gaussiana): %d\n", LENGTH, RADIUS, MODE, RAND_MODE);

    fprintf(fres, "\nPRINTING THE DISTRIBUTION OF MATRIX NON-ZERO ELEMENTS BETWEEN THREADS...\n\n");

    if (MODE == 0) {
        fprintf(fres, "\n--------------static schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
        {
            unsigned long elements_per_thread = 0;
            #pragma omp for schedule(static)
            for (int i=0; i<LENGTH; i++)
            {
                elements_per_thread += matrix.row_indexing[i + 1] - matrix.row_indexing[i];
            }
            iter_per_thread[omp_get_thread_num()] = elements_per_thread;
        }

    }
    else if (MODE == 1) {
        fprintf(fres, "\n--------------guided schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
        {
            unsigned long elements_per_thread = 0;
            #pragma omp for schedule(guided)
            for (int i=0; i<LENGTH; i++)
            {
                elements_per_thread += matrix.row_indexing[i + 1] - matrix.row_indexing[i];
            }
            iter_per_thread[omp_get_thread_num()] = elements_per_thread;
        }
 
    } else if (MODE == 2) {
        fprintf(fres, "\n--------------dynamic schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
        {
            unsigned long elements_per_thread = 0;
            #pragma omp for schedule(dynamic)
            for (int i=0; i<LENGTH; i++)
            {
                elements_per_thread += matrix.row_indexing[i + 1] - matrix.row_indexing[i];
            }
            iter_per_thread[omp_get_thread_num()] = elements_per_thread;
        }
    } else {
        fprintf(fres, "\nUNKNOWN MODE\n\n");
    }
    for (int i = 0; i < iter_per_thread.size(); i++) {
        fprintf(fres, "%lu\n", iter_per_thread[i]);
    }
    if (MODE == 1) {
        sort(iter_per_thread.begin(), iter_per_thread.end(), greater<unsigned long>());
        fprintf(fres, "\n-------sorted-------\n\n");
        for (int i = 0; i < iter_per_thread.size(); i++) {
            fprintf(fres, "%lu\n", iter_per_thread[i]);
        }
    }
    fclose(fres);
}

void Init(csr &matrix)
{
    res = (double *)malloc(sizeof(double) * LENGTH);
//    print_stats(matrix);
}

void mul_with_static_schedule(csr &matrix, vector<base_type> &vec)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(static)
        for (int i=0; i<LENGTH; i++)
        {
            double result = 0.0;
            for (int j = matrix.row_indexing[i]; j < matrix.row_indexing[i + 1]; j++)
            {
                int cidx = matrix.column_indexes[j];
                result += matrix.value[j] * vec[cidx];
            }
            res[i] = result;
        }
    }
}

void mul_with_guided_schedule(csr &matrix, vector<base_type> &vec)
{
    #pragma omp parallel    
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(guided)
        for (int i=0; i<LENGTH; i++)
        {
            double result = 0.0;
            for (int j = matrix.row_indexing[i]; j < matrix.row_indexing[i + 1]; j++)
            {
                int cidx = matrix.column_indexes[j];
                result += matrix.value[j] * vec[cidx];
            }
            res[i] = result;
        }
    }
}

void mul_with_dynamic_schedule(csr &matrix, vector<base_type> &vec)
{
    #pragma omp parallel
    {
        unsigned int myseed = omp_get_thread_num();
        #pragma omp for schedule(dynamic)
        for (int i=0; i<LENGTH; i++)
        {
            double result = 0.0;
            for (int j = matrix.row_indexing[i]; j < matrix.row_indexing[i + 1]; j++)
            {
                int cidx = matrix.column_indexes[j];
                result += matrix.value[j] * vec[cidx];
            }
            res[i] = result;
        }
    }
}


void Kernel(int core_type, csr &matrix,  vector<base_type> &v)
{
    switch (core_type) {
        case 0:
            mul_with_static_schedule(matrix, v);
            break;
        case 1:
            mul_with_guided_schedule(matrix, v);
            break;
        case 2:
            mul_with_dynamic_schedule(matrix, v);
            break;
        default: fprintf(stderr, "Unknown core type\n");
    }
}
