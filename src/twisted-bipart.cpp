//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Finn Smith
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
      return fmt::format("<Zero for TwistedBipartitions of degree {}>",
                         x.degree());
    }

    if (x.is_one()) {
      return fmt::format("<One for TwistedBipartitions of degree {}>",
                         x.degree());
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
