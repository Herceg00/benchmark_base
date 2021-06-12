#include <string>
#include <vector>
#include <omp.h>
#include <sched.h>

using std::string;

int ret_code;
int M, N, nz;   //M is row number, N is column number and nz is the number of entry
int tmp, i, j, vecdim, *rIndex, *cIndex, *rsIndex, *reIndex;
double *val, *res, *vIndex, *res_seq;

void print_stats()
{
    omp_set_num_threads(THREADS);

    FILE *fres = fopen("spmv_stats.txt", "a");

    fprintf(fres, "N: %d\nNZ_PART: %d\nMODE(0 - static, 2 - guided, 3 - static): %d\nRAND_MODE(0 - norm. dist./1 - gaussiana): %d\n", LENGTH, RADIUS, MODE, RAND_MODE);

    fprintf(fres, "\nPRINTING THE DISTRIBUTION OF MATRIX NON-ZERO ELEMENTS BETWEEN THREADS...\n");

    if (MODE == 0) {
        fprintf(fres, "\n--------------static schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
            {
                int elements_per_thread = 0;
                #pragma omp for private(j, tmp) schedule(static)
                for (i=0; i<M; i++)
                {
                        for (j = rsIndex[i]; j <= reIndex[i]; j++) {
                            elements_per_thread++;
                        }
                }
                fprintf(fres, "%d\n", elements_per_thread);
            }

    }
    else if (MODE == 1) {
        fprintf(fres, "\n--------------guided schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
            {
                int elements_per_thread = 0;
                #pragma omp for private(j, tmp) schedule(guided)
                for (i=0; i<M; i++)
                {
                        for (j = rsIndex[i]; j <= reIndex[i]; j++) {
                            elements_per_thread++;
                        }
                }
                fprintf(fres, "%d\n", elements_per_thread);
            }

    } else if (MODE == 2) {
        fprintf(fres, "\n--------------dynamic schedule--------------\n\n");

        #pragma omp parallel num_threads(THREADS)
            {
                int elements_per_thread = 0;
                #pragma omp for private(j, tmp) schedule(dynamic)
                for (i=0; i<M; i++)
                {
                        for (j = rsIndex[i]; j <= reIndex[i]; j++) {
                            elements_per_thread++;
                        }
                }
                fprintf(fres, "%d\n", elements_per_thread);
            }
            fclose(fres);
    } else {
        fprintf(fres, "\nUNKNOWN MODE\n\n");
    }
    fclose(fres);
}

void Init(vector<nz_matrix_element> &matrix, matrix_head &head, vector<double> &v)
{
    M = head.rows_am;
    N = head.columns_am;
    nz = head.nz;

    /* reseve memory for matrices */
    rIndex = (int *)malloc(nz * sizeof(int));
    cIndex = (int *)malloc(nz * sizeof(int));
    val = (double *)malloc(nz * sizeof(double));
    res = (double*)malloc(M*sizeof(double));

    for (int i = 0; i < nz; i++) {
        rIndex[i] = matrix[i].row_index;
        cIndex[i] = matrix[i].column_index;
        val[i] = matrix[i].value;
    }

    vecdim = v.size();

    if (vecdim != M)
    {
        printf("dimension mismatch!\n");
        exit(1);
    }

    //the original calculation result
    res_seq = (double*)calloc(M, sizeof(double));

    getmul(val, v, rIndex, cIndex, nz, res_seq);

    vIndex = (double*)calloc(nz, sizeof(double));
    for (int i = 0; i < nz; i++)
    {
        vIndex[i] = (double)rIndex[i] * N + cIndex[i];
        if (vIndex[i] < 0)
        {
            printf("Error!\n");
            exit(1);
        }
    }

    quicksort(val, vIndex, rIndex, cIndex, nz);

    //We use rsIndex/reIndex to keep the start/end position of each row. The intial values are 
    //-1 or -2 for all entries.  rsIndex[i] indicates the start poistion of the i-th row. Hence 
    //the position index of the i-th row is from rsIndex[i] to reIndex[i]
    rsIndex = (int*)calloc(M, sizeof(int)); //start/end position of each row

    reIndex = (int*)calloc(M, sizeof(int));
       
    for (int i = 0; i<nz; i++)
    {
        int tmp = (int)(vIndex[i] / N);
        if (rsIndex[tmp] == -1)
        {
            rsIndex[tmp] = i;
            reIndex[tmp] = i;
        }
        else
            reIndex[tmp] = i;
    } 

    print_stats();

}


void mul_with_static_schedule(vector<nz_matrix_element> &matrix, matrix_head &head, vector<double> &v)
{
    omp_set_num_threads(THREADS);
    #pragma omp parallel num_threads(THREADS)
    {
        #pragma omp for private(j, tmp) schedule(static)
        for (int i=0; i<M; i++)
        {
            double result = 0.0;
            for (j = rsIndex[i]; j <= reIndex[i]; j++)
            {
                tmp = cIndex[j];
                result += val[j] * v[tmp];
            }
            res[i] = result;
        }
    }
}

void mul_with_guided_schedule(vector<nz_matrix_element> &matrix, matrix_head &head, vector<double> &v)
{
    omp_set_num_threads(THREADS);
    #pragma omp parallel num_threads(THREADS)
    {
        #pragma omp for private(j, tmp) schedule(guided)
        for (int i=0; i<M; i++)
        {
            double result = 0.0;
            for (j = rsIndex[i]; j <= reIndex[i]; j++)
            {
                tmp = cIndex[j];
                result += val[j] * v[tmp];
            }
            res[i] = result;
        }
    }
}

void mul_with_dynamic_schedule(vector<nz_matrix_element> &matrix, matrix_head &head, vector<double> &v)
{
    omp_set_num_threads(THREADS);
    #pragma omp parallel num_threads(THREADS)
    {
        #pragma omp for private(j, tmp) schedule(dynamic)
        for (int i=0; i<M; i++)
        {
            double result = 0.0;
            for (j = rsIndex[i]; j <= reIndex[i]; j++)
            {
                tmp = cIndex[j];
                result += val[j] * v[tmp];
            }
            res[i] = result;
        }
    }
}


void Kernel(int core_type, vector<nz_matrix_element> &matrix, matrix_head &head, vector<double> &v)
{
    switch (core_type) {
        case 0:
            mul_with_static_schedule(matrix, head, v);
            break;
        case 1:
            mul_with_guided_schedule(matrix, head, v);
            break;
        case 2:
            mul_with_dynamic_schedule(matrix, head, v);
            break;
        default: fprintf(stderr, "Unknown core type\n");
    }
}
