/**
 * @file libosal.h
 * @brief Operating System Abstraction Layer — public API.
 *
 * Isolates all platform-specific code (pthreads, stdatomic) from the rest of
 * the library. Consumers include only this header and have zero dependency on
 * any OS or compiler extension header.
 *
 * To port to a new platform, implement every function declared here in a new
 * translation unit (e.g. freertos/osal_freertos.c) and link it instead of the
 * POSIX implementation.
 *
 * MISRA C:2012 deviation note:
 *   Rule 21.21 (<stdatomic.h>) and POSIX dependencies are confined to
 *   posix/osal_posix.c only.
 *   Rationale: release/acquire memory ordering is required for the lock-free
 *   SPSC ring buffer and cannot be achieved with standard C alone.
 */

#ifndef OSAL_H
# define OSAL_H

/* ---- Version ------------------------------------------------------------- */
# define LIBOSAL_VERSION_MAJOR 0
# define LIBOSAL_VERSION_MINOR 1
# define LIBOSAL_VERSION_PATCH 0
# define LIBOSAL_VERSION       "0.1.0"

# include <stddef.h>

/* ---- Storage size constants ---------------------------------------------- */
/* Verified against the real platform types via _Static_assert in osal_posix.c */

# define OSAL_MUTEX_STORAGE 40 /**< Byte size of the opaque mutex storage.   */
# define OSAL_TASK_STORAGE   8 /**< Byte size of the opaque task storage.    */

/* ---- Opaque handle types ------------------------------------------------- */
/* Never access the internal _s field directly.                               */

/** Opaque mutex handle. Initialized by osal_mutex_init(). */
typedef struct { unsigned char _s[OSAL_MUTEX_STORAGE]; } osal_mutex_t;

/** Opaque task (thread) handle. Initialized by osal_task_create(). */
typedef struct { unsigned char _s[OSAL_TASK_STORAGE];  } osal_task_t;

/** Atomic int. Access only through osal_atomic_int_* functions. */
typedef struct { int    _val; } osal_atomic_int_t;

/** Atomic size_t. Access only through osal_atomic_size_* functions. */
typedef struct { size_t _val; } osal_atomic_size_t;

/* ---- Mutex --------------------------------------------------------------- */

/**
 * @brief Initialize a mutex.
 * @param m Uninitialized mutex handle.
 * @return 0 on success, non-zero on failure.
 */
int osal_mutex_init(osal_mutex_t *m);

/**
 * @brief Acquire a mutex (blocking).
 * @param m Initialized mutex handle.
 * @return 0 on success, non-zero on failure.
 */
int osal_mutex_lock(osal_mutex_t *m);

/**
 * @brief Release a previously acquired mutex.
 * @param m Initialized and locked mutex handle.
 * @return 0 on success, non-zero on failure.
 */
int osal_mutex_unlock(osal_mutex_t *m);

/**
 * @brief Destroy a mutex and release its resources.
 * @param m Initialized mutex handle. Must not be locked at the time of call.
 * @return 0 on success, non-zero on failure.
 */
int osal_mutex_destroy(osal_mutex_t *m);

/* ---- Task ---------------------------------------------------------------- */

/**
 * @brief Create and immediately start a task (thread).
 * @param t   Uninitialized task handle.
 * @param fn  Entry function with signature @c void* fn(void*).
 * @param arg Argument forwarded to @p fn.
 * @return 0 on success, non-zero on failure.
 */
int osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg);

/**
 * @brief Block until a task finishes (join).
 * @param t Running task handle.
 * @return 0 on success, non-zero on failure.
 */
int osal_task_join(osal_task_t *t);

/* ---- Atomic int ---------------------------------------------------------- */

/**
 * @brief Initialize an atomic int.
 * @param a   Uninitialized atomic int.
 * @param val Initial value.
 */
void osal_atomic_int_init(osal_atomic_int_t *a, int val);

/**
 * @brief Store a value with release memory ordering.
 * @param a   Initialized atomic int.
 * @param val Value to store.
 */
void osal_atomic_int_store(osal_atomic_int_t *a, int val);

/**
 * @brief Load a value with acquire memory ordering.
 * @param a Initialized atomic int.
 * @return Current value.
 */
int  osal_atomic_int_load(const osal_atomic_int_t *a);

/* ---- Atomic size_t ------------------------------------------------------- */

/**
 * @brief Initialize an atomic size_t.
 * @param a   Uninitialized atomic size_t.
 * @param val Initial value.
 */
void osal_atomic_size_init(osal_atomic_size_t *a, size_t val);

/**
 * @brief Store a value with release memory ordering.
 * @param a   Initialized atomic size_t.
 * @param val Value to store.
 */
void osal_atomic_size_store(osal_atomic_size_t *a, size_t val);

/**
 * @brief Load a value with acquire memory ordering.
 *
 * Use when reading an index written by the *other* side across a thread
 * boundary (e.g. producer reading read_idx, consumer reading write_idx).
 * @param a Initialized atomic size_t.
 * @return Current value.
 */
size_t osal_atomic_size_load(const osal_atomic_size_t *a);

/**
 * @brief Load a value with relaxed memory ordering.
 *
 * Use when reading an index owned by the *current* side (e.g. producer
 * reading its own write_idx). No cross-thread synchronization is needed
 * because the owning thread is always coherent with its own writes.
 * @param a Initialized atomic size_t.
 * @return Current value.
 */
size_t osal_atomic_size_load_relaxed(const osal_atomic_size_t *a);

#endif
