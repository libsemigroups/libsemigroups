//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains TODO

#ifndef LIBSEMIGROUPS_SRC_KNUTH_BENDIX_OVERLAP_HPP_
#define LIBSEMIGROUPS_SRC_KNUTH_BENDIX_OVERLAP_HPP_

struct OverlapMeasure {
  virtual size_t operator()(Rule const*                                 AB,
                            Rule const*                                 BC,
                            internal_string_type::const_iterator const& it)
      = 0;
  virtual ~OverlapMeasure() {}
};

struct ABC : public OverlapMeasure {
  size_t operator()(Rule const*                                 AB,
                    Rule const*                                 BC,
                    internal_string_type::const_iterator const& it) final {
    LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
    LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
    LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
    // |A| + |BC|
    return (it - AB->lhs()->cbegin()) + BC->lhs()->size();
  }
};

struct AB_BC : public OverlapMeasure {
  size_t operator()(Rule const*                                 AB,
                    Rule const*                                 BC,
                    internal_string_type::const_iterator const& it) final {
    LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
    LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
    LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
    (void) it;
    // |AB| + |BC|
    return AB->lhs()->size() + BC->lhs()->size();
  }
};

struct MAX_AB_BC : public OverlapMeasure {
  size_t operator()(Rule const*                                 AB,
                    Rule const*                                 BC,
                    internal_string_type::const_iterator const& it) final {
    LIBSEMIGROUPS_ASSERT(AB->active() && BC->active());
    LIBSEMIGROUPS_ASSERT(AB->lhs()->cbegin() <= it);
    LIBSEMIGROUPS_ASSERT(it < AB->lhs()->cend());
    (void) it;
    // max(|AB|, |BC|)
    return std::max(AB->lhs()->size(), BC->lhs()->size());
  }
};

#endif  // LIBSEMIGROUPS_SRC_KNUTH_BENDIX_OVERLAP_HPP_
