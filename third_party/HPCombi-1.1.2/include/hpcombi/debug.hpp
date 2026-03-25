//****************************************************************************//
//    Copyright (C) 2023-2024 James D. Mitchell <jdm3@st-andrews.ac.uk>       //
//    Copyright (C) 2023-2024 Florent Hivert <Florent.Hivert@lisn.fr>,        //
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
@brief defines the macro \c HPCOMBI_ASSERT */

#ifndef HPCOMBI_DEBUG_HPP_
#define HPCOMBI_DEBUG_HPP_

#ifdef HPCOMBI_DEBUG
#include <cassert>
#define HPCOMBI_ASSERT(x) assert(x)
#else
#define HPCOMBI_ASSERT(x)
#endif

#endif  // HPCOMBI_DEBUG_HPP_
