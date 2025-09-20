//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

#include <algorithm>  // for minmax
#include <chrono>
#include <string_view>  // for basic_st...
#include <tuple>        // for tie

#include "libsemigroups/constants.hpp"  // for operator!=
#include "libsemigroups/obvinf.hpp"     // for is_obvio...
#include "libsemigroups/types.hpp"      // for tril

#include "libsemigroups/detail/felsch-graph.hpp"        // for Register...
#include "libsemigroups/detail/node-managed-graph.hpp"  // for random_a...
#include "libsemigroups/detail/report.hpp"              // for report_d...
#include "libsemigroups/detail/todd-coxeter-impl.hpp"   // for ToddCoxeterImpl
#include "libsemigroups/detail/word-graph-with-sources.hpp"  // for WordGrap...

namespace libsemigroups {

  using DoRegisterDefs = detail::felsch_graph::DoRegisterDefs<
      detail::NodeManagedGraph<detail::ToddCoxeterImpl::Graph::node_type>,
      detail::ToddCoxeterImpl::Definitions>;

  namespace todd_coxeter {
    [[nodiscard]] tril is_non_trivial(detail::ToddCoxeterImpl&  tc,
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
        detail::ToddCoxeterImpl copy(tc);
        copy.save(true);
        while (!copy.finished()) {
          copy.run_for(try_for);
          size_t limit = copy.current_word_graph().number_of_nodes_active();
          while (copy.current_word_graph().number_of_nodes_active()
                     >= threshold * limit
                 && !copy.finished()) {
            auto  c1 = random_active_node(copy.current_word_graph());
            auto  c2 = random_active_node(copy.current_word_graph());
            auto& wg = const_cast<detail::ToddCoxeterImpl::Graph&>(
                copy.current_word_graph());
            wg.merge_nodes_no_checks(c1, c2);
            wg.process_coincidences(DoRegisterDefs(wg));
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

    // TODO(1) improve this to use the reporting from the ToddCoxeterImpl
    // itself, and to periodically process coincidences when a certain number
    // are discovered.
    // TODO(1) write a more general version that takes a function as 2nd
    // argument that takes a single argument (a word) and returns another
    // equivalent word in the congruence represented by the ToddCoxeter. We
    // could then, for example, use a KnuthBendix to rewrite the words, in case
    // that provided any collapse in the graph. This would then mean we could
    // possibly rewrite prefixes of words in the onesided case, so we could lift
    // the restriction at the start of the function.
    void perform_lookbehind(detail::ToddCoxeterImpl& tc) {
      if (tc.kind() == congruence_kind::onesided
          && !tc.internal_generating_pairs().empty()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument <tc> (ToddCoxeter) must be a 2-sided congruence")
      } else if (!tc.started()) {
        return;
      }

      using word_graph_type = detail::ToddCoxeterImpl::word_graph_type;
      using node_type       = word_graph_type::node_type;
      using detail::group_digits;

      auto& wg = const_cast<word_graph_type&>(tc.current_word_graph());

      auto current = wg.initial_node();

      word_type w1, w2;

      using detail::group_digits;

      std::chrono::high_resolution_clock::time_point lookbehind_start_time;
      size_t                                         at = 0, found = 0;
      uint64_t num_nodes_before_lookbehind = wg.number_of_nodes_active();

      if (reporting_enabled()) {
        // TODO(1) update to be more like the reporting coming from ToddCoxeter
        // itself
        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: performing lookbehind at "
                       "{} ({} active nodes) . . .\n",
                       detail::string_time(delta(tc.start_time())),
                       group_digits(wg.number_of_nodes_active()));
        report_no_prefix("{:+<90}\n", "");
        lookbehind_start_time = std::chrono::high_resolution_clock::now();
        tc.reset_last_report();
      }

      while (current != wg.detail::NodeManager<node_type>::first_free_node()) {
        if (reporting_enabled()) {
          if (delta(tc.last_report()) > std::chrono::seconds(1)) {
            tc.reset_last_report();
            // TODO(1) update to be more like the reporting coming from
            // ToddCoxeter itself
            report_default(
                "ToddCoxeter: at {} of {} found {} pairs of distinct "
                "nodes so far\n",
                group_digits(at),
                group_digits(wg.number_of_nodes_active()),
                group_digits(found));
          }
        }
        w1.clear();
        w2.clear();
        tc.current_word_of_no_checks(std::back_inserter(w1), current);
        tc.reduce_no_run_no_checks(
            std::back_inserter(w2), w1.cbegin(), w1.cend());
        if (!std::equal(w1.begin(), w1.end(), w2.begin(), w2.end())) {
          node_type other = word_graph::follow_path_no_checks(
              wg, wg.initial_node(), w2.begin(), w2.end());
          if (other != UNDEFINED && other != current) {
            ++found;
            wg.merge_nodes_no_checks(current, other);
          }
        }
        ++at;
        current = wg.detail::NodeManager<node_type>::next_active_node(current);
      }
      wg.process_coincidences();
      // TODO(1) process_definitions?
      if (reporting_enabled()) {
        report_no_prefix("{:+<90}\n", "");
        report_default("ToddCoxeter: lookabehind complete with    |{:>12} "
                       "(active) |{:>12} (diff)\n",
                       group_digits(wg.number_of_nodes_active()),
                       group_digits(wg.number_of_nodes_active()
                                    - num_nodes_before_lookbehind));
        report_default("ToddCoxeter: after                        |{:>12} "
                       "(time)   |{:>12} (total)\n",
                       detail::string_time(delta(lookbehind_start_time)),
                       detail::string_time(delta(tc.start_time())));
        report_no_prefix("{:+<90}\n", "");
      }
    }

  }  // namespace todd_coxeter
}  // namespace libsemigroups
