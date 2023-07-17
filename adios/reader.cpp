#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <adios2.h>
#include <string>

static char fmt[] = "file%d.bp";
static char buf[] = "0123456789abcdefghijklmnopqrstuvwxyz";

const adios2::Dims start{0, 0};


static inline void readdata(adios2::fstream* istream, size_t size)
{
	size_t len = 1024 * 1024;
	char *buf2 = (char *)malloc(size);
	if (buf2 == NULL)
	{
		fprintf(stderr, "readdata malloc buf2 error\n");
		exit(-1);
	}


	adios2::fstep iStep;
	while(adios2::getstep(*istream, iStep)){
		istream->read<char>("data",buf2);
	}

	
}

static inline double diffmsec(const struct timeval a,
							  const struct timeval b)
{
	long sec = (a.tv_sec - b.tv_sec);
	long usec = (a.tv_usec - b.tv_usec);

	if (usec < 0)
	{
		--sec;
		usec += 1000000;
	}
	return ((double)(sec * 1000) + ((double)usec) / 1000.0);
}

int main(int argc, char *argv[])
{
	if (argc != 4)
	{
		fprintf(stderr, "use: %s nfile start_file file_size\n", argv[0]);
		return -1;
	}
	struct timeval before, after;

	gettimeofday(&before, NULL);
	int nfile = strtol(argv[1], NULL, 10);
	int startfile = strtol(argv[2], NULL, 10);
	size_t file_size = strtol(argv[3], NULL, 10);
	char filename[128];
	for (int i = 0 + startfile; i < (nfile + startfile); ++i)
	{
		sprintf(filename, fmt, i);
		std::string filenameString(filename);
		adios2::fstream* istream = new adios2::fstream(filenameString, adios2::fstream::in);

		readdata(istream, file_size);
		
		delete istream;
	}

	gettimeofday(&after, NULL);
	double elapsed_time = diffmsec(after, before);
	printf("elapsed time (ms) : %g\n", elapsed_time);

	return 0;
}
