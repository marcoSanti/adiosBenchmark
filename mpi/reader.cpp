#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

static char fmt[]= "file%d.dat";
static char buf[]= "0123456789abcdefghijklmnopqrstuvwxyz";

static inline void readdata(FILE *fp, size_t size) {
	size_t len = 1024 * 1024;
	char* buf2 = (char*) malloc(size);
	if (buf2 == NULL) {
		fprintf(stderr, "readdata malloc buf2 error\n");
		exit(-1);
	}
	
	size_t r=-1;
	int i = 0;
	while(r!=0) {
		r = fread(buf2 + i * len, 1, len, fp);
		/*if (r < 1000000 && r != 0) {
			if (memcmp(buf2, buf, r) == 0) {
				
				fprintf(stderr, "risultato ok\n");

			}
		}
		*/
		//printf("r = %ld\n", r);
		if (r!=0 && r != len) {
			fprintf(stderr, "READ ERROR, abort execution\n");
			exit(-1);
		}
		/*if (memcmp(buf, buf2, r) != 0) {
			fprintf(stderr, "READ ERROR, abort execution\n");
			exit(-1);
		}
		*/
	++i;
	}
/*	if (buf2[0] != 'a' ||  buf2[2] != 'b' || buf2[len -1 ] != 'c' || buf2[size - 3 ] != 'd'|| buf2[size - 2 ] != 'e'|| buf2[size - 1 ] != 'f') {
			fprintf(stderr, "READ ERROR, abort execution\n");
			exit(-1);

	}	
	*/
	
	
	
	
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
	if (argc != 4) {
		fprintf(stderr, "use: %s nfile start_file file_size\n", argv[0]);
		return -1;
	}
	struct timeval before, after;

	gettimeofday(&before, NULL);
	int    nfile = strtol(argv[1], NULL, 10);
	int    startfile = strtol(argv[2], NULL, 10);
	size_t    file_size = strtol(argv[3], NULL, 10);
	char filename[128];
	for(int i=0+startfile;i<(nfile + startfile);++i) {
		sprintf(filename, fmt, i); 
		FILE *fp= fopen(filename, "r");
		assert(fp);
		readdata(fp, file_size);
		//sleep(1);
		fclose(fp);
	}

	gettimeofday(&after, NULL);
	double elapsed_time = diffmsec(after, before);
	printf("elapsed time (ms) : %g\n",elapsed_time);

	return 0;

}
