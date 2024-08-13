//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains the implementations of the functions declared in
// ranges.hpp.

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Custom ranges
  ////////////////////////////////////////////////////////////////////////

  template <typename InputRange>
  typename Random::Range<InputRange>::output_type
  Random::Range<InputRange>::get() const noexcept {
    LIBSEMIGROUPS_ASSERT(!_input.at_end() && !_at_end);

    static std::random_device rd;
    static std::mt19937       gen(rd());

    if (!_val_set) {
      std::uniform_int_distribution<size_t> dist(0, _input | rx::count());
      _val     = (_input | rx::skip_n(dist(gen)) | rx::take(1)).get();
      _val_set = true;
    }
    return _val;
  }

  ////////////////////////////////////////////////////////////////////////
  // Comparison functions
  ////////////////////////////////////////////////////////////////////////

  template <typename Range, typename Compare>
  constexpr bool is_sorted(Range r, Compare&& comp) {
    if (!r.at_end()) {
      auto first = r.get();
      r.next();
      while (!r.at_end()) {
        auto second = r.get();
        if (comp(second, first)) {
          return false;
        }
        first = second;
        r.next();
      }
    }
    return true;
  }

  template <typename Range1, typename Range2>
  constexpr bool equal(Range1 r1, Range2 r2) {
    while (!r1.at_end()) {
      if (r1.get() != r2.get()) {
        return false;
      }
      r1.next();
      r2.next();
    }
    return true;
  }

  template <typename Range1, typename Range2>
  constexpr bool lexicographical_compare(Range1 r1, Range2 r2) {
    while (!r1.at_end() && !r2.at_end()) {
      auto next1 = r1.get();
      auto next2 = r2.get();
      if (next1 < next2) {
        return true;
      }
      if (next2 < next1) {
        return false;
      }
      r1.next();
      r2.next();
    }
    return r1.at_end() && !r2.at_end();
  }

  template <typename Range1, typename Range2>
  constexpr bool shortlex_compare(Range1 r1, Range2 r2) {
    size_t n1 = rx::count()(r1);
    size_t n2 = rx::count()(r2);
    if (n1 != n2) {
      return n1 < n2;
    }
    return lexicographical_compare(r1, r2);
  }

  ////////////////////////////////////////////////////////////////////////
  // String representation
  ////////////////////////////////////////////////////////////////////////

  template <typename Range, typename>
  std::ostream& operator<<(std::ostream& os, Range r) {
    os << "{{";  // {{ is an escaped single { for fmt
    std::string sep = "";
    for (auto const& item : r) {
      os << sep << detail::to_string(item);
      sep = ", ";
    }
    os << "}}";
    return os;
  }
}  // namespace libsemigroups
