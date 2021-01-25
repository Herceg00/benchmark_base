
#include "include.h"
#include "output.h"
#include "cluster.h"

/** 
 * This function marks cluster as already read. 
 **/
int markRead(map<map_key,list<luint> >::iterator map_it, list<luint>::iterator list_it);


/** 
 * This function finds out what type of fragment form first_num and second_num clusters.
 * 	Return value - type of fragment. 0 - identical clusters, 1 - similar clusters, 2 - different clusters.
 **/
int getType(long int* values, vector<luint>& cluster_ends, luint first_num, luint second_num);


/** 
 * This function checks if curr_num cluster belongs to fragment that starts at beg_num and currently ends at curr_num-1.
 * 	Return value - true if belongs; false if not.
 **/
bool ifBelongs(long int* values, vector<luint>& cluster_ends, luint beg_num, luint curr_num, int fragm_type); 


/** 
 * This function checks if number of clusters between main_num and curr_num forms a complete fragment. 
 * Return value - reason why this is not a fragment.
 * Also return value is fragm_type, which contains type of fragment.
 **/
int checkIfFragment(long int* values, vector<luint>& cluster_ends, luint main_num, luint curr_num, int& fragm_type);


/**
 * function for finding identical sets of clusters
 **/
int findIdent(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends);


/**
 * This function gets array of clusters and refactor it so 
 * clusters are better suited for identification of similar fragments.
 * Refactoring algorithm can vary.
 * 		Current algorithm: cluster can be only split into several similar clusters. 
 * 		Criteria of splitting - its similar clusters must be similar to two previous and to subsequent clusters.
 * Return value - refactored array of clusters.
 **/
int clustersRefactor(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends);