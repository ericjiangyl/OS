#include "tas.h"
#include <stdio.h>

//extern int tas(volatile char *lock);

void spin_lock(int * lock)
{
    while(tas((char *)lock) == 1) //tas returns 1 means the lock is occupied, keep trying
        ;
}

void spin_unlock(int * lock)
{
    *lock = 0;
}