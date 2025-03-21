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
@brief  Generic compile-time unrolling of the fast exponentiation algorithm.

Allows to write expressions such as
- @c pow<23>(2.5) : entirely computed at compile time
- @c pow<n>(x) expanded at compile time to a O(log n) long sequence of
multiplications.

Such expressions work for numbers but also for any type where there is a
neutral element and an associative (non necessarily commutative) product,
ie what mathematicians call \e monoids.
These include for example
strings where the neutral element is the empty string and the product is
the concatenation.

See HPCombi::power_helper::Monoid<std::string>

The algorithm used here is based on the base-2 representation of n,
it is a 2-approximation of the optimum number of multiplications.
The general problem is called *addition chain* and one can sometimes do better,
eg on fibonaci numbers, use rather the fibonacci recurrence relation
to choose which products to compute.

@example stringmonoid.cpp
how to use pow with a non numerical Monoid.
*/

#ifndef HPCOMBI_POWER_HPP_
#define HPCOMBI_POWER_HPP_

namespace HPCombi {

namespace power_helper {

// Forward declaration
template <typename T> struct Monoid;

}  // namespace power_helper

/** A generic compile time squaring function
 *
 *  @param x      the number to square
 *  @return       @a x squared
 *
 *  @details To use for a specific type the user should pass a monoid
 *  structure as second parameter to the template. Alternatively a
 *  default monoid structure can be defined for a given type by specializing
 *  the template struct #HPCombi::power_helper::Monoid
 */
template <typename T, typename M = power_helper::Monoid<T>>
const T square(const T x) {
    return M::prod(x, x);
}

/** A generic compile time exponentiation function
 *
 *  @tparam exp the power
 *  @param x    the number to exponentiate
 *  @return @a x to the power @a exp
 *
 *  @details Raise x to the exponent exp where exp is known at compile
 *  time. We use the classical recursive binary algorithm, but the recursion
 *  is unfolded and optimized at compile time giving an assembly code which is
 *  just a sequence of multiplication.
 *
 *  To use for a specific type the user should pass a Monoid structure (see
 *  below) as third parameter to the template. Alternatively a default monoid
 *  structure can be defined for a given type by specializing the template
 *  struct #HPCombi::power_helper::Monoid
 */
template <unsigned exp, typename T, typename M = power_helper::Monoid<T>>
const T pow(const T x) {
    return (exp == 0) ? M::one()
           : (exp % 2 == 0)
               ? square<T, M>(pow<unsigned(exp / 2), T, M>(x))
               : M::prod(x, square<T, M>(pow<unsigned(exp / 2), T, M>(x)));
}

namespace power_helper {

/** Algebraic monoid structure used by default for type T by the pow
 *  function and prod function
 *
 *  @details A Monoid structure is required to define two static members
 *  - T #one() : the unit of the monoid
 *  - T #prod(T, T) : the product of two elements in the monoid
 *
 * By default for any type \c T, #one is constructed from the literal 1 and
 * #prod calls the operator *. One can change these default by specializing
 * the template for some specific type \c T.
 */
template <typename T> struct Monoid {
    /// The one of type T
    static const T one() { return 1; }

    /** the product of two elements of type T
     *  @param a the first element to be multiplied
     *  @param b the second element to be multiplied
     *  @return the product a * b
     */
    static const T prod(T a, T b) { return a * b; }
};

}  // namespace power_helper

}  // namespace HPCombi

#endif  // HPCOMBI_POWER_HPP_
