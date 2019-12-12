#include "dll_lib.c"
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>


int main()
{
    struct slab * my_slab = mmap(NULL, sizeof(struct slab), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, -1, 0);
    if(my_slab == (void*)-1)
    {
        fprintf(stderr, "mmap error: %s\n", strerror(errno));
        return -1;
    }

    struct slab * temp = (struct slab *)(calloc(1, sizeof(struct slab)));
    *my_slab = *temp; //make the shared region to start with all zeros

    //Initialized anchor node
    struct dll * anchor = slab_alloc(my_slab);
    anchor->dll_lock = 0;
    anchor->fwd = anchor;
    anchor->rev = anchor;
    anchor->value = 0; //anchor->value carries no meaning and shouldn't be used

    int fork_result = 0;

    for(int i = 0; i < 8; i++) //create 4 children
    {
        fork_result = fork();
        if(fork_result == -1)
        {
            fprintf(stderr, "fork error: %s\n", strerror(errno));
            return -1;
        }
        else if(fork_result == 0)
        {
            for (int j=0; j < 100000; ++j) //Each child will do 100,000 random operations
            {
                int op = rand() % 3; //select among the three random operations
                int value = rand() % 100; //get a random value between 0 and 99
                struct dll * result;
                switch(op)
                {
                    case 0:
                        result = dll_insert(anchor, value, my_slab);
                        //if(result == (struct dll *)NULL)
                        //fprintf(stderr, "Slab is full\n");
                        //else
                        //    fprintf(stderr, "Insertion value %d\n", value);
                        break;
                    case 1:
                        result = dll_find(anchor, value);
                        //fprintf(stderr, "Find\n");
                        break;
                    case 2:

                        result = dll_find(anchor, value);
                        if(result == (struct dll *)NULL)
                        {
                            break;//No such item exists
                        //    fprintf(stderr, "Find in deletion\n");
                        }
                        else
                        {
                        //    fprintf(stderr, "Deletion value %d is founded.\n", value);   
                            dll_delete(anchor, result, my_slab);
                        //    fprintf(stderr, "Deletion is done.\n");
                        }

                        break;
                }
            }
            exit(0);
        }
    }

    for(int i = 0; i < 8; i++)
        wait(NULL);

    //Print out each value of the struct dll
    struct dll * curr_dll = anchor;
    while(1)
    {
        curr_dll = curr_dll->fwd;
        if(curr_dll == anchor)
            break;
        else
        {
            printf("dll value is %d\n", curr_dll->value);  
        }
    }  
}


