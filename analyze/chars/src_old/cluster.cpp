
#include "cluster.h"

ldouble countAVG_LEN(long int* values, luint start, luint end) {
	
	luint sum = 0;
	luint tmp = 0;
	for (luint i = start+1; i < end; i++) {
		tmp = abs(values[i]-values[i-1]);
		sum += tmp;
	}
	ldouble avg_len;
	if (end-start-1 != 0) avg_len = (ldouble)sum/(end-start-1);
	else avg_len = -1;
	return avg_len;
}


ldouble countSEQ_L(long int* values, luint start, luint end) {
	
	ldouble seq_l = 0;
	luint num_seqs = 0;
	luint i = start;
	luint count = 0;
	while (i < end) {
		i++;
		count = 1;
		while ((abs(values[i]-values[i-1]) <= 1) && (i < end)) {
			i++;
			count++;
		}
		num_seqs++;
	}
	seq_l = ((ldouble)(end-start))/num_seqs;
	return seq_l;
}


void nullChars(Features& chars) {
        chars.l = chars.avg_len = chars.seq_l = chars.alpha = chars.alt_alpha = chars.n_alpha = chars.p = 0;
}


Features countChars(long int* values, long int* copy_values, luint end) {
	
	luint i,j;
	long int tmp;

	luint cluster_lines = end;
	luint num_unique = 0; // counting number of unique elements in values

	Features chars; // all main characteristics are stores here
	nullChars(chars);


	/** 
	 * the only loop that runs through whole array 
	 * counts tmp values for ALPHA and P 
	 * counts needed  variable - num_unique
	 */
	luint tmp_p = 0, tmp_j = 0;
	luint tmp_alt = 0;
	luint found = 0;
	for (i = 0; i < end; i++) {
		if (copy_values[i] == -1) continue;
		tmp = copy_values[i];
		found = 0;
		tmp_j = i;
		for (j = i+1; j < end; j++) {
			if (copy_values[j] == tmp) {
				copy_values[j] = -1;
				tmp_j = j;
				found++;
			}
		}
		if (!found) tmp_p++;
		else {
			chars.alpha +=(ldouble)found/(tmp_j-i);
			chars.alt_alpha += (ldouble)(tmp_j-i)/found;
			tmp_alt++;
		}

		num_unique++;
	}

	/********* counting ALPHA and P ***********/
	chars.alpha = chars.alpha/(ldouble)num_unique;
	// old version
	if (tmp_alt != 0) chars.alt_alpha = chars.alt_alpha/(ldouble)tmp_alt;
	else chars.alt_alpha = -1; 
	
	// if all elements are addressed only once, than alt_alpha should be max (size of reference profile)
	//chars.alt_alpha = (chars.alt_alpha+(num_unique-tmp_alt)*(end))/(ldouble)num_unique;
	chars.p = 1 - (ldouble)tmp_p/cluster_lines;

	/******* counting AVG_LEN,SEQ_L and N_ALPHA ********/
	chars.avg_len = countAVG_LEN(values,0,end);
	if (chars.avg_len != -1) chars.l = 1.0/(chars.avg_len+1);
	else chars.l = -1;
	chars.seq_l = countSEQ_L(values,0,end);
	chars.n_alpha = (ldouble)cluster_lines/num_unique;

	/********** OUTPUT ********/
	//chOutput(chars,alt_alpha,cluster_lines,num_unique,tmp_p);


	return chars;
}