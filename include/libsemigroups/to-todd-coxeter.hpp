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

#ifndef LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TO_TODD_COXETER_HPP_

#include "knuth-bendix.hpp"
#include "todd-coxeter.hpp"

namespace libsemigroups {

  class FroidurePinBase;

  // TODO to_todd_coxeter for FroidurePin<TCE> just return the original
  // ToddCoxeter instance.

  // TODO(0) allow template param "word_type" to be specified
  template <typename Node>
  ToddCoxeter<word_type> to_todd_coxeter(congruence_kind        knd,
                                         FroidurePinBase&       fpb,
                                         WordGraph<Node> const& wg) {
    using node_type  = typename ToddCoxeterBase::word_graph_type::node_type;
    using label_type = typename ToddCoxeterBase::word_graph_type::label_type;

    WordGraph<node_type> copy(wg.number_of_nodes() + 1, wg.out_degree());

    for (label_type a = 0; a < copy.out_degree(); ++a) {
      copy.target_no_checks(0, a, fpb.position_of_generator_no_checks(a) + 1);
    }

    for (node_type n = 0; n < copy.number_of_nodes() - 1; ++n) {
      for (label_type a = 0; a < copy.out_degree(); ++a) {
        copy.target_no_checks(n + 1, a, wg.target_no_checks(n, a) + 1);
      }
    }
    // TODO(1) move "copy" into ToddCoxeter.
    return ToddCoxeter<word_type>(knd, copy);
  }

  // TODO(0) allow template param "word_type" to be specified
  template <typename Rewriter, typename ReductionOrder>
  ToddCoxeter<word_type>
  to_todd_coxeter(congruence_kind                            knd,
                  KnuthBendixBase<Rewriter, ReductionOrder>& kb) {
    if (kb.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot construct a ToddCoxeterBase instance using the Cayley graph "
          "of "
          "an infinite KnuthBendixBase<> object, maybe try ToddCoxeterBase({}, "
          "kb.presentation()) instead?",
          kb.kind());
    }
    // TODO why are we doing this? Why not just use the active rules of kb?
    auto fp = to_froidure_pin(kb);
    return to_todd_coxeter(knd, fp, fp.right_cayley_graph());
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TO_TODD_COXETER_HPP_
