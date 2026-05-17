# libosal

> **Repo:** [github.com/Ertugrul-Pakdamar/libosal](https://github.com/Ertugrul-Pakdamar/libosal)

A standalone C11 Operating System Abstraction Layer (OSAL) that isolates all
platform-specific code — threads, mutexes, and atomic operations — behind a
single clean API.

Consumers include only `osal.h` and have **zero dependency** on any OS or
compiler-extension header. To run on a new platform, one file is written; nothing
else in the codebase changes.

---

## Why OSAL?

| Without OSAL | With OSAL |
|---|---|
| `#include <pthread.h>` scattered across the codebase | All OS code in one file |
| `_Atomic` / `<stdatomic.h>` in every translation unit | Atomics behind `osal_atomic_*` |
| Porting requires modifying many files | Write `<platform>/osal_<platform>.c`, done |
| MISRA Rule 21.21 violation everywhere | Deviation documented and contained |

---

## Repository Layout

```
libosal/
├── include/
│   └── osal.h                   public API  ← the only file consumers see
├── posix/
│   └── osal_posix.c             POSIX / Linux / macOS implementation
├── examples/
│   ├── 01_mutex.c               two threads, shared counter, mutex protection
│   └── 02_atomic.c              lock-free producer/consumer with release/acquire
├── build/                       generated — not committed
├── Makefile
├── README.md
└── CONTRIBUTING.md
```

When a new platform is added, it follows the same pattern:

```
libosal/
├── freertos/
│   └── osal_freertos.c          FreeRTOS implementation  (contributed)
├── windows/
│   └── osal_windows.c           Win32 implementation     (contributed)
```

---

## API Reference

All platform types are opaque structs. Never access internal fields directly.

### Mutex

```c
int osal_mutex_init(osal_mutex_t *m);
int osal_mutex_lock(osal_mutex_t *m);
int osal_mutex_unlock(osal_mutex_t *m);
int osal_mutex_destroy(osal_mutex_t *m);
```

All functions return `0` on success, non-zero on failure.

### Task (thread)

```c
int osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg);
int osal_task_join(osal_task_t *t);
```

### Atomic `int`

```c
void osal_atomic_int_init(osal_atomic_int_t *a, int val);
void osal_atomic_int_store(osal_atomic_int_t *a, int val);  /* release */
int  osal_atomic_int_load(const osal_atomic_int_t *a);      /* acquire */
```

### Atomic `size_t`

```c
void   osal_atomic_size_init(osal_atomic_size_t *a, size_t val);
void   osal_atomic_size_store(osal_atomic_size_t *a, size_t val);        /* release */
size_t osal_atomic_size_load(const osal_atomic_size_t *a);               /* acquire */
size_t osal_atomic_size_load_relaxed(const osal_atomic_size_t *a);       /* relaxed */
```

Use `_load` (acquire) when reading an index owned by the **other** side of a
lock-free structure. Use `_load_relaxed` when reading an index owned by the
**current** side — no cross-thread synchronisation is needed in that case.

---

## Getting Started

```bash
git clone https://github.com/Ertugrul-Pakdamar/libosal.git
cd libosal
make all     # builds libosal.a using the POSIX implementation
```

### Available targets

```
make all                    Build libosal.a  (default platform: posix)
make all PLATFORM=freertos  Build with a custom platform implementation
make examples               Build all programs in examples/
make clean                  Remove object files and example binaries
make fclean                 clean + remove libosal.a
make re                     fclean + all
```

---

## Examples

Ready-to-run programs live in `examples/`. Build them all with:

```bash
make examples
```

| Binary | Source | What it shows |
|---|---|---|
| `examples/bin/01_mutex` | `examples/01_mutex.c` | Two threads incrementing a counter under mutex protection |
| `examples/bin/02_atomic` | `examples/02_atomic.c` | Lock-free producer/consumer hand-off with release/acquire atomics |

---

## Integration into Your Project

```bash
# Option A — copy files
cp libosal.a         /your/project/lib/
cp include/osal.h    /your/project/include/

# Link (POSIX requires -lpthread)
cc main.c -o app -Iinclude -Llib -losal -lpthread
```

```makefile
# Option B — use as a sub-project
LIBOSAL = deps/osal/libosal.a
$(LIBOSAL):
	$(MAKE) -C deps/osal all
```

---

## MISRA C:2012 Deviation

Rule 21.21 (`<stdatomic.h>`) and POSIX dependencies are confined to
`posix/osal_posix.c` only, with documented rationale:

> Release/acquire memory ordering is required for the lock-free SPSC ring
> buffer and cannot be achieved with standard C11 alone.

All other files remain MISRA-compliant. This deviation is intentional,
contained, and documented — not an oversight.

---

## Contributing

See [CONTRIBUTING.md](CONTRIBUTING.md).


---

## Purpose

| Without OSAL | With OSAL |
|---|---|
| `#include <pthread.h>` scattered everywhere | All OS code confined to one file |
| `_Atomic` / `stdatomic.h` in every translation unit | Atomics hidden behind `osal_atomic_*` wrappers |
| Hard to port to FreeRTOS / Windows / bare-metal | Write one new `osal_<platform>.c` to port |

---

## API Overview

### Mutex

```c
int osal_mutex_init(osal_mutex_t *m);
int osal_mutex_lock(osal_mutex_t *m);
int osal_mutex_unlock(osal_mutex_t *m);
int osal_mutex_destroy(osal_mutex_t *m);
```

### Task (thread)

```c
int osal_task_create(osal_task_t *t, void *(*fn)(void *), void *arg);
int osal_task_join(osal_task_t *t);
```

### Atomic `int`

```c
void osal_atomic_int_init(osal_atomic_int_t *a, int val);
void osal_atomic_int_store(osal_atomic_int_t *a, int val);  /* release */
int  osal_atomic_int_load(const osal_atomic_int_t *a);      /* acquire */
```

### Atomic `size_t`

```c
void   osal_atomic_size_init(osal_atomic_size_t *a, size_t val);
void   osal_atomic_size_store(osal_atomic_size_t *a, size_t val); /* release */
size_t osal_atomic_size_load(const osal_atomic_size_t *a);        /* acquire */
size_t osal_atomic_size_load_relaxed(const osal_atomic_size_t *a);/* relaxed */
```

Use `_load` (acquire) when reading an index owned by the **other** side of the
ring buffer. Use `_load_relaxed` when reading an index owned by the **current**
side — no cross-thread synchronization is needed in that case.

---

## Directory Layout

```
deps/osal/
├── include/
│   └── osal.h              public API (no OS headers)
├── posix/
│   └── osal_posix.c        POSIX implementation (pthread.h + stdatomic.h)
├── build/                  object files (generated)
├── Makefile
├── .gitignore
└── README.md
```

---

## Building

```bash
cd deps/osal
make        # produces libosal.a
make clean
make re
```

To use as a sub-project from a parent Makefile:

```makefile
LIBOSAL = deps/osal/libosal.a
$(LIBOSAL):
	$(MAKE) -C deps/osal all
```

---

## Porting to a New Platform

1. Create a new directory inside `deps/osal/`, e.g. `deps/osal/freertos/`.
2. Implement every function declared in `include/osal.h` in a new source file
   `osal_freertos.c` using the target platform's primitives.
3. Update the `Makefile` (or parent build system) to compile your file instead
   of `posix/osal_posix.c`.
4. No other file in the codebase needs to change.

---

## MISRA C:2012 Deviation

Rule 21.21 (`<stdatomic.h>`) and POSIX dependencies are confined to
`posix/osal_posix.c` only, with documented rationale:

> Release/acquire memory ordering is required for the lock-free SPSC ring
> buffer and cannot be achieved with standard C11 alone.
