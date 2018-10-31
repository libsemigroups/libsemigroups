#****************************************************************************#
#       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           #
#                                                                            #
#  Distributed under the terms of the GNU General Public License (GPL)       #
#                                                                            #
#    This code is distributed in the hope that it will be useful,            #
#    but WITHOUT ANY WARRANTY; without even the implied warranty of          #
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       #
#   General Public License for more details.                                 #
#                                                                            #
#  The full text of the GPL is available at:                                 #
#                                                                            #
#                  http://www.gnu.org/licenses/                              #
#****************************************************************************#

cmake_minimum_required(VERSION 3.7)

find_package(benchmark)

include(CheckCXXSymbolExists)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)

include_directories(
  ${CMAKE_SOURCE_DIR}/../include
  ${CMAKE_SOURCE_DIR}/../src
  ${PROJECT_BINARY_DIR})

set(CMAKE_BUILD_TYPE Release)
set(CMAKE_CXX_STANDARD 11)
set(benchmark_src bench_digraph.cpp)

if ( CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  # Workaround of CMAKE bug https://stackoverflow.com/questions/47213356/
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} -std=c++11)
  add_compile_options( -std=c++11 -Wall)
endif ( )

foreach(f ${benchmark_src})
  get_filename_component(benchName ${f} NAME_WE)
  add_executable (${benchName} ${f})
  target_compile_options(${benchName} PUBLIC -mavx -mtune=native -funroll-loops
    -flax-vector-conversions -O3 -g)
  target_link_libraries(${benchName} benchmark pthread)
endforeach(f)
