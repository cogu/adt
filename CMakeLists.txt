cmake_minimum_required(VERSION 3.14)

file(GLOB ADT_HEADER_LIST CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h")
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
            src/adt_u16Map.c
)

add_library(adt ${ADT_SOURCE_LIST} ${ADT_HEADER_LIST})

target_include_directories(adt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_compile_definitions(adt PRIVATE
    UNIT_TEST
    MEM_LEAK_CHECK
    ADT_RBFU16_ENABLE=1
    ADT_RBFS_ENABLE=1
)

target_compile_definitions(adt PRIVATE
    ADT_RBFH_ENABLE=1
)

target_include_directories(adt PRIVATE
                          "${PROJECT_BINARY_DIR}"
                          "${CMAKE_CURRENT_SOURCE_DIR}/test"
                          )


project(adt_unit LANGUAGES C VERSION 1.0)

#configure_file(adt_cfg.h.in adt_cfg.h)

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
            test/adt/testsuite_adt_u16map.c
            test/adt/testsuite_adt_u32List.c
            test/adt/testsuite_adt_u32Set.c
)


add_executable(adt_unit test/TestMain.c test/CuTest.c test/CMemLeak.c ${ADT_TEST_SUITE_LIST})


target_link_libraries(adt_unit PUBLIC adt)

target_include_directories(adt_unit PUBLIC
                          "${PROJECT_BINARY_DIR}"
                          "${CMAKE_CURRENT_SOURCE_DIR}/inc"
                          "${CMAKE_CURRENT_SOURCE_DIR}/test"
                          )
target_compile_definitions(adt_unit PUBLIC
    UNIT_TEST
    MEM_LEAK_CHECK
    TEST_ADT_HASH_FULL=0
)

