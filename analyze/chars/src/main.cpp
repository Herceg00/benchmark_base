
#include "core.h"

/** 
@brief Read input params and save them in global variables 
@param argc Input argc parameter.
@param argv Input argv parameter.
*/
void readParams(int argc, char** argv){
	// reading parameters
	if (argc < 2 || argc > 5) {
		cout << "Wrong input parameters!" << endl;
		exit(-1);
	}

	filename = argv[1];
	mode = 1;
	if (argc > 2) K = atoi(argv[2]);
	if (argc > 3) N = atoi(argv[3]);
    if (argc > 4) mode = atoi(argv[4]);
}


/** 
 @brief Get number of lines in input file 
 */
int getNumLines() {
	ifstream fin(filename.c_str());
	if (!fin) {
		cout << "Wrong name of file!" << endl;
		exit(-1);
	}

	/*** get num_lines ***/
	luint num_lines = 0;
	string line;
    while (getline(fin,line)) {
            if (line=="") continue;
            num_lines++;
    }
	fin.close();
	
	if (num_lines == 0) {
		cout << "Empty file!" << endl;
		exit(-1);
	}

	return num_lines;
}


/**
@brief Reading file into array.
@param num_lines Number of lines in input \filename file
@param values Output array where input data from file should be written
@copy_values Output copy of \values array 
@return: Filled arrays as params. 
*/
void readFromFile(luint num_lines, long int* values, long int* copy_values) {
	
	ifstream fin(filename.c_str());
	if (!fin) {
		cout << "Wrong name of file!" << endl;
		exit(-1);
	}

	/*** read to arrays ***/
	string line;
	luint i = 0;
	long int tmp = 0;
	while (getline(fin,line)) {
		if (line=="") continue;
		tmp = atol(line.c_str());
		if (tmp < 0) {
			cout << "Wrong values in file!" << endl;
			exit(-1);
		}
		values[i] = tmp;
		copy_values[i] = tmp;

		i++;
	}
	fin.close();

	serviceOutput();
}


/** 
 @brief Start of the program.
 @detailed Input parameters: 
				argv[1] = filename; 
				argv[2] shows, what characteristics should be counted. Default for argv[2] - all.
 **/
int main(int argc, char** argv) {

	//struct timeval start,end,input,main;
	//gettimeofday(&start,NULL);
	clock_t end,start,read;
	start = clock();

	// 1. read input params
	readParams(argc,argv);

	// 2. read everything needed from file
	luint num_lines = getNumLines();
	long int* values = new long int[num_lines];
	long int* copy_values = new long int[num_lines];
	readFromFile(num_lines, values, copy_values);
	read = clock();
	cout << endl << "Time on file work: " << (read-start)/CLOCKS_PER_SEC << " sec" << endl;

	// 3 (main).
	main_part(num_lines, values, copy_values);

	
    //gettimeofday(&end,NULL);
    //cout << "Overall ti me: " << ((end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec)/BILLION) << " sec" << endl << endl;
	end = clock();
	cout << "Overall time: " << (end-start)/CLOCKS_PER_SEC << " sec" << endl;
	cout << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx" << endl;

	return 0;
}
