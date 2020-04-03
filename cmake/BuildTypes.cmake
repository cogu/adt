# Set a default build type if none was specified
set(default_build_type "Debug")
 
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}'")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "UnitTest")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "UnitTest")
  if(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /Zi")
  else()
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
  endif()
endif()