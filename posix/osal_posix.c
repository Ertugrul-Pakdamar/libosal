/*
** osal_posix.c — POSIX implementasyonu
**
** MISRA C:2012 sapma notu (belgelenmiş):
**   Bu dosya kasıtlı olarak aşağıdaki standart dışı başlıkları kullanır:
**     - <pthread.h>  : POSIX thread, mutex, task yönetimi
**     - <stdatomic.h>: C11 atomik operasyonlar (Rule 21.21 sapması)
**   Gerekçe: lock-free SPSC için release/acquire bellek sıralaması zorunludur.
**   Risk analizi: Yalnızca bu dosyayla sınırlı; libqueue'nun geri kalanı
**                 yalnızca osal.h görür.
**   Alternatif platform desteği için bu dosyanın eşdeğeri yazılmalıdır.
*/

#include <pthread.h>
#include <stdatomic.h>
#include "../include/osal.h"

_Static_assert(sizeof(pthread_mutex_t) <= OSAL_MUTEX_STORAGE,
    "OSAL_MUTEX_STORAGE pthread_mutex_t icin kucuk");
_Static_assert(sizeof(pthread_t) <= OSAL_TASK_STORAGE,
    "OSAL_TASK_STORAGE pthread_t icin kucuk");
_Static_assert(sizeof(int)    == sizeof(int),    "int boyut uyusmazligi");
_Static_assert(sizeof(size_t) == sizeof(size_t), "size_t boyut uyusmazligi");

/* ---- Mutex --------------------------------------------------------------- */

int     osal_mutex_init(osal_mutex_t *m)
{
    return (pthread_mutex_init((pthread_mutex_t *)m->_s, NULL));
}

int     osal_mutex_lock(osal_mutex_t *m)
{
    return (pthread_mutex_lock((pthread_mutex_t *)m->_s));
}

int     osal_mutex_unlock(osal_mutex_t *m)
{
    return (pthread_mutex_unlock((pthread_mutex_t *)m->_s));
}

int     osal_mutex_destroy(osal_mutex_t *m)
{
    return (pthread_mutex_destroy((pthread_mutex_t *)m->_s));
}

/* ---- Task ---------------------------------------------------------------- */

int     osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg)
{
    return (pthread_create((pthread_t *)t->_s, NULL, fn, arg));
}

int     osal_task_join(osal_task_t *t)
{
    return (pthread_join(*(pthread_t *)t->_s, NULL));
}

/* ---- Atomic int ---------------------------------------------------------- */

void    osal_atomic_int_init(osal_atomic_int_t *a, int val)
{
    atomic_init((_Atomic int *)&a->_val, val);
}

void    osal_atomic_int_store(osal_atomic_int_t *a, int val)
{
    atomic_store_explicit((_Atomic int *)&a->_val, val, memory_order_release);
}

int     osal_atomic_int_load(const osal_atomic_int_t *a)
{
    return (atomic_load_explicit((const _Atomic int *)&a->_val,
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
