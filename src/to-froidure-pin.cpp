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

#include "libsemigroups/to-froidure-pin.hpp"

#include "libsemigroups/cong.hpp"

namespace libsemigroups {

  using TCE = detail::TCE;

  FroidurePin<TCE> to_froidure_pin(ToddCoxeter& tc) {
    using word_graph_type = typename ToddCoxeter::word_graph_type;

    if (tc.kind() != congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be a two-sided congruence, found a {} congruence",
          tc.kind());
    }

    tc.run();
    tc.shrink_to_fit();
    // Ensure class indices and letters are equal!
    auto wg        = std::make_shared<word_graph_type>(tc.current_word_graph());
    size_t const n = tc.current_word_graph().out_degree();
    size_t       m = n;
    for (letter_type a = 0; a < m;) {
      if (wg->target_no_checks(0, a) != a + 1) {
        wg->remove_label(a);
        m--;
      } else {
        ++a;
      }
    }

    FroidurePin<TCE> result(wg);
    for (size_t i = 0; i < n; ++i) {
      // We use _word_graph.target_no_checks instead of just i, because there
      // might be more generators than cosets.
      result.add_generator(TCE(tc.current_word_graph().target_no_checks(0, i)));
    }
    return result;
  }

  std::unique_ptr<FroidurePinBase> to_froidure_pin(Congruence& cong) {
    cong.run();
    if (cong.has<Kambites<word_type>>()) {
      // TODO(0) there an issue here that if the Kambites clause isn't first,
      // then we incorrectly start running the other algos here, which run
      // forever. Probably something goes wrong that the other runners don't
      // get deleted if Kambites wins, since it's run first.
      return to_froidure_pin(*cong.get<Kambites<word_type>>());
    } else if (cong.has<ToddCoxeter>()) {
      auto fp = to_froidure_pin(*cong.get<ToddCoxeter>());
      return std::make_unique<decltype(fp)>(std::move(fp));
    } else if (cong.has<KnuthBendix<>>()) {
      auto fp = to_froidure_pin(*cong.get<KnuthBendix<>>());
      return std::make_unique<decltype(fp)>(std::move(fp));
    }
    LIBSEMIGROUPS_EXCEPTION("TODO");
  }

}  // namespace libsemigroups
