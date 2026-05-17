/**
 * examples/01_mutex.c
 *
 * Demonstrates osal_mutex_t:
 *   - Two threads increment a shared counter under mutex protection.
 *   - The final value must equal INCREMENTS_PER_THREAD * 2 without races.
 *
 * Build:  make examples
 * Run:    ./examples/bin/01_mutex
 */

#include <stdio.h>
#include "osal.h"

#define INCREMENTS_PER_THREAD 100000

typedef struct
{
    osal_mutex_t *lock;
    long         *counter;
} t_worker_args;

static void *increment_worker(void *arg)
{
    t_worker_args *a = (t_worker_args *)arg;

    for (int i = 0; i < INCREMENTS_PER_THREAD; i++)
    {
        osal_mutex_lock(a->lock);
        (*a->counter)++;
        osal_mutex_unlock(a->lock);
    }
    return (NULL);
}

int main(void)
{
    osal_mutex_t  lock;
    osal_task_t   t1, t2;
    long          counter = 0;

    if (osal_mutex_init(&lock) != 0)
    {
        fprintf(stderr, "osal_mutex_init failed\n");
        return (1);
    }

    t_worker_args args = { &lock, &counter };

    osal_task_create(&t1, increment_worker, &args);
    osal_task_create(&t2, increment_worker, &args);

    osal_task_join(&t1);
    osal_task_join(&t2);
    osal_mutex_destroy(&lock);

    long expected = (long)INCREMENTS_PER_THREAD * 2;
    printf("counter:  %ld\n", counter);
    printf("expected: %ld\n", expected);
    printf("result:   %s\n", counter == expected ? "PASS" : "FAIL (race condition)");
    return (counter == expected ? 0 : 1);
}
