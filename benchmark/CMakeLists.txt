cmake_minimum_required(VERSION 3.5.0)
project(Libsemigroups_Benchmarks LANGUAGES CXX)
include_directories(/usr/local/include/)

# Location of additional cmake modules
#
set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${PROJECT_SOURCE_DIR}/cmake
    )

# Guard against in-source builds and bad build-type strings
#
include(ConfigSafeGuards)

#~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# If you want to self-test benchmark lib too, turn me ON
#
set(BENCHMARK_ENABLE_TESTING OFF)

# Configure google micro benchmark
#

# c++11 is required
#
if((CMAKE_CXX_COMPILER_ID MATCHES GNU) OR (CMAKE_CXX_COMPILER_ID MATCHES Clang))
   set(CMAKE_CXX_FLAGS         "${CMAKE_CXX_FLAGS} -std=c++11")
   set(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g3")
   set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")
endif()

include(ConfigGBench)

add_subdirectory(${PROJECT_SOURCE_DIR}/bench/)
