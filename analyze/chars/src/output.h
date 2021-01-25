
#ifndef H_SERVICE
#define H_SERVICE

#include "include.h"

/** 
@brief Output of service parameters.
*/
void serviceOutput();

/** 
 @brief Output of all characteristics and their variances. 
 @param chars Characteristics to output
 @param num_lines Overall number of elements in input data
 @param num_unique Is not used.
 @param tmp_p Is not used.
 */
void chOutput(Features& chars, luint num_lines, luint num_unique, luint tmp_p);


/**
 @brief Output of all characteristics and their variances into file in convenient form. 
 @param type Type of characteristics. They may refer to: 1) sequence, 2) clusters, 3) fragment, 4) clusters (fragment).
 @param chars Characteristics to output
 @param num_lines Overall number of elements in input data
 @param num_clusters
 @param avg_len_clusters
 @param avg_len_power
 @param avg_len_fragments
 */
void chFileOutput(int type, Features& chars, luint num_lines, ldouble num_clusters, ldouble avg_len_clusters, ldouble avg_len_power, ldouble avg_len_fragments);


/**
@brief Just output of whole map <size,cluster that has such size>.
@param ids Array that hold needed map.
*/
void outputMap(map<map_key,list<luint> >& ids);

/**
@brief Output header to file with results.
*/
void headerOutputResult();

#endif