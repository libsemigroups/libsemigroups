//****************************************************************************//
//     Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
//                                                                            //
//  This file is part of HP-Combi <https://github.com/libsemigroups/HPCombi>  //
//                                                                            //
//  HP-Combi is free software: you can redistribute it and/or modify it       //
//  under the terms of the GNU General Public License as published by the     //
//  Free Software Foundation, either version 3 of the License, or             //
//  (at your option) any later version.                                       //
//                                                                            //
//  HP-Combi is distributed in the hope that it will be useful, but WITHOUT   //
//  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     //
//  FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License      //
//  for  more details.                                                        //
//                                                                            //
//  You should have received a copy of the GNU General Public License along   //
//  with HP-Combi. If not, see <https://www.gnu.org/licenses/>.               //
//****************************************************************************//

/** @file
@brief check the required compiler flags for SSE-4.1 */

#ifndef HPCOMBI_ARCH_HPP_
#define HPCOMBI_ARCH_HPP_

#if defined(SIMDE_ARCH_AMD64) && !defined(SIMDE_ARCH_X86_SSE4_1)
char const msg[] =
    R("x86_64 architecture without required compiler flags for SSE-4.1 "
      "instruction set. Did you forget to provide the flag -march="
      "(native,avx,sse4.1) flag ?");
#error(msg)
#endif

#endif  // HPCOMBI_ARCH_HPP_
