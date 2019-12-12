#include <stdio.h>
#include <signal.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

void sig_segv_handler(int sigNum)
{
    fprintf(stderr, "Signal SIG_SEGV received.\n");
    exit(SIGSEGV);
}

int main()
{
    signal(SIGSEGV, sig_segv_handler);
    fprintf(stderr, "Executing Test #1 (write to r/o mmap): \n");
    int fd = open("/home/eric/Desktop/ECE_357_Operating_system/ps5/test1", O_RDONLY);
    if(fd == -1)
    {
        fprintf(stderr, "Can't open the test file: %s\n", strerror(errno));
        exit(-1);
    }
    char * retVal;
    retVal = mmap(NULL, 4096, PROT_READ, MAP_PRIVATE, fd, 0);
    fprintf(stderr, "map[3] == %c\n", retVal[3]);
    fprintf(stderr, "Writing a 'B'\n");
    retVal[0] = 'B';
    return 0;
}