//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// This file contains implementations of specializations of the class adapters
// in adapters.hpp for the element types in HPCombi.

#include "libsemigroups/hpcombi.hpp"

#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Konieczny Transf16 adapters
  ////////////////////////////////////////////////////////////////////////

  void ImageLeftAction<HPCombi::Transf16, HPCombi::Vect16>::operator()(
      HPCombi::Vect16&         res,
      HPCombi::Transf16 const& x,
      HPCombi::Vect16 const&   y) const noexcept {
    // We cannot use "right_one" here because in HPCombi, right_one for a
    // partial transformation is not an L-class invariant unfortunately.
    HPCombi::Vect16 buf  = {0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff};
    size_t          next = 0;
    for (size_t i = 0; i < 16; ++i) {
      if (buf[x[y[i]]] == 0xff) {
        buf[x[y[i]]] = next++;
      }
      res[i] = buf[x[y[i]]];
    }
  }

  void Rho<HPCombi::Transf16, HPCombi::Vect16>::operator()(
      HPCombi::Vect16&         res,
      HPCombi::Transf16 const& x) const noexcept {
    HPCombi::Vect16 buf  = {0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff};
    size_t          next = 0;
    for (size_t i = 0; i < 16; ++i) {
      if (buf[x[i]] == 0xff) {
        buf[x[i]] = next++;
      }
      res[i] = buf[x[i]];
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Konieczny PTransf16 adapters
  ////////////////////////////////////////////////////////////////////////

  void ImageLeftAction<HPCombi::PTransf16, HPCombi::Vect16>::operator()(
      HPCombi::Vect16&          res,
      HPCombi::PTransf16 const& x,
      HPCombi::Vect16 const&    y) const noexcept {
    // We cannot use "right_one" here because in HPCombi, right_one for a
    // partial transformation is not an L-class invariant unfortunately.
    HPCombi::Vect16 buf  = {0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff};
    size_t          next = 0;
    for (size_t i = 0; i < 16; ++i) {
      if (y[i] != 0xff && x[y[i]] != 0xff) {
        if (buf[x[y[i]]] == 0xff) {
          buf[x[y[i]]] = next++;
        }
        res[i] = buf[x[y[i]]];
      } else {
        res[i] = 0xff;
      }
    }
  }

  void Rho<HPCombi::PTransf16, HPCombi::Vect16>::operator()(
      HPCombi::Vect16&          res,
      HPCombi::PTransf16 const& x) const noexcept {
    // This is the opposite of what might be expected because multiplication
    // is the wrong way around in HPCombi
    HPCombi::Vect16 buf  = {0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff,
                            0xff};
    size_t          next = 0;
    for (size_t i = 0; i < 16; ++i) {
      if (x[i] != 0xff) {
        if (buf[x[i]] == 0xff) {
          buf[x[i]] = next++;
        }
        res[i] = buf[x[i]];
      } else {
        res[i] = 0xff;
      }
    }
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_HPCOMBI_ENABLED
