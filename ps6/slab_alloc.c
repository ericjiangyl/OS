#include <stdio.h>
#include "slab_alloc.h"

void * slab_alloc(struct slab * my_slab)
{
    int * lock_addr = &(my_slab->slab_lock);
    //BEGIN CRITICAL REGION
    //We are the only taks that could acess my_slab
    spin_lock(lock_addr);
    for(int i = 0; i < NSLOTS; i++)
    {
        //if the slot is free, occupies this slot
        if(my_slab->freemap[i] == 0)
        {
            my_slab->freemap[i] = 1;
            spin_unlock(lock_addr);
            return &(my_slab->slots[i]);
        }
    }
    spin_unlock(lock_addr);
    //END CRITICAL REGION
    return (void *)NULL;
}

int slab_dealloc(struct slab * my_slab, void * object)
{
    //check whether void * object is a valid address for an entry in struct dll slots
    void * begin = &(my_slab->slots[0]);
    void * end = &(my_slab->slots[NSLOTS]);
    if(object < begin)
        return -1;
    else if(object > end)
        return -1;
    
    struct dll * temp = (struct dll *) object;
    int pos = temp - my_slab->slots;
    int * lock_addr = &(my_slab->slab_lock);
    spin_lock(lock_addr);
    //check if the intended address is free. If free, error.
    //BEGIN CRITICAL REGION
    //We are the only task that could be accessing my_slab
    if(my_slab->freemap[pos] == 0)
    {
        spin_unlock(lock_addr);
        return -1;
    }
    else
    {
        my_slab->freemap[pos] = 0;    
        spin_unlock(lock_addr);
        return 1;
    }
    //END CRITICAL REGION
}