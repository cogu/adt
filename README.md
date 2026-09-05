![unit tests](https://github.com/cogu/adt/workflows/unit%20tests/badge.svg)
[![Documentation Status](https://readthedocs.org/projects/c-adt/badge/?version=latest)](https://c-adt.readthedocs.io/en/latest/?badge=latest)

# ADT

Abstract data types for the C programming language.

Online documentation and API reference: **[c-adt.readthedocs.io](https://c-adt.readthedocs.io/)**

## What is it?

**ADT** provides platform-independent and compiler-independent abstract data types for the C programming language (C99 and later). It offers a rich suite of containers designed for both standard application development and resource-constrained embedded systems:

* **Generic Object Containers (`void*`)**: Dynamic pointer array (`adt_ary_t`), hash table with string keys (`adt_hash_t`), doubly-linked list (`adt_list_t`), LIFO stack (`adt_stack_t`), and binary heap priority queue (`adt_heap_t`).
* **Byte & String Buffers**: Mutable resizable byte array (`adt_bytearray_t`), immutable byte buffer (`adt_bytes_t`), and UTF-8/ASCII string container (`adt_str_t`).
* **Embedded & Zero-Heap Containers**: Static element ring buffer (`adt_rbfs_t`), embedded 16-bit ring buffer (`adt_rbfu16_t`), and sorted-array key-value map (`adt_u16Map_t`).
* **Specialized Value Containers**: 32-bit integer linked list (`adt_u32List_t`), unique 32-bit integer set (`adt_u32Set_t`), and dynamically resizing heap ring buffer (`adt_rbfh_t`).

### Available Data Structures

| Name | Header | Storage Type | Requires Heap | Description |
|------|--------|--------------|---------------|-------------|
| `adt_ary_t` | `adt_ary.h` | Objects (`void*`) | Yes | Contiguous pointer array with O(1) random access |
| `adt_bytearray_t` | `adt_bytearray.h` | Bytes (`uint8_t`) | Yes | Mutable byte array with chunked growth |
| `adt_bytes_t` | `adt_bytes.h` | Bytes (`uint8_t`) | Yes | Immutable byte array |
| `adt_str_t` | `adt_str.h` | Characters (`char*`) | Yes | Dynamic UTF-8 / ASCII string container |
| `adt_hash_t` | `adt_hash.h` | Objects (`void*`) | Yes | Hash table with string keys |
| `adt_u16Map_t` | `adt_u16Map.h` | Objects (`void*`) | No | Sorted-array map for `uint16_t` keys |
| `adt_list_t` | `adt_list.h` | Objects (`void*`) | Yes | Doubly-linked list for mid-sequence edits |
| `adt_u32List_t` | `adt_list.h` | Values (`uint32_t`) | Yes | Specialized linked list for 32-bit integers |
| `adt_stack_t` | `adt_stack.h` | Objects (`void*`) | Yes | LIFO stack for generic pointers |
| `adt_heap_t` | `adt_heap.h` | Objects (`void*`) | Yes | Binary heap priority queue |
| `adt_rbfs_t` | `adt_ringbuf.h` | Elements (`uint8_t*`) | No | Static circular FIFO buffer (zero heap) |
| `adt_rbfu16_t` | `adt_ringbuf.h` | Values (`uint16_t`) | No | Embedded circular buffer for `uint16_t` (zero heap) |
| `adt_rbfh_t` | `adt_ringbuf.h` | Elements (`uint8_t*`) | Yes | Heap-allocated circular FIFO buffer |
| `adt_u32Set_t` | `adt_set.h` | Values (`uint32_t`) | Yes | Unique set of 32-bit integers |

> **Embedded Development Note**: Data structures with **Requires Heap: No** (such as `adt_rbfs_t`, `adt_rbfu16_t`, and `adt_u16Map_t`) or those initialized via in-place `*_create` / `*_destroy` functions operate completely without dynamic heap allocation (`malloc` / `free`), making them safe for microcontrollers, safety-critical code, and real-time systems.

## Where is it used?

* [cogu/amber](https://github.com/cogu/amber)
* [cogu/bstr](https://github.com/cogu/bstr)
* [cogu/c-apx](https://github.com/cogu/c-apx)
* [cogu/cutil](https://github.com/cogu/cutil)
* [cogu/dtl_json](https://github.com/cogu/dtl_json)
* [cogu/dtl_type](https://github.com/cogu/dtl_type)

## Dependencies

None, except for a C compiler.

## Related projects

If you are looking for higher level data types in C you can check out the [cogu/dtl_type](https://github.com/cogu/dtl_type) project. It offers reference-counted variables with an easy to use API. It internally uses ADT for data storage.

## Building with CMake

### Using CMake Presets (Clang 18 + Ninja)

```bash
# Run unit tests
cmake --preset clang-test
cmake --build --preset clang-test
ctest --preset clang-test

# Address and Undefined Behavior Sanitizers (ASan + UBSan)
cmake --preset clang-asan
cmake --build --preset clang-asan
ctest --preset clang-asan

# Static Analysis
cmake --preset clang-tidy
cmake --build --preset clang-tidy
```

### Manual CMake Workflows (Linux and Windows)

For Windows, use a "Native tools command prompt" from your Visual Studio installation. It comes with a cmake binary that
by default chooses the appropriate compiler version.

#### Running unit tests

Configure:

```sh
cmake -S . -B build-test -GNinja -DUNIT_TEST=ON
```

Build:

```sh
cmake --build build-test
```

Run test cases:

```sh
ctest --test-dir build-test --output-on-failure
```

#### Measuring Code Coverage (gcov)

Configure with `--coverage` compiler and linker flags:

```sh
cmake -S . -B build-cov -DUNIT_TEST=ON -DADT_U16MAP_ENABLE=ON -DADT_RBFH_ENABLE=ON \
  -DCMAKE_C_FLAGS="--coverage" \
  -DCMAKE_EXE_LINKER_FLAGS="--coverage"
```

Build and run tests:

```sh
cmake --build build-cov
ctest --test-dir build-cov --output-on-failure
```

Analyze coverage with `gcov`:

```sh
# For a single file:
gcov -o build-cov/CMakeFiles/adt.dir/src/adt_stack.c.o src/adt_stack.c

# For all source files:
for f in src/*.c; do gcov -o build-cov/CMakeFiles/adt.dir/src/$(basename $f).o $f; done
```

`gcov` prints the summary percentage to stdout and produces annotated `.gcov` files (e.g. `adt_stack.c.gcov`) in the current working directory. Untested lines are highlighted with `#####:`.

#### Building a release version of the ADT library

Configure:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
```

Build:

```sh
cmake --build build --target adt
```

### ADT CMake Options

CMake options can be set from command line or using a CMake GUI tool (such as ccmake for Linux).

#### Generic Options

| CMake Option      | Usage                                  | Description                                      |
|-------------------|----------------------------------------|--------------------------------------------------|
| LEAK_CHECK        | -DLEAK_CHECK=ON                        | Enables memory leak check detection              |
| UNIT_TEST         | -DUNIT_TEST=ON                         | Activates UNIT_TEST preprocessor define          |
| ADT_SANITIZERS    | -DADT_SANITIZERS="address,undefined"   | Enables sanitizers for GCC or Clang              |

#### ADT Hash Performance Benchmark

When `-DUNIT_TEST=ON` is set, a dedicated benchmark executable `adt_perf` is built. It benchmarks insertion and lookup performance across ~22,000 dictionary words using [test/3esl.txt](file:///home/cogu/repo/c-apx/adt/test/3esl.txt).

Run directly:

```sh
./build-test/adt_perf
```

Or run via CTest:

```sh
ctest --test-dir build-test -L benchmark --output-on-failure --verbose
```

#### ADT Ringbuffer

By default, `adt_ringbuf.c` will not compile anything unless you explicitly enable it using CMake options. This allows embedded targets to enable `adt_rbfs_t` (static buffer) and `adt_rbfu16_t` (embedded `uint16_t` buffer) without enabling or linking heap memory.

| CMake Option      | Usage                  | Storage / Memory   | Description                      |
|-------------------|------------------------|--------------------|----------------------------------|
| ADT_RBFS_ENABLE   | -DADT_RBFS_ENABLE=ON   | Static (zero heap) | Enables adt_rbfs_t and its API   |
| ADT_RBFU16_ENABLE | -DADT_RBFU16_ENABLE=ON | Static (zero heap) | Enables adt_rbfu16_t and its API |
| ADT_RBFH_ENABLE   | -DADT_RBFH_ENABLE=ON   | Heap (dynamic)     | Enables adt_rbfh_t and its API   |

#### ADT U16Map

`adt_u16Map_t` is optional and disabled by default. Enable it via CMake:

| CMake Option      | Usage                  | Description                      |
|-------------------|------------------------|----------------------------------|
| ADT_U16MAP_ENABLE | -DADT_U16MAP_ENABLE=ON | Enables adt_u16Map_t and its API |


#### Unit testing all data structures


Command to build unit test with all optional data structures enabled:

```sh
cmake -S . -B build-test -GNinja -DUNIT_TEST=ON -DADT_RBFS_ENABLE=ON -DADT_RBFU16_ENABLE=ON -DADT_RBFH_ENABLE=ON -DADT_U16MAP_ENABLE=ON
cmake --build build-test
ctest --test-dir build-test --output-on-failure
```