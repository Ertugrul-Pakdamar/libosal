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
**                Every other libqueue/libosal file sees only libosal.h.
**   Porting: to support a new platform, provide an equivalent file
**            (e.g. freertos/osal_freertos.c) implementing the same API.
*/

#include <pthread.h>
#include <stdatomic.h>
#include "../include/libosal.h"

_Static_assert(sizeof(pthread_mutex_t) <= OSAL_MUTEX_STORAGE,
    "OSAL_MUTEX_STORAGE is too small for pthread_mutex_t");
_Static_assert(sizeof(pthread_t) <= OSAL_TASK_STORAGE,
    "OSAL_TASK_STORAGE is too small for pthread_t");

/* ---- Mutex --------------------------------------------------------------- */

int32_t     osal_mutex_init(osal_mutex_t *m)
{
    return (pthread_mutex_init((pthread_mutex_t *)m->_s, NULL));
}

int32_t     osal_mutex_lock(osal_mutex_t *m)
{
    return (pthread_mutex_lock((pthread_mutex_t *)m->_s));
}

int32_t     osal_mutex_unlock(osal_mutex_t *m)
{
    return (pthread_mutex_unlock((pthread_mutex_t *)m->_s));
}

int32_t     osal_mutex_destroy(osal_mutex_t *m)
{
    return (pthread_mutex_destroy((pthread_mutex_t *)m->_s));
}

/* ---- Task ---------------------------------------------------------------- */

int32_t     osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg)
{
    return (pthread_create((pthread_t *)t->_s, NULL, fn, arg));
}

int32_t     osal_task_join(osal_task_t *t)
{
    return (pthread_join(*(pthread_t *)t->_s, NULL));
}

/* ---- Atomic int32_t ---------------------------------------------------------- */

void    osal_atomic_int_init(osal_atomic_int_t *a, int32_t val)
{
    atomic_init((_Atomic int32_t *)&a->_val, val);
}

void    osal_atomic_int_store(osal_atomic_int_t *a, int32_t val)
{
    atomic_store_explicit((_Atomic int32_t *)&a->_val, val, memory_order_release);
}

int32_t     osal_atomic_int_load(const osal_atomic_int_t *a)
{
    return (atomic_load_explicit((const _Atomic int32_t *)&a->_val,
                memory_order_acquire));
}

/* ---- Atomic size_t ------------------------------------------------------- */

void    osal_atomic_size_init(osal_atomic_size_t *a, size_t val)
{
    atomic_init((_Atomic size_t *)&a->_val, val);
}

void    osal_atomic_size_store(osal_atomic_size_t *a, size_t val)
{
    atomic_store_explicit((_Atomic size_t *)&a->_val, val,
        memory_order_release);
}

size_t  osal_atomic_size_load(const osal_atomic_size_t *a)
{
    return (atomic_load_explicit((const _Atomic size_t *)&a->_val,
                memory_order_acquire));
}

size_t  osal_atomic_size_load_relaxed(const osal_atomic_size_t *a)
{
    return (atomic_load_explicit((const _Atomic size_t *)&a->_val,
                memory_order_relaxed));
}

void    osal_atomic_size_fetch_or(osal_atomic_size_t *a, size_t val)
{
    atomic_fetch_or_explicit((_Atomic size_t *)&a->_val, val, memory_order_release);
}

void    osal_atomic_size_fetch_and(osal_atomic_size_t *a, size_t val)
{
    atomic_fetch_and_explicit((_Atomic size_t *)&a->_val, val, memory_order_release);
}

int32_t osal_atomic_size_compare_exchange(osal_atomic_size_t *a, size_t *expected, size_t desired)
{
    return atomic_compare_exchange_weak_explicit((_Atomic size_t *)&a->_val, expected, desired,
        memory_order_acq_rel, memory_order_acquire);
}

/* ---- Atomic pointer ------------------------------------------------------ */

void    osal_atomic_ptr_init(osal_atomic_ptr_t *a, void *val)
{
    atomic_init((_Atomic(void *) *)&a->_val, val);
}

void    osal_atomic_ptr_store(osal_atomic_ptr_t *a, void *val)
{
    atomic_store_explicit((_Atomic(void *) *)&a->_val, val, memory_order_release);
}

void*   osal_atomic_ptr_load(const osal_atomic_ptr_t *a)
{
    return atomic_load_explicit((const _Atomic(void *) *)&a->_val, memory_order_acquire);
}

void*   osal_atomic_ptr_exchange(osal_atomic_ptr_t *a, void *val)
{
    return atomic_exchange_explicit((_Atomic(void *) *)&a->_val, val, memory_order_acq_rel);
}

int32_t osal_atomic_ptr_compare_exchange(osal_atomic_ptr_t *a, void **expected, void *desired)
{
    return atomic_compare_exchange_weak_explicit((_Atomic(void *) *)&a->_val, expected, desired,
        memory_order_acq_rel, memory_order_acquire);
}
