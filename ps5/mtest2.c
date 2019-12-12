#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

# define BUFFSIZE 4096

int main()
{
    char buff [BUFFSIZE];
    int fd = open("/home/eric/Desktop/ECE_357_Operating_system/ps5/test1", O_RDWR);
    if(fd == -1)
    {
        fprintf(stderr, "Can't open the test file: %s\n", strerror(errno));
        exit(-1);
    }
    char * retVal;
    retVal = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    fprintf(stderr, "Executing Test #2 (write to MAP_SHARED): \n");
    int bytes = read(fd, buff, BUFFSIZE);
    fprintf(stderr, "Original content of the file is %s \n", buff);
    fprintf(stderr, "Writing a 'B' at the third byte\n");
    char oldVal = buff[3];
    retVal[3] = 'B';
    int fd2 = open("/home/eric/Desktop/ECE_357_Operating_system/ps5/test1", O_RDONLY);
    if(fd2 == -1)
    {
        fprintf(stderr, "Can't open the test file: %s\n", strerror(errno));
        exit(-1);
    }
    bytes = read(fd2, buff, BUFFSIZE);
    fprintf(stderr, "New content of the file is %s \n", buff);
    if(buff[3] == oldVal)
        exit(1);
    else
        exit(0);    
}