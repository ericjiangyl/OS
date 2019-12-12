#ifndef __DLL_H
#include "slab_alloc.c"

struct seqlock
{
    int lock;
    int count;
};

struct dll * dll_insert(struct dll *anchor, int value, struct slab *slab);
void dll_delete(struct dll *anchor, struct dll*node, struct slab *slab);
struct dll *dll_find(struct dll *anchor, int value);
#define __DLL_H
#endif