//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

// This file contains implementations of the PBR class.

#include "libsemigroups/tlcode.hpp"

#include <algorithm>  // for all_of, fill
#include <ostream>    // for operator<<, cha...
#include <string>     // for operator+, char...
#include <thread>     // for thread

#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_E...

#include "libsemigroups/detail/containers.hpp"  // for DynamicArray2
#include "libsemigroups/detail/string.hpp"      // for to_string

namespace libsemigroups {

  TLCode tlcode::one(size_t deg) { return TLCode(deg); }

  TLCode tlcode::one(TLCode const& x) {
    return tlcode::one(x.degree());
  }

  TLCode operator*(TLCode const& x, TLCode const& y) {
    TLCode xy(x);
    xy.product_inplace_no_checks(y);
    return xy;
  }

  [[nodiscard]] std::string to_human_readable_repr(TLCode const& x) {
    // TODO(2) allow different braces
    // TODO(now) Make this better, probably by including some data from
    // x._vector
    return fmt::format("<TLCode of degree {}>", x.degree());
  }

  ////////////////////////////////////////////////////////////////////////
  // Partitioned binary relations (TLCodes)
  ////////////////////////////////////////////////////////////////////////

  TLCode::TLCode(std::vector<uint32_t> vec) : _code(vec) {}

  TLCode::TLCode(size_t degree) : TLCode(std::vector<uint32_t>(degree, 0)) {}

  // TLCode::TLCode(TLCode::initializer_list_type<int32_t> left,
  //          TLCode::initializer_list_type<int32_t> right)
  //     : TLCode(process_left_right_no_checks(left, right)) {}

  // TLCode::TLCode(TLCode::vector_type<int32_t> left, TLCode::vector_type<int32_t> right)
  //     : TLCode(process_left_right_no_checks(left, right)) {}

  std::ostringstream& operator<<(std::ostringstream& os, TLCode const& tlc) {
    os << fmt::format("{}", tlc._code);
    return os;
  }

  std::ostream& operator<<(std::ostream& os, TLCode const& tlc) {
    os << detail::to_string(tlc);
    return os;
  }

  void TLCode::throw_if_entry_out_of_bounds() const {
    uint32_t mx = 0;
    for (size_t i = 0; i < _code.size(); i++) {
      if (_code[i] > mx) {
          LIBSEMIGROUPS_EXCEPTION(
            fmt::format(
                "entry out of bounds, {} in position {} should be less than {}",
                _code[i], i, mx));
      }
      if (_code[i] == 0) mx ++;
      else mx = _code[i];
    }
  }

  uint32_t TLCode::TL_max(size_t pos) const {
    if (pos == 0) return 0;
    // search for the last non zero value in _code strictly before pos
    size_t lastn0 = pos - 1;
    while (lastn0 > 0 && _code[lastn0] == 0) lastn0--;
    if (lastn0 == 0) return pos;  // no value found
    return _code[lastn0] + (pos - lastn0) - 1;
  }

  void TLCode::product_by_generator_inplace_no_checks(uint32_t t) {
    LIBSEMIGROUPS_ASSERT(t + 1 < _code.size()); // ensure that _code is not empty
    // fmt::print("**************\nact {} by {}\n", _code, t);
    product_by_generator_inplace_no_checks(t, _code.size() - 1);
    // fmt::print("=============> {} \n", _code);
  }
  void TLCode::product_by_generator_inplace_no_checks(uint32_t t, size_t pos) {
    uint32_t last = pos - _code[pos];
    // fmt::print("act {} by {} in position {}, last = {}, TL_max = {}\n",
    //           _code, t, pos, last, TL_max(pos));
    if (last > t + 1) { // t commute with col => act on the rest of the word
        product_by_generator_inplace_no_checks(t, pos - 1);
        if (TL_max(pos) < _code[pos]) _code[pos] -= 2;
    }
    else if (last == t + 1) { // try to extends n, n-1, ..., t+1 to n, n-1, ..., t
        if (_code[pos] < TL_max(pos)) {
            // fmt::print("max = {}, extent 1 : {}\n", TL_max(pos), _code[pos]);
            _code[pos]++;
        } else {
            // fmt::print("max = {}, remove 1 : {}\n", TL_max(pos), _code[pos]);
            _code[pos]--;
        }
    }
    else if (last < t) {
      // case: t belongs to the col n, n-1, ..., last and cuts it
      // new column is n, n-1, ..., t + 1 of length n - t + 1
        _code[pos] = pos - t;
        // let t-1, ..., last commute and act on the remaining of the word
        for (uint32_t i1 = t - 1; i1 > last; i1 --) {
            product_by_generator_inplace_no_checks(i1 - 1, pos - 1);
        }
    } // else (last == t) { // col = n, n-1, ..., t => apply t^2 = t
  }

  void TLCode::product_inplace_no_checks(TLCode const& x) {
    LIBSEMIGROUPS_ASSERT(degree() == x.degree());
    for (uint32_t pos = 0; pos < degree(); pos++) {
      for (uint32_t i = 0; i < x._code[pos]; i++) {
        product_by_generator_inplace_no_checks(pos - i - 1);
      }
    }
  }

  void TLCode::product_inplace(TLCode const& x) {
      if (degree() != x.degree()) {
          LIBSEMIGROUPS_EXCEPTION(
              fmt::format("degree mismatch {} and {}", _code, x._code));
      }
      product_inplace_no_checks(x);
  }
}  // namespace libsemigroups
