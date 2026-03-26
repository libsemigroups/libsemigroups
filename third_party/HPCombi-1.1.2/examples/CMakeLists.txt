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

include (CheckIncludeFileCXX)
check_include_file_cxx("sparsehash/dense_hash_map" HPCOMBI_HAVE_DENSEHASHMAP)
if (NOT HPCOMBI_HAVE_DENSEHASHMAP)
  message(STATUS "Google 'dense_hash_map' not found, using std::unordered_map")
else()
   add_compile_definitions(HPCOMBI_HAVE_DENSEHASHMAP)
endif()
check_include_file_cxx("sparsehash/dense_hash_set" HPCOMBI_HAVE_DENSEHASHSET)
if (NOT HPCOMBI_HAVE_DENSEHASHSET)
  message(STATUS "Google 'dense_hash_set' not found, using std::unordered_set")
else()
   add_compile_definitions(HPCOMBI_HAVE_DENSEHASHSET)
endif()

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/third_party
  ${PROJECT_BINARY_DIR})

set(usage_examples RD.cpp Renner.cpp stringmonoid.cpp Trans.cpp CF.cpp
    image.cpp pattern.cpp)

foreach(f ${usage_examples})
  get_filename_component(exampleName ${f} NAME_WE)
  add_executable (${exampleName} ${f})
  # install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${exampleName}
  #   DESTINATION bin
  #   RENAME ${CMAKE_PROJECT_NAME}-${exampleName})
endforeach(f)
