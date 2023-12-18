//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for semigroups and monoid.

#include "libsemigroups/sims1.hpp"

#include <algorithm>   // for max, find_if, fill
#include <chrono>      // for duration, durat...
#include <functional>  // for ref
#include <memory>      // for unique_ptr, mak...
#include <string>      // for string
#include <string>      // for operator+, basi...
#include <thread>      // for thread, yield
#include <utility>     // for swap

#include "libsemigroups/constants.hpp"     // for operator!=, ope...
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_E...
#include "libsemigroups/felsch-graph.hpp"  // for FelschGraph
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/sims1.hpp"
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/transf.hpp"           // for Transf, validate
#include "libsemigroups/types.hpp"            // for congruence_kind
#include "libsemigroups/word-graph.hpp"       // for follow_path_no_...

#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/detail/stl.hpp"     // for JoinThreads
#include "libsemigroups/detail/string.hpp"  // for group_digits

#include "fmt/format.h"  // for buffer::append

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Sims1Stats
  ////////////////////////////////////////////////////////////////////////

  Sims1Stats::Sims1Stats()
      : count_last(),
        count_now(),
        max_pending(),
        total_pending_last(),
        total_pending_now() {
    stats_zero();
  }

  Sims1Stats& Sims1Stats::stats_zero() {
    count_last         = 0;
    count_now          = 0;
    max_pending        = 0;
    total_pending_last = 0;
    total_pending_now  = 0;
    return *this;
  }

  Sims1Stats& Sims1Stats::init_from(Sims1Stats const& that) {
    count_last         = that.count_last;
    count_now          = that.count_now.load();
    max_pending        = that.max_pending.load();
    total_pending_last = that.total_pending_last;
    total_pending_now  = that.total_pending_now.load();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Sims1
  ////////////////////////////////////////////////////////////////////////

  Sims1& Sims1::kind(congruence_kind ck) {
    if (ck == congruence_kind::left && kind() != ck) {
      presentation::reverse(_presentation);
      reverse(_include);
      reverse(_exclude);
    }
    _kind = ck;
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // RepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  RepOrc& RepOrc::init() {
    _min  = 0;
    _max  = 0;
    _size = 0;
    return *this;
  }

  Sims1::word_graph_type RepOrc::word_graph() const {
    using word_graph_type = typename Sims1::word_graph_type;
    using node_type       = typename word_graph_type::node_type;

    report_no_prefix("{:+<80}\n", "");
    report_default(
        "RepOrc: Searching for a faithful rep. o.r.c. on [{}, {}) points\n",
        _min,
        _max + 1);
    if (_min > _max || _max == 0) {
      report_no_prefix("{:+<80}\n", "");
      report_default(
          "RepOrc: No faithful rep. o.r.c. exists in [{}, {}) = \u2205\n",
          _min,
          _max + 1);
      return word_graph_type(0, 0);
    }

    SuppressReportFor suppressor("FroidurePin");

    std::atomic_uint64_t count(0);

    auto hook = [&](word_graph_type const& x) {
      ++count;
      if (x.number_of_active_nodes() >= _min) {
        auto first = (presentation().contains_empty_word() ? 0 : 1);
        auto S     = to_froidure_pin<Transf<0, node_type>>(
            x, first, x.number_of_active_nodes());
        // It'd be nice to reuse S here, but this is tricky because hook
        // maybe called in multiple threads, and so we can't easily do this.
        if (presentation().contains_empty_word()) {
          auto one = S.generator(0).identity();
          if (!S.contains(one)) {
            S.add_generator(one);
          }
        }
        LIBSEMIGROUPS_ASSERT(S.size() <= _size);
        if (S.size() == _size) {
          return true;
        }
      }
      return false;
    };

    auto result = Sims1(congruence_kind::right)
                      .settings_copy_from(*this)
                      .find_if(_max, hook);

    if (result.number_of_active_nodes() == 0) {
      report_default(
          "RepOrc: No faithful rep. o.r.c. on [{}, {}) points found\n",
          _min,
          _max + 1);
      result.induced_subgraph_no_checks(0, 0);
    } else {
      report_default("RepOrc: Found a faithful rep. o.r.c. on {} points\n",
                     presentation().contains_empty_word()
                         ? result.number_of_active_nodes()
                         : result.number_of_active_nodes() - 1);
      if (presentation().contains_empty_word()) {
        result.induced_subgraph_no_checks(0, result.number_of_active_nodes());
      } else {
        result.induced_subgraph_no_checks(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
      }
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // MinimalRepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  // An alternative to the approach used below would be to do a sort of
  // binary search for a minimal representation. It seems that in most
  // examples that I've tried, this actually finds the minimal rep close to
  // the start of the search. The binary search approach would only really
  // be useful if the rep found at the start of the search was much larger
  // than the minimal one, and that the minimal one would not be found until
  // late in the search through all the reps with [1, best rep so far). It
  // seems that in most examples, binary search will involve checking many
  // of the same graphs repeatedly, i.e. if we check [1, size = 1000) find a
  // rep on 57 points, then check [1, 57 / 2) find nothing, then check
  // [57/2, 57) and find nothing, then the search through [57/2, 57)
  // actually iterates through all the digraphs with [1, 57 / 2) again (just
  // doesn't construct a FroidurePin object for these). So, it seems to be
  // best to just search through the digraphs with [1, 57) nodes once.
  //
  // TODO(later) perhaps find minimal 2-sided congruences first (or try to)
  // and then run MinimalRepOrc for right congruences excluding all the
  // generating pairs from the minimal 2-sided congruences. Also with this
  // approach FroidurePin wouldn't be required in RepOrc. This might not work,
  // given that the minimal rc might contain some pairs from minimal 2-sided
  // congs, just not all of them.
  Sims1::word_graph_type MinimalRepOrc::word_graph() const {
    auto cr = RepOrc(*this);

    size_t hi   = (presentation().contains_empty_word() ? _size : _size + 1);
    auto   best = cr.min_nodes(1).max_nodes(hi).target_size(_size).word_graph();

    if (best.number_of_nodes() < 1) {
      stats_copy_from(cr.stats());
      return best;
    }

    hi        = best.number_of_nodes();
    auto next = cr.max_nodes(hi - 1).word_graph();
    while (next.number_of_nodes() != 0) {
      hi   = next.number_of_nodes();
      best = std::move(next);
      next = cr.max_nodes(hi - 1).word_graph();
    }
    stats_copy_from(cr.stats());
    return best;
  }

}  // namespace libsemigroups
