#ifndef OSAL_H
# define OSAL_H

# include <stddef.h>

/*
** OSAL — Operating System Abstraction Layer
**
** Tüm platform bağımlı kod (pthread, stdatomic) bu katmanda izole edilir.
** libqueue, platform başlıklarını doğrudan include etmez; yalnızca osal.h görür.
**
** MISRA C:2012 sapma notu:
**   Rule 21.21 (<stdatomic.h>) ve POSIX bağımlılıkları yalnızca
**   osal/posix/osal_posix.c dosyasına hapsedilmiştir.
**   Sapma gerekçesi: lock-free SPSC için bellek sıralaması garantisi zorunludur.
*/

/* ---- Platform depolama boyutları ---------------------------------------- */
/* _Static_assert ile osal_posix.c içinde doğrulanır                         */
# define OSAL_MUTEX_STORAGE 40
# define OSAL_TASK_STORAGE   8

/* Opak Types */

typedef struct { unsigned char _s[OSAL_MUTEX_STORAGE]; } osal_mutex_t;
typedef struct { unsigned char _s[OSAL_TASK_STORAGE];  } osal_task_t;
typedef struct { int    _val; }                           osal_atomic_int_t;
typedef struct { size_t _val; }                           osal_atomic_size_t;

/* Mutex */

int  osal_mutex_init(osal_mutex_t *m);
int  osal_mutex_lock(osal_mutex_t *m);
int  osal_mutex_unlock(osal_mutex_t *m);
int  osal_mutex_destroy(osal_mutex_t *m);

/* Task */

int  osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg);
int  osal_task_join(osal_task_t *t);

/* Atomic int */

void osal_atomic_int_init(osal_atomic_int_t *a, int val);
void osal_atomic_int_store(osal_atomic_int_t *a, int val);   /* release */
int  osal_atomic_int_load(const osal_atomic_int_t *a);       /* acquire */

/* Atomic size_t */
/* _relaxed: kendi index'ini okuyan taraf (produce/consumer kendi yazısını    */
/*           görmesi garanti — relaxed yeterli)                               */
/* varsayılan load: acquire (karşı tarafın index'ini okurken)                 */

void   osal_atomic_size_init(osal_atomic_size_t *a, size_t val);
void   osal_atomic_size_store(osal_atomic_size_t *a, size_t val); /* release */
size_t osal_atomic_size_load(const osal_atomic_size_t *a);        /* acquire */
size_t osal_atomic_size_load_relaxed(const osal_atomic_size_t *a);

#endif
