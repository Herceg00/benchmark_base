
#ifndef H_SERVICE
#define H_SERVICE

#include "include.h"

/**
 * output of service parameters 
 **/
void serviceOutput();

/** 
 * output of all characteristics and their variances 
 **/
void chOutput(Features& chars, luint num_lines, luint num_unique, luint tmp_p);

/** 
 * output of all characteristics and their variances into file in convenient form 
 **/
void chFileOutput(int type, Features& chars, luint num_lines, ldouble num_clusters, ldouble avg_len_clusters, ldouble avg_len_power, ldouble avg_len_fragments);

/**
 * just output of whole map 
 **/
void outputMap(map<map_key,list<luint> >& ids);

/**
 * output header to file with results
 **/
void headerOutputResult();

#endif