
#ifndef H_INCLUDE
#define H_INCLUDE

#include <stdlib.h>
#include <math.h>
//#include <sys/time.h>
#include <time.h>

#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <vector>
#include <map>
#include <iomanip>

using namespace std;


// output debug info
//#define DEBUG 1 

// refactor clusters before finding fragments.
// this could help in cush cases as: 0,1,2,0,1,2,3,4,5,3,4,5 
// While refactoring central cluster (0,1,2,3,4,5) will be split into 2, so similar fragments will be found.
#define REFACTOR 1  

#define BILLION 1000000.0
#define luint long unsigned int
#define ldouble long double
#define map_key luint

// these constants show how 2 clusters are correlated
#define IDENT 0
#define SIMILAR 1
#define DIFF 2

// return error value in checkIfFragment
#define ERR_NOT_A_FRAGMENT 1
#define ERR_NOT_FRAGMENT_END -1
#define ERR_NOT_SIMILAR_FRAGMENT -2

// service constant. *Probably* defines the possible ratio between new and old fragment coverage (new multiplied by FACTOR should not be less old)
#define FACTOR 100



/*** important global variables ***/
/**
 * MODE = 0 - count only basic characteristics
 * MODE = 1 - count all characteristics
 **/
extern int mode;

/**
 * name of input file
 **/
extern string filename;
/**
 * Rule for defining clusters: next reference must me less than N elements away from all K last references (for more info see cluster definition in my work)
 **/
extern luint K;
extern luint N;

/**
 * filename where results will be stored
 **/
extern string result_file;


/**
 * main struct for characteristics 
 **/
struct Features 
{
	ldouble l;
	ldouble avg_len;
	ldouble seq_l;
	ldouble alpha;
	ldouble alt_alpha;
	ldouble n_alpha;
	ldouble p;
};


#endif