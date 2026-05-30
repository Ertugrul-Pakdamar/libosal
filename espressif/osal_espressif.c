/*
** osal_posix.c — POSIX implementation of the OSAL layer.
**
** MISRA C:2012 documented deviation:
**   This file intentionally includes non-standard headers:
**     - <pthread.h>  : POSIX thread, mutex, and task management.
**     - <stdatomic.h>: C11 atomic operations (Rule 21.21 deviation).
**   Rationale: release/acquire memory ordering is required for the lock-free
**              SPSC ring buffer and cannot be achieved with standard C alone.
**   Containment: all OS dependencies are confined to this single file.
**                Every other libqueue/libosal file sees only osal.h.
**   Porting: to support a new platform, provide an equivalent file
**            (e.g. freertos/osal_freertos.c) implementing the same API.
*/

#include "../include/osal.h"

/* ---- Mutex --------------------------------------------------------------- */

int     osal_mutex_init(osal_mutex_t *m)
{
    return (1);
}

int     osal_mutex_lock(osal_mutex_t *m)
{
    return (1);
}

int     osal_mutex_unlock(osal_mutex_t *m)
{
    return (1);
}

int     osal_mutex_destroy(osal_mutex_t *m)
{
    return (1);
}

/* ---- Task ---------------------------------------------------------------- */

int     osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg)
{
    return (1);
}

int     osal_task_join(osal_task_t *t)
{
    return (1);
}

/* ---- Atomic int ---------------------------------------------------------- */

void    osal_atomic_int_init(osal_atomic_int_t *a, int val)
{}

void    osal_atomic_int_store(osal_atomic_int_t *a, int val)
{}

int     osal_atomic_int_load(const osal_atomic_int_t *a)
{
    return (1);
}

/* ---- Atomic size_t ------------------------------------------------------- */

void    osal_atomic_size_init(osal_atomic_size_t *a, size_t val)
{}

void    osal_atomic_size_store(osal_atomic_size_t *a, size_t val)
{}

size_t  osal_atomic_size_load(const osal_atomic_size_t *a)
{
    return (1);
}

size_t  osal_atomic_size_load_relaxed(const osal_atomic_size_t *a)
{
    return (1);
}
