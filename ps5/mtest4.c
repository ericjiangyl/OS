#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    int file_size;
    struct stat st; 
    if (stat("/home/eric/Desktop/ECE_357_Operating_system/ps5/test1", &st) == 0)
        file_size = st.st_size;
    else
    {
        fprintf(stderr, "Can't determine the file size: %s\n", strerror(errno));
        exit(-1);
    }

    char * retVal;
    retVal = mmap(NULL, 4096, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);

    fprintf(stderr, "Executing Test #4 (write into a hole): \n");
    fprintf(stderr, "Writing a 'B' to one byte beyond the last byte\n");
    retVal[file_size] = 'B';
    int lseekVal = lseek(fd, 16, SEEK_END);
    int writeVal = write(fd, "E",1);
    lseekVal = lseek(fd, 0, SEEK_SET);
    int bytes = read(fd, buff, BUFFSIZE);
    fprintf(stderr, "Byte x is %c\n", buff[file_size]);
    if(buff[file_size] == 'B')
        exit(0);
    else
        exit(1);    

}