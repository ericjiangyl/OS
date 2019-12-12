#ifndef __SLAB_H

#define NSLOTS 50
#include "spinlock.c"

struct dll
{
    int dll_lock;
    int value;
    struct dll *fwd, *rev;
};

struct slab
{
    int slab_lock;
    char freemap[NSLOTS];
    struct dll slots[NSLOTS];
};

void * slab_alloc(struct slab * my_slab);
int slab_dealloc(struct slab * my_slab, void * object);

#define __SLAB_H
#endif