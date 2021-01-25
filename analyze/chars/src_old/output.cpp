
#include "output.h"


void serviceOutput() {
	cout << "##########################" << endl;
	cout << "Filename: " <<  filename << ", Mode: " << mode << ", K=" << K << " ,N=" << N << endl;
	cout << "##########################" << endl;
	return;
}


void chOutput(Features& chars, luint num_lines, luint num_unique, luint tmp_p) {

		cout << endl << "Length: " << num_lines << ", mode: " << mode <<  ", name of file: " << filename.c_str() << endl;
        cout << endl << "************************" << endl;

        cout << "Value of L: " << chars.l << " = 1/(1+" << chars.avg_len << ")" << endl << endl;
        cout << "Value of SEQ_L:  " << chars.seq_l << " = " << num_lines << "/" << (int)(num_lines/chars.seq_l) << endl << endl;
        cout << "Value of Alpha: " << chars.alpha << endl << endl;
        cout << "Value of NAlpha: " << chars.n_alpha << ", num_unique: " << num_unique << endl << endl;
        cout << "Value of P: " << chars.p << ", tmp_p: " << tmp_p << endl;
        cout << endl << "************************" << endl;
        cout << "AVG_LEN: " << chars.avg_len << endl;
        cout << "ALT ALPHA: " << chars.alt_alpha << endl;
        cout << "************************" << endl << endl;

	return;
}


void chFileOutput(int type, Features& chars, luint num_lines, ldouble num_clusters, ldouble avg_len_clusters, ldouble avg_len_power, ldouble avg_len_fragments) {

	ofstream foutput(result_file.c_str(),ios::app);
	//foutput  << "TYPE;AVG_LEN;SEQ_L;ALT_ALPHA;NALPHA;L;ALPHA" << endl;
	string str;
	if (type == 0) str = "clusters";
	else if (type == 1) str = "clusters (fragment)";
	else if (type == -1) str = "fragment";
	else str = "sequence";
	foutput << str.c_str() << ';' << chars.avg_len << ';' << chars.seq_l << ';' << chars.alt_alpha << ';' << chars.n_alpha << \
		';' << chars.l << ';' << chars.alpha;
	foutput << ';' << num_lines << ';' << num_clusters << ';' << avg_len_clusters << ';' << avg_len_power << \
		';' << avg_len_fragments;
	foutput << endl;
	foutput.close();
	return;
}


void outputMap(map<map_key,list<luint> >& ids) {
	list<luint>::iterator list_it;
	map<map_key,list<luint> >::iterator it;
 	for (it = ids.begin(); it != ids.end(); it++) {
        	cout << "First reference of cluster: " << it->first << ", number of clusters: " << it->second.size() << endl;
	        for (list_it = it->second.begin(); list_it != it->second.end(); list_it++) {
        		cout << (*list_it) << " ";
	        }
        	cout << endl;
        }
	return;
}


void headerOutputResult() {

	ofstream foutput(result_file.c_str(),ios::app);
	foutput << "TYPE;AVG_LEN;SEQ_L;ALT_ALPHA;NALPHA;L;ALPHA";
	foutput << ";size;num_clusters;avg_len_clusters;avg_len_power;avg_len_fragments";
	foutput << endl;
	foutput.close();
}