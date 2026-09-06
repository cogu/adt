# ADT Submodule TODO List

This document tracks identified API gaps, inconsistencies, and proposed enhancements across the `adt` library containers for future branches/releases.

---

## 1. Missing Virtual Destructors (`_vdelete(void *arg)`)

Containers supporting dynamic heap allocation (`_new` / `_delete`) should provide a type-erased `void (*)(void*)` destructor wrapper so instances can be stored as element payloads inside other containers (`adt_ary_t`, `adt_list_t`, `adt_hash_t`, `adt_stack_t`) and cleaned up automatically.

- [ ] **`adt_stack_vdelete(void *arg)`** in `include/adt_stack.h` & `src/adt_stack.c`:
  - `adt_stack_new` and `adt_stack_delete` exist, but no type-erased virtual destructor wrapper is provided.
- [ ] **`adt_u16Map_vdelete(void *arg)`** in `include/adt_map.h` & `src/adt_map.c`:
  - Under heap mode (`#if (!defined(ADT_NO_HEAP_MEM) || (ADT_NO_HEAP_MEM == 0))`), `adt_u16Map_new` and `adt_u16Map_delete` exist, but lacks `_vdelete`.
- [ ] **`adt_rbfh_vdelete(void *arg)`** in `include/adt_ringbuf.h` & `src/adt_ringbuf.c`:
  - Dynamically allocated heap ring buffer has `adt_rbfh_new` and `adt_rbfh_delete`, but lacks `_vdelete`.

---

## 2. Missing Emptiness Checks (`_is_empty`)

While `_is_empty(const T *self) -> bool` is present in `adt_ary`, `adt_list`, `adt_u32List`, `adt_u32Set`, `adt_hash`, and `adt_str`, several containers lack this standard check:

- [ ] **`adt_stack_is_empty(const adt_stack_t *self)`** in `include/adt_stack.h` & `src/adt_stack.c`
- [ ] **`adt_bytearray_is_empty(const adt_bytearray_t *self)`** in `include/adt_bytearray.h` & `src/adt_bytearray.c`
- [ ] **`adt_bytes_is_empty(const adt_bytes_t *self)`** in `include/adt_bytes.h` & `src/adt_bytes.c`
- [ ] **`adt_u16Map_is_empty(const adt_u16Map_t *self)`** in `include/adt_map.h` & `src/adt_map.c`
- [ ] **Ring buffer emptiness and fullness checks** in `include/adt_ringbuf.h` & `src/adt_ringbuf.c`:
  - `adt_rbfs_is_empty(const adt_rbfs_t *rbf)` & `adt_rbfs_is_full(const adt_rbfs_t *rbf)`
  - `adt_rbfu16_is_empty(const adt_rbfu16_t *rbf)` & `adt_rbfu16_is_full(const adt_rbfu16_t *rbf)`
  - `adt_rbfh_is_empty(const adt_rbfh_t *self)` & `adt_rbfh_is_full(const adt_rbfh_t *self)`

---

## 3. Sibling Container Asymmetries

### A. `adt_list` vs `adt_u32List` (`include/adt_list.h`)
- [ ] **`adt_u32List_first(const adt_u32List_t *self)`** & **`adt_u32List_last(const adt_u32List_t *self)`**:
  - `adt_list` provides `adt_list_first()` and `adt_list_last()`, but `adt_u32List` requires manual iteration.
- [ ] **`adt_u32List_remove(adt_u32List_t *self, uint32_t item)`**:
  - `adt_list` provides search-and-remove by value (`adt_list_remove`), whereas `adt_u32List` only provides `adt_u32List_erase` by node pointer.

### B. `adt_bytearray` vs `adt_bytes` (`include/adt_bytearray.h`, `include/adt_bytes.h`)
- [ ] **`adt_bytes_data_equals(const adt_bytes_t *self, const uint8_t *data, uint32_t dataLen)`**:
  - `adt_bytearray` supports raw buffer comparison via `adt_bytearray_data_equals`, but `adt_bytes` only supports comparison with another `adt_bytes_t` object.
- [ ] **`adt_bytearray_pop(adt_bytearray_t *self)`**:
  - `adt_bytearray` has `push()` for appending single bytes, but lacks a corresponding `pop()` (which is present in `adt_str`).

### C. Ring Buffer Parity (`include/adt_ringbuf.h`)
- [ ] **`adt_rbfu16_clear(adt_rbfu16_t *rbf)`**:
  - Present in `adt_rbfs` and `adt_rbfh`, but missing in `adt_rbfu16`.
- [ ] **`adt_rbfu16_free(const adt_rbfu16_t *rbf)`**:
  - Present in `adt_rbfs` and `adt_rbfh` (returns remaining free element slots), but missing in `adt_rbfu16`.

---

## 4. Naming Inconsistencies & Aliases

### A. `_size` vs `_length`
Most containers in `adt` use `_length()` to query element counts (`adt_ary_length`, `adt_list_length`, `adt_u32List_length`, `adt_u32Set_length`, `adt_bytearray_length`, `adt_bytes_length`, `adt_hash_length`, `adt_rbfu16_length`, `adt_rbfh_length`).

Three containers use `_size()` instead:
- [ ] Add `#define adt_stack_length adt_stack_size` in `include/adt_stack.h`
- [ ] Add `#define adt_u16Map_length adt_u16Map_size` in `include/adt_map.h`
- [ ] Add `#define adt_rbfs_length adt_rbfs_size` in `include/adt_ringbuf.h`

### B. Legacy CamelCase Functions
Add snake_case function definitions and retain camelCase aliases for backward compatibility:
- [ ] `include/adt_heap.h`:
  - Add `adt_heap_sort_up` / `adt_heap_sort_down`
  - Alias `#define adt_heap_sortUp adt_heap_sort_up`, `#define adt_heap_sortDown adt_heap_sort_down`
- [ ] `include/adt_str.h`:
  - Add `adt_str_char_at` (alias `adt_str_charAt`)
  - Add `adt_str_set_encoding` / `adt_str_get_encoding` (alias `adt_str_setEncoding` / `adt_str_getEncoding`)

---

## 5. Type Consistency & Destructor Control

- [ ] **`adt_u16Map_destructor_enable`** (`include/adt_map.h`):
  - Change signature from `(adt_u16Map_t *self, uint8_t enable)` to `(adt_u16Map_t *self, bool enable)` to match `adt_ary_destructor_enable` and `adt_list_destructor_enable` (include `<stdbool.h>`).
- [ ] **Destructor status inspection**:
  - `adt_ary` provides `adt_ary_has_destructor` and `adt_ary_destructor_is_enabled`. Consider adding matching query functions for `adt_list_t` and `adt_u16Map_t`.
