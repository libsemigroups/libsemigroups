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

  using TCE = v3::detail::TCE;

  FroidurePin<TCE> to_froidure_pin(ToddCoxeter& tc) {
    using digraph_type = typename ToddCoxeter::digraph_type;

    if (tc.kind() != congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION_V3(
          "the argument must be a two-sided congruence, found a {} congruence",
          tc.kind());
    }

    tc.run();
    tc.shrink_to_fit();
    // Ensure class indices and letters are equal!
    auto         wg = std::make_shared<digraph_type>(tc.word_graph());
    size_t const n  = tc.word_graph().out_degree();
    size_t       m  = n;
    for (letter_type a = 0; a < m;) {
      if (wg->neighbor_no_checks(0, a) != a + 1) {
        wg->remove_label(a);
        m--;
      } else {
        ++a;
      }
    }

    FroidurePin<TCE> result(wg);
    for (size_t i = 0; i < n; ++i) {
      // We use _word_graph.neighbor_no_checks instead of just i, because there
      // might be more generators than cosets.
      result.add_generator(TCE(tc.word_graph().neighbor_no_checks(0, i)));
    }
    return result;
  }

  using KBE = v3::detail::KBE;

  FroidurePin<KBE> to_froidure_pin(KnuthBendix& kb) {
    size_t const n = kb.presentation().alphabet().size();

    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION_V3("TODO");
    } else if (kb.kind() != congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION_V3(
          "the argument must be a two-sided congruence, found a {} congruence",
          kb.kind());
    }
    kb.run();

    FroidurePin<KBE> result(kb);
    for (size_t i = 0; i < n; ++i) {
      result.add_generator(KBE(kb, i));
    }
    return result;
  }

  std::unique_ptr<FroidurePinBase> to_froidure_pin(Congruence& cong) {
    cong.run();
    if (cong.has_knuth_bendix()) {
      auto fp = to_froidure_pin(*cong.knuth_bendix());
      return std::make_unique<decltype(fp)>(std::move(fp));
    } else if (cong.has_todd_coxeter()) {
      auto fp = to_froidure_pin(*cong.todd_coxeter());
      return std::make_unique<decltype(fp)>(std::move(fp));
    } else if (cong.has_kambites()) {
      auto fp = to_froidure_pin(*cong.kambites());
      return std::make_unique<decltype(fp)>(std::move(fp));
    }
    LIBSEMIGROUPS_EXCEPTION_V3("TODO");
  }

}  // namespace libsemigroups
