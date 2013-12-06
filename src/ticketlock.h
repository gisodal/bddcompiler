#include "atomics.h"

#ifndef TICKETLOCK_H
#define TICKETLOCK_H

typedef union ticketlock
{
    unsigned u;
    struct
    {
        unsigned short ticket;
        unsigned short users;
    } s;
} ticketlock_t;

static inline __attribute__((always_inline)) void ticketlock_lock(ticketlock_t *t)
{
    register unsigned short me = xinc(&t->s.users);
    register volatile unsigned short *tick = &t->s.ticket;
    while (*tick != me) cpu_relax();
}

static inline __attribute__((always_inline)) void ticketlock_unlock(ticketlock_t *t)
{
    barrier();
    t->s.ticket++;
}

static inline int ticketlock_trylock(ticketlock_t *t)
{
    unsigned short me = t->s.users;
    unsigned short menew = me + 1;
    unsigned cmp = ((unsigned) me << 16) + me;
    unsigned cmpnew = ((unsigned) menew << 16) + me;

    if (cmpxchg(&t->u, cmp, cmpnew) == cmp) return 0;

    return 1;
}

#endif
