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
  KnuthBendix<Word, Rewriter, ReductionOrder>&
  KnuthBendix<Word, Rewriter, ReductionOrder>::init(congruence_kind      knd,
                                                    Presentation<Word>&& p) {
    if constexpr (!std::is_same_v<Word, std::string>) {
      // to_presentation throws in the next line if p isn't valid
      KnuthBendixBase_::init(
          knd, to_presentation<std::string>(p, [](auto x) { return x; }));
    } else {
      KnuthBendixBase_::init(knd, p);
    }
    _presentation = std::move(p);
    return *this;
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
    // *this and KnuthBendixBase_ may not be the same
    KnuthBendixBase_::add_generating_pair_no_checks(
        first1, last1, first2, last2);
    return *this;
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
    // KnuthBendix and not KnuthBendixBase_
    return detail::CongruenceCommon::contains<KnuthBendix>(
        first1, last1, first2, last2);
  }

}  // namespace libsemigroups
