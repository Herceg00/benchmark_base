
#include "fragment.h"


int markRead(map<map_key,list<luint> >::iterator map_it, list<luint>::iterator list_it){
	map_it->second.erase(list_it);
	return 0;
}


int getType(long int* values, vector<luint>& cluster_ends, luint first_num, luint second_num) {
	
	luint first_size, second_size, first_beg, second_beg;

	second_size = cluster_ends[second_num]-cluster_ends[second_num-1];
	second_beg = cluster_ends[second_num-1];
	if (first_num == 0) {
		first_size = cluster_ends[0];
		first_beg = 0;
	}
	else {
		first_size = cluster_ends[first_num]-cluster_ends[first_num-1];
		first_beg = cluster_ends[first_num-1];
	}
	
	// if sizes are different, than 
	if (first_size != second_size) {
		return DIFF;
	}

	bool ident = true;
	int sim_value = values[first_beg] - values[second_beg];
	for (luint i = 0; i < first_size; i++) {
		if (values[first_beg+i] != values[second_beg+i]+sim_value) {
				//cout << sim_value << "____" << first_beg << ' ' << second_beg << ' ' << endl;
				// this means that this cluster is NOT identical to <main_num> cluster
				ident = false;
				break;
		}
	}

	if (!ident) return DIFF;

	// if sim_value=0, that means that clusters are identical
	if (sim_value == 0) return IDENT;
	
	return SIMILAR;
}


bool ifBelongs(long int* values, vector<luint>& cluster_ends, luint beg_num, luint curr_num, int fragm_type) {
	
	int type;
	long int curr_step, fragm_step;
	luint fragm_size = curr_num-beg_num;
	
	if (curr_num == beg_num) return true;

	// if these types, we just need to check its correlation with the last cluster. If type matches, return true.
	if ((fragm_type == IDENT) || (fragm_type == SIMILAR)) {
		type = getType(values,cluster_ends,curr_num-1,curr_num);
		
		if ((fragm_type == SIMILAR) && (fragm_size > 1)) {
			// diff between first refs in curr_num-1 and curr_num
			curr_step = values[cluster_ends[curr_num-1]] - values[cluster_ends[curr_num-2]]; 
			// diff between first refs in main_num and main_num+1
			if (beg_num == 0) fragm_step = values[cluster_ends[beg_num]] - values[0];
			else fragm_step = values[cluster_ends[beg_num]] - values[cluster_ends[beg_num-1]];
			if (curr_step != fragm_step) {
				//cout << "curr_num: " << curr_num << "beg_num: " << beg_num << endl;
				//cout << "curr_step: " << curr_step << "fragm_step: " << fragm_step << endl;
				return false;
			}
			else return true;
		}
		if (type == fragm_type) return true;
		else return false;
	}

	// if type = DIFF, we need to check that it is different from EVERY cluster in fragment
	for (luint i = beg_num; i < curr_num; i++) {
		type = getType(values,cluster_ends,i,curr_num);
		if (type != DIFF) return false;
	}
	return true;
}


int checkIfFragment(long int* values, vector<luint>& cluster_ends, luint main_num, luint curr_num, int& fragm_type) {
	int error = 0;
	bool forms_fragment = true;
	// we check first two clusters in order to find out what type of fragment this should be
	if (curr_num-main_num == 1) {
		fragm_type = IDENT;
		return 0;
	}
	int fragment_type = getType(values,cluster_ends,main_num,main_num+1);
	//cout << "fragment_type: " << fragment_type << endl;
	
	luint fragment_size = curr_num-main_num;
	for (luint i = 2; i < fragment_size; i++) {
		// check if every next cluster in fragment matches fragment type
		if (!ifBelongs(values,cluster_ends,main_num,main_num+i,fragment_type)) {
			forms_fragment = false;
			//cout << "main: " << main_num << ", curr_num: " << curr_num << ", main_num+i: " << main_num+i << ", fragment_size: " << fragment_size << endl;
			return ERR_NOT_A_FRAGMENT;
		}
	}

	// if curr_num, according to the rules, should belong to current fragment, this is another error
	if (ifBelongs(values,cluster_ends,main_num,curr_num,fragm_type)) {
		if (fragm_type != DIFF) error = ERR_NOT_FRAGMENT_END;
		else error = ERR_NOT_SIMILAR_FRAGMENT;
	}

	fragm_type = fragment_type;
	return error;
}


int findIdent(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends) {
	vector<luint>::iterator vect_it;
	list<luint>::iterator list_it;
	list<luint>::iterator curr_list_it;
	list<luint>::iterator tmp_list_it;
	map<map_key,list<luint> >::iterator map_it;
	map<map_key,list<luint> >::iterator tmp_map_it;

	luint num_lines = cluster_ends.back();
	luint main_size, main_num, main_beg, main_end, curr_beg, curr_num, curr_size;

	luint top_references_used = 0, top_fragment_start = 0, top_fragment_size = 0, top_num_fragments = 0;
	luint i;

	luint cluster_ends_size = cluster_ends.size();
	luint values_size = cluster_ends[cluster_ends.size()-1];
	luint main_counter = 0, prev_percent = 0;

	vector<luint> start_num_of_fragments, iter_start_num_of_fragments, top_start_num_of_fragments;
	luint similar_size, iter_similar_size, top_similar_size;
	luint num_clusters, iter_num_clusters, top_num_clusters;
	ofstream ident_out("ident_clusters.txt");

	top_similar_size = 0;
	top_num_clusters = 0;
	// main loop, map_it references to the first free cluster
	main_size = cluster_ends[0]; // size of current main cluster
	main_beg = 0;
	main_end = main_beg+main_size;
	main_num = 0; // number of current main cluster
	/** START OF MAIN ifIdent() LOOP **/
	while (true) {

		/*** one iteration - one current main cluster to start with ***/

		map_it = ids.find(main_size); // find map element with key equal size of current main cluster
		list_it = map_it->second.begin();
		if ((*list_it) != main_num) { //just checking, this should be always false
			cout << "Wrong sequence of clusters in list! " << (*list_it) << ' ' << main_num << endl;
			outputMap(ids);
			return -1;
		}

		/* output of number of percents that is analysed */
		main_counter++;
		//luint percent = main_counter*100/cluster_ends_size;
		luint counter = cluster_ends_size - main_counter;
		luint percent = ((counter+cluster_ends_size)*(cluster_ends_size-counter)/2)*100/\
			((1+cluster_ends_size)*cluster_ends_size/2);
		// new version
		if ((percent > 0) && (percent != prev_percent)) {
			cout << ".";
			if (percent % 10 == 0) cout << endl;
			prev_percent = percent;
		}
		// old version
		/*if ((percent % 5 == 0) && (percent > 0) && (percent != prev_percent)) {
			cout << percent << " %" << endl;
			prev_percent = percent;
		}*/
		/* end of output */

		/* checking for faster work */
		/* if size of unchecked is less than size of current cover, than finish */
		if (values_size-main_beg < top_similar_size) {
			cout << endl << "preliminary exit on " << percent << " %" << endl << endl;
			break;
		}
		/* end of checking for faster work */
		
		iter_similar_size = 0;
		iter_num_clusters = 0;
		iter_start_num_of_fragments.clear();

		// run through every cluster of the same size
		for (list_it = map_it->second.begin(); list_it != map_it->second.end(); list_it++) {
			
			/**TODO: change ifIdent to find not only sequential fragments
			* 1. Find similar fragments that starts from main_num and curr_num (on this step identify main_end)
			* 2. If no similar fragments - get another curr_num.
			* 3. If there is, for EVERY cluster in map_it->second that is AFTER curr_num we need to check if 
			* fragment from main_beg to main_end is similar to fragment that starts at new current cluster.
			*	We can just check every values between these borders!!
			* 4. If current is similar, add its first cluster to vector of similar fragments; if not - go to next.
			* 5. COUNTING CHARS
			**/

			curr_num = (*list_it);
			if (main_num == curr_num) continue;
			curr_beg = cluster_ends[curr_num-1] ;
			curr_size = cluster_ends[curr_num] - curr_beg;

			//cout << "main_num: " << main_num << ", curr_num: " << curr_num << endl;

			similar_size = 0;
			start_num_of_fragments.clear();
						
			int sim_value = values[main_beg] - values[curr_beg];
			bool not_ident = false;
			luint clusters_in_fragment = 0;
			luint tmp_curr_num = 0;
			luint tmp_main_beg = main_beg;
			luint tmp_curr_beg = curr_beg;
			luint tmp_curr_size = curr_size;
			luint num_similar_fragments = 0;
			luint tmp_main_size = 0;
			luint fragm_size = 0;
			int fragm_type = 0, tmp_fragm_type = 0, tmp_step = 0, step = 0;
			// STEP 1. Check for similar fragments which start at main_num and curr_num
			while (true) {
				/* this loop compare two clusters in different fragments */
				not_ident = false;
				for (i = 0; i < tmp_curr_size; i++) {
					if (values[tmp_main_beg+i] != values[tmp_curr_beg+i]+sim_value) {
						not_ident = true;
						break;
					}
				}
				if (not_ident) break;

				clusters_in_fragment++;

				/** getting next clusters **/
				tmp_main_beg = tmp_main_beg+tmp_curr_size;
				tmp_curr_beg = tmp_curr_beg+tmp_curr_size;

				// this means that that we've checked the last cluster
				if (tmp_curr_beg >= values_size) break;
				if (curr_num+clusters_in_fragment >= cluster_ends.size()) break;

				// this means fragment from main_num has reached fragment from curr_num
				if (tmp_main_beg >= curr_beg) break;

				tmp_curr_size = cluster_ends[curr_num+clusters_in_fragment]-tmp_curr_beg;
				tmp_main_size = cluster_ends[main_num+clusters_in_fragment]-tmp_main_beg;

				// if cluster have different sizes, that means they are definitely not similar
				if (tmp_main_size != tmp_curr_size) break;

				// now we need to check type of fragment
				tmp_fragm_type = getType(values,cluster_ends,main_num,main_num+clusters_in_fragment);
				tmp_step = values[cluster_ends[main_num]-1] - values[cluster_ends[main_num+clusters_in_fragment]-1];
				//cout << "main_num: " << main_num << ", curr_num: " << main_num+clusters_in_fragment << ", type: " << tmp_fragm_type << endl;
				if (clusters_in_fragment == 1) {
					fragm_type = tmp_fragm_type;
					step = tmp_step; // step between first and second clusters
				}
				else {
					// fragment can't consist of clusters of different types
					if (fragm_type != tmp_fragm_type) break;
					// if fragm_type = 1, we neeed to check that step between clusters is the same in whole fragment
					if ((fragm_type == SIMILAR) && (tmp_step != clusters_in_fragment*step)) break;
				}
			}

			// if we haven't found similar fragment of any size that start at main_num and curr_num,
			// than we need to go to next iteration
			if (clusters_in_fragment == 0) continue;

			/** STEP 2. We have similar fragments, so we need to check if there are more fragments similar to these **/
			num_similar_fragments++;
			fragm_size = cluster_ends[main_num+clusters_in_fragment-1]-main_beg;
			num_clusters = clusters_in_fragment;
			start_num_of_fragments.push_back(main_num);
			start_num_of_fragments.push_back(curr_num);

			/* second checking for faster work */
			/* if size of unchecked is less than size of current cover, than finish */
			luint remained = (values_size-curr_beg) + fragm_size;
			if (remained < top_similar_size) break;
			/* end of second checking for faster work */

			tmp_curr_beg = curr_beg;
			tmp_list_it = list_it;
			tmp_list_it++;
			// check all remained clusters with the same size - does similar fragment start from it?
			for (tmp_list_it; tmp_list_it != map_it->second.end(); tmp_list_it++) {

				/** This could happen if <tmp_curr_num> cluster is part of previous fragment.
				* We should skip all such clusters. **/
				if (cluster_ends[(*tmp_list_it)-1] < tmp_curr_beg+fragm_size) continue;

				tmp_curr_num = (*tmp_list_it);
				tmp_curr_beg = cluster_ends[tmp_curr_num-1];

				// this means that similar fragment won't fit if it starts at tmp_curr_num
				if (tmp_curr_beg+fragm_size > values_size) {
					//cout << tmp_curr_beg << ' ' << fragm_size << ' ' << values_size << endl;
					//cout << "Doesn't fit into stream!" << endl;
					continue;
				}

				not_ident = false;
				sim_value = values[main_beg]-values[tmp_curr_beg];
				for (i = 0; i < fragm_size; i++) {
					if (values[main_beg+i] != values[tmp_curr_beg+i]+sim_value) {
						not_ident = true;
						break;
					}
				}

				if (not_ident) continue;

				// this means that we found another similar fragment, so we need to remember it
				num_similar_fragments++;
				start_num_of_fragments.push_back(tmp_curr_num);
			}
			//cout << "main_num: " << main_num << ", curr_num: " << curr_num << ", num_fragments: " << num_similar_fragments << endl;
		
			/** we have finished checking for current main_num and curr_num, and now need to compare results **/
			/** num_similar_fragments=0 means that for this couple <main_num,curr_num> we haven't found similar fragments **/
			if (num_similar_fragments == 0) continue;
			else {
				// yep, we found something! Time to remember info about them.

				//cout << "num_similar_fragments: " << num_similar_fragments << endl;
				/** tmp output **/
				/*
				cout << "num_similar_fragments: " << num_similar_fragments << endl;
				cout << "fragment: " << endl;
				for (i = main_beg; i < cluster_ends[main_num+fragment_size_in_clusters-1]; i++) {
					cout << values[i] << ' ';
				}
				cout << endl;
				cout << "whole similar part: " << endl;
				for (i = main_beg; i < cluster_ends[main_num+(num_similar_fragments+1)*fragment_size_in_clusters-1]; i++) {
					cout << values[i] << ' ';
				}
				cout << endl;
				cout << "SIZE: " << (num_similar_fragments+1)*fragment_size << endl;
				exit(0);*/
				/** end of tmp output **/

				// current criteria for one main cluster - we choose max fragment that has similar
				similar_size = (num_similar_fragments+1)*fragm_size;
				/** we should change currently chosen fragments, if: 1) size of new fragment is bigger; 
				* 2) new fragments cover not a very small fraction. **/
				luint iter_fragm_size;
				if (iter_similar_size == 0) iter_fragm_size = 0;
				else iter_fragm_size = iter_similar_size/iter_start_num_of_fragments.size();
				if ((FACTOR*(similar_size) > iter_similar_size) && (iter_fragm_size < fragm_size)) {
					//cout << "start_num_of_fragments: " << (*start_num_of_fragments.begin()) << ", fragm_size: " \
						<< fragm_size << ", overall_size: " << similar_size << \
						", COVERAGE:" << 100.0*similar_size/values_size << endl;
					iter_start_num_of_fragments = start_num_of_fragments;
					iter_similar_size = similar_size;
					iter_num_clusters = num_clusters;
				}
			}	
		}

		// current criteria for which fragments to choose - we choose fragments that covers most part of whole stream
		if (top_similar_size < iter_similar_size) {
			top_start_num_of_fragments = iter_start_num_of_fragments;
			top_similar_size = iter_similar_size;
			top_num_clusters = iter_num_clusters;
		}

		/* the main question - if we found some similar fragments, do we stop or continue search for another fragments? */
		markRead(map_it,map_it->second.begin());
		main_num++;
		if (main_num == cluster_ends.size()) break;
		main_beg = cluster_ends[main_num-1];
        main_end = cluster_ends[main_num];
        main_size = main_end-main_beg;
		
	}
	/** END OF MAIN ifIdent() LOOP **/

	if (top_similar_size == 0) {
		cout << "No fragments found!" << endl;
		ofstream foutput(result_file.c_str());
		foutput << "No fragments found!" << endl;
		foutput.close();
		headerOutputResult();
		return 0;
	}

	luint first_fragment_num = (*top_start_num_of_fragments.begin());
	/*** tmp output, start ***/
	
	vector<luint>::iterator hhh;
	luint fff = top_similar_size/top_start_num_of_fragments.size();
	bool first_output = true;
	for (hhh = top_start_num_of_fragments.begin(); hhh != top_start_num_of_fragments.end(); hhh++) {
		luint ggg;
		if ((*hhh) == 0) ggg = 0;
		else ggg = cluster_ends[(*hhh)-1];
		
		if (first_output) {
			cout << "fragm_beg: " << ggg << endl;
			for (i = 0; i < fff; i++) {
				cout << values[ggg+i] << ' ';
			}
			cout << endl;
		}
		//else cout << ggg << ", ";
		
		first_output = false;
	}
	cout << endl;
	
	/*** tmp output, end ***/
	cout << endl << "NUM OF START CLUSTER: " << first_fragment_num << \
		", SIZE OF FRAGMENT IN ELEMENTS: " << top_similar_size/top_start_num_of_fragments.size() << \
		", NUMBER OF FRAGMENTS: " << top_start_num_of_fragments.size() << ", OVERALL SIZE: " << top_similar_size << endl;
	cout << "COVERED: " << (double)top_similar_size/values_size*100 << "% ("  << top_similar_size << \
		" out of " << values_size << ")" << endl;

	/* output to file */
	ofstream foutput(result_file.c_str());
	foutput << endl << "NUM OF START CLUSTER: " << first_fragment_num << \
		", SIZE OF FRAGMENT IN ELEMENTS: " << top_similar_size/top_start_num_of_fragments.size() << \
		", NUMBER OF FRAGMENTS: " << top_start_num_of_fragments.size() << ", OVERALL SIZE: " << top_similar_size << endl;
	foutput << "COVERED: " << (double)top_similar_size/values_size*100 << "% ("  << top_similar_size << \
		" out of " << values_size << ")" << endl;
	headerOutputResult();
	foutput.close();


	/*******************************************/
	/*** After this we have found similar fragments. Now we need to count characteristics for it. ***/
	/*******************************************/
	Features current_chars;
	Features all_chars;
	nullChars(all_chars);
	luint first; // number of first reference in first fragment
	luint size = top_similar_size/top_start_num_of_fragments.size(); // size of fragment in references
	
	/*for chars*/
	luint l_count = 0, avg_len_count = 0, alt_alpha_count = 0;
	ldouble avg_len_clusters = 0, avg_len_power = 0, avg_len_fragments = 0;
	luint start_cluster, cluster_size;

	if (first_fragment_num == 0) first = 0;
	else first = cluster_ends[first_fragment_num-1];

	// create copy of values that will be modified. We need only part of values where fragments could be - from <first> to end.
	long int* copy_values = new long int[size];
	for (i = 0; i < size; i++) {
		copy_values[i] = values[i+first];
	}

	// need to count avg_len between fragments
	vect_it = top_start_num_of_fragments.begin();
	vect_it++;
	for (vect_it; vect_it != top_start_num_of_fragments.end(); vect_it++) {
		avg_len_fragments += abs(values[cluster_ends[(*vect_it)-1]] - values[cluster_ends[(*vect_it)-1]-1]);
	}
	avg_len_fragments /= (ldouble)(top_start_num_of_fragments.size()-1);

	
	top_fragment_start = first_fragment_num;
	/* count cluster characteristics within fragment */
	// run through every cluster in fragment
	for (i = 0; i < top_num_clusters; i++) {
		if ((top_fragment_start == 0) && (i == 0)) start_cluster = 0;
		else start_cluster = cluster_ends[top_fragment_start+i-1];
		cluster_size = cluster_ends[top_fragment_start+i]-start_cluster;
		
		nullChars(current_chars);
		current_chars = countChars(values+start_cluster,copy_values+start_cluster-first,cluster_size);
		if (current_chars.l != -1) all_chars.l += current_chars.l; 
		else l_count++;
		if (current_chars.avg_len != -1) all_chars.avg_len += current_chars.avg_len;
		else avg_len_count++;
		all_chars.seq_l += current_chars.seq_l;
		all_chars.alpha += current_chars.alpha; 
		if (current_chars.alt_alpha != -1) all_chars.alt_alpha += current_chars.alt_alpha;
		else alt_alpha_count++;
		all_chars.n_alpha += current_chars.n_alpha;

		if (i != top_num_clusters-1) 
			avg_len_clusters += abs(values[start_cluster+cluster_size] - values[start_cluster+cluster_size-1]);
	}

	// final operations for getting characteristics
	if ((top_num_clusters-avg_len_count) > 0) all_chars.avg_len /= (ldouble)(top_num_clusters-avg_len_count); 
	else all_chars.avg_len = -1;
	if ((top_num_clusters-l_count) > 0) all_chars.l /= (ldouble)(top_num_clusters-l_count);
	else all_chars.l = -1;

	all_chars.seq_l /= (ldouble)top_num_clusters;
	all_chars.alpha /= (ldouble)top_num_clusters; 
	if ((top_num_clusters-alt_alpha_count) > 0) all_chars.alt_alpha /= (ldouble)(top_num_clusters-alt_alpha_count);
	else all_chars.alt_alpha = -1;
	all_chars.n_alpha /= (ldouble)top_num_clusters;

	if (top_num_clusters != 1) avg_len_clusters /= (ldouble)(top_num_clusters-1);
	else avg_len_clusters = -1;
	avg_len_power = size/(ldouble)top_num_clusters;
	
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << endl << "!!!!!INFORMATION ABOUT FRAGMENTS !!!!!" << endl << endl;
	chOutput(all_chars,size,0,0);
	chFileOutput(1,all_chars,size,top_num_clusters,avg_len_clusters,avg_len_power,avg_len_fragments);

	// general output about clusters
	cout << "Number of clusters in fragment: " << top_num_clusters << ", AVG_LEN for clusters: " << avg_len_clusters << \
		", AVG size of cluster: " << avg_len_power << endl;
	cout << "AVG_LEN for fragments: " << avg_len_fragments << endl;
	

	/***********************/
	/* now counting characteristics for whole fragment as one cluster*/
	for (i = 0; i < size; i++) {
		copy_values[i] = values[i+first];
	}
	current_chars = countChars(values+first,copy_values,size);
	cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
	cout << endl << "!!!!!INFORMATION ABOUT WHOLE FRAGMENT !!!!!" << endl << endl;
	chOutput(current_chars,size,0,0);
	chFileOutput(-1,current_chars,size,-1,-1,-1,avg_len_fragments);
	/***********************/

	return 0;	
}


int clustersRefactor(long int* values, map<map_key,list<luint> >& ids, vector<luint>& cluster_ends) {
	
	vector<luint>::iterator vect_it;

	// running through every cluster in subsequent order, each cluster should be checked for refactoring
	luint num_clusters = cluster_ends.size();
	// if we can't check criteria - return to main
	if (num_clusters < 5) return 0;

	luint i, j, k;
	int curr_type, prev_type, next_type;
	luint curr_size, prev_size, times;
	luint prev_cluster = 0, next_cluster = 4;

	luint main_times = 0, main_counter = 0;

	// we don't check first and last 2 clusters, because they don't have 2 previous and 2 next clusters
	i = 1;
	while (true) {

		if (cluster_ends.size()-i <= 3) break;

		i++;
		// we don't need to do this on first iteration
		if (i != 2) {
			prev_cluster++;
			next_cluster++;
		}

		/*** checking if current cluster matches criteria ***/
		// current cluster and its neighbours should be different 
		curr_type = getType(values, cluster_ends, i-1, i);
		if (curr_type != DIFF) continue;

		// size of current cluster should be at least K times of the size of previous cluster, K >= 2
		curr_size = cluster_ends[i]-cluster_ends[i-1];
		prev_size = cluster_ends[i-1]-cluster_ends[i-2];
		times = curr_size/prev_size;
		if ((curr_size % prev_size != 0) || (times < 2)) continue;

		// all the neighbours should be similar (and similarity for previous and next shoule be the same)
		prev_type = getType(values, cluster_ends, i-2, i-1);
		if (prev_type == DIFF) continue;
		
		next_type = getType(values, cluster_ends, i+1, i+2);
		if (next_type == DIFF) continue;
		if (prev_type != next_type) continue;
		if (getType(values, cluster_ends, i-1, i+1) == DIFF) continue;
		

		// if we are here, it means that: 1) curr cluster is different from every neighbour; 2)all neighbours are similar;
		// 3) size of curr cluster is k*size_of_previous_cluster, so it potentially could be split into several similar
		bool match = true;
		luint sim_value = 0;
		luint curr_start = cluster_ends[i-1];
		luint prev_start = cluster_ends[i-2];
		for (j = 0; j < times; j++) {
			curr_start = cluster_ends[i-1] + j*prev_size;
			sim_value = values[prev_start] - values[curr_start];
			for (k = 0; k < prev_size; k++) {
				if (values[prev_start+k] != values[curr_start+k]+sim_value) {
					match = false;
					break;
				}
			}
			if (!match) break;
		}
		if (!match) continue;
		/*** end of checking ***/

		// now we know that current_cluster should be split into <times> similar clusters
		// we need to modify cluster_ends (add <times> records instead of one)
		luint curr_end = cluster_ends[i];
		vect_it = cluster_ends.begin()+i;
		cluster_ends.erase(vect_it);
		for (j = 0; j < times; j++) {
			vect_it = cluster_ends.begin()+i; /***I don't know why but sometimes vect_it is spolied by vector.insert() ***/
			cluster_ends.insert(vect_it+j,cluster_ends[i-1]+(j+1)*prev_size);
		}

		/* for output */
		main_counter++;
		main_times += times;
	}

	/******************************/
	list<luint>::iterator list_it;
	map<map_key,list<luint> >::iterator it;

	ofstream f1("f1.txt");
 	for (it = ids.begin(); it != ids.end(); it++) {
        f1 << "Size of cluster: " << it->first << ", number of clusters: " << it->second.size() << endl;
        for (list_it = it->second.begin(); list_it != it->second.end(); list_it++) {
    		f1 << (*list_it) << " ";
        }
    	f1 << endl;
    }
	/******************************/

	map<map_key,list<luint> > ids2;
	map<map_key,list<luint> >::iterator map_it;
	// Now we need to modify <ids>
	// I decided that best decision is to create new <ids> because a lot of changes sholud be made to it
	luint size;
	for (i = 0; i < cluster_ends.size(); i++) {
		if (i == 0) size = cluster_ends[0];
		else size = cluster_ends[i] - cluster_ends[i-1];

		if ((map_it=ids2.find(size)) != ids2.end()) {
			map_it->second.push_back(i);
		}
		else {
			list<luint> tmp_list;
			tmp_list.push_back(i);
			ids2.insert(pair<map_key,list<luint> >(size,tmp_list) );
		}	
	}

	// we have created new map, so we destroy previous one and copy new one to it
	ids.clear();
	ids = ids2;

	/* output */
	if (main_counter != 0) {
		cout << "Clusters refactored: " << main_counter << ", avg cluster split into " << (float)main_times/main_counter << \
			" new clusters." << endl;
	} else cout << "No clusters refactored." << endl;

	ofstream fff("refactor.txt");
	//cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	fff << "Number of clusters: " << cluster_ends.size() << endl;
	for (j = 0; j < cluster_ends.size(); j++) {
		fff << cluster_ends[j] << endl;
	}
	//cout << endl << "%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
	return 0;
}