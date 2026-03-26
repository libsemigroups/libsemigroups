#****************************************************************************#
#      Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,      #
#                                                                            #
#  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  #
#                                                                            #
#  HP-Combi is free software: you can redistribute it and/or modify it       #
#  under the terms of the GNU General Public License as published by the     #
#  Free Software Foundation, either version 3 of the License, or             #
#  (at your option) any later version.                                       #
#                                                                            #
#  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   #
#  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     #
#  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      #
#  for  more details.                                                        #
#                                                                            #
#  You should have received a copy of the GNU General Public License along   #
#  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               #
#****************************************************************************#

find_package(Catch2 3 QUIET)

if(NOT Catch2_FOUND)
  message(STATUS "Cloning Catch2 from GIT_REPOSITORY https://github.com/catchorg/Catch2.git")
  Include(FetchContent)

  FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.4.0 # or a later release
  )

  FetchContent_MakeAvailable(Catch2)
else()
  message(STATUS "Using system Catch2")
endif()

message(STATUS "Building benchmark")

set(benchmark_src
  bench_epu8.cpp bench_perm16.cpp bench_bmat8.cpp bench_bmat16.cpp)

foreach(f ${benchmark_src})
  get_filename_component(benchName ${f} NAME_WE)
  add_executable (${benchName} ${f})
  target_link_libraries(${benchName} PRIVATE Catch2::Catch2WithMain)
endforeach(f)
