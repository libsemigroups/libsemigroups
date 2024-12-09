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

#include "detail/formatters.hpp"         // for formatting POSITIVE_INFINITY
#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/string.hpp"             // for is_prefix
#include "detail/uf.hpp"                 // for Duf<>

namespace libsemigroups {

  //! \defgroup kambites_group Kambites
  //!
  //! Defined in ``kambites.hpp``.
  //!
  //! On this page there are links to the documentation for the algorithms in
  //! ``libsemigroups`` for small overlap monoids by Mark %Kambites and the
  //! authors of ``libsemigroups``;
  //! see \cite Kambites2009aa, \cite Kambites2009ab, and \cite Mitchell2021aa.

  //! \ingroup kambites_group
  //!
  //! Defined in ``kambites.hpp``.
  //!
  //! \brief Class template implementing small overlap class, equality, and
  //! normal forms for small overlap monoids.
  //!
  //! This page describes the class template Kambites for determining the
  //! small overlap class of a presentation, and, for small overlap monoids
  //! (those with small overlap class 4 or higher) checking equality of words
  //! and for computing normal forms.
  //!
  //! Note that a Kambites instance represents a congruence on the free monoid
  //! or semigroup containing the rules of a presentation used to construct the
  //! instance, and the \ref generating_pairs. As such generating pairs or rules
  //! are interchangeable in the context of Kambites objects.
  //!
  //! \tparam Word the type of the words in the presentation.
  // TODO(1) example
  // TODO(0) it seems a bit weird that the internal_type is the thing used as a
  // template parameter here. Discuss with Joe.
  template <typename Word = detail::MultiStringView>
  class Kambites : public CongruenceInterface {
   public:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - aliases - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Type of the words in the relations of the presentation stored in
    //! a Kambites instance.
    //!
    //! A Kambites instance can be constructed or initialised from a
    //! presentation with arbitrary types of letters and words. Internally the
    //! words are converted to \ref native_word_type.
    using native_word_type
        = std::conditional_t<std::is_same_v<Word, detail::MultiStringView>,
                             std::string,
                             Word>;

    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref Kambites instance.
    //!
    //! A \ref Kambites instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! letters are converted to \ref native_letter_type.
    using native_letter_type = typename native_word_type::value_type;

    //! \brief Type of the presentation stored in a \ref Kambites instance.
    //!
    //! A \ref Kambites instance can be constructed or initialised from a
    //! presentation of arbitrary types of letters and words. Internally the
    //! presentation is stored as a \ref native_presentation_type.
    using native_presentation_type = Presentation<native_word_type>;

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
    mutable native_word_type           _tmp_value1, _tmp_value2;

    Presentation<native_word_type> _presentation;
    Ukkonen                        _suffix_tree;

    using internal_type_iterator = typename internal_type::const_iterator;

    void throw_if_1_sided(congruence_kind knd) const;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - Constructors, destructor, initialisation - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref
    //! Kambites instance.
    Kambites();

    //! \brief Re-initialize a \ref Kambites instance.
    //!
    //! This function puts a \ref Kambites instance back into the state that it
    //! would have been in if it had just been newly default constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Kambites& init();

    //! Default copy constructor.
    Kambites(Kambites const&);

    //! Default move constructor.
    Kambites(Kambites&&);

    //! Default copy assignment operator.
    Kambites& operator=(Kambites const&);

    //! Default move assignment operator.
    Kambites& operator=(Kambites&&);

    ~Kambites();

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a  \ref Kambites instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! Kambites instances can only be used to compute two-sided congruences,
    //! and so the first parameter \p knd must always be
    //! congruence_kind::twosided. The parameter \p knd is included for
    //! uniformity of interface between with \ref KnuthBendix, \ref
    //! Kambites, and \ref Congruence.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p knd is not \ref
    //! congruence_kind::twosided.
    // TODO(1) simplify constructors (like for KnuthBendix + ToddCoxeter)
    Kambites(congruence_kind knd, Presentation<native_word_type> const& p);

    //! \brief Re-initialize a \ref Kambites instance.
    //!
    //! This function puts a \ref Kambites instance back into the state that
    //! it would have been in if it had just been newly constructed from \p knd
    //! and \p p.
    //!
    //! Kambites instances can only be used to compute two-sided congruences,
    //! and so the first parameter \p knd must always be
    //! congruence_kind::twosided. The parameter \p knd is included for
    //! uniformity of interface between with \ref KnuthBendix, \ref
    //! Kambites, and \ref Congruence.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p knd is not \ref
    //! congruence_kind::twosided.
    Kambites& init(congruence_kind                       knd,
                   Presentation<native_word_type> const& p);

    //! \copydoc Kambites(congruence_kind, Presentation<native_word_type>
    //! const&)
    Kambites(congruence_kind knd, Presentation<native_word_type>&& p);

    //! \copydoc init(congruence_kind, Presentation<native_word_type>
    //! const&)
    Kambites& init(congruence_kind knd, Presentation<native_word_type>&& p);

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a  \ref Kambites instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by
    //! the presentation \p p. The type of the words in \p p can be
    //! anything, but will be converted in to \ref native_word_type. This
    //! means that if the input presentation uses std::string, for example,
    //! as the word type, then this presentation is converted into a \ref
    //! native_presentation_type. This converted presentation can be
    //! recovered using \ref presentation.
    //!
    //! \tparam OtherWord the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p knd is not \ref
    //! congruence_kind::twosided.
    // No rvalue ref version of this because we can't use the presentation.
    template <typename OtherWord>
    Kambites(congruence_kind knd, Presentation<OtherWord> const& p)
        : Kambites(
            knd,
            // The lambda in the next line converts, say, chars to
            // size_ts, but doesn't convert size_ts to human_readable
            // characters.
            to_presentation<native_word_type>(p, [](auto x) { return x; })) {
      throw_if_1_sided(knd);
    }

    //! \brief Re-initialize from \ref congruence_kind and Presentation.
    //!
    //! This function re-initializes a \ref Kambites instance as if it had
    //! been newly constructed from \p knd and \p p.
    //!
    //! \tparam OtherWord the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    //! \throws LibsemigroupsException if \p knd is not \ref
    //! congruence_kind::twosided.
    template <typename OtherWord>
    Kambites& init(congruence_kind knd, Presentation<OtherWord> const& p) {
      throw_if_1_sided(knd);
      // The lambda in the next line converts, say, chars to size_ts, but
      // doesn't convert size_ts to human_readable characters.
      init(knd, to_presentation<native_word_type>(p, [](auto x) { return x; }));
      return *this;
    }

    //! \brief Get the presentation used to define a \ref Kambites instance
    //! (if any).
    //!
    //! If a \ref Kambites instance is constructed or initialised using a
    //! presentation, then a const reference to the
    //! \ref native_presentation_type version of this presentation is
    //! returned by this function.
    //!
    //! \returns A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Presentation<native_word_type> const&
    presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - add_pairs
    ////////////////////////////////////////////////////////////////////////

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented
    //! by a
    //! \ref Kambites instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
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

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented
    //! by a
    //! \ref Kambites instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
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

    //! \brief Compute the number of classes in the congruence.
    //!
    //! This function computes the number of classes in the congruence
    //! represented by a \ref Kambites instance if the \ref
    //! small_overlap_class is at least \f$4\f$. \ref Kambites instances can
    //! only compute the number of classes if the condition of the previous
    //! sentence is fulfilled, and in this case the number of classes is
    //! always \ref POSITIVE_INFINITY. Otherwise an exception is thrown.
    //!
    //! \returns The number of congruences classes of a \ref
    //! Kambites if \ref small_overlap_class is at least \f$4\f$.
    //!
    //! \throws LibsemigroupsException if it is not possible to compute the
    //! number of classes of the congruence because the small overlap class
    //! is too small.
    // Not noexcept, throws
    [[nodiscard]] uint64_t number_of_classes() {
      throw_if_not_C4();
      return POSITIVE_INFINITY;
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

   private:
    // The Kambites class requires that input to contains to be actual
    // objects not iterators. This is different from KnuthBendix and
    // ToddCoxeter. One way to resolve this more satisfactorily would be to
    // implement MultiStringView for non-strings, so that we can just
    // construct a light-weight view and bung that in here instead.
    [[nodiscard]] bool contains_no_checks(native_word_type const& u,
                                          native_word_type const& v);

   public:
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to
    //! be contained in the congruence represented by a \ref Kambites
    //! instance. This function performs no enumeration, so it is possible
    //! for the words to be contained in the congruence, but that this is
    //! not currently known.
    //!
    //! If any of the iterators point at words that do not belong to the
    //! ``presentation().alphabet()``, then \ref tril::FALSE or \ref
    //! tril::unknown is returned (depending on whether \ref finished
    //! returns
    //! \c true or \c false, respectively).
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the
    //! congruence;
    //! * tril::unknown otherwise.
    //!
    //! \warning It is assumed but not checked that the \ref
    //! small_overlap_class is at least \f$4\f$.
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] tril currently_contains_no_checks(Iterator1 first1,
                                                    Iterator2 last1,
                                                    Iterator3 first2,
                                                    Iterator4 last2) const;

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to
    //! be contained in the congruence represented by a \ref Kambites
    //! instance. This function performs no enumeration, so it is possible
    //! for the words to be contained in the congruence, but that this is
    //! not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the
    //! congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
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

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref Kambites instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed but not checked that the \ref
    //! small_overlap_class is at least \f$4\f$.
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains_no_checks(Iterator1 first1,
                                          Iterator2 last1,
                                          Iterator3 first2,
                                          Iterator4 last2);

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref Kambites instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \throws LibsemigroupsException if \ref small_overlap_class is not at
    //! least \f$4\f$.
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    [[nodiscard]] bool contains(Iterator1 first1,
                                Iterator2 last1,
                                Iterator3 first2,
                                Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

   private:
    void normal_form_no_checks(native_word_type&       result,
                               native_word_type const& w) const;

   public:
    //! \brief Reduce a word with no computation of the small_overlap_class
    //! or checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output
    //! iterator \p d_first. If \ref finished returns \c true, then the word
    //! output by this function is the lexicographically least word in the
    //! congruence class of the input word. Otherwise, the input word is
    //! output. Note that in a small overlap monoid, every congruence class
    //! is finite, and so this lexicographically least word always exists.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed but not checked that the \ref
    //! small_overlap_class is at least \f$4\f$.
    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           Iterator1      first,
                                           Iterator2      last) const;

    //! \brief Reduce a word with no computation of the small_overlap_class.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output
    //! iterator \p d_first. If \ref finished returns \c true, then the word
    //! output by this function is the lexicographically least word in the
    //! congruence class of the input word. Otherwise, the input word is
    //! output. Note that in a small overlap monoid, every congruence class
    //! is finite, and so this lexicographically least word always exists.
    //! form for the input word.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \throws LibsemigroupsException if the \ref small_overlap_class is known
    //! and is not at least \f$4\f$.
    template <typename OutputIterator, typename Iterator1, typename Iterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 Iterator1      first,
                                 Iterator2      last) const {
      throw_if_letter_out_of_bounds(first, last);
      if (finished() && small_overlap_class() < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "small overlap class must be at least 4, but found {}",
            small_overlap_class());
      }
      return reduce_no_run_no_checks(d_first, first, last);
    }

    //! \brief Reduce a word with no checks.
    //!
    //! This function computes the \ref small_overlap_class and then writes
    //! a reduced word equivalent to the input word described by the
    //! iterator \p first and \p last to the output iterator \p d_first. The
    //! word output by this function is the lexicographically least word in
    //! the congruence class of the input word. Note that in a small overlap
    //! monoid, every congruence class is finite, and so this
    //! lexicographically least word always exists.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed but not checked that the \ref
    //! small_overlap_class is at least \f$4\f$.
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      run();
      return reduce_no_run_no_checks(d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function computes the \ref small_overlap_class and then writes
    //! a reduced word equivalent to the input word described by the
    //! iterator \p first and \p last to the output iterator \p d_first. The
    //! word output by this function is the lexicographically least word in
    //! the congruence class of the input word. Note that in a small overlap
    //! monoid, every congruence class is finite, and so this
    //! lexicographically least word always exists.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \throws LibsemigroupsException if \ref small_overlap_class is not at
    //! least \f$4\f$.
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      throw_if_letter_out_of_bounds(first, last);
      throw_if_not_C4();
      return reduce_no_checks(d_first, first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Get the small overlap class.
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
    //! The member functions \ref contains and \ref reduce only work if
    //! the return value of this function is at least \f$4\f$.
    // not noexcept because number_of_pieces_no_checks isn't
    [[nodiscard]] size_t small_overlap_class();

    //! \brief Get the current value of the small overlap class.
    //!
    //! See \ref small_overlap_class() for more details.
    //!
    //! \returns
    //! The small overlap class if known or \ref UNDEFINED otherwise.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t small_overlap_class() const noexcept;

    //! \brief Returns the suffix tree used to compute pieces.
    //!
    //! This function returns a const reference to the generalised suffix
    //! tree Ukkonoen object containing the relation words of a Kambites
    //! object, that is used to determine the pieces, and decompositions of
    //! the relation words.
    //!
    //! \returns A const reference to a \ref Ukkonen object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto const& ukkonen() noexcept {
      run();
      return _suffix_tree;
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - validation functions - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of bounds
    //! (i.e. does not belong to the alphabet of the \ref presentation used
    //! to construct the \ref Kambites instance).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from \p
    //! first to \p last is out of bounds.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      _presentation.validate_word(first, last);
    }

    //! \brief Throws if \ref small_overlap_class isn't at least \f$4\f$.
    //!
    //! This function throws an exception if the \ref small_overlap_class is
    //! not at least \f$4\f$.
    //!
    //! \throws LibsemigroupsException if \ref small_overlap_class is not at
    //! least \f$4\f$.
    void throw_if_not_C4();

    //! \brief Check if the small overlap class has been computed and is at
    //! least 4.
    //!
    //! This function calls \ref finished and checks whether or not the \ref
    //! small_overlap_class is at least \f$4\f$. This function can be used to
    //! check whether or not the functions \ref reduce and \ref contains can be
    //! used.
    //!
    //! \returns Whether or not the small overlap class is known and that it is
    //! at least \f$4\f$.
    [[nodiscard]] bool success() const override {
      return finished() && _class >= 4;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - init functions - private
    ////////////////////////////////////////////////////////////////////////

    Kambites& private_init_from_presentation(bool call_init);

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
    // w, and there's no factor of <s> of the form X_jY_j starting before
    // the beginning of Y_i. If no such exists, UNDEFINED is returned. Not
    // noexcept because relation_prefix isn'Word
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
    // * <p.first> is the starting index of the suffix of <s> that contains
    // a clean_overlap_prefix.
    //
    // * <p.second> is the index of the relation word that's a clear overlap
    //   prefix of a suffix of <s>
    [[nodiscard]] std::pair<size_t, size_t>
    clean_overlap_prefix_mod(internal_type const& s, size_t n) const;

    // If x + [first, last) = aX_sY_sw words a, w and for some index s,
    // where X_s = X_s'X_s'', x = aX_s', and [first, last) = X_s''Y_sw, then
    // this function returns a tuple <Word> where:
    //
    // 1. std::get<0>(Word) is the index <s>
    //
    // 2. std::get<1>(Word) is an iterator <it1> into <x>, such that
    //    [it1, x.cend()) = X_s'
    //
    // 3. std::get<2>(Word) is an iterator <it2> into [first, last), such
    //    that [it2, last) = w
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

    // Returns a word equal to w in this, starting with the piece p, no
    // checks are performed. Used in the normal_form function. Not noexcept
    // because detail::is_prefix isn'Word
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

    // Returns the index j of a complement of X_iY_iZ_i such that X_jY_j is
    // a prefix of w. Otherwise, UNDEFINED is returned.
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

    template <typename native_word_type>
    static void pop_front(native_word_type& x) {
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
    // Returns true if u and v represent the same element of the fp
    // semigroup represented by this, and p is a possible prefix of u, and
    // v.
    //
    // Parameters are given by value because they are modified by wp_prefix,
    // and it was too difficult to untangle the different cases (when u, v
    // are equal, not equal, references, rvalue references etc). It's
    // possible that it could be modified to only copy when necessary, but
    // this doesn't seem worth it at present.
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
      return _have_class;
    }
  };

  //! \ingroup kambites_group
  //
  //! \brief Deduction guide.
  //!
  //! Deduction guide to construct a Kambites<Word> from a
  //! Presentation<Word> const reference.
  template <typename Word>
  Kambites(congruence_kind, Presentation<Word> const&) -> Kambites<Word>;

  //! \ingroup kambites_group
  //!
  //! \brief Helper functions for the \ref Kambites class template.
  //!
  //! Defined in \c kambites.hpp.
  //!
  //! This page contains documentation for many helper functions for the \ref
  //! Kambites class template. In particular, these functions include versions
  //! of several of the member functions of \ref KnuthBendix (that accept
  //! iterators) whose parameters are not iterators, but objects instead. The
  //! helpers documented on this page all belong to the namespace ``kambites``.
  //!
  //! \sa \ref cong_intf_helpers_group
  //!
  //! @{
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

    //! \brief Returns a range object containing normal forms.
    //!
    //! Defined in \c kambites.hpp.
    //!
    //! This function returns a range object containing short-lex normal forms
    //! of the classes of the congruence represented by a Kambites instance.
    //!
    //! \tparam Word the type of the words contained in the parameter \p k.
    //!
    //! \param k the \ref Kambites instance.
    //!
    //! \returns A range object.
    //!
    //! \throws LibsemigroupsException if the Kambites::small_overlap_class of
    //! \p k is not at least \f$4\f$.
    //!
    //! \warning The returned range object is always infinite.
    // TODO(1) should be allowed to specify the OutputWord here too, this
    // requires changes to KambitesNormalFormRange however.
    template <typename Word>
    auto normal_forms(Kambites<Word>& k) {
      k.throw_if_not_C4();
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

    //! @}
  }  // namespace kambites

  //! \ingroup kambites_group
  //!
  //! \brief Return a human readable representation of a Kambites object.
  //!
  //! Defined in ``kambites.hpp``.
  //!
  //! This function returns a human readable representation of a
  //! Kambites object.
  //!
  //! \param k the \ref Kambites object.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  std::string to_human_readable_repr(Kambites<Word> const& k) {
    std::string suffix;
    if (k.finished()) {
      suffix += " with small overlap class ";
      if (k.small_overlap_class() == POSITIVE_INFINITY) {
        suffix += fmt::format("{}", POSITIVE_INFINITY);
      } else {
        suffix += fmt::format("{}", k.small_overlap_class());
      }
    }
    return fmt::format("<Kambites over {}{}>",
                       to_human_readable_repr(k.presentation()),
                       suffix);
  }

}  // namespace libsemigroups

#include "kambites.tpp"

#endif  // LIBSEMIGROUPS_KAMBITES_HPP_
