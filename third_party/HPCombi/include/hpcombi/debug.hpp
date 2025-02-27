////////////////////////////////////////////////////////////////////////////////
//       Copyright (C) 2023 James D. Mitchell <jdm3@st-andrews.ac.uk>         //
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

#ifndef HPCOMBI_DEBUG_HPP_
#define HPCOMBI_DEBUG_HPP_

#ifdef HPCOMBI_DEBUG
#include <cassert>
#define HPCOMBI_ASSERT(x) assert(x)
#else
#define HPCOMBI_ASSERT(x)
#endif

#endif  // HPCOMBI_DEBUG_HPP_
