
#include "include.h"
#include "output.h"
#include "cluster.h"

/** 
 @brief This function marks cluster as already read. 
 @param map_it 
 @param list_it
 @return 
 */
int markRead(map<map_key,list<luint> >::iterator map_it, list<luint>::iterator list_it);


/** 
 @brief This function finds out what type of fragment form first_num and second_num clusters. 
 @param values Global array of input values
 @param cluster_ends Array that hold end indexes for all clusters
 @param first_num Number of first cluster
 @param second_num Number of second cluster
 @return Type of fragment. 0 - identical clusters, 1 - similar clusters, 2 - different clusters.
 */
int getType(long int* values, vector<luint>& cluster_ends, luint first_num, luint second_num);


/** 
@brief This function checks if curr_num cluster belongs to fragment that starts at beg_num and currently ends at curr_num-1.
@param values Global array of input values
@param cluster_ends Array that hold end indexes for all clusters
@param beg_num Number of cluster where fragment begins
@param curr_num Number of current cluster
@param fragm_type Type of fragment
@return True if belongs; false if not.
*/
bool ifBelongs(long int* values, vector<luint>& cluster_ends, luint beg_num, luint curr_num, int fragm_type); 


/** 
@brief This function checks if number of clusters between main_num and curr_num forms a complete fragment. 
@param values Global array of input values
@param cluster_ends Array that hold end indexes for all clusters
@param main_num Number of cluster where fragment should begin
@param curr_num Number of cluster where fragment should end
@param fragm_type This param is used to return type of fragment
@return Reason why this is not a fragment.
 */
int checkIfFragment(long int* values, vector<luint>& cluster_ends, luint main_num, luint curr_num, int& fragm_type);


/**
@brief Function for finding identical sets of clusters.
@param values Global array of input values
@param ids Array that holds map <size,cluster that has such size> for all clusters
@param cluster_ends Array that hold end indexes for all clusters
@return
*/
int findIdent(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends);



/**
@brief Refactor array of cluster.
@param values Global array of input values
@param ids Array that holds map <size,cluster that has such size> for all clusters
@param cluster_ends Array that hold end indexes for all clusters
@detailed This function gets array of clusters and refactor it so clusters are better suited for identification of similar fragments.
			Refactoring algorithm can vary.
			Current algorithm: cluster can be only split into several similar clusters. 
			Criteria of splitting - its similar clusters must be similar to two previous and to subsequent clusters.
@return Refactored array of clusters.
*/
int clustersRefactor(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends);