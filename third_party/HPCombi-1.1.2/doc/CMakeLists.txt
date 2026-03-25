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

find_package(Doxygen)
if(NOT DOXYGEN_FOUND)
    message("Doxygen not found, I will not generate/install the documentation")
else()
   configure_file(Doxyfile.in Doxyfile)

   set(DOXYGEN_INPUT ${CMAKE_BINARY_DIR}/doc/Doxyfile)
   set(DOXYGEN_OUTPUT ${APIDOC_DIR}/html/index.html)

   add_custom_target(doc ALL
     COMMAND ${CMAKE_COMMAND} -E echo_append "Building API Documentation..."
     COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT} > /dev/null
     COMMAND ${CMAKE_COMMAND} -E echo "Done."
     )

#  install(DIRECTORY ${CMAKE_BINARY_DIR}/doc/html
#          DESTINATION doc/${CMAKE_PROJECT_NAME})

endif()
