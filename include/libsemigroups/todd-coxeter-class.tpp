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
  ToddCoxeter<Word>::~ToddCoxeter() = default;

  template <typename Word>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init() {
    ToddCoxeterImpl::init();
    _generating_pairs.clear();
    _presentation.init();
    return *this;
  }

  template <typename Word>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind      knd,
                                             Presentation<Word>&& p) {
    if constexpr (!std::is_same_v<Word, word_type>) {
      // to<Presentation> throws in the next line if p isn't valid
      ToddCoxeterImpl::init(knd, v4::to<Presentation<word_type>>(p));
      _presentation = std::move(p);
    } else {
      p.throw_if_bad_alphabet_or_rules();
      _presentation = p;  // copy p in to _presentation
      presentation::normalize_alphabet(p);
      ToddCoxeterImpl::init(knd, std::move(p));
    }
    _generating_pairs.clear();
    return *this;
  }

  template <typename Word>
  template <typename Node>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind        knd,
                                             WordGraph<Node> const& wg) {
    ToddCoxeterImpl::init(knd, wg);
    _presentation = v4::to<Presentation<Word>>(internal_presentation());
    _generating_pairs.clear();
    return *this;
  }

  template <typename Word>
  template <typename Node>
  ToddCoxeter<Word>& ToddCoxeter<Word>::init(congruence_kind           knd,
                                             Presentation<Word> const& p,
                                             WordGraph<Node> const&    wg) {
    if constexpr (!std::is_same_v<Word, word_type>) {
      // to<Presentation> throws in the next line if p isn't valid
      ToddCoxeterImpl::init(knd, v4::to<Presentation<word_type>>(p), wg);
      _presentation = p;
    } else {
      p.throw_if_bad_alphabet_or_rules();
      _presentation = p;  // copy p in to _presentation
      auto copy     = p;
      presentation::normalize_alphabet(copy);
      ToddCoxeterImpl::init(knd, copy, wg);
    }
    _generating_pairs.clear();
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
    ToddCoxeterImpl::add_generating_pair_no_checks(detail::citow(this, first1),
                                                   detail::citow(this, last1),
                                                   detail::citow(this, first2),
                                                   detail::citow(this, last2));
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
    // ToddCoxeter and not ToddCoxeterImpl
    return detail::CongruenceCommon::contains<ToddCoxeter>(
        first1, last1, first2, last2);
  }

  template <typename Word>
  std::string to_human_readable_repr(ToddCoxeter<Word> const& tc) {
    using detail::group_digits;
    return fmt::format(
        "<{} ToddCoxeter over {} with {} gen. pair{} + {} node{}>",
        tc.kind() == congruence_kind::twosided ? "2-sided" : "1-sided",
        to_human_readable_repr(tc.presentation()),
        group_digits(tc.number_of_generating_pairs()),
        tc.number_of_generating_pairs() == 1 ? "" : "s",
        group_digits(tc.current_word_graph().number_of_nodes_active()),
        tc.current_word_graph().number_of_nodes_active() == 1 ? "" : "s");
  }

}  // namespace libsemigroups
