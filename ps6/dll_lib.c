#include "dll_lib.h"
#include <stdio.h>


struct dll * dll_insert(struct dll *anchor, int value, struct slab *my_slab)
{
    int * anchor_lock = &(anchor->dll_lock);
    spin_lock(anchor_lock); 
    //BEGIN CRITICAL REGION
    //We are the only task to access anchor
    struct dll * dll_addr = (struct dll *)(slab_alloc(my_slab));
    //slab_alloc fails, no enough space in the slab
    if(dll_addr == (void *)NULL)
    {
        spin_unlock(anchor_lock);
        return (struct dll *)NULL;
    }
    struct dll * curr_dll = anchor;
    struct dll * prev_dll = anchor;

    spin_lock(&(my_slab->slab_lock));
    while(1)
    {
        prev_dll = curr_dll;
        curr_dll = curr_dll->fwd;
        if( (curr_dll == anchor) || (curr_dll->value >= value) )
        {
            /*We insert dll and break out the while loop under either of the two conditions:
            1. We have iterated through the entire linked list
            2. Next dll has a higher or equal value. Added equal value for the sake of later testing, we might encounter cases
               where dll structs that have the same value needed to be inserted
            */
            struct dll temp;
            temp.value = value;
            temp.fwd = curr_dll;
            temp.dll_lock = 0;
            temp.rev = prev_dll;
            *dll_addr = temp;
            prev_dll->fwd = dll_addr;
            curr_dll->rev = dll_addr;
            break;
        }  
    }
    spin_unlock(&(my_slab->slab_lock));
    //END CRITICAL REGION
    spin_unlock(anchor_lock);
    return dll_addr;
}

void dll_delete(struct dll *anchor, struct dll*node, struct slab *my_slab)
{
    int * anchor_lock = &(anchor->dll_lock);
    spin_lock(anchor_lock); 
    //BEGIN CRITICAL REGION
    //We are the only task to acess anchor.
    int result = slab_dealloc(my_slab, (void *)node); 
    if(result == -1)
    {
        spin_unlock(anchor_lock);
        return;
    }
    struct dll * prev_node = node->rev;
    struct dll * next_node = node->fwd;
    prev_node->fwd = next_node;
    next_node->rev = prev_node;
    //END CRITICAL REGION
    spin_unlock(anchor_lock);
    return;
}

//Before using dll_find function, be sure to lock the slab containing the dll list
struct dll *dll_find(struct dll *anchor, int value)
{
    int * anchor_lock = &(anchor->dll_lock);
    spin_lock(anchor_lock); 
    //BEGIN CRITICAL REGION
    //We are the only task to acess anchor
    struct dll * curr_dll = anchor;
    while(1)
    {
        curr_dll = curr_dll->fwd;
        if( (curr_dll == anchor) || (curr_dll->value > value) )
        {
            /*We stop iterating through the dll list under either of the two conditions:
            1. We have iterated through the entire linked list
            2. Next dll has a higher value
            */
            break;
        }
        if(curr_dll->value == value)
        {
            spin_unlock(anchor_lock);
            return curr_dll;  
        }
    }  
    //END CRITICAL REGION
    spin_unlock(anchor_lock);
    return (struct dll *)NULL;
}
