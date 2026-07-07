/*
** osal_espressif.c — ESP-IDF (FreeRTOS) implementation of the OSAL layer.
**
** Uses FreeRTOS primitives for Tasks and Mutexes.
** Uses C11 <stdatomic.h> for lock-free atomic operations, which is fully
** supported by the GCC toolchain provided in ESP-IDF.
*/

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>
#include <stdatomic.h>
#include "../include/libosal.h"

/* Compile-time checks to ensure opaque storage is large enough */
_Static_assert(sizeof(SemaphoreHandle_t) <= OSAL_MUTEX_STORAGE,
    "OSAL_MUTEX_STORAGE is too small for SemaphoreHandle_t");
_Static_assert(sizeof(TaskHandle_t) <= OSAL_TASK_STORAGE,
    "OSAL_TASK_STORAGE is too small for TaskHandle_t");

/* ---- Mutex --------------------------------------------------------------- */

int32_t     osal_mutex_init(osal_mutex_t *m)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t *)m->_s;
    *sem = xSemaphoreCreateMutex();
    return (*sem == NULL ? 1 : 0);
}

int32_t     osal_mutex_lock(osal_mutex_t *m)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t *)m->_s;
    if (xSemaphoreTake(*sem, portMAX_DELAY) == pdTRUE)
        return (0);
    return (1);
}

int32_t     osal_mutex_unlock(osal_mutex_t *m)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t *)m->_s;
    if (xSemaphoreGive(*sem) == pdTRUE)
        return (0);
    return (1);
}

int32_t     osal_mutex_destroy(osal_mutex_t *m)
{
    SemaphoreHandle_t *sem = (SemaphoreHandle_t *)m->_s;
    vSemaphoreDelete(*sem);
    return (0);
}

/* ---- Task ---------------------------------------------------------------- */

int32_t     osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg)
{
    TaskHandle_t *handle = (TaskHandle_t *)t->_s;
    /* Create task with default parameters. Priority 5 is moderate.
       Stack size is 4096 bytes. */
    BaseType_t res = xTaskCreate(
        (TaskFunction_t)fn,
        "osal_task",
        4096,
        arg,
        5,
        handle
    );
    return (res == pdPASS ? 0 : 1);
}

int32_t     osal_task_join(osal_task_t *t)
{
    /* FreeRTOS does not have a native "join" like pthreads.
       For a complete OSAL, one would typically use an EventGroup or TaskNotify
       to wait for the task to finish before deleting it.
       For now, since libqueue's listener_stop() manages the running flag,
       we delay to allow the task to exit.
       A robust join implementation in ESP-IDF requires additional OSAL state. */
    vTaskDelay(pdMS_TO_TICKS(50));
    return (0);
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
    return (atomic_load_explicit((const _Atomic int32_t *)&a->_val, memory_order_acquire));
}

/* ---- Atomic size_t ------------------------------------------------------- */

void    osal_atomic_size_init(osal_atomic_size_t *a, size_t val)
{
    atomic_init((_Atomic size_t *)&a->_val, val);
}

void    osal_atomic_size_store(osal_atomic_size_t *a, size_t val)
{
    atomic_store_explicit((_Atomic size_t *)&a->_val, val, memory_order_release);
}

size_t  osal_atomic_size_load(const osal_atomic_size_t *a)
{
    return (atomic_load_explicit((const _Atomic size_t *)&a->_val, memory_order_acquire));
}

size_t  osal_atomic_size_load_relaxed(const osal_atomic_size_t *a)
{
    return (atomic_load_explicit((const _Atomic size_t *)&a->_val, memory_order_relaxed));
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
