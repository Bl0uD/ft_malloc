# 42 Malloc — Dynamic Memory Allocator

<div align="center">

![42 School](https://img.shields.io/badge/42-Project-000000?style=for-the-badge&logo=42)
![Language](https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c)
![Score](https://img.shields.io/badge/Score-125%20%2F%20100-success?style=for-the-badge)
![Status](https://img.shields.io/badge/Status-Completed-brightgreen?style=for-the-badge)

*A clean, thread-safe, 16-byte aligned dynamic memory management library adhering to Version 7.1 of the 42 subject.*

</div>

---

## 📌 Overview

This project is a complete reimplementation of the standard C dynamic memory allocation family:
- `malloc(size_t size)`
- `free(void *ptr)`
- `realloc(void *ptr, size_t size)`
- `show_alloc_mem(void)`

The allocator interfaces directly with the Unix virtual memory subsystem via `mmap(2)` and `munmap(2)`. It avoids `brk(2)` / `sbrk(2)`, never relies on libc `malloc` or `printf` internally, and enforces strict **16-byte memory alignment** on 64-bit architectures (`max_align_t`).

---

## 🏗️ Architecture & Memory Sizing

To minimize the overhead of system calls (`mmap`), allocations are grouped into pre-allocated memory pools called **Zones**. Each `TINY` and `SMALL` zone is dimensioned to store **at least 100 allocations** and its total size is page-aligned (`getpagesize()`).

| Zone Category | Allocation Range | Sizing Formula | Apple Silicon Size (16 KB pages) | Linux Size (4 KB pages) |
| :--- | :--- | :--- | :--- | :--- |
| **`TINY`** | `1` to `1024` bytes | $\lceil (48 + 100 \times (1024 + 32)) / \text{page} \rceil \times \text{page}$ | **114,688 bytes** (7 pages) | **106,496 bytes** (26 pages) |
| **`SMALL`** | `1025` to `32768` bytes | $\lceil (48 + 100 \times (32768 + 32)) / \text{page} \rceil \times \text{page}$ | **3,293,184 bytes** (201 pages) | **3,280,896 bytes** (801 pages) |
| **`LARGE`** | `> 32768` bytes | Dedicated page-aligned `mmap` allocation per request | Dynamic | Dynamic |

### Metadata & 16-Byte Alignment

Every zone and block has an explicit header designed with manual padding to guarantee strict 16-byte alignment across all platforms:

- **`t_zone` (48 bytes — multiple of 16)**:
  - `next`, `prev` pointers (doubly-linked zone list).
  - `first` pointer to the first `t_block`.
  - `size` (total mmapped size) and `type` (`ZONE_TINY`, `ZONE_SMALL`, `ZONE_LARGE`).
  - `char padding[12]`.
- **`t_block` (32 bytes — multiple of 16)**:
  - `next`, `prev` pointers (doubly-linked intra-zone list).
  - `size` (payload size).
  - `is_free` (boolean).
  - `char padding[7]`.

---

## ⚡ Core Algorithms

1. **Allocation (`malloc`)**:
   - **First-Fit Search**: Scans the active zones of the matching category for the first free block of sufficient size.
   - **Splitting**: When an available block has enough excess space ($\ge \text{sizeof}(t\_block) + 16$), it is split into an allocated block and a smaller free block.
   - **Dynamic Pool Expansion**: Automatically allocates a new `t_zone` via `mmap` when existing zones are saturated.
2. **Deallocation (`free`)**:
   - **Pointer Validation**: Scans all active zones to verify that the pointer points to an actual payload before touching metadata.
   - **Immediate LARGE Release**: `LARGE` zones are unlinked and immediately returned to the kernel via `munmap(2)`.
   - **Safety**: Robust against double-free and invalid pointer corruption.
3. **Reallocation (`realloc`)**:
   - **In-Place Shrinking**: Re-splits the current block without any memory copying.
   - **In-Place Growth**: Automatically absorbs an adjacent free right neighbor if available without memory relocation.
   - **Category Relocation**: Migrates blocks across categories (e.g. `TINY` ➔ `LARGE`) via `malloc / ft_memcpy / free` when needed.
4. **Visualization (`show_alloc_mem`)**:
   - Displays all active allocations sorted by ascending memory addresses, concluding with the cumulative total bytes allocated.

---

## 🌟 Bonus Features (125 / 100)

### 1. Thread-Safety (`libpthread`)
- Protected by a recursive mutex (`PTHREAD_RECURSIVE_MUTEX_INITIALIZER`).
- Guarantees full thread-safety under heavy concurrent workloads.
- Avoids self-deadlocks when `realloc()` internally calls `malloc()` and `free()`.

### 2. Memory Defragmentation (Coalescing)
- Bidirectional merging on `free()`:
  - Merges with the right block if `block->next->is_free`.
  - Merges with the left block if `block->prev->is_free`.
- Completely eliminates external fragmentation within zones.

### 3. Extended Memory Dump (`show_alloc_mem_ex`)
- Prints memory zone metadata alongside a formatted **16-byte hexadecimal & ASCII dump** of all allocated blocks.

### 4. Debug Environment Variable (`MALLOC_DEBUG`)
- Enable diagnostic mode by exporting:
  ```bash
  export MALLOC_DEBUG=1
  ```
- Logs real-time operations: `malloc(size) = ptr`, `free(ptr)`, `realloc(ptr, size) = new_ptr`.
- Detects and reports memory violations:
  - `[MALLOC ERROR] free(0x...): invalid pointer or double free`

---

## 🛠️ Build & Installation

The shared library dynamically evaluates the host architecture:

```bash
make        # Compiles libft_malloc_$HOSTTYPE.so and symlink libft_malloc.so
make clean  # Removes object files
make fclean # Removes object files, library, and symlink
make re     # Complete rebuild
```

Artifacts produced:
- `libft_malloc_$HOSTTYPE.so` (e.g. `libft_malloc_arm64_Darwin.so` or `libft_malloc_x86_64_Linux.so`)
- `libft_malloc.so` (symbolic link)

---

## 🧪 Automated Test Suite

A comprehensive test suite and runner script is included in [`tests/`](tests/):

```bash
./tests/run_tests.sh
```

### Tests Covered:
- **`test0_basic.c`**: Basic allocations across `TINY`, `SMALL`, `LARGE` and `show_alloc_mem`.
- **`test1_zones.c`**: Zone saturation test verifying capacity $\ge 100$ allocations per zone.
- **`test2_align.c`**: Verifies strict 16-byte alignment (`(uintptr_t)ptr % 16 == 0`).
- **`test3_realloc.c`**: Tests in-place shrink/expansion, data integrity, and zone relocation.
- **`test4_coalesce.c`**: Proves bidirectional coalescing and demonstrates `show_alloc_mem_ex()`.
- **`test5_threads.c`**: Concurrent multi-threaded stress test (8 threads, 1200 operations).
- **`MALLOC_DEBUG` verification**: Automated double-free and invalid pointer detection test.

---

## 💻 Linking & Usage

### 1. Linking at Compilation
```bash
cc -Wall -Wextra -Werror main.c -L. -lft_malloc -Iincludes -o my_program
./my_program
```

### 2. Dynamic Library Injection (Hooking)
Run pre-existing programs with your custom allocator:

**macOS**:
```bash
DYLD_INSERT_LIBRARIES=./libft_malloc.so DYLD_FORCE_FLAT_NAMESPACE=1 /bin/ls
```

**Linux**:
```bash
LD_PRELOAD=./libft_malloc.so /bin/ls
```
