////////////////////////////////////////////////////////////////////////////////
//       Copyright (C) 2016 Florent Hivert <Florent.Hivert@lri.fr>,           //
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

/** @file
 * @brief Generic compile time power
 *
 * The goal of this file is to be able to write expressions such as @c
 * pow<23>(2.5) or @c pow<n>(x) where the first expression is entirely
 * computed as compile time and the second one is expanded also as compile
 * time to a O(log n) long sequence of multiplication. Furthermore such
 * expression not only works for numbers for for any type where there is a
 * neutral element and an associative (non necessarily commutative) product,
 * namely what mathematicians call \e monoids. These include for example,
 * strings where the neutral element is the empty string and the product is
 * the concatenation.
 *
 * see HPCombi::power_helper::Monoid<std::string>
 *
 * @example stringmonoid.cpp
 * This is an example of how to use pow with a non numerical Monoid.
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
