option(UNIT_TEST "Unit Test Build" OFF)
if (NOT UNIT_TEST AND CMAKE_BUILD_TYPE STREQUAL "UnitTest")    
    set(UNIT_TEST ON CACHE BOOL "Always on for UnitTest" FORCE)
endif()