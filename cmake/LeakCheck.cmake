option(LEAK_CHECK "Memory Leak Detection" OFF)
if (NOT LEAK_CHECK AND CMAKE_BUILD_TYPE STREQUAL "UnitTest")    
    set(LEAK_CHECK ON CACHE BOOL "Always on for UnitTest" FORCE)
endif()