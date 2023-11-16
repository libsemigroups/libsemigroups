////////////////////////////////////////////////////////////////////////////////
//        Copyright (C) 2023 Florent Hivert <Florent.Hivert@lisn.fr>,         //
//                                                                            //
//  Distributed under the terms of the GNU General Public License (GPL)       //
//                                                                            //
//    This code is distributed in the hope that it will be useful,            //
//    but WITHOUT ANY WARRANTY; without even the implied warranty of          //
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       //
//   General Public License for more details.                                 //
//                                                                            //
//  The full text of the GPL is available at:                                 //
//                                                                            //
//                  http://www.gnu.org/licenses/                              //
////////////////////////////////////////////////////////////////////////////////

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
