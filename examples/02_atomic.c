/**
 * examples/02_atomic.c
 *
 * Demonstrates osal_atomic_int_t and osal_atomic_size_t:
 *   - A producer task stores values with release ordering.
 *   - The main thread loads them with acquire ordering.
 *   - No mutex required for the flag/counter hand-off.
 *
 * Build:  make examples
 * Run:    ./examples/bin/02_atomic
 */

#include <stdio.h>
#include "osal.h"

#define ITEM_COUNT 5

typedef struct
{
    osal_atomic_int_t    ready;     /* set to 1 after each item is produced */
    osal_atomic_size_t   value;     /* the produced value                   */
    osal_atomic_int_t    done;      /* set to 1 when producer is finished   */
} t_channel;

static void *producer(void *arg)
{
    t_channel *ch = (t_channel *)arg;

    for (size_t i = 1; i <= ITEM_COUNT; i++)
    {
        /* Write value before signalling ready (release). */
        osal_atomic_size_store(&ch->value, i);
        osal_atomic_int_store(&ch->ready, 1);

        /* Spin until the consumer acknowledges (clears ready). */
        while (osal_atomic_int_load(&ch->ready) != 0)
            ;
    }

    osal_atomic_int_store(&ch->done, 1);
    return (NULL);
}

int main(void)
{
    t_channel  ch;
    osal_task_t producer_task;

    osal_atomic_int_init(&ch.ready, 0);
    osal_atomic_size_init(&ch.value, 0);
    osal_atomic_int_init(&ch.done,  0);

    osal_task_create(&producer_task, producer, &ch);

    /* Consumer: wait for each item, then acknowledge. */
    int received = 0;
    while (received < ITEM_COUNT)
    {
        /* Spin until producer signals (acquire). */
        while (osal_atomic_int_load(&ch.ready) == 0)
            ;

        size_t v = osal_atomic_size_load(&ch.value);
        printf("[consumer] received: %zu\n", v);
        received++;

        /* Acknowledge: clear the flag so producer can advance. */
        osal_atomic_int_store(&ch.ready, 0);
    }

    osal_task_join(&producer_task);
    printf("all %d items transferred without a mutex.\n", ITEM_COUNT);
    return (0);
}
