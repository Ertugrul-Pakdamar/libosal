# Contributing to libosal

Contributions are welcome. Please read this guide before opening a PR.

---

## Adding a New Platform (most valuable contribution)

If you are targeting FreeRTOS, Windows, Zephyr, bare-metal, or any other
platform not yet supported, here is exactly what to do:

### 1. Create a directory for your platform

The directory name is the platform identifier used in the `PLATFORM` variable.

```
libosal/
└── <platform>/
    └── osal_<platform>.c
```

Examples:
```
libosal/freertos/osal_freertos.c
libosal/windows/osal_windows.c
libosal/zephyr/osal_zephyr.c
```

### 2. Implement every function declared in `include/osal.h`

Open `include/osal.h` — every function listed there must have an implementation
in your file. There are 14 functions total across four groups:

| Group | Functions |
|---|---|
| Mutex | `osal_mutex_init`, `osal_mutex_lock`, `osal_mutex_unlock`, `osal_mutex_destroy` |
| Task | `osal_task_create`, `osal_task_join` |
| Atomic int | `osal_atomic_int_init`, `osal_atomic_int_store`, `osal_atomic_int_load` |
| Atomic size_t | `osal_atomic_size_init`, `osal_atomic_size_store`, `osal_atomic_size_load`, `osal_atomic_size_load_relaxed` |

Use `posix/osal_posix.c` as a reference for the expected behaviour and
memory ordering semantics.

### 3. Verify storage size constants

`include/osal.h` defines two constants that must be large enough to hold the
platform's native types:

```c
#define OSAL_MUTEX_STORAGE  40   /* must be >= sizeof(your_mutex_type) */
#define OSAL_TASK_STORAGE    8   /* must be >= sizeof(your_task_type)  */
```

Add `_Static_assert` checks at the top of your implementation file, as done in
`posix/osal_posix.c`. If your platform requires larger values, update the
constants in `osal.h` and explain the reasoning in your PR.

### 4. Build and test

```bash
make re PLATFORM=<platform>
```

This compiles `<platform>/osal_<platform>.c` and produces `libosal.a`.
If you have access to a consuming project (such as [libqueue](https://github.com/Ertugrul-Pakdamar/libqueue)),
run its test suite against your new `libosal.a`.

### 5. Open a pull request

- Branch name: `platform/<name>` (e.g. `platform/freertos`)
- PR title: `[platform] Add FreeRTOS implementation`
- Describe which OS/RTOS version and toolchain you tested against
- If `OSAL_MUTEX_STORAGE` or `OSAL_TASK_STORAGE` needed changes, explain why

---

## Other contributions

- Bug fixes in the POSIX implementation (`posix/osal_posix.c`)
- Additional atomic types (e.g. `osal_atomic_uint32_t`)
- New examples in `examples/` (see [Adding an example](#adding-an-example) below)
- Documentation improvements

---

## How to contribute

```bash
# 1. Fork the repo on GitHub
# 2. Clone your fork
git clone https://github.com/<your-username>/libosal.git
cd libosal

# 3. Create a feature branch
git checkout -b platform/freertos

# 4. Add your implementation, build clean
make re PLATFORM=freertos

# 5. Open a pull request against main
```

---

## Code style

- C11, compiled with `-Wall -Wextra -Werror` — zero warnings required
- No `#include <pthread.h>`, `<stdatomic.h>`, or any OS header in `include/osal.h`
  — those belong exclusively in the implementation file
- Use `memory_order_release` for stores and `memory_order_acquire` for cross-thread
  loads to ensure correctness of the SPSC ring buffer
- Follow the `osal_` prefix convention for all exported symbols

---

## Adding an example

Examples live in `examples/` and are built with `make examples`. Each example
links against `libosal.a` and `-lpthread`.

### Naming convention

```
examples/NN_short_name.c     (NN = two-digit number, e.g. 03_task_join.c)
```

### Checklist

1. Compiles without warnings under `-Wall -Wextra -Werror`.
2. Top-of-file doc comment explains what the example demonstrates, how to build,
   and how to run it.
3. `main()` returns `0` on success, non-zero on failure — so CI can detect broken examples.
4. All platform-specific includes stay in the OSAL implementation, never in the example.
5. Add a row to the **Examples** table in `README.md`.

---

## MISRA C:2012 Deviation

Rule 21.21 (`<stdatomic.h>`) and POSIX dependencies are confined to
`posix/osal_posix.c` only, with documented rationale:

> Release/acquire memory ordering is required for the lock-free SPSC ring
> buffer and cannot be achieved with standard C11 alone.

New platform implementations may similarly confine platform-specific deviations
to a single `.c` file — but must document each deviation with an inline comment.
