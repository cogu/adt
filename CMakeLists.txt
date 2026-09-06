cmake_minimum_required(VERSION 3.20)

project(adt LANGUAGES C VERSION 0.3.3)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_OPTIMIZE_DEPENDENCIES ON)

if(MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
    add_compile_options(/W4)
    option(ENABLE_MSVC_ANALYZE "Enable MSVC static code analysis (/analyze)" OFF)
    if(ENABLE_MSVC_ANALYZE)
        add_compile_options(/analyze)
    endif()
else()
    add_compile_options(-Wall -Wextra)
endif()

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
        set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Choose the type of build." FORCE)
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
    endif()

    option(UNIT_TEST "Unit Test Build" OFF)
    option(LEAK_CHECK "Memory Leak Detection" OFF)
endif()
include(CMakeDependentOption)

option(ADT_RBFU16_ENABLE "ADT U16 Ringbuffer" OFF)
option(ADT_RBFS_ENABLE "ADT Statically allocated Ringbuffer" OFF)
option(ADT_RBFH_ENABLE "ADT Heap-managed Ringbuffer" OFF)
option(ADT_U16MAP_ENABLE "ADT U16 Map" OFF)
set(ADT_SANITIZERS "" CACHE STRING "Comma-separated sanitizers to enable (for example: address,undefined or thread)")
set_property(CACHE ADT_SANITIZERS PROPERTY STRINGS "" "address" "address,undefined" "thread")

if(ADT_SANITIZERS)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang|AppleClang")
        add_compile_options(-fsanitize=${ADT_SANITIZERS} -fno-omit-frame-pointer)
        add_link_options(-fsanitize=${ADT_SANITIZERS})
    else()
        message(FATAL_ERROR "ADT_SANITIZERS is only supported with GCC or Clang")
    endif()
endif()

if (LEAK_CHECK)
    message(STATUS "LEAK_CHECK=${LEAK_CHECK} (ADT)")
endif()

if (UNIT_TEST)
    message(STATUS "UNIT_TEST=${UNIT_TEST} (ADT)")
endif()

### Library adt
set (ADT_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_ary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_bytearray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_bytes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_error.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_heap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_ringbuf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_set.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_stack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_str.h
)

set (ADT_SOURCE_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_ary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_bytearray.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_bytes.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_hash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_heap.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_list.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_ringbuf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_set.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_str.c
)

add_library(adt ${ADT_SOURCE_LIST} ${ADT_HEADER_LIST})
target_include_directories(adt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (UNIT_TEST)
    target_compile_definitions(adt PRIVATE UNIT_TEST)
endif()

if (LEAK_CHECK)
    target_include_directories(adt PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/test")
    target_compile_definitions(adt PRIVATE MEM_LEAK_CHECK)
endif()

if(ADT_RBFU16_ENABLE)
    message(STATUS "ADT_RBFU16_ENABLE=1")
    target_compile_definitions(adt PUBLIC ADT_RBFU16_ENABLE=1)
endif()

if(ADT_RBFS_ENABLE)
    message(STATUS "ADT_RBFS_ENABLE=1")
    target_compile_definitions(adt PUBLIC ADT_RBFS_ENABLE=1)
endif()

if(ADT_RBFH_ENABLE)
    message(STATUS "ADT_RBFH_ENABLE=1")
    target_compile_definitions(adt PUBLIC ADT_RBFH_ENABLE=1)
endif()

if(ADT_U16MAP_ENABLE)
    message(STATUS "ADT_U16MAP_ENABLE=1")
    target_sources(adt PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include/adt_u16Map.h
        ${CMAKE_CURRENT_SOURCE_DIR}/src/adt_u16Map.c
    )
    target_compile_definitions(adt PUBLIC ADT_U16MAP_ENABLE=1)
endif()

###

###Executable adt_unit
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    if (UNIT_TEST)
        set (ADT_TEST_SUITE_LIST
                test/adt/testsuite_adt_ary.c
                test/adt/testsuite_adt_bytearray.c
                test/adt/testsuite_adt_bytes.c
                test/adt/testsuite_adt_hash.c
                test/adt/testsuite_adt_heap.c
                test/adt/testsuite_adt_list.c
                test/adt/testsuite_adt_ringbuf.c
                test/adt/testsuite_adt_stack.c
                test/adt/testsuite_adt_str.c
                test/adt/testsuite_adt_u32List.c
                test/adt/testsuite_adt_u32Set.c
        )

        if(ADT_U16MAP_ENABLE)
            list(APPEND ADT_TEST_SUITE_LIST test/adt/testsuite_adt_u16map.c)
        endif()

        set(ADT_LIB_EXTRA)
        if (LEAK_CHECK)
            list (APPEND ADT_LIB_EXTRA test/CMemLeak.h)
            list (APPEND ADT_LIB_EXTRA test/CMemLeak.c)
        endif()


        add_executable(adt_unit test/TestMain.c test/CuTest.c ${ADT_LIB_EXTRA} ${ADT_TEST_SUITE_LIST})
        target_link_libraries(adt_unit PRIVATE adt)
        target_include_directories(adt_unit PRIVATE
                                "${PROJECT_BINARY_DIR}"
                                "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                "${CMAKE_CURRENT_SOURCE_DIR}/test"
                                )
        target_compile_definitions(adt_unit PRIVATE UNIT_TEST)
        if(ADT_U16MAP_ENABLE)
            target_compile_definitions(adt_unit PRIVATE TEST_ADT_U16MAP_ENABLE=1)
        endif()
        if (LEAK_CHECK)
            target_compile_definitions(adt_unit PRIVATE MEM_LEAK_CHECK)
        endif()

        enable_testing()
        add_test(NAME adt_unit COMMAND adt_unit)
        set_tests_properties(adt_unit PROPERTIES
            PASS_REGULAR_EXPRESSION "OK \\([0-9]+ tests\\)"
            LABELS "unit"
        )

        ### Benchmark / Performance Test
        add_executable(adt_perf test/perf_adt_hash.c test/perf_adt_ary.c ${ADT_LIB_EXTRA})
        target_link_libraries(adt_perf PRIVATE adt)
        target_include_directories(adt_perf PRIVATE
                                "${CMAKE_CURRENT_SOURCE_DIR}/include"
                                )
        target_compile_definitions(adt_perf PRIVATE
            DICTIONARY_PATH="${CMAKE_CURRENT_SOURCE_DIR}/test/enable1.txt"
        )
        add_test(NAME adt_perf COMMAND adt_perf)
        set_tests_properties(adt_perf PROPERTIES
            LABELS "benchmark"
        )

    endif()
endif()
###


