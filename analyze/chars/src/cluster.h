
#ifndef H_CLUSTER
#define H_CLUSTER

#include "include.h"
#include "output.h"

/**
 @brief Ñounting AVG_LEN characteristic 
 @param values Array of values
 @param start Start index in \values
 @param end End index in \values
 @return AVG_LEN value
 **/
ldouble countAVG_LEN(long int* values, luint start, luint end);

/**
 @brief Ñounting SEQ_L characteristic 
 @param values Array of values
 @param start Start index in \values
 @param end End index in \values
 @return SEQ_L value
 **/
ldouble countSEQ_L(long int* values, luint start, luint end);

/**
@ brief Initialize Feature variable with zeros
@ param chars Variable to make zero fill
*/
void nullChars(Features& chars);

/**
@brief Count all characteristics in one cluster
@param values Array of values (its a part of global array, starting from needed index)
@param copy_values Copy of \values (it will be ruined)
@param end Index showing where cluster ends (to go through \values from 0 till \end)
@detailed Last parameter END references to next element after the cluster, so it shouldn't be addressed
@return Counted characteristics
 **/
Features countChars(long int* values, long int* copy_values, luint end);

#endif