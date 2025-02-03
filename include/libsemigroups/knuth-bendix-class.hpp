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

// This file contains a class KnuthBendix TODO(0)
// TODO(0) doc
// TODO(0) iwyu
// TODO(0) active rules, also anything else that returns anything related to
// the word type.

#ifndef LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
#define LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_

#include "order.hpp"
#include "presentation.hpp"

#include "detail/knuth-bendix-base.hpp"
#include "detail/rewriters.hpp"

namespace libsemigroups {

  //! \defgroup knuth_bendix_group Knuth-Bendix
  //!
  //! This page contains links to the documentation related to the
  //! implementation of the Knuth-Bendix algorithm in `libsemigroups`.

  //! \ingroup knuth_bendix_group
  //!
  //! \brief Class template containing an implementation of the Knuth-Bendix
  //! Algorithm.
  //!
  //! Defined in \c knuth-bendix.hpp.
  //!
  //! On this page we describe the functionality relating to the Knuth-Bendix
  //! algorithm for semigroups and monoids in \c libsemigroups. This page
  //! contains details of the member functions of the class KnuthBendix.
  //!
  //! This class is used to represent a [string rewriting
  //! system](https://w.wiki/9Re) defining a 1- or 2-sided congruence on a
  //! finitely presented monoid or semigroup.
  //!
  //! \tparam Word TODO
  //! \tparam Rewriter TODO
  //! \tparam ReductionOrder TODO
  //!
  //! \par Example
  //! \code
  //! Presentation<std::string> p;
  //! p.contains_empty_word(true);
  //! p.alphabet("abcd");
  //! presentation::add_rule_no_checks(p, "ab", "");
  //! presentation::add_rule_no_checks(p, "ba", "");
  //! presentation::add_rule_no_checks(p, "cd", "");
  //! presentation::add_rule_no_checks(p, "dc", "");
  //! presentation::add_rule_no_checks(p, "ca", "ac");
  //!
  //! KnuthBendix kb(twosided, p);
  //!
  //! !kb.confluent();              // true
  //! kb.run();
  //! kb.number_of_active_rules();  // 8
  //! kb.confluent();               // true
  //! kb.number_of_classes();       // POSITIVE_INFINITY
  //! \endcode
  template <typename Word,
            typename Rewriter       = detail::RewriteTrie,
            typename ReductionOrder = ShortLexCompare>
  class KnuthBendix : public detail::KnuthBendixBase<Rewriter, ReductionOrder> {
   private:
    using KnuthBendixBase_ = detail::KnuthBendixBase<Rewriter, ReductionOrder>;

    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

   public:
    using native_word_type = Word;
    // TODO(0) rm native_letter_type/presentation_type??

    KnuthBendix() = default;

    KnuthBendix& init() {
      _generating_pairs.clear();
      _presentation.init();
      KnuthBendixBase_::init();
      return *this;
    }

    KnuthBendix(KnuthBendix const&)            = default;
    KnuthBendix(KnuthBendix&&)                 = default;
    KnuthBendix& operator=(KnuthBendix const&) = default;
    KnuthBendix& operator=(KnuthBendix&&)      = default;

    KnuthBendix(congruence_kind knd, Presentation<Word>&& p) : KnuthBendix() {
      init(knd, std::move(p));
    }

    KnuthBendix& init(congruence_kind knd, Presentation<Word>&& p);

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
                                               Iterator4 last2);

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    KnuthBendix& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase
      return detail::CongruenceCommon::add_generating_pair<KnuthBendix>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    // The detail::CongruenceCommon::currently_contains_no_checks and
    // contains_no_checks, are used directly, since the letters pointed at by
    // iterators in the presentation() and internal_presentation() are currently
    // the same.

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to be
    //! contained in the congruence represented by a \ref
    //! todd_coxeter_class_group "KnuthBendixBase_" instance. This function
    //! performs no enumeration, so it is possible for the words to be contained
    //! in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase_
      return detail::CongruenceCommon::currently_contains<KnuthBendix>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref todd_coxeter_class_group
    //! "KnuthBendixBase_" instance. This function triggers a full enumeration,
    //! which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // KnuthBendix - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    // The functions detail::CongruenceCommon::reduce_*_no_checks are used
    // directly, since the letters pointed at by iterators in the presentation()
    // and internal_presentation() are currently the same.

    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator \p
    //! d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence defined
    //! by a \ref todd_coxeter_class_group "KnuthBendixBase_" instance. If the
    //!  \ref todd_coxeter_class_group "KnuthBendixBase_" instance is \ref
    //!  finished, then the output word is a normal
    //! form for the input word. If the  \ref todd_coxeter_class_group
    //! "KnuthBendixBase_" instance is not \ref finished, then it might be that
    //! equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      return detail::CongruenceCommon::reduce_no_run<KnuthBendix>(
          d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by a
    //! \ref todd_coxeter_class_group "KnuthBendixBase_" instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_warn_undecidable{Knuth-Bendix}
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in KnuthBendix and not KnuthBendixBase_
      return detail::CongruenceCommon::reduce<KnuthBendix>(
          d_first, first, last);
    }

    // Include the following to pull the doc into KnuthBendix
    using KnuthBendixBase_::active_rules;
    using KnuthBendixBase_::number_of_active_rules;
    using options = typename KnuthBendixBase_::options;

  };  // class KnuthBendix

  // TODO(0) doc
  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word> const&) -> KnuthBendix<Word>;

  // TODO(0) doc
  template <typename Word>
  KnuthBendix(congruence_kind, Presentation<Word>&&) -> KnuthBendix<Word>;

  // TODO(0) doc
  template <typename Word>
  KnuthBendix(KnuthBendix<Word> const&) -> KnuthBendix<Word>;

  // TODO(0) doc
  template <typename Word>
  KnuthBendix(KnuthBendix<Word>&&) -> KnuthBendix<Word>;

}  // namespace libsemigroups

#include "knuth-bendix-class.tpp"
#endif  // LIBSEMIGROUPS_KNUTH_BENDIX_CLASS_HPP_
