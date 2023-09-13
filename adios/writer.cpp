#include <sys/time.h>
#include <unistd.h>
#include <adios2.h>
#include <string>

static char fmt[] = "file%d.bp";


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

    for (int i = 0 + start_file ; i < nfile; ++i)
    {
        sprintf(filename, fmt, i);
        std::string filenameString = std::string(filename);

        adios2::fstream oStream(filenameString, adios2::fstream::out);
        std::string msg(size, 'a');
        oStream.write<std::string>("data", msg );

        oStream.close();
      
    }

    gettimeofday(&after, NULL);
    double elapsed_time = diffmsec(after, before);
    printf("elapsed time (ms) : %g\n", elapsed_time);

    return 0;
}
