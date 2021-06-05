//g++ -fopenmp -march=armv8.2-a thrdtest.cpp

// 24 threads: OMP: 12.82 us, ARM: 2.69 us
// 48 threads: OMP: 31.17 us, ARM: 11.00 us

//#define __PRINT_STATS__


#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <omp.h>
#include <unistd.h>
#include <iostream>
#include <limits>

using namespace std;

//typedef unsigned long long int ulong;

#pragma omp barrier
data[tid]=Rdtsc();

static inline ulong  Rdtsc()
{
    ulong  count_num;
    __asm__ __volatile__("mrs %0, cntvct_el0" : "=r" (count_num));
    return count_num;
}


#define ATTYPE char

#define ATOMIC(mnem,code) static inline ATTYPE mnem (ulong p, ATTYPE s) \
{ ATTYPE t; \
 __asm__ __volatile__( code  : "=r"(t), "+m"(*(ATTYPE*)p) : "r"(s) ); \
 return t; }
//-end-macro

static inline void YIELD(void) { __asm__("yield"); }

ATOMIC(AtomicAdd, "ldaddalb %w0, %w2, %1");
ATOMIC(AtomicClr, "ldclralb %w0, %w2, %1");
ATOMIC(AtomicSet, "ldsetalb %w0, %w2, %1");
ATOMIC(AtomicXor, "ldeoralb %w0, %w2, %1");

#define ARM_BARRIER { volatile int test, _one=-1; LOC=!LOC; \
  test=AtomicAdd((ulong)(&BAR),_one)&0xFF;\
  if(test==1) { BAR=NT; LOCK=LOC; } else while(LOCK!=LOC) YIELD; }
//--end-macro



constexpr int WIDTH=16, MAXTHREADS=128;

static ulong Base[WIDTH*MAXTHREADS];
static int Mark[MAXTHREADS];

#include <float.h>

void RunOMP(double &_min_time)
{
    memset(Base,0,sizeof(Base));
    int NT=omp_get_max_threads(),i;
    int ERR=0;
    memset(Mark,0,sizeof(Mark));

#pragma omp parallel  default(none), shared(Base,Mark,ERR,NT)
    {
        int MyT=omp_get_thread_num();
        ulong* data=Base+MyT*WIDTH;

        data[0]=Rdtsc();

#ifdef __LARGE_BB__
#define BB(n)	data[n]=Rdtsc(); Mark[MyT]=n;\
        for(int j=0; j<NT; ++j) { int M=Mark[j]; if( (M!=n) && (M!=(n-1))) ERR=1; }
#else
#define BB(n)	data[n]=Rdtsc();
#endif

/*
#define BB(n)	data[n]=Rdtsc();

#define BB(n)	data[n]=Rdtsc(); Mark[MyT]=n;\
        for(int j=0; j<NT; ++j) { int M=Mark[j]; if( (M!=n) && (M!=(n-1))) ERR=1; }*/

#pragma omp barrier
        BB(1)
#pragma omp barrier
        BB(2)
#pragma omp barrier
        BB(3)
#pragma omp barrier
        BB(4)
#pragma omp barrier
        BB(5)
#pragma omp barrier
        BB(6)
#pragma omp barrier
        BB(7)
#pragma omp barrier
        BB(8)
#pragma omp barrier
        BB(9)
#pragma omp barrier
        BB(10)
#pragma omp barrier
        BB(11)
#pragma omp barrier
        BB(12)
#pragma omp barrier
        BB(13)
#pragma omp barrier
        BB(14)
#pragma omp barrier
        BB(15)
    } // parallel

    if(ERR) puts(" OMP:ERROR! ");
    ERR=0;
    ulong sum=0;
    for(i=0; i<NT;++i)
        sum+=Base[i*WIDTH+15]-=Base[i*WIDTH];
    float resOMP=((float)sum)/15.0f/NT/100.0f; // 100 MHZ clock
    //double resOMP = ((double)sum)/15.0f/NT/(2.3*1e3);

    if(_min_time > resOMP)
        _min_time = resOMP;


    /*volatile int BAR=NT,LOCK=0;

    memset(Base,0,sizeof(Base));
    memset(Mark,0,sizeof(Mark));

    #pragma omp parallel  default(none), shared(Base,BAR,Mark,ERR,LOCK,NT)
    {
        int LOC=LOCK;
        int MyT=omp_get_thread_num();
        ulong* data=Base+MyT*WIDTH;

        data[0]=Rdtsc();

        ARM_BARRIER
        BB(1)
        ARM_BARRIER
        BB(2)
        ARM_BARRIER
        BB(3)
        ARM_BARRIER
        BB(4)
        ARM_BARRIER
        BB(5)
        ARM_BARRIER
        BB(6)
        ARM_BARRIER
        BB(7)
        ARM_BARRIER
        BB(8)
        ARM_BARRIER
        BB(9)
        ARM_BARRIER
        BB(10)
        ARM_BARRIER
        BB(11)
        ARM_BARRIER
        BB(12)
        ARM_BARRIER
        BB(13)
        ARM_BARRIER
        BB(14)
        ARM_BARRIER
        BB(15)
    } // parallel

    if(ERR) puts(" ARM:ERROR! ");
    sum=0;
    for(i=0; i<NT;++i) sum+=Base[i*WIDTH+15]-=Base[i*WIDTH];
    float resARM=((float)sum)/15.0f/NT/100.0f; // 100 MHZ clock*/

    #ifdef __PRINT_STATS__
    //printf("\n %2i threads: OMP: %.2f us, ARM: %.2f us ",NT, resOMP,resARM);
    cout << "OMP time: " << resOMP << " us" << endl;
    #endif

}//RunOMP





int main(int argc, char** argv)
{
    const int tmax=omp_get_max_threads();

    ulong ll;

    #ifdef __PRINT_STATS__
    int min_core = MAXTHREADS;
    int max_core = -1;
    #pragma omp parallel
    {
        int thread_num = omp_get_thread_num();
        int cpu_num = sched_getcpu();
        if(cpu_num > max_core)
            max_core = cpu_num;
        if(cpu_num < min_core)
            min_core = cpu_num;
        printf("Thread %3d is running on CPU %3d\n", thread_num, cpu_num);
    }
    cout << "cores: [" << min_core << ", " << max_core << "]" << endl;
    #endif

    double min_time = std::numeric_limits<double>::max();
    for(int i=0; i<10; ++i)
        RunOMP(min_time );
    #ifdef __PRINT_STATS__
    cout << endl << "MIN time: " << min_time << endl;
    #else
    cout << min_time << endl;
    #endif

    return 0;
}