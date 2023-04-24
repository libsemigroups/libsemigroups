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

#include "libsemigroups/to-todd-coxeter.hpp"

#include <utility>  // for move
#include <vector>   // for allocator, vector

#include "libsemigroups/constants.hpp"          // for Max, operator==, POSI...
#include "libsemigroups/word-graph.hpp"            // for WordGraph
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTI...
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/knuth-bendix-new.hpp"   // for KnuthBendix
#include "libsemigroups/to-froidure-pin.hpp"    // for to_froidure_pin
#include "libsemigroups/todd-coxeter-new.hpp"   // for ToddCoxeter
#include "libsemigroups/types.hpp"              // for congruence_kind

namespace libsemigroups {

  ToddCoxeter to_todd_coxeter(congruence_kind knd, FroidurePinBase& fp) {
    using node_type         = typename ToddCoxeter::digraph_type::node_type;
    using label_type        = typename ToddCoxeter::digraph_type::label_type;
    using cayley_graph_type = typename FroidurePinBase::cayley_graph_type;

    cayley_graph_type const* ad;

    if (knd == congruence_kind::left) {
      ad = &fp.left_cayley_graph();
    } else {
      ad = &fp.right_cayley_graph();
    }

    WordGraph<node_type> tc_arg(ad->number_of_nodes() + 1,
                                    ad->out_degree());

    for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
      tc_arg.set_target_no_checks(0, a, fp.current_position(a) + 1);
    }

    for (node_type n = 0; n < tc_arg.number_of_nodes() - 1; ++n) {
      for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
        tc_arg.set_target_no_checks(n + 1, a, ad->neighbor_no_checks(n, a) + 1);
      }
    }
    return ToddCoxeter(knd, std::move(tc_arg));
  }

  ToddCoxeter to_todd_coxeter(congruence_kind knd, KnuthBendix& kb) {
    if (kb.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION_V3(
          "cannot construct a ToddCoxeter instance using the Cayley graph of "
          "an infinite KnuthBendix object, maybe try ToddCoxeter({}, "
          "kb.presentation()) instead?",
          kb.kind());
    }
    auto fp = to_froidure_pin(kb);
    return to_todd_coxeter(knd, fp);
  }
}  // namespace libsemigroups
