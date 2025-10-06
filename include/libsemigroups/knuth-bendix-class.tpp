//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell + Joseph Edwards
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
  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>::KnuthBendix() = default;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>::KnuthBendix(
      KnuthBendix const& that)
      = default;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>::KnuthBendix(KnuthBendix&&)
      = default;
  template <typename Word, typename Rewriter, typename ReductionOrder>

  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::operator=(KnuthBendix const&)
      = default;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::operator=(KnuthBendix&&)
      = default;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>::~KnuthBendix() = default;

  template <typename Word, typename Rewriter, typename ReductionOrder>
  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::init(congruence_kind      knd,
                                                    Presentation<Word>&& p) {
    using string_type = typename KnuthBendixImpl_::native_word_type;

    if constexpr (std::is_signed_v<char>) {
      // if std::is_signed_v<char>, and p.alphabet().size() > 128, then in the
      // next lines if <x> is the 129th letter in the alphabet, then
      // p.index_no_checks(x) == size_type 128, and (int)(char)128 == -128, so
      // the alphabet of the return presentation will be of the form [0, ...
      // , 127, -128 ...], which is never normalised, and so can't be used in
      // KnuthBendix.
      //
      // This could be fixed by using [-128, ..., -128 + size of alphabet]
      // always inside KnuthBendixImpl (instead of [0, ..., size of alphabet] as
      // we currently do) and then instead of using a letter "a" in the alphabet
      // when accessing (for example) the trie, we use "a + 128" so that the
      // values are always in [0, ..., size of alphabet].
      if (p.alphabet().size() > 128) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected the 2nd argument (presentation) to have alphabet of size "
            "at most 128, but found {}",
            p.alphabet().size());
      }
    }
    KnuthBendixImpl_::init(knd,
                           v4::to<Presentation<string_type>>(p, [&p](auto x) {
                             return p.index_no_checks(x);
                           }));
    _extra_letter_added = false;
    _generating_pairs.clear();
    _presentation = std::move(p);

    return *this;
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  template <typename Iterator1, typename Iterator2>
  void
  KnuthBendix<Word, Rewriter, ReductionOrder>::throw_if_letter_not_in_alphabet(
      Iterator1 first,
      Iterator2 last) const {
    presentation().throw_if_letter_not_in_alphabet(first, last);
    if (_extra_letter_added) {
      // It is necessary to represent the "extra" letter in the alphabet here
      // because o/w the output of (for example) active_rules inexplicably
      // includes an extra letter.
      auto const& alpha = presentation().alphabet();
      auto        it    = std::find_if(first, last, [&alpha](auto val) {
        return static_cast<typename native_word_type::value_type>(val)
               == alpha.back();
      });
      if (it != last) {
        LIBSEMIGROUPS_ASSERT(!alpha.empty());
        std::decay_t<decltype(alpha)> alpha_to_print(alpha.begin(),
                                                     alpha.end() - 1);

        LIBSEMIGROUPS_EXCEPTION(
            "invalid letter {}, valid letters are {} (for 1-sided "
            "congruences KnuthBendix requires an additional letter in "
            "the alphabet of its presentation, in this case {}, which "
            "belongs to presentation().alphabet() but cannot belong to the "
            "input for any of its member functions)",
            detail::to_printable(*it),
            detail::to_printable(alpha_to_print),
            detail::to_printable(alpha.back()));
      }
    }
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::add_generating_pair_no_checks(
      Iterator1 first1,
      Iterator2 last1,
      Iterator3 first2,
      Iterator4 last2) {
    // Add the input iterators to _generating_pairs
    _generating_pairs.emplace_back(first1, last1);
    _generating_pairs.emplace_back(first2, last2);
    // Although this looks like it does nothing, remember the type of words in
    // *this and KnuthBendixImpl_ may not be the same
    KnuthBendixImpl_::add_generating_pair_no_checks(detail::citow(this, first1),
                                                    detail::citow(this, last1),
                                                    detail::citow(this, first2),
                                                    detail::citow(this, last2));
    return *this;
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::add_generating_pair(
      Iterator1 first1,
      Iterator2 last1,
      Iterator3 first2,
      Iterator4 last2) {
    if (detail::CongruenceCommon::kind() == congruence_kind::onesided) {
      size_t max_alphabet
          = std::numeric_limits<std::string::value_type>::max()
            - std::numeric_limits<std::string::value_type>::min() + 1;
      if (_presentation.alphabet().size() == max_alphabet) {
        LIBSEMIGROUPS_EXCEPTION(
            "it is not possible to add generating pairs to a 1-sided "
            "KnuthBendix instance over a presentation with {} generators, "
            "(because an additional letter is required in the alphabet that "
            "cannot belong to any generating pair)",
            max_alphabet);
      }
    }
    // Call detail::CongruenceCommon version so that we perform bound checks
    // in KnuthBendix and not KnuthBendixImpl
    return detail::CongruenceCommon::add_generating_pair<KnuthBendix>(
        first1, last1, first2, last2);
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  bool KnuthBendix<Word, Rewriter, ReductionOrder>::contains(Iterator1 first1,
                                                             Iterator2 last1,
                                                             Iterator3 first2,
                                                             Iterator4 last2) {
    // TODO(1) remove when is_free is implemented
    if (presentation().rules.empty() && generating_pairs().empty()) {
      return std::equal(first1, last1, first2, last2);
    }
    // Call CongruenceCommon version so that we perform bound checks in
    // KnuthBendix and not KnuthBendixImpl_
    return detail::CongruenceCommon::contains<KnuthBendix>(
        first1, last1, first2, last2);
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  auto KnuthBendix<Word, Rewriter, ReductionOrder>::active_rules() {
    // TODO(1) remove allocations here somehow (probably by making a custom
    // range object holding memory to put the incoming rules into)
    auto        result = KnuthBendixImpl_::active_rules();
    auto const& p      = presentation();
    return result | rx::transform([&p](auto const* rule) {
             auto pair
                 = std::make_pair(Word(rule->lhs().begin(), rule->lhs().end()),
                                  Word(rule->rhs().begin(), rule->rhs().end()));
             auto convert = [&p](auto& val) { val = p.letter_no_checks(val); };
             std::for_each(pair.first.begin(), pair.first.end(), convert);
             std::for_each(pair.second.begin(), pair.second.end(), convert);
             return pair;
           });
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  std::vector<Word>
  KnuthBendix<Word, Rewriter, ReductionOrder>::gilman_graph_node_labels() {
    auto base_result = KnuthBendixImpl_::gilman_graph_node_labels();
    std::vector<Word> result;
    auto const&       p = presentation();
    for (auto& label : base_result) {
      result.emplace_back(label.begin(), label.end());
      std::for_each(result.back().begin(),
                    result.back().end(),
                    [&p](auto& val) { val = p.letter_no_checks(val); });
    }
    return result;
  }

  template <typename Word, typename Rewriter, typename ReductionOrder>
  void KnuthBendix<Word, Rewriter, ReductionOrder>::run_impl() {
    if (requires_extra_letter() && !_extra_letter_added) {
      // It is necessary to represent the "extra" letter in the alphabet here
      // because o/w the output of (for example) active_rules inexplicably
      // includes an extra letter.
      _presentation.add_generator();
      _extra_letter_added = true;
    }
    KnuthBendixImpl_::run_impl();
  }

}  // namespace libsemigroups
