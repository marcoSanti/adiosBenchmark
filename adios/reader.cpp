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
	for (int i = 0 + startfile; i < nfile; ++i)
	{
		sprintf(filename, fmt, i);
		std::string filenameString(filename);
		adios2::fstream istream(filenameString, adios2::fstream::in);

		for (adios2::fstep iStep; adios2::getstep(istream, iStep);){
                   std::string msg = istream.read<std::string>("data").front();
                }
	      
	}

	gettimeofday(&after, NULL);
	double elapsed_time = diffmsec(after, before);
	printf("elapsed time (ms) : %g\n", elapsed_time);

	return 0;
}
