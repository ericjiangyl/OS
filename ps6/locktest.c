#include "spinlock.c"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <sys/wait.h>
#include <stdlib.h>

#define BUFFSIZE 4096

int main()
{
    // Without any spinlock
    void * shared_region = mmap(NULL, BUFFSIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(shared_region == (void*)-1)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        return -1;
    }
    unsigned long * val = (unsigned long *)shared_region;
    * val = 0;

    int num_CPU = get_nprocs();
    int fork_result = 0;
    for(int i = 0; i < num_CPU; i++) //create number of threads equal to the number of CPUs. For each thread, add 1 for a million times
    {
        fork_result = fork();
        if(fork_result == -1)
        {
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            return -1;
        }
        else if(fork_result == 0)
        {
            for (int j=0; j < 1000000; ++j) 
            {
                *val += 1;
            }
            exit(0);
        }
    }

    for(int i = 0; i < num_CPU; i++)
        wait(NULL);

    printf("Number of CPU is: %d\n", num_CPU);
    printf("Without any spinlock:\n");
    printf("Result should be 1,000,000*12 = 12,000,000. The actual value is %ld\n", *val);

    //With spinlock
    void * shared_region2 = mmap(NULL, BUFFSIZE, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(shared_region2 == (void*)-1)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        return -1;
    }

    typedef struct val2_struct {
        int lock;
        unsigned long val2;
    } val2_struct;

    val2_struct * val2_p = (val2_struct *)shared_region2;
    val2_p->lock = 0;
    val2_p->val2 = 0;

    for(int i = 0; i < num_CPU; i++) //create number of threads equal to the number of CPUs. For each thread, add 1 for a million times
    {
        fork_result = fork();
        if(fork_result == -1)
        {
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            return -1;
        }
        else if(fork_result == 0)
        {
            for (int j=0; j < 1000000; ++j) 
            {
                spin_lock(&val2_p->lock);
                // When we get here, we are the only task that could be acessing val2
                val2_p->val2 += 1; //CRITICAL REGION
                spin_unlock(&val2_p->lock);
            }
            exit(0);
        }
    }

    for(int i = 0; i < num_CPU; i++)
        wait(NULL);

    printf("Number of CPU is: %d\n", num_CPU);
    printf("With spinlock:\n");
    printf("Result should be 1,000,000*12 = 12,000,000. The actual value is %ld\n", val2_p->val2);

}

