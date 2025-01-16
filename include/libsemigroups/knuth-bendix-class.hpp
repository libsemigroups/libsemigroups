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

// This file contains a class KnuthBendix TODO

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_

#include "knuth-bendix-base.hpp"
#include "order.hpp"
#include "presentation.hpp"

#include "detail/rewriters.hpp"

namespace libsemigroups {

  template <typename Word,
            typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public KnuthBendixBase<Rewriter, ReductionOrder> {
   private:
    using KnuthBendixBase_ = KnuthBendixBase<Rewriter, ReductionOrder>;

    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

   public:
    using native_word_type = Word;

    KnuthBendix() = default;

    KnuthBendix& init() {
      _generating_pairs.clear();
      _presentation.init();
      KnuthBendixBase_::init();
    }

    KnuthBendix(KnuthBendix const&)            = default;
    KnuthBendix(KnuthBendix&&)                 = default;
    KnuthBendix& operator=(KnuthBendix const&) = default;
    KnuthBendix& operator=(KnuthBendix&&)      = default;

    KnuthBendix(congruence_kind knd, Presentation<Word>&& p) : KnuthBendix() {
      init(knd, std::move(p));
    }

    KnuthBendix& init(congruence_kind knd, Presentation<Word>&& p) {
      if constexpr (!std::is_same_v<Word, std::string>) {
        // to_presentation throws in the next line if p isn't valid
        KnuthBendixBase_::init(knd, to_presentation<std::string>(p));
      } else {
        // TODO(1) normalize? presentation::normalize_alphabet(p);
        KnuthBendixBase_::init(knd, p);
      }
      _presentation = std::move(p);
      return *this;
    }

    KnuthBendix(congruence_kind knd, Presentation<Word> const& p)
        // call the rval ref constructor
        : KnuthBendix(knd, Presentation<Word>(p)) {}

    KnuthBendix& init(congruence_kind knd, Presentation<Word> const& p) {
      // call the rval ref init
      return init(knd, Presentation<Word>(p));
    }

    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      presentation().validate_word(first, last);
    }

    [[nodiscard]] std::vector<Word> const& generating_pairs() const noexcept {
      return _generating_pairs;
    }

    [[nodiscard]] Presentation<Word> const& presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair_no_checks(Iterator1 first1,
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

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2) {
      // Call CongruenceInterface version so that we perform bound checks in
      // KnuthBendix and not KnuthBendixBase
      return CongruenceInterface::add_generating_pair<KnuthBendix>(
          first1, last1, first2, last2);
    }
  };  // class KnuthBendix

  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word> const&) -> KnuthBendix<Word>;

  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word>&&) -> KnuthBendix<Word>;

  namespace knuth_bendix {
    // This function exists so that we can do make<Rewriter>(knd, p)
    // and the type of p can be used to deduce the template parameter Word.
    // TODO(0) the template params should be in the same order
    template <typename Rewriter       = detail::RewriteTrie,
              typename ReductionOrder = ShortLexCompare,
              typename Word>
    KnuthBendix<Word, Rewriter, ReductionOrder>
    make(congruence_kind knd, Presentation<Word> const& p) {
      return KnuthBendix<Word, Rewriter, ReductionOrder>(knd, p);
    }

  }  // namespace knuth_bendix

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
