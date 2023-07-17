#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static char fmt[]= "file%d.dat";
static char buf[]= "0123456789abcdefghijklmnopqrstuvwxyz";

static inline void readdata(FILE *fp, long int num_bytes) {
	size_t size = num_bytes;
	long int window_size = 1024 * 1024;
	char* buf2 = (char*) malloc(size);
	long int tot_bytes_read = 0;
	size_t r=-1;
	int i = 0;
	while(tot_bytes_read < num_bytes) {
		r = fread(buf2 + i * window_size, 1, window_size, fp);
		
		if (r!=0 && r != window_size) {
			fprintf(stderr, "READ ERROR, abort execution\n");
			exit(-1);
		}
		tot_bytes_read += r;
		
		++i;
	}

	free(buf2);
}

static inline double diffmsec(const struct timeval a, 
		                              const struct timeval b) {
	    long sec  = (a.tv_sec  - b.tv_sec);
	        long usec = (a.tv_usec - b.tv_usec);
		    
		    if(usec < 0) {
			            --sec;
				            usec += 1000000;
					        }
		        return ((double)(sec*1000)+ ((double)usec)/1000.0);
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		fprintf(stderr, "use: %s start_byte num_bytes\n", argv[0]);
		return -1;
	}
	struct timeval before, after;

	gettimeofday(&before, NULL);
	long int    start_byte = strtol(argv[1], NULL, 10);
	long int 	n_bytes = strtol(argv[2], NULL, 10); 
	char filename[128];
	int i = 0;
	sprintf(filename, fmt, i); 
	FILE *fp= fopen(filename, "r");
	assert(fp);
	fseek(fp, start_byte, SEEK_SET);
	readdata(fp, n_bytes);
	//sleep(1);
	fclose(fp);

	gettimeofday(&after, NULL);
	double elapsed_time = diffmsec(after, before);
	printf("elapsed time (ms) : %g\n",elapsed_time);

	return 0;

}
