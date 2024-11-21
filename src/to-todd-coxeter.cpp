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
#include "libsemigroups/exception.hpp"          // for LIBSEMIGROUPS_EXCEPTI...
#include "libsemigroups/froidure-pin-base.hpp"  // for FroidurePinBase
#include "libsemigroups/froidure-pin.hpp"       // for FroidurePin
#include "libsemigroups/knuth-bendix.hpp"       // for KnuthBendix<>
#include "libsemigroups/to-froidure-pin.hpp"    // for to_froidure_pin
#include "libsemigroups/todd-coxeter.hpp"       // for ToddCoxeter
#include "libsemigroups/types.hpp"              // for congruence_kind
#include "libsemigroups/word-graph.hpp"         // for WordGraph

namespace libsemigroups {

  ToddCoxeter to_todd_coxeter(congruence_kind knd, FroidurePinBase& fp) {
    using node_type         = typename ToddCoxeter::word_graph_type::node_type;
    using label_type        = typename ToddCoxeter::word_graph_type::label_type;
    using cayley_graph_type = typename FroidurePinBase::cayley_graph_type;

    cayley_graph_type const* wg;

    if (knd == congruence_kind::left) {
      wg = &fp.left_cayley_graph();
    } else {
      wg = &fp.right_cayley_graph();
    }

    WordGraph<node_type> tc_arg(wg->number_of_nodes() + 1, wg->out_degree());

    for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
      tc_arg.target_no_checks(0, a, fp.position_of_generator_no_checks(a) + 1);
    }

    for (node_type n = 0; n < tc_arg.number_of_nodes() - 1; ++n) {
      for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
        tc_arg.target_no_checks(n + 1, a, wg->target_no_checks(n, a) + 1);
      }
    }
    return ToddCoxeter(knd != congruence_kind::twosided ? congruence_kind::right
                                                        : knd,
                       std::move(tc_arg));
  }

}  // namespace libsemigroups
