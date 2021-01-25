
#ifndef H_CLUSTER
#define H_CLUSTER

#include "include.h"
#include "output.h"

/**
 * counting AVG_LEN characteristic 
 **/
ldouble countAVG_LEN(long int* values, luint start, luint end);

/**
 * counting SEQ_L characteristic 
 **/
ldouble countSEQ_L(long int* values, luint start, luint end);

void nullChars(Features& chars);

/**
 * function for counting all characteristics in one cluster
 * last parameter END references to next element after the cluster, so it shouldn't be addressed
 **/
Features countChars(long int* values, long int* copy_values, luint end);

#endif