#****************************************************************************#
#  Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,          #
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

# Require at least 3.8 so that we can set cmake_policy CMP0067 below
cmake_minimum_required(VERSION 3.10)

#####################
# Project description
project(HPCombi)

set(DESCRIPTION  "High Performance Combinatorics in C++ using vector instructions"
  CACHE STRING "Project description.")
set(VERSION_MAJOR   1   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   2   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)

message(STATUS "**** Build type = ${CMAKE_BUILD_TYPE}")
# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_VERBOSE_MAKEFILE 1)

################################
# General compiler configuration
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF) # -std=c++14 instead of -std=gnu++14

message(STATUS "*** Compiler id is ${CMAKE_CXX_COMPILER_ID}")
# See https://stackoverflow.com/questions/47213356/ for discussion
cmake_policy(SET CMP0067 NEW)

##################################
# Checks for compiler capabilities
include(CheckCXXSymbolExists)
include(CheckIncludeFileCXX)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)

check_cxx_compiler_flag('-funroll-loops' HPCOMBI_HAVE_REQUIRED_COMPILER_FLAGS)
check_cxx_compiler_flag('-flax-vector-conversions' HPCOMBI_HAVE_REQUIRED_COMPILER_FLAGS)

if (NOT HPCOMBI_HAVE_REQUIRED_COMPILER_FLAGS)
  message(FATAL_ERROR "HPCombi requires a C++ compiler accepting the flags: -funroll-loops + -flax-vector-conversions")
endif()

add_compile_options(-funroll-loops -flax-vector-conversions)

if (NOT HPCOMBI_DISABLE_MARCH_NATIVE)
  check_cxx_compiler_flag('-march=native' HPCOMBI_HAVE_MARCH_NATIVE)

  if (HPCOMBI_HAVE_MARCH_NATIVE)
    add_compile_options(-march=native)
  else()
    message(STATUS "The C++ compiler does not accept the flag -march-native")
  endif()
endif()

check_cxx_compiler_flag('-fopenmp-simd', HPCOMBI_HAVE_OPENMP_SIMD_FLAG)

if (HPCOMBI_HAVE_OPENMP_SIMD_FLAG) 
  add_compile_options(-fopenmp-simd -DSIMDE_ENABLE_OPENMP)
endif()

# Currently the code coverage fails to link on Mac OSX, not sure why
if(CODE_COVERAGE)
  include(CheckCCompilerFlag)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR})
  include(CodeCoverage)
endif(CODE_COVERAGE)

file(READ ${CMAKE_SOURCE_DIR}/list_builtin.txt hpcombi_compiler_builtins)
string(REPLACE ";" "|" hpcombi_compiler_builtins "${hpcombi_compiler_builtins}")
string(REPLACE "\n" ";" hpcombi_compiler_builtins "${hpcombi_compiler_builtins}")
foreach (builtin ${hpcombi_compiler_builtins})
  if ("${builtin}" MATCHES "^#" ) # Comment
    continue()
  endif()
  string(REPLACE "|" ";" builtin "${builtin}")
  list(GET builtin 0 builtin_name)
  list(GET builtin 1 builtin_params)
  list(GET builtin 2 builtin_return_value)
  check_cxx_source_runs("
  #include <iostream>
    int main() {
      std::cout << \"<parameter;return value> from list_builtin.txt: \" << ${builtin_params} << \";\" << ${builtin_return_value} << std::endl;
      std::cout << \"Actual return value: \" << ${builtin_name}(${builtin_params}) << std::endl;
      return ${builtin_name}(${builtin_params}) != ${builtin_return_value};
    }
  "
  "HPCOMBI_HAVE_${builtin_name}"
  )
  if (NOT "${HPCOMBI_HAVE_${builtin_name}}")
    message(FATAL_ERROR "HPCombi requires a C++ compiler supporting ${builtin_name}")
  endif()
endforeach()

###################
# Project Structure
add_subdirectory(examples)
add_subdirectory(doc)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/third_party
  ${PROJECT_BINARY_DIR})

# add_compile_options(-Wall -Wextra -Wpedantic)

#########
# Testing

if (BUILD_TESTING)
  include(CTest)
  enable_testing ()
  add_subdirectory(tests)
  add_subdirectory(benchmark)
endif(BUILD_TESTING)

###################
# pkgconfig stuff
configure_file(${CMAKE_SOURCE_DIR}/HPCOMBI_VERSION.in
               ${CMAKE_CURRENT_BINARY_DIR}/HPCOMBI_VERSION)
configure_file(${CMAKE_SOURCE_DIR}/hpcombi.pc.in
               ${CMAKE_CURRENT_BINARY_DIR}/hpcombi.pc @ONLY)

# The next lines are included so that the files HPCOMBI_VERSION and hpcombi.pc
# are included in the archive generated by "make package_source"
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/HPCOMBI_VERSION.in
               ${CMAKE_CURRENT_SOURCE_DIR}/HPCOMBI_VERSION)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/hpcombi.pc.in
               ${CMAKE_CURRENT_SOURCE_DIR}/hpcombi.pc @ONLY)

####################
# Installation

string(TOLOWER ${CMAKE_PROJECT_NAME} HPCOMBI_INSTALL_DIR)
set(install_misc README.md LICENSE)
foreach(f ${install_misc})
  install (FILES ${CMAKE_SOURCE_DIR}/${f}
  DESTINATION share/${HPCOMBI_INSTALL_DIR})
endforeach(f)

install (
  FILES ${CMAKE_CURRENT_BINARY_DIR}/HPCOMBI_VERSION
  DESTINATION share/${HPCOMBI_INSTALL_DIR})
install (
  DIRECTORY ${CMAKE_SOURCE_DIR}/include/hpcombi
  DESTINATION include
  FILES_MATCHING PATTERN "*.hpp")
install (
  DIRECTORY ${CMAKE_SOURCE_DIR}/third_party/simde-0.8.2
  DESTINATION include/${HPCOMBI_INSTALL_DIR})
install (
  FILES ${CMAKE_CURRENT_BINARY_DIR}/hpcombi.pc
  DESTINATION lib/pkgconfig)

#################
# Packing stuff
#
set(CPACK_SOURCE_PACKAGE_DESCRIPTION_SUMMARY "${DESCRIPTION}")
set(CPACK_SOURCE_PACKAGE_VENDOR "Florent Hivert <florent.hivert@lri.fr>")
set(CPACK_SOURCE_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
set(CPACK_SOURCE_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
set(CPACK_SOURCE_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
set(CPACK_SOURCE_PACKAGE_VERSION_PATCH "${VERSION_PATCH}")

SET(CPACK_SOURCE_PACKAGE_FILE_NAME
  "HPCombi-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
  ".circleci"
  "/\\.git/"
  "/build/"
  "/doc.html"
  "/experiments/"
  "/gh-pages/"
)
include(CPack)

########################
# Custom target for TAGS
if (UNIX)
  add_custom_target(tags etags --members --declarations  `find ${CMAKE_SOURCE_DIR}/ -name *.cpp -or -name *.hpp -or -name *.c -or -name *.h` -o ${CMAKE_SOURCE_DIR}/TAGS)
  add_custom_target(etags DEPENDS tags)
endif (UNIX)
