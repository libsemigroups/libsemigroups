//****************************************************************************//
//     Copyright (C) 2016-2024 Florent Hivert <Florent.Hivert@lisn.fr>,       //
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
 * @brief Example of how to use #HPCombi::pow with
 * #HPCombi::power_helper::Monoid
 */

#include <cassert>
#include <string>

#include "hpcombi/power.hpp"

namespace HPCombi {
namespace power_helper {

// Algebraic monoid for string with concatenation
template <> struct Monoid<std::string> {

    // The one of the string monoid
    static std::string one() { return {}; };

    /* The product of two strings that is their concatenation
     * @param a the first string to be concatenated
     * @param b the second string to be concatenated
     * @return the concatenation of \a a and \a b
     */
    static std::string prod(std::string a, std::string b) { return a + b; }
};

}  // namespace power_helper
}  // namespace HPCombi

int main() {
    assert(HPCombi::pow<0>(std::string("ab")) == "");
    assert(HPCombi::pow<4>(std::string("ab")) == "abababab");
    assert(HPCombi::pow<5>(std::string("abc")) == "abcabcabcabcabc");
}
