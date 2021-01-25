#include <stdio.h>
#include "time.h"

const int N = 200000;

void Loop( )
{
    float iSum = 0.0f;

    clock_t iStart, iEnd;

    iStart = clock();
    for ( int j = 0; j < N; j++)
    for ( int i = 0; i < N; i++ )
    {
        iSum += i * (j + 1);
    }
    iEnd = clock();
    printf("Coefficient: %li clock ticks\n", iEnd - iStart );
}

void Loop2( )
{
    float iSum = 0.0f;

    clock_t iStart, iEnd;

    iStart = clock();
    for ( int j = 0; j < N; j++ )
    for ( int i = 0; i < N; i++ )
    {
        iSum += 1 * i * (j + 1) + 0 * 1;
    }
    iEnd = clock();
    printf("Coefficient: %li clock ticks\n", iEnd - iStart );
}

void Loop3( )
{
    float iSum = 0.0f;

    clock_t iStart, iEnd;

    iStart = clock();
    for ( int j = 0; j < N; j++ )
    for ( int i = 0; i < N; i++ )
    {
        iSum += 4 * i * (j + 1) + 2 * 1;
    }
    iEnd = clock();
    printf("Coefficient: %li clock ticks\n", iEnd - iStart );
}

int main(int argc, const char * argv[])
{
    Loop( );
    Loop2( );
    Loop3( );
    return 0;
}
