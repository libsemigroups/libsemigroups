//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

#include "libsemigroups/todd-coxeter-helpers.hpp"

#include <algorithm>    // for minmax
#include <string_view>  // for basic_st...
#include <tuple>        // for tie

#include "libsemigroups/constants.hpp"          // for operator!=
#include "libsemigroups/obvinf.hpp"             // for is_obvio...
#include "libsemigroups/todd-coxeter-base.hpp"  // for ToddCoxeterBase
#include "libsemigroups/types.hpp"              // for tril

#include "libsemigroups/detail/felsch-graph.hpp"             // for Register...
#include "libsemigroups/detail/node-managed-graph.hpp"       // for random_a...
#include "libsemigroups/detail/report.hpp"                   // for report_d...
#include "libsemigroups/detail/word-graph-with-sources.hpp"  // for WordGrap...

namespace libsemigroups {
  namespace todd_coxeter {
    [[nodiscard]] tril is_non_trivial(ToddCoxeterBase&          tc,
                                      size_t                    tries,
                                      std::chrono::milliseconds try_for,
                                      float                     threshold) {
      using detail::node_managed_graph::random_active_node;

      if (is_obviously_infinite(tc)) {
        return tril::TRUE;
      } else if (tc.finished()) {
        return tc.number_of_classes() == 1 ? tril::FALSE : tril::TRUE;
      }

      for (size_t try_ = 0; try_ < tries; ++try_) {
        report_default(
            "trying to show non-triviality: {} / {}\n", try_ + 1, tries);
        ToddCoxeterBase copy(tc);
        copy.save(true);
        while (!copy.finished()) {
          copy.run_for(try_for);
          size_t limit = copy.current_word_graph().number_of_nodes_active();
          while (copy.current_word_graph().number_of_nodes_active()
                     >= threshold * limit
                 && !copy.finished()) {
            auto  c1 = random_active_node(copy.current_word_graph());
            auto  c2 = random_active_node(copy.current_word_graph());
            auto& wg = const_cast<ToddCoxeterBase::word_graph_type&>(
                copy.current_word_graph());
            wg.merge_nodes_no_checks(c1, c2);
            wg.process_coincidences<detail::RegisterDefs>();
            wg.process_definitions();
            copy.run_for(try_for);
          }
        }
        if (copy.number_of_classes() > 1) {
          report_default("successfully showed non-triviality!\n");
          return tril::TRUE;
        }
      }
      report_default("failed to show non-triviality!\n");
      return tril::unknown;
    }
  }  // namespace todd_coxeter
}  // namespace libsemigroups
