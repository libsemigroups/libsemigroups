//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Finn Smith
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

// This file contains the implementation of the TwistedBipartition class.

#include "libsemigroups/twisted-bipart.hpp"

#include <cmath>     // for abs
#include <iterator>  // for distance
#include <limits>    // for numeric_limits
#include <numeric>   // for iota
#include <thread>    // for get_id
#include <utility>   // for move

#include "libsemigroups/constants.hpp"  // for UNDEFINED, operator==, operator!=
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "libsemigroups/detail/fmt.hpp"     // for join, join_view
#include "libsemigroups/detail/report.hpp"  // for this_threads_id
#include "libsemigroups/detail/uf.hpp"      // for Duf

namespace libsemigroups {

  size_t bipartition::number_floating_components(Bipartition const& a,
                                                 Bipartition const& b) {
    size_t              n   = a.degree();
    size_t              anr = a.number_of_blocks();
    size_t              bnr = b.number_of_blocks();
    std::vector<size_t> fuse(anr + bnr);

    for (size_t i = 0; i < fuse.size(); ++i) {
      fuse[i] = i;
    }

    auto find_root = [&fuse](size_t i) -> size_t {
      while (fuse[i] < i) {
        i = fuse[i];
      }
      return i;
    };

    for (size_t i = 0; i < n; ++i) {
      size_t x = find_root(a[i + n]);
      size_t y = find_root(b[i] + anr);
      if (x < y) {
        fuse[y] = x;
      } else if (x > y) {
        fuse[x] = y;
      }
    }

    std::vector<bool> earthed(fuse.size(), false);
    for (size_t i = 0; i < n; ++i) {
      earthed[find_root(a[i])]           = true;
      earthed[find_root(b[i + n] + anr)] = true;
    }

    std::vector<bool> already_counted(fuse.size(), false);
    size_t            floating_count = 0;

    for (size_t i = n; i < 2 * n; ++i) {
      size_t x = find_root(a[i]);
      if (!earthed[x] && !already_counted[x]) {
        already_counted[x] = true;
        ++floating_count;
      }
    }

    for (size_t i = 0; i < n; ++i) {
      size_t x = find_root(b[i] + anr);
      if (!earthed[x] && !already_counted[x]) {
        already_counted[x] = true;
        ++floating_count;
      }
    }

    return floating_count;
  }

  bool TwistedBipartition::operator==(TwistedBipartition const& other) const {
    if (is_zero() && other.is_zero() and _threshold == other._threshold) {
      return true;
    }

    return (_bipartition == other._bipartition
            && _floating_components == other._floating_components
            && _threshold == other._threshold);
  }

  bool TwistedBipartition::operator!=(TwistedBipartition const& other) const {
    return !(*this == other);
  }

  bool TwistedBipartition::operator<(TwistedBipartition const& other) const {
    if (is_zero()) {
      if (other.is_zero()) {
        return _threshold < other._threshold;
      }
      return true;
    }

    return _bipartition < other._bipartition
           || (_bipartition == other._bipartition
               && _threshold < other._threshold)
           || (_bipartition == other._bipartition
               && _threshold == other._threshold
               && _floating_components < other._floating_components);
  }

  size_t TwistedBipartition::hash_value() const {
    if (is_zero()) {
      return 0;
    }
    return Hash<Bipartition>()(_bipartition)
           ^ std::hash<size_t>()(_floating_components);
  }

  void
  TwistedBipartition::product_inplace_no_checks(TwistedBipartition const& x,
                                                TwistedBipartition const& y,
                                                size_t thread_id) {
    if (x.degree() != y.degree()) {
      LIBSEMIGROUPS_EXCEPTION(
          "Cannot multiply TwistedBipartitions of different degrees, found "
          "degrees {} and {}.",
          x.degree(),
          y.degree());
    }
    if (x.threshold() != y.threshold()) {
      LIBSEMIGROUPS_EXCEPTION("Cannot multiply TwistedBipartitions with "
                              "different thresholds, found "
                              "thresholds {} and {}.",
                              x.threshold(),
                              y.threshold());
    }

    _threshold = x._threshold;

    if (x.is_zero() || y.is_zero()) {
      _floating_components = _threshold + 1;
      return;
    }

    _floating_components = x._floating_components + y._floating_components
                           + bipartition::number_floating_components(
                               x._bipartition, y._bipartition);
    _bipartition.product_inplace_no_checks(
        x._bipartition, y._bipartition, thread_id);
  }

  TwistedBipartition operator*(TwistedBipartition const& x,
                               TwistedBipartition const& y) {
    TwistedBipartition result(x);
    result.product_inplace_no_checks(x, y);
    return result;
  }

  [[nodiscard]] std::string to_human_readable_repr(TwistedBipartition const& x,
                                                   std::string_view braces,
                                                   size_t           max_width) {
    if (x.is_zero()) {
      return fmt::format(
          "<Zero for TwistedBipartitions of degree {}, threshold {}>",
          x.degree(),
          x.threshold());
    }

    std::string bipart_str
        = to_human_readable_repr(x.bipartition(), braces, max_width);

    std::string full_string = fmt::format(
        "TwistedBipartition({}, threshold {}, {} floating components)",
        bipart_str,
        x.threshold(),
        x.floating_components());

    if (full_string.size() < max_width) {
      return full_string;
    }

    // TODO(later): instead re-call Bipartition with lower max_width?
    return fmt::format(
        "<TwistedBipartition of degree {} with {} blocks, rank {}, "
        "threshold {}, and {} floating components>",
        x.degree(),
        x.bipartition().number_of_blocks(),
        x.bipartition().rank(),
        x.threshold(),
        x.floating_components());
  }

}  // namespace libsemigroups
