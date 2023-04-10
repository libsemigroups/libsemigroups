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

#ifndef LIBSEMIGROUPS_MAKE_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_MAKE_TODD_COXETER_HPP_

#include <cstddef>                // for size_t
#include <type_traits>            // for enable_if_t, is_base_of
                                  //
#include "debug.hpp"              // for LIBSEMIGROUPS_ASSERT
#include "digraph.hpp"            // for ActionDigraph
#include "exception.hpp"          // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin-base.hpp"  // for FroidurePinBase
#include "knuth-bendix-new.hpp"   // for KnuthBendix
#include "todd-coxeter-new.hpp"   // for ToddCoxeter

namespace libsemigroups {
  class FroidurePinBase;

  template <typename T,
            typename = std::enable_if_t<std::is_same_v<ToddCoxeter, T>>>
  ToddCoxeter make(congruence_kind knd, FroidurePinBase& fp) {
    using node_type         = typename ToddCoxeter::digraph_type::node_type;
    using label_type        = typename ToddCoxeter::digraph_type::label_type;
    using digraph_type      = ActionDigraph<node_type>;
    using cayley_graph_type = typename FroidurePinBase::cayley_graph_type;

    cayley_graph_type const* ad;

    if (knd == congruence_kind::left) {
      ad = &fp.left_cayley_graph();
    } else {
      ad = &fp.right_cayley_graph();
    }
    digraph_type tc_arg(ad->number_of_nodes() + 1, ad->out_degree());

    for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
      tc_arg.def_edge_nc(0, a, fp.current_position(a) + 1);
    }

    for (node_type n = 0; n < tc_arg.number_of_nodes() - 1; ++n) {
      for (label_type a = 0; a < tc_arg.out_degree(); ++a) {
        tc_arg.def_edge_nc(n + 1, a, ad->unsafe_neighbor(n, a) + 1);
      }
    }
    return ToddCoxeter(knd, std::move(tc_arg));
  }

  template <typename T,
            typename = std::enable_if_t<std::is_same_v<ToddCoxeter, T>>>
  ToddCoxeter make(congruence_kind knd, KnuthBendix& kb) {
    // TODO uncomment
    // if (kb.finished() && kb.is_obviously_finite()) {
    //   froidure_pin_policy(options::froidure_pin::use_cayley_graph);
    // }

    return ToddCoxeter(knd, kb.presentation());
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_MAKE_TODD_COXETER_HPP_
