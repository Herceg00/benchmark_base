#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <climits>
#include <random>
#include <cstdio>
#include <cstdlib>
//#include <cdouble>
#include <set>

using namespace std;

struct coord {
    int y;
    int x;
};  

struct nz_matrix_element {
    int row_index;
    int column_index;
    double value;
};

struct matrix_head {
    int rows_am;
    int columns_am;
    int nz;
};

bool operator<(const coord &a, const coord &b) {
    if (a.y != b.y) {
        return a.y < b.y;
    }
    return a.x < b.x;
}

void get_matrix1(vector<nz_matrix_element> &v, matrix_head &h) //square matrix
{
    srand(time(0));

    int nz = (LENGTH * LENGTH) / 100 * RADIUS;
    h.rows_am = LENGTH;
    h.columns_am = LENGTH;
    h.nz = nz;

    set<coord> s;

    for (int i = 0; i < nz; i++) {
        coord a;
        a.y = rand() % LENGTH;
        a.x = rand() % LENGTH;
        while (s.find(a) != s.end()) {
            a.y = rand() % LENGTH;
            a.x = rand() % LENGTH;
        }   
        double val = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / SHRT_MAX)) - static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / SHRT_MAX)); 
        v.push_back({a.y, a.x, val});
    }
}

void get_matrix2(vector<nz_matrix_element> &v, matrix_head &h)
{
    default_random_engine generator;
    normal_distribution<double> distribution(-LENGTH, LENGTH);

    int nz = (LENGTH * LENGTH) / 100 * RADIUS;
    h.rows_am = LENGTH;
    h.columns_am = LENGTH;
    h.nz = nz;

    set<coord> s;

    for (int i = 0; i < nz; i++) {
        coord a;
        a.y = abs((int) distribution(generator)) % LENGTH;
        a.x = abs((int) distribution(generator)) % LENGTH;
        while (s.find(a) != s.end()) {
            a.y = abs((int) distribution(generator)) % LENGTH;
            a.x = abs((int) distribution(generator)) % LENGTH;
        }   
        double val = static_cast <double> (distribution(generator)) / (static_cast <double> (RAND_MAX / SHRT_MAX)) - static_cast <double> (distribution(generator)) / (static_cast <double> (RAND_MAX / SHRT_MAX)); 
        v.push_back({a.y, a.x, val});
    }
}

void get_vector(vector<double> &v)
{
    srand(time(0));
    for (int i = 0; i < LENGTH; i++) {
        double val = static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / SHRT_MAX)) - static_cast <double> (rand()) / (static_cast <double> (RAND_MAX / SHRT_MAX)); 
        v.push_back(val);
    }
}

void quicksort(double* a, double* vindex, int* rindex, int* cindex, int n)
{
    int i, j, m;
    double p, t, s;
    if (n < 2)
        return;
    p = vindex[n / 2];

    for (i = 0, j = n - 1;; i++, j--) {
        while (vindex[i]<p)
            i++;
        while (p<vindex[j])
            j--;
        if (i >= j)
            break;
        t = a[i];
        a[i] = a[j];
        a[j] = t;

        s = vindex[i];
        vindex[i] = vindex[j];
        vindex[j] = s;

        m = rindex[i];
        rindex[i] = rindex[j];
        rindex[j] = m;

        m = cindex[i];
        cindex[i] = cindex[j];
        cindex[j] = m;
    }
    quicksort(a, vindex, rindex, cindex, i);
    quicksort(a + i, vindex + i, rindex + i, cindex + i, n - i);
}

void getmul(double *val, vector<double> &vec, int* rIndex, int*cIndex, int nz, double* res)
{
    int i; 
    for (i = 0; i < nz; i++)
    {
        int rInd = rIndex[i];
        int cInd = cIndex[i];
        res[rInd] += val[i] * vec[cInd];
    }
}

