cmake_minimum_required(VERSION 3.14)

project(adt LANGUAGES C VERSION 0.1.3)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(cmake/BuildTypes.cmake)
    include(cmake/LeakCheck.cmake)
    include(cmake/UnitTest.cmake)
endif()

option(ADT_RBFU16_ENABLE "ADT U16 Ringbuffer" OFF)
option(ADT_RBFS_ENABLE "ADT Statically allocated Ringbuffer" OFF)
option(ADT_RBFH_ENABLE "ADT Heap-managed Ringbuffer" OFF)

if (LEAK_CHECK)
    message(STATUS "LEAK_CHECK=${LEAK_CHECK} (ADT)")
endif()

if (UNIT_TEST)
    message(STATUS "UNIT_TEST=${UNIT_TEST} (ADT)")
endif()

### Library adt
file(GLOB ADT_HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set (ADT_HEADER_LIST
            inc/adt_ary.h
            inc/adt_bytearray.h
            inc/adt_bytes.h
            inc/adt_error.h
            inc/adt_hash.h
            inc/adt_heap.h
            inc/adt_list.h
            inc/adt_ringbuf.h
            inc/adt_set.h
            inc/adt_stack.h
            inc/adt_str.h
)

set (ADT_SOURCE_LIST
            src/adt_ary.c
            src/adt_bytearray.c
            src/adt_bytes.c
            src/adt_hash.c
            src/adt_heap.c
            src/adt_list.c
            src/adt_ringbuf.c
            src/adt_set.c
            src/adt_stack.c
            src/adt_str.c
)

add_library(adt ${ADT_SOURCE_LIST} ${ADT_HEADER_LIST})
target_include_directories(adt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

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
###

###Executable adt_unit
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
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

    set(ADT_UNIT_EXTRA)
    if (LEAK_CHECK)
        list (APPEND ADT_UNIT_EXTRA test/CMemLeak.h)
        list (APPEND ADT_UNIT_EXTRA test/CMemLeak.c)
    endif()


    add_executable(adt_unit test/TestMain.c test/CuTest.c ${ADT_UNIT_EXTRA} ${ADT_TEST_SUITE_LIST})
    target_link_libraries(adt_unit PUBLIC adt)
    target_include_directories(adt_unit PUBLIC
                              "${PROJECT_BINARY_DIR}"
                              "${CMAKE_CURRENT_SOURCE_DIR}/inc"
                              "${CMAKE_CURRENT_SOURCE_DIR}/test"
                              )
    target_compile_definitions(adt_unit PUBLIC UNIT_TEST TEST_ADT_HASH_FULL=0)
    if (LEAK_CHECK)
        target_compile_definitions(adt_unit PRIVATE MEM_LEAK_CHECK)
    endif()
endif()
###
