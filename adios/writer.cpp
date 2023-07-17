#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>
#include <unistd.h>
#include <adios2.h>


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

static inline void writedata(adios2::fstream *oStream, size_t size)
{
    size_t len = 1024 * 1024;
    size_t sz = size / len;
    size_t r = size % len;
    char *buf2 = (char *)malloc(size);

    for (size_t i = 0; i < sz; ++i)
        oStream->write<char>("data",buf2+1*len);

    free(buf2);
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "use: %s size-in-byte num_files start_file\n", argv[0]);
        return -1;
    }

    size_t size = strtol(argv[1], NULL, 10);
    if (size == 0)
    {
        fprintf(stderr, "Invalid size (%ld)\n", size);
        return -1;
    }


    struct timeval before, after;
    gettimeofday(&before, NULL);
    int nfile = strtol(argv[2], NULL, 10);
    int start_file = strtol(argv[3], NULL, 10);
    char filename[128];

    for (int i = 0; i < nfile; ++i)
    {
        sprintf(filename, fmt, i + start_file);
        std::string filenameString = std::string(filename);

        adios2::fstream *oStream = new adios2::fstream(filenameString, adios2::fstream::out);

        writedata(oStream, size);

        delete oStream;
    }

    gettimeofday(&after, NULL);
    double elapsed_time = diffmsec(after, before);
    printf("elapsed time (ms) : %g\n", elapsed_time);

    return 0;
}
