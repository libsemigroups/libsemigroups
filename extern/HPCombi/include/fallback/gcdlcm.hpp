//****************************************************************************//
//       Copyright (C) 2017 Florent Hivert <Florent.Hivert@lri.fr>,           //
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
//****************************************************************************//

#ifndef HPCOMBI_FALLBACK_GCDLCM_HPP_INCLUDED
#define HPCOMBI_FALLBACK_GCDLCM_HPP_INCLUDED


// FallBack gcd and lcm in case experimental/numeric is not present

namespace std {
namespace experimental {

constexpr unsigned gcd( unsigned m, unsigned n )
{ return n == 0 ? m : gcd(n, m % n); }

constexpr unsigned lcm( unsigned m, unsigned n )
{ return m == 0 or n == 0 ? 0 : (m / gcd(m,n)) * n; }

}
}

#endif  // HPCOMBI_FALLBACK_GCDLCM_HPP_INCLUDED

