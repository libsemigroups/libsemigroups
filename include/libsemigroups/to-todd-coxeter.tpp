//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

namespace libsemigroups {
  template <typename Result, typename Node>
  auto to(congruence_kind knd, FroidurePinBase& fpb, WordGraph<Node> const& wg)
      -> std::enable_if_t<
          std::is_same_v<ToddCoxeter<typename Result::native_word_type>,
                         Result>,
          Result> {
    using node_type =
        typename detail::ToddCoxeterImpl::word_graph_type::node_type;
    using label_type =
        typename detail::ToddCoxeterImpl::word_graph_type::label_type;
    using Word = typename Result::native_word_type;

    if (&wg != &fpb.left_cayley_graph() && &wg != &fpb.right_cayley_graph()) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 3rd argument (WordGraph) to be the left_cayley_graph "
          "or right_cayley_graph of the 2nd argument (FroidurePin)!")
    }

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
    return ToddCoxeter<Word>(knd, copy);
  }

  template <template <typename...> typename Thing,
            typename Word,
            typename Rewriter,
            typename ReductionOrder>
  auto to(congruence_kind knd, KnuthBendix<Word, Rewriter, ReductionOrder>& kb)
      -> std::enable_if_t<std::is_same_v<ToddCoxeter<Word>, Thing<Word>>,
                          ToddCoxeter<Word>> {
    if (kb.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot construct a ToddCoxeter instance using the Cayley graph "
          "of an infinite KnuthBendix object, maybe try ToddCoxeter({}, "
          "kb.presentation()) instead?",
          kb.kind());
    }
    auto fp = to<FroidurePin>(kb);  // throws if kb.kind() != twosided
    return to<ToddCoxeter<Word>>(knd, fp, fp.right_cayley_graph());
  }

}  // namespace libsemigroups
