
#include "core.h"

/******************************
**** CORE OF WHOLE PROGRAM ****
*******************************/
void main_part(luint num_lines, long int* values, long int* copy_values) {
	
	clock_t end,start,mid;
	start = clock();

	// cluster characteristics 
	luint num_clusters = 0;
	ldouble avg_len_clusters = 0;
	ldouble avg_cluster_power = 0;
	Features all_chars;
	Features current_chars;
	nullChars(all_chars);

	// here are characteristics about identical and similar clusters
	// these ones for storing id of every cluster, id is the key in map
	map<map_key,list<luint> > ids;
	map<map_key,list<luint> >::iterator it;
	// this one for storing indexes of clusters' ends
	vector<luint> cluster_ends;

	// service characteristics
    luint j = 0; // inner counter
    bool match = true; // true if current reference matches to be in current cluster
	luint start_check = 0; // index of position from which cluster check should start
	luint start_cluster = 0; // index of position from which current cluster starts
	luint l_count = 0;
	luint avg_len_count = 0;
	luint alt_alpha_count = 0;
	luint size;

#ifdef DEBUG 
	ofstream fout("_clusters.txt"); // tmp output
	fout << values[0] << endl; // tmp output
#endif

	/**
	 * main part, where we define clusters and start countChars for every found cluster
	 * characteristics counted for clusters are just averaged
	 *
	 * Rule for defining clusters: next reference must me less than N elements away from all K last references
	 */
	luint i;
	for (i = 1; i < num_lines; i++) {
        
		if (i-start_check > K) start_check++;

		for (j = start_check; j < i; j++) {
			if (abs(values[i]-values[j]) > N) {
				match = false;
				break;
			}
		}
		if (!match) {

			/*** one iteration of counting characteristics ***/
			///// for counting inner cluster characteristics
			nullChars(current_chars);
			current_chars = countChars(values+start_cluster,copy_values+start_cluster,i-start_cluster);
			if (current_chars.l != -1) all_chars.l += current_chars.l; 
			else l_count++;
			if (current_chars.avg_len != -1) all_chars.avg_len += current_chars.avg_len;
			else avg_len_count++;
			all_chars.seq_l += current_chars.seq_l;
			all_chars.alpha += current_chars.alpha; 
			if (current_chars.alt_alpha != -1) all_chars.alt_alpha += current_chars.alt_alpha;
			else alt_alpha_count++;
			all_chars.n_alpha += current_chars.n_alpha;

			// for finding identical
			if (mode == 1) {                                                
				// storing current cluster's end
				cluster_ends.push_back(i);
				// gathering id values for clusters
				size = i - start_cluster;
				if ((it=ids.find(size)) != ids.end()) {
					it->second.push_back(num_clusters);
				}
				else {
					list<luint> tmp_list;
					tmp_list.push_back(num_clusters);
					ids.insert(pair<map_key,list<luint> >(size,tmp_list) );
				}
			}

            ///// for counting general characteristics about clusters
            num_clusters++;
            avg_len_clusters += abs(values[i]-values[i-1]);

#ifdef DEBUG
            cout << "Cluster N. " << num_clusters << ",  start: " << start_cluster << ", end: " << i << endl;
            cout << current_chars.l << ", " << current_chars.avg_len << ", " << current_chars.seq_l << ", " << \
                    current_chars.alpha << ", " << current_chars.alt_alpha << ", " << current_chars.n_alpha << \
                    ", " << current_chars.p << endl;
            fout << "____" << endl; // tmp output
#endif
			/*** end of one iteration ***/
			

			// ended with this cluster, going to next one
			start_check = i;
			start_cluster = i;
			match = true;
		}
#ifdef DEBUG
		fout << values[i] << endl; // tmp output
#endif
	}
	/*** last one iteration of counting characteristics***/
	///// for counting inner cluster characteristics
	nullChars(current_chars);
	current_chars = countChars(values+start_cluster,copy_values+start_cluster,i-start_cluster);
	if (current_chars.l != -1) all_chars.l += current_chars.l;
        else l_count++;
        if (current_chars.avg_len != -1) all_chars.avg_len += current_chars.avg_len;
        else avg_len_count++;
        all_chars.seq_l += current_chars.seq_l;
        all_chars.alpha += current_chars.alpha;
        if (current_chars.alt_alpha != -1) all_chars.alt_alpha += current_chars.alt_alpha;
        else alt_alpha_count++;
        all_chars.n_alpha += current_chars.n_alpha;

	mid = clock();
	cout << "Time in main part before ident: " << (mid-start)/CLOCKS_PER_SEC << endl;
	
	/*** for finding identical ***/
	if (mode == 1) {
		// storing current cluster's end
        cluster_ends.push_back(i);
		// gathering id values for clusters
		size = i-start_cluster;
	        if ((it=ids.find(size)) != ids.end()) {
        		it->second.push_back(num_clusters);
	        } else {
        		list<luint> tmp_list;
                	tmp_list.push_back(num_clusters);
	                ids.insert(pair<map_key,list<luint> >(size,tmp_list) );
        	}
		// output of vector
		vector<luint>::iterator vect_it;
		ofstream fvect("vector.txt");
                fvect << "Number of clusters in vect: " << cluster_ends.size() << endl;
		for (vect_it = cluster_ends.begin(); vect_it < cluster_ends.end(); vect_it++) {
			fvect << (*vect_it) << endl;
		}
		// output of map
		ofstream fmap("map.txt");
		fmap << "$$$$$$$$" << endl;
		list<luint>::iterator list_it;
		for (it = ids.begin(); it != ids.end(); it++) {
			it->second.sort();
			fmap << "Size of cluster: " << it->first << ", number of clusters: " << it->second.size() << endl;
			for (list_it = it->second.begin(); list_it != it->second.end(); list_it++) {
				fmap << (*list_it) << " ";
			}
			fmap << endl;
		}

		/*** Refactoring of array of clusters before finding similar fragments***/
		clock_t before_refact = clock();

#ifdef REFACTOR
		cout << "DOING REFACTORING..." << endl;
		clustersRefactor(values,ids,cluster_ends);
#else
		cout << "NO REFACTOR" << endl;
#endif

		clock_t after_refact = clock();
		cout << "Time on modification: " << (after_refact-before_refact)/CLOCKS_PER_SEC << endl;
		/******************************/
		ofstream f2("f2.txt");
 		for (it = ids.begin(); it != ids.end(); it++) {
			f2 << "Size of cluster: " << it->first << ", number of clusters: " << it->second.size() << endl;
			for (list_it = it->second.begin(); list_it != it->second.end(); list_it++) {
    			f2 << (*list_it) << " ";
			}
    		f2 << endl;
		}
		/******************************/

		/*********** ONE OF MAIN FUNCTIONS ***************/
		/*** function for finding identical and similar fragments ***/
		findIdent(values,ids,cluster_ends);
		/*************************************************/
	}

    ///// for counting general characteristics about clusters
    num_clusters++;
    if (num_clusters != 1) avg_len_clusters /= (num_clusters-1);
    else avg_len_clusters = -1;
    avg_cluster_power = (ldouble)num_lines/num_clusters;

#ifdef DEBUG
    cout << "Cluster N. " << num_clusters << ",  start: " << start_cluster << ", end: " << i << endl;
    cout << current_chars.l << ", " << current_chars.avg_len << ", " << current_chars.seq_l << ", " << \
            current_chars.alpha << ", " << current_chars.alt_alpha << ", " << current_chars.n_alpha << \
            ", " << current_chars.p << endl;
    fout << "____" << endl; // tmp output
#endif
	/*** end of last one iteration ***/


	// final operations for getting characteristics
	if ((num_clusters-avg_len_count) > 0) all_chars.avg_len /= (ldouble)(num_clusters-avg_len_count); 
	else all_chars.avg_len = -1;
	if ((num_clusters-l_count) > 0) all_chars.l /= (ldouble)(num_clusters-l_count);
	else all_chars.l = -1;

	all_chars.seq_l /= (ldouble)num_clusters;
	all_chars.alpha /= (ldouble)num_clusters; 
	if ((num_clusters-alt_alpha_count) > 0) all_chars.alt_alpha /= (ldouble)(num_clusters-alt_alpha_count);
	else all_chars.alt_alpha = -1;
	all_chars.n_alpha /= (ldouble)num_clusters;

	
	cout << endl << endl << "!!!!!INFORMATION ABOUT CLUSTERS !!!!!" << endl << endl;
    chOutput(all_chars,num_lines,0,0);
	chFileOutput(0,all_chars,num_lines,num_clusters,avg_len_clusters,avg_cluster_power,-1);
	// general output about clusters
	cout << "Number of clusters: " << num_clusters << ", AVG_LEN for clusters: " << avg_len_clusters << \
		", AVG size of cluster: " << avg_cluster_power << endl;


	for (i = 0 ; i < num_lines; i++) {
		copy_values[i] = values[i];
	}
	nullChars(current_chars);
	current_chars = countChars(values,copy_values,num_lines);

	cout << endl << endl << "!!!!!INFORMATION ABOUT OVERALL SEQUENCE !!!!!" << endl << endl;
    chOutput(current_chars,num_lines,0,0);
	chFileOutput(2,current_chars,num_lines,-1,-1,-1,-1);
	end = clock();
	cout << "Time on ident: " << (end-mid)/CLOCKS_PER_SEC << " sec" << endl;
	cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;
}