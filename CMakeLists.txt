cmake_minimum_required(VERSION 3.14)

project(adt LANGUAGES C VERSION 0.2.0)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    include(cmake/BuildTypes.cmake)
    include(cmake/LeakCheck.cmake)
    include(cmake/UnitTest.cmake)
endif()
include(CMakeDependentOption)

option(ADT_RBFU16_ENABLE "ADT U16 Ringbuffer" OFF)
option(ADT_RBFS_ENABLE "ADT Statically allocated Ringbuffer" OFF)
option(ADT_RBFH_ENABLE "ADT Heap-managed Ringbuffer" OFF)
CMAKE_DEPENDENT_OPTION(TEST_ADT_HASH_FULL "Activate entire adt_hash test suite" OFF "UNIT_TEST" OFF)

if (LEAK_CHECK)
    message(STATUS "LEAK_CHECK=${LEAK_CHECK} (ADT)")
endif()

if (UNIT_TEST)
    message(STATUS "UNIT_TEST=${UNIT_TEST} (ADT)")
endif()

### Library adt
file(GLOB ADT_HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
set (ADT_HEADER_LIST
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_ary.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_bytearray.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_bytes.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_error.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_hash.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_heap.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_list.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_ringbuf.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_set.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_stack.h
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/adt_str.h
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

if(MSVC)
    target_compile_definitions(adt PUBLIC _CRT_SECURE_NO_WARNINGS)
endif()

###

###Executable adt_unit
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    if (UNIT_TEST)
        set(TEST_ADT_HASH_FULL_VAL 0)
        if (TEST_ADT_HASH_FULL)
            set(TEST_ADT_HASH_FULL_VAL 1)
            message(STATUS "TEST_ADT_HASH_FULL_VAL=${TEST_ADT_HASH_FULL_VAL}")
        endif()

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
        target_compile_definitions(adt_unit PUBLIC UNIT_TEST TEST_ADT_HASH_FULL=${TEST_ADT_HASH_FULL_VAL})
        if (LEAK_CHECK)
            target_compile_definitions(adt_unit PRIVATE MEM_LEAK_CHECK)
        endif()

        enable_testing()
        add_test(adt_test ${CMAKE_CURRENT_BINARY_DIR}/adt_unit)
        set_tests_properties(adt_test PROPERTIES PASS_REGULAR_EXPRESSION "OK \\([0-9]+ tests\\)")

    endif()
endif()
###

