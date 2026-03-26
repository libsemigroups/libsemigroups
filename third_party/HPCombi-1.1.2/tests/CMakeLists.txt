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

message(STATUS "Building tests")

set(test_src
  test_epu8.cpp test_perm16.cpp test_perm_all.cpp test_bmat8.cpp test_bmat16.cpp)

foreach(f ${test_src})
  get_filename_component(testName ${f} NAME_WE)
  add_executable (${testName} ${f} test_main.cpp)
  target_link_libraries(${testName} PRIVATE Catch2::Catch2WithMain)
endforeach(f)

add_executable(test_all ${test_src} test_main.cpp)

target_link_libraries(test_all PRIVATE Catch2::Catch2WithMain)

if(CODE_COVERAGE)
  # FIXME the next line fails on JDM's M1 Mac with gcov not found (even though
  # it's installed)
  find_library(GCOV gcov REQUIRED)
  # FIXME the next line fails on JDM's M1 Mac with "ld: library not found for -lgcov"
  target_link_libraries(test_all PRIVATE gcov) 
  append_coverage_compiler_flags_to_target(test_all)
  setup_target_for_coverage_lcov(
              NAME coverage-test_all
              EXECUTABLE test_all)
endif(CODE_COVERAGE)

add_test (TestEPU8 test_epu8)
add_test (TestPerm16 test_perm16)
add_test (TestPermAll test_perm_all)
add_test (TestBMat8 test_bmat8)
add_test (TestBMat16 test_bmat16)
