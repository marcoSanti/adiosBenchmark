#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>

static char fmt[]= "file%d.dat";
static char buf[]= "0123456789abcdefghijklmnopqrstuvwxyz";

static inline void writedata(FILE *fp, size_t size) {
	size_t len = 1024 * 1024;
	size_t sz = size / len;
	size_t r  = size % len;
	char* buf2 = (char*) malloc(size);	
/*	buf2[0] = 'a';
	buf2[2] = 'b';
	buf2[len -1 ] = 'c';
	buf2[size - 3 ] = 'd';
	buf2[size - 2 ] = 'e';
	buf2[size - 1 ] = 'f';

	*/
	for(size_t i=0;i<sz;++i)
		fwrite(buf2 + i * len, 1, len, fp);
	free(buf2);

	//fwrite(buf, 1, sizeof(buf), fp);
	//if (r)
	//	fwrite(buf, 1, r, fp);
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
	if (argc != 4) {
		fprintf(stderr, "use: %s size-in-byte num_files start_file\n", argv[0]);
		return -1;
	}

	size_t size  = strtol(argv[1], NULL, 10);
	if (size==0) {
		fprintf(stderr, "Invalid size (%ld)\n", size);
		return -1;
	}
	struct timeval before, after;
	gettimeofday(&before, NULL);
	int    nfile = strtol(argv[2], NULL, 10);
	int    start_file = strtol(argv[3], NULL, 10);
	char filename[128];
	for(int i=0;i<nfile;++i) {
		sprintf(filename, fmt, i + start_file); 
		FILE *fp= fopen(filename, "w");
		assert(fp);
		//sleep(3);
		writedata(fp, size);
		fclose(fp);
	}

	gettimeofday(&after, NULL);
	double elapsed_time = diffmsec(after, before);
	printf("elapsed time (ms) : %g\n",elapsed_time);

	return 0;

}
