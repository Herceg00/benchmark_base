#include <sys/time.h>
#include <stdio.h>

#include <stdlib.h>
#include <string.h>


// unchangable constants
#define POW_10_6 1000000.0 // 10^6
#define POW_10_9 1000000000.0 // 10^9
#define KB 1024
#define MB KB*KB


//this constants can be changed
#define MIN_ST 1
#define MAX_ST 1024*256
#define MIN_SIZE 4*KB
#define MAX_SIZE 256*MB
#define TOTAL_ACCESS 4*MB

#define NUM_PROCS 13
char procs[NUM_PROCS][4] = {"cl1","cl2","cl3","wo2","wo3","wo5","wo6","op3","op4","op5","ba1","har","gra"};
double peaks[NUM_PROCS] = {1.6,2.33,2.66,2.66,2.66,3,2,2.4,1.8,1.8,2.7,3.02,2.67};


void bm(long array_8N,long stride, long total_access,double peak,FILE* pf,FILE *csv) {

	volatile long* array_address;
	register long data;
	register long i, total_access2;

	unsigned long array_N;
	struct timeval start,stop;
	double time_in_sec,time_in_ns,cost_per_op,ticks;

	array_N = array_8N/sizeof(long);
	array_address = (long*)calloc(array_N,sizeof(long));

	for (i = 0; i < array_N; i+=stride) {
		array_address[i] = i + stride;
	}
	array_address[i-stride] = 0;

	data = 0;
	gettimeofday(&start,NULL);
	for (i = 0 ; i < total_access; i+=8) {
		data = array_address[data];
                data = array_address[data];
                data = array_address[data];
                data = array_address[data];
                data = array_address[data];
                data = array_address[data];
                data = array_address[data];
                data = array_address[data];
	}
	gettimeofday(&stop,NULL);

	time_in_sec = (double)((stop.tv_sec-start.tv_sec)+(stop.tv_usec-start.tv_usec)/POW_10_6)/total_access;
	cost_per_op = time_in_sec*peak*POW_10_9;
	time_in_ns = time_in_sec*POW_10_9;
	fprintf(pf,"%-*.2f",10,cost_per_op);
        fprintf(csv,";%.2f",cost_per_op);

	delete [] array_address;
}


int main(int argc, char **argv) {
	int i = 0;
	int size = 0;
	int stride = 0;
	char *full_proc;
	char proc_type[4];

        FILE *pf,*csv;
        pf = fopen("res.txt","a");
	csv = fopen("res.csv","a");


        for (i = 1; i < argc; i++) {
                switch(argv[i][0]) {
                        case '-': {
	                        full_proc = argv[i]+1*sizeof(char); // +1 - skips first 1 element
				proc_type[0] = full_proc[0];
				proc_type[1] = full_proc[1];
				proc_type[2] = full_proc[2];
				proc_type[3] = 0;
                                break;
                        }
                }
        }

	printf("_%s_",proc_type);

        double peak = 0.0;
	bool found = false;
	for (i = 0; i < NUM_PROCS; i++) {
		if (!strcmp(procs[i],proc_type)) {
			peak = peaks[i];
			found = true;
			break;
		}
	}
	if (found) {
		printf("\nProc found: %s\n\n",full_proc);
		fprintf(pf,"\n=========================\n");
                fprintf(pf,"Proc found: %s\n\n",full_proc);
                fprintf(csv,"%s\n",full_proc);
	}
	else {
		printf("\nProc not found: %s\n\n",full_proc);
		return 0;
	}


	fprintf(pf,"%-17s","size\\stride");
	for (stride = MIN_ST; stride < MAX_ST; stride = stride*2) {
		fprintf(pf,"%-10d",stride);
		fprintf(csv,";step %d",stride);
	}
	fprintf(pf,"\n\n");
	fprintf(csv,"\n");
	int hsize = 0;
	char mnem[3] = "KB";
	for (size = MIN_SIZE; size < MAX_SIZE; size = size*2) {
		if (size/KB > KB) { hsize = size/(KB*KB); mnem[0] = 'M';}
		else hsize = size/KB;
		fprintf(pf,"%5d %-10s",hsize,mnem);
		fprintf(csv,"%d %s",hsize,mnem);
		for (stride = MIN_ST; stride < MAX_ST; stride = stride*2) {
			bm(size,stride,TOTAL_ACCESS,peak,pf,csv);
		}
		fprintf(csv,"\n");
		fprintf(pf,"\n");

		// step 1.5x
                fprintf(pf,"%5d %-10s",3*hsize/2,mnem);
                fprintf(csv,"%d %s",3*hsize/2,mnem);
                for (stride = MIN_ST; stride < MAX_ST; stride = stride*2) {
                        bm(3*size/2,stride,TOTAL_ACCESS,peak,pf,csv);
                }
                fprintf(csv,"\n");
                fprintf(pf,"\n");
	}
        fprintf(csv,"\n");

	return 0;
}



