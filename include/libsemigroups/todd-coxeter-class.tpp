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

// This file contains out-of-line ToddCoxeter mem fn templates.

namespace libsemigroups {

  template <typename Word>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init() {
    ToddCoxeterBase::init();
    _generating_pairs.clear();
    _presentation.init();
    return *this;
  }

  template <typename Word>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind      knd,
                                             Presentation<Word>&& p) {
    // TODO(0) seems like _generating_pairs is not reset by this
    if constexpr (!std::is_same_v<Word, word_type>) {
      // to_presentation throws in the next line if p isn't valid
      auto copy = to_presentation<word_type>(p);
      init();
      ToddCoxeterBase::init(knd, std::move(copy));
      _presentation = std::move(p);
    } else {
      p.validate();
      init();
      _presentation = p;  // copy p in to _presentation
      presentation::normalize_alphabet(p);
      ToddCoxeterBase::init(knd, std::move(p));
    }
    return *this;
  }

  template <typename Word>
  template <typename Node>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind        knd,
                                             WordGraph<Node> const& wg) {
    ToddCoxeterBase::init(knd, wg);
    if constexpr (std::is_same_v<Word, word_type>) {
      _presentation = current_word_graph().presentation();
    } else {
      // TODO(0) implement to_presentation<word_type>(Presentation<word_type>)
      // which just returns the input unchanged, and remove this if-statement
      _presentation
          = to_presentation<Word>(current_word_graph().presentation());
    }
    _generating_pairs.clear();
    return *this;
  }

  template <typename Word>
  template <typename Node>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind           knd,
                                             Presentation<Word> const& p,
                                             WordGraph<Node> const&    wg) {
    if constexpr (!std::is_same_v<Word, word_type>) {
      // to_presentation throws in the next line if p isn't valid
      ToddCoxeterBase::init(knd, to_presentation<word_type>(p), wg);
      _presentation = p;
    } else {
      p.validate();
      _presentation = p;  // copy p in to _presentation
      auto copy     = p;
      presentation::normalize_alphabet(copy);
      ToddCoxeterBase::init(knd, copy, wg);
    }
    return *this;
  }

  template <typename Word>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  ToddCoxeter<Word>&
  ToddCoxeter<Word>::add_generating_pair_no_checks(Iterator1 first1,
                                                   Iterator2 last1,
                                                   Iterator3 first2,
                                                   Iterator4 last2) {
    // Add the input iterators to _generating_pairs
    _generating_pairs.emplace_back(first1, last1);
    _generating_pairs.emplace_back(first2, last2);
    ToddCoxeterBase::add_generating_pair_no_checks(make_citow(first1),
                                                   make_citow(last1),
                                                   make_citow(first2),
                                                   make_citow(last2));
    return *this;
  }

  template <typename Word>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  bool ToddCoxeter<Word>::contains(Iterator1 first1,
                                   Iterator2 last1,
                                   Iterator3 first2,
                                   Iterator4 last2) {
    // TODO(1) remove when is_free is implemented
    if (presentation().rules.empty() && generating_pairs().empty()
        && current_word_graph().number_of_nodes_active() == 1) {
      return std::equal(first1, last1, first2, last2);
    }
    // Call detail::CongruenceCommon version so that we perform bound checks in
    // ToddCoxeter and not ToddCoxeterBase
    return detail::CongruenceCommon::contains<ToddCoxeter>(
        first1, last1, first2, last2);
  }

  template <typename Word>
  std::string to_human_readable_repr(ToddCoxeter<Word> const& tc) {
    return fmt::format("<ToddCoxeter over {} with {}/{} active/nodes>",
                       to_human_readable_repr(tc.presentation()),
                       tc.current_word_graph().number_of_nodes_active(),
                       tc.current_word_graph().number_of_nodes());
  }

}  // namespace libsemigroups
