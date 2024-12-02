//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2023 James D. Mitchell + Maria Tsalakou
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

// This file contains the declaration of the Kambites class implementing the
// algorithm described in:
//
// Kambites, M. (2009). Small overlap monoids. I. The word problem. J. Algebra,
// 321(8), 2187–2205.
//
// for solving the word problem in small overlap monoids, and a novel algorithm
// for computing normal forms in small overlap monoids, by Maria Tsalakou.

#ifndef LIBSEMIGROUPS_KAMBITES_HPP_
#define LIBSEMIGROUPS_KAMBITES_HPP_

#include <algorithm>      // for min
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <functional>     // for equal_to
#include <string>         // for string
#include <tuple>          // for tuple
#include <type_traits>    // for is_base_of, is_trivial
#include <unordered_map>  // for unordered_map
#include <utility>        // for pair
#include <vector>         // for vector

#include "adapters.hpp"         // for One
#include "cong-intf.hpp"        // for CongruenceInterface
#include "constants.hpp"        // for UNDEFINED, POSITIVE_INFINITY
#include "debug.hpp"            // for LIBSEMIGROUPS_DEBUG
#include "exception.hpp"        // for LIBSEMIGROUPS_EXCEPTION
#include "order.hpp"            // for lexicographical_compare
#include "presentation.hpp"     // for Presentation
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type, tril, letter_type
#include "ukkonen.hpp"          // for Ukkonen

#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/string.hpp"             // for is_prefix
#include "detail/uf.hpp"                 // for Duf<>

namespace libsemigroups {

  //! \defgroup kambites_group Kambites
  //!
  //! TODO

  //! \ingroup kambites_group
  //!
  //! Defined in ``kambites.hpp``.
  //!
  //! On this page we describe the functionality relating to the algorithms
  //! for small overlap monoids by
  //! [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038) and the
  //! authors of ``libsemigroups``.
  //!
  //! This page describes the implementation in the class template Kambites.
  // TODO(later) example
  template <typename Word = detail::MultiStringView>
  class Kambites : public CongruenceInterface {
   public:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - aliases - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of strings used by a Kambites instance.
    // TODO(0) update doc
    // TODO(0) remove use native_word_type instead
    using value_type
        = std::conditional_t<std::is_same_v<Word, detail::MultiStringView>,
                             std::string,
                             Word>;

    using native_word_type   = value_type;
    using native_letter_type = typename native_word_type::value_type;

   private:
    using internal_type = Word;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - inner classes - private
    ////////////////////////////////////////////////////////////////////////

    // Data structure for caching the regularly accessed parts of the
    // relation words.
    struct RelationWords;

    // Data structure for caching the complements of each relation word.
    //
    // We say that a relation word u' is a *complement* of a relation word u
    // if there are relation words u = r_1,r_2, ..., r_n = u'$ such that
    // either (r_i,r_{i+1}) \in R or (r_{i+1},r_i) \in R for $1 \leq i \leq
    // n$. We say that $u'$ is a *proper complement* of the relation word u
    // if it is a complement of u and u is not equal to u'.
    class Complements;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - data members - private
    ////////////////////////////////////////////////////////////////////////

    mutable size_t                     _class;
    mutable Complements                _complements;
    mutable bool                       _have_class;
    mutable std::vector<RelationWords> _XYZ_data;
    mutable value_type                 _tmp_value1, _tmp_value2;

    Presentation<value_type> _presentation;
    Ukkonen                  _suffix_tree;

    using internal_type_iterator = typename internal_type::const_iterator;

    // TODO(0) to tpp
    void throw_if_1_sided(congruence_kind knd) {
      if (knd == congruence_kind::onesided) {
        LIBSEMIGROUPS_EXCEPTION("the 1st argument (congruence_kind) must be "
                                "twosided, but found onesided");
      }
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - Constructors, destructor, initialisation - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    Kambites();

    Kambites& init();

    //! Default copy constructor.
    Kambites(Kambites const&);

    //! Default move constructor.
    Kambites(Kambites&&) = default;

    //! Default copy assignment operator.
    Kambites& operator=(Kambites const&) = default;

    //! Default move assignment operator.
    Kambites& operator=(Kambites&&) = default;

    ~Kambites();

    // Although congruence_kind can only be twosided for Kambites objects, we
    // have the parameter for uniformity with KnuthBendix, ToddCoxeter, and
    // Congruence.
    // TODO(1) simplify constructors (like for KnuthBendix + ToddCoxeter)
    explicit Kambites(congruence_kind knd, Presentation<value_type> const& p)
        : Kambites() {
      throw_if_1_sided(knd);
      p.validate();
      _presentation = p;
      private_init_from_presentation(false);
    }

    Kambites& init(congruence_kind knd, Presentation<value_type> const& p) {
      throw_if_1_sided(knd);
      p.validate();
      _presentation = p;
      return private_init_from_presentation(true);
    }

    explicit Kambites(congruence_kind knd, Presentation<value_type>&& p)
        : Kambites() {
      throw_if_1_sided(knd);
      p.validate();
      _presentation = std::move(p);
      private_init_from_presentation(false);
    }

    Kambites& init(congruence_kind knd, Presentation<value_type>&& p) {
      throw_if_1_sided(knd);
      p.validate();
      _presentation = std::move(p);
      return private_init_from_presentation(true);
    }

    // No rvalue ref version of this because we can't use the presentation.
    template <typename OtherWord>
    explicit Kambites(congruence_kind ck, Presentation<OtherWord> const& p)
        : Kambites(ck,
                   // The lambda in the next line converts, say, chars to
                   // size_ts, but doesn't convert size_ts to human_readable
                   // characters.
                   to_presentation<value_type>(p, [](auto x) { return x; })) {
      throw_if_1_sided(ck);
    }

    template <typename OtherWord>
    Kambites& init(congruence_kind ck, Presentation<OtherWord> const& p) {
      throw_if_1_sided(ck);
      // The lambda in the next line converts, say, chars to size_ts, but
      // doesn't convert size_ts to human_readable characters.
      init(ck, to_presentation<value_type>(p, [](auto x) { return x; }));
      return *this;
    }

    [[nodiscard]] Presentation<value_type> const&
    presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - add_pairs
    ////////////////////////////////////////////////////////////////////////

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Kambites& add_generating_pair_no_checks(Iterator1 first1,
                                            Iterator2 last1,
                                            Iterator3 first2,
                                            Iterator4 last2) {
      return CongruenceInterface::add_generating_pair_no_checks<Kambites>(
          first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    Kambites& add_generating_pair(Iterator1 first1,
                                  Iterator2 last1,
                                  Iterator3 first2,
                                  Iterator4 last2) {
      return CongruenceInterface::add_generating_pair<Kambites>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, throws
    [[nodiscard]] uint64_t number_of_classes() {
      validate_small_overlap_class();
      return POSITIVE_INFINITY;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

   private:
    // The Kambites class requires that input to contains to be actual objects
    // not iterators. This is different from KnuthBendix and ToddCoxeter.
    // One way to resolve this more satisfactorily would be to implement
    // MultiStringView for non-strings, so that we can just construct a
    // light-weight view and bung that in here instead.
    [[nodiscard]] bool contains_no_checks(value_type const& u,
                                          value_type const& v);

    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    template <typename SFINAE = bool>
    [[nodiscard]] auto contains_no_checks(word_type const& u,
                                          word_type const& v)
        -> std::enable_if_t<!std::is_same_v<value_type, word_type>, SFINAE>;

   public:
    // TODO(0) to tpp
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                    Iterator2 last1,
                                                    Iterator3 first2,
                                                    Iterator4 last2) const {
      if (finished()) {
        return const_cast<Kambites*>(this)->contains_no_checks(
                   first1, last1, first2, last2)
                   ? tril::TRUE
                   : tril::FALSE;
      }
      return std::equal(first1, last1, first2, last2) ? tril::TRUE
                                                      : tril::FALSE;
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2) const {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);

      return currently_contains_no_checks(first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2);

    // TODO(0) to tpp
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2) {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      validate_small_overlap_class();
      return contains_no_checks(first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

   private:
    void normal_form_no_checks(value_type& result, value_type const& w) const;

    template <typename SFINAE = word_type>
    auto normal_form_no_checks(value_type& result, word_type const& w)
        -> std::enable_if_t<!std::is_same_v<value_type, word_type>,
                            SFINAE> const;

   public:
    // TODO(0) to tpp file
    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           Iterator1      first,
                                           Iterator2      last) const {
      if (finished()) {
        _tmp_value2.clear();
        _tmp_value1.assign(first, last);
        normal_form_no_checks(_tmp_value2, _tmp_value1);
        return std::copy(
            std::begin(_tmp_value2), std::end(_tmp_value2), d_first);
      }

      // Does nothing in this case
      return std::copy(first, last, d_first);
    }

    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 Iterator1      first,
                                 Iterator2      last) const {
      throw_if_letter_out_of_bounds(first, last);
      return reduce_no_run_no_checks(d_first, first, last);
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      run();
      return reduce_no_run_no_checks(d_first, first, last);
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      throw_if_letter_out_of_bounds(first, last);
      validate_small_overlap_class();
      return reduce_no_checks(d_first, first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! Get the small overlap class of the finitely presented semigroup
    //! represented by \c this.
    //!
    //! If \f$S\f$ is a finitely presented semigroup with generating set
    //! \f$A\f$, then a word \f$w\f$ over \f$A\f$ is a *piece* if \f$w\f$
    //! occurs as a factor in at least two of the relations defining \f$S\f$
    //! or if it occurs as a factor of one relation in two different
    //! positions (possibly overlapping).
    //!
    //! A finitely presented semigroup \f$S\f$ satisfies the condition
    //! \f$C(n)\f$, for a positive integer \f$n\f$ if the minimum number of
    //! pieces in any factorisation of a word occurring as the left or right
    //! hand side of a relation of \f$S\f$ is at least \f$n\f$.
    //!
    //! \returns
    //! The greatest positive integer \f$n\f$ such that the finitely
    //! semigroup represented by \c this satisfies the condition \f$C(n)\f$;
    //! or \ref POSITIVE_INFINITY if no word occurring in a
    //! relation can be written as a product of pieces.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! The current implementation has complexity no worse than \f$O(m ^
    //! 3)\f$ where \f$m\f$ is the sum of the lengths of the words occurring
    //! in the relations of the semigroup.
    //!
    //! \warning
    //! The member functions \ref equal_to and \ref normal_form only work if
    //! the return value of this function is at least \f$4\f$.
    // not noexcept because number_of_pieces_no_checks isn't
    [[nodiscard]] size_t small_overlap_class();

    //! Returns the Ukkonen suffix tree object used to compute pieces.
    //!
    //! This function returns a reference to the Ukkonen generalised suffix
    //! tree object containing the relation words of a Kambites object, that
    //! is used to determine the pieces, and decompositions of the relation
    //! words.
    //!
    //! \returns A const reference to a \ref Ukkonen object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto const& ukkonen() noexcept {
      run();
      return _suffix_tree;
    }

    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      _presentation.validate_word(first, last);
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - init functions - private
    ////////////////////////////////////////////////////////////////////////

    Kambites& private_init_from_presentation(bool call_init);

    ////////////////////////////////////////////////////////////////////////
    // Kambites - validation functions - private
    ////////////////////////////////////////////////////////////////////////

    // Throws exception if the small_overlap_class is < 4.
    //
    // Not noexcept, throws
    void validate_small_overlap_class();

    ////////////////////////////////////////////////////////////////////////
    // Kambites - XYZ functions - private
    ////////////////////////////////////////////////////////////////////////

    void really_init_XYZ_data(size_t i) const;

    // init_XYZ_data is split into the function that really does the work
    // (really_init_XYZ_data) which is called once, and init_XYZ_data which
    // can be called very often.
    void init_XYZ_data(size_t i) const {
      LIBSEMIGROUPS_ASSERT(i < _presentation.rules.size());
      if (_XYZ_data.empty()) {
        _XYZ_data.resize(_presentation.rules.size());
      }
      if (!_XYZ_data[i].is_initialized) {
        really_init_XYZ_data(i);
      }
    }

    [[nodiscard]] internal_type const& X(size_t i) const;
    [[nodiscard]] internal_type const& Y(size_t i) const;
    [[nodiscard]] internal_type const& Z(size_t i) const;
    [[nodiscard]] internal_type const& XY(size_t i) const;
    [[nodiscard]] internal_type const& YZ(size_t i) const;
    [[nodiscard]] internal_type const& XYZ(size_t i) const;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - helpers - private
    ////////////////////////////////////////////////////////////////////////

    // Returns the index of the relation word r_i = X_iY_iZ_i if [first,
    // last) = X_iY_iw for some w. If no such exists, then UNDEFINED is
    // returned.
    // Not noexcept because is_prefix isn'Word
    [[nodiscard]] size_t
    relation_prefix(internal_type_iterator const& first,
                    internal_type_iterator const& last) const;

    // Returns the index of the relation word r_i = X_iY_iZ_i such that
    // X_iY_i is a clean overlap prefix of <s>, i.e. <s> = X_iY_iw for some
    // w, and there's no factor of <s> of the form X_jY_j starting before the
    // beginning of Y_i. If no such exists, UNDEFINED is returned.
    // Not noexcept because relation_prefix isn'Word
    [[nodiscard]] inline size_t
    clean_overlap_prefix(internal_type const& s) const {
      return clean_overlap_prefix(s.cbegin(), s.cend());
    }

    [[nodiscard]] size_t
    clean_overlap_prefix(internal_type_iterator const& first,
                         internal_type_iterator const& last) const;

    // Calls clean_overlap_prefix on every suffix of <s> starting within the
    // range [0, n), and returns a std::pair p where:
    //
    // * <p.first> is the starting index of the suffix of <s> that contains a
    //   clean_overlap_prefix.
    //
    // * <p.second> is the index of the relation word that's a clear overlap
    //   prefix of a suffix of <s>
    [[nodiscard]] std::pair<size_t, size_t>
    clean_overlap_prefix_mod(internal_type const& s, size_t n) const;

    // If x + [first, last) = aX_sY_sw words a, w and for some index s, where
    // X_s = X_s'X_s'', x = aX_s', and [first, last) = X_s''Y_sw, then this
    // function returns a tuple <Word> where:
    //
    // 1. std::get<0>(Word) is the index <s>
    //
    // 2. std::get<1>(Word) is an iterator <it1> into <x>, such that
    //    [it1, x.cend()) = X_s'
    //
    // 3. std::get<2>(Word) is an iterator <it2> into [first, last), such that
    //    [it2, last) = w
    //
    // If no such relation word exists, then
    //
    //    (UNDEFINED, first.cend(), second.cend())
    //
    // is returned.
    //
    // Not noexcept because relation_prefix isn'Word
    [[nodiscard]] std::
        tuple<size_t, internal_type_iterator, internal_type_iterator>
        p_active(internal_type const&          x,
                 internal_type_iterator const& first,
                 internal_type_iterator const& last) const;

    // Returns a word equal to w in this, starting with the piece p, no checks
    // are performed. Used in the normal_form function.
    // Not noexcept because detail::is_prefix isn'Word
    void replace_prefix(internal_type& w, internal_type const& p) const;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - complement helpers - private
    ////////////////////////////////////////////////////////////////////////

    // Returns j among the complements of i such that [first,
    // last) is a prefix of X_jY_jZ_j, and UNDEFINED otherwise.
    [[nodiscard]] size_t
    prefix_of_complement(size_t                        i,
                         internal_type_iterator const& first,
                         internal_type_iterator const& last) const;

    // Helper for the above
    [[nodiscard]] size_t prefix_of_complement(size_t               i,
                                              internal_type const& w) const {
      return prefix_of_complement(i, w.cbegin(), w.cend());
    }

    // Returns the index j of a complement of X_iY_iZ_i such that X_jY_j is a
    // prefix of w. Otherwise, UNDEFINED is returned.
    [[nodiscard]] size_t complementary_XY_prefix(size_t               i,
                                                 internal_type const& w) const;

    // Returns j such that w is Z_j-active for some Z_j in the
    // complements of Z_i.  Otherwise it returns UNDEFINED.
    //
    // See also: p_active.
    [[nodiscard]] size_t Z_active_complement(size_t               i,
                                             internal_type const& w) const;

    // Returns j such that w is Z_j-active for some Z_j in the
    // proper (j != i) complements of Z_i.  Otherwise it returns UNDEFINED.
    [[nodiscard]] size_t
    Z_active_proper_complement(size_t i, internal_type const& w) const;

    [[nodiscard]] size_t
    Z_active_proper_complement(size_t                        i,
                               internal_type_iterator const& first,
                               internal_type_iterator const& last) const;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - static functions - private
    ////////////////////////////////////////////////////////////////////////

    [[nodiscard]] static size_t complementary_relation_word(size_t i) {
      return (i % 2 == 0 ? i + 1 : i - 1);
    }

    template <typename value_type>
    static void pop_front(value_type& x) {
      x.erase(x.begin());
    }

    static void pop_front(detail::MultiStringView& x) {
      x.pop_front();
    }

    template <typename Iterator>
    static void append(std::string& w, Iterator first, Iterator last) {
      w.append(first, last);
    }

    template <typename Iterator>
    static void append(detail::MultiStringView& w,
                       Iterator                 first,
                       Iterator                 last) {
      w.append(first, last);
    }

    template <typename Iterator>
    static void append(word_type& w, Iterator first, Iterator last) {
      w.insert(w.end(), first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - main functions - private
    ////////////////////////////////////////////////////////////////////////

    // copies u, v, and/or p if they are an rrvalue ref or a const ref.

    // Implementation of the function of the same name in: Kambites, M.
    // (2009). Small overlap monoids. I. The word problem. J. Algebra,
    // 321(8), 2187–2205.
    //
    // Returns true if u and v represent the same element of the fp semigroup
    // represented by this, and p is a possible prefix of u, and v.
    //
    // Parameters are given by value because they are modified by wp_prefix,
    // and it was too difficult to untangle the different cases (when u, v
    // are equal, not equal, references, rvalue references etc). It's
    // possible that it could be modified to only copy when necessary, but
    // this doesn'Word seem worth it at present.
    [[nodiscard]] bool wp_prefix(internal_type u,
                                 internal_type v,
                                 internal_type p) const;

    // Implementational detail
    // Not noexcept because nothing else is and lots allocations
    void normal_form_inner(size_t& r, internal_type& v, internal_type& w) const;

    ////////////////////////////////////////////////////////////////////////
    // Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void run_impl() override;

    bool finished_impl() const override {
      return _have_class && _class >= 4;
    }
  };

  //! TODO(0) doc
  template <typename Word>
  Kambites(congruence_kind, Presentation<Word> const&) -> Kambites<Word>;

  //! \ingroup kambites_group
  //!
  //! TODO
  namespace kambites {
    using congruence_interface::add_generating_pair;
    using congruence_interface::add_generating_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::contains;
    using congruence_interface::contains_no_checks;
    using congruence_interface::currently_contains;
    using congruence_interface::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::reduce;
    using congruence_interface::reduce_no_checks;
    using congruence_interface::reduce_no_run;
    using congruence_interface::reduce_no_run_no_checks;

  }  // namespace kambites
}  // namespace libsemigroups

#include "detail/kambites-normal-form-range.hpp"  // for KambitesNormalFormRange

namespace libsemigroups {
  namespace kambites {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    // short_lex normal forms, could be lex normal forms too
    template <typename Word>
    auto normal_forms(Kambites<Word>& k) {
      return detail::KambitesNormalFormRange(k);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - partition
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::partition;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::non_trivial_classes;

    // There's no non_trivial_classes(Kambites k1, Kambites k2) because it's
    // unclear how this could be computed (because they always define infinite
    // semigroups/monoids), so we can't just do non_trivial_classes(k1,
    // kambites::normal_forms(k2)) (as in ToddCoxeter) because there are
    // infinitely many normal_forms.
  }  // namespace kambites

}  // namespace libsemigroups

#include "kambites.tpp"

#endif  // LIBSEMIGROUPS_KAMBITES_HPP_
