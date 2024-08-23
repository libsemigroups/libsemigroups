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
#include "froidure-pin.hpp"     // for FroidurePin, FroidurePinTraits
#include "order.hpp"            // for lexicographical_compare
#include "presentation.hpp"     // for Presentation
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type, tril, letter_type
#include "ukkonen.hpp"          // for Ukkonen
#include "words.hpp"            // for ToStrings

#include "detail/multi-string-view.hpp"  // for MultiStringView
#include "detail/string.hpp"             // for is_prefix
#include "detail/uf.hpp"                 // for Duf<>

namespace libsemigroups {

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
    // TODO update doc
    using value_type
        = std::conditional_t<std::is_same_v<Word, detail::MultiStringView>,
                             std::string,
                             Word>;

   private:
    //! The template parameter \p Word.
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

    Presentation<value_type> _presentation;
    Ukkonen                  _suffix_tree;

   private:
    using internal_type_iterator = typename internal_type::const_iterator;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Kambites - Constructors, destructor, initialisation - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    //!
    //! \par Parameters
    //! (None)
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

    explicit Kambites(Presentation<value_type> const& p) : Kambites() {
      p.validate();
      _presentation = p;
      private_init_from_presentation(false);
    }

    Kambites& init(Presentation<value_type> const& p) {
      p.validate();
      _presentation = p;
      return private_init_from_presentation(true);
    }

    explicit Kambites(Presentation<value_type>&& p) : Kambites() {
      p.validate();
      _presentation = std::move(p);
      private_init_from_presentation(false);
    }

    Kambites& init(Presentation<value_type>&& p) {
      p.validate();
      _presentation = std::move(p);
      return private_init_from_presentation(true);
    }

    // No rvalue ref version of this because we can't use the presentation.
    template <typename OtherWord>
    explicit Kambites(Presentation<OtherWord> const& p)
        : Kambites(to_presentation<value_type>(p)) {}

    template <typename OtherWord>
    Kambites& init(Presentation<OtherWord> const& p) {
      init(to_presentation<value_type>(p));
      return *this;
    }

    Presentation<value_type> const& presentation() const noexcept {
      return _presentation;
    }

    //! \copydoc FpSemigroupInterface::size
    //!
    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, throws
    [[nodiscard]] uint64_t number_of_classes() override {
      validate_small_overlap_class();
      return POSITIVE_INFINITY;
    }

    //!
    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, throws
    [[nodiscard]] bool contains(word_type const& u,
                                word_type const& v) override;

    // Note that the asymmetry between how contains and normal_form are
    // implemented is because contains is a pure virtual function. Without this
    // restriction contains could be more simply implemented as normal_form is.
    template <typename SFINAE = bool>
    [[nodiscard]] auto contains(value_type const& u, value_type const& v)
        -> std::enable_if_t<!std::is_same_v<value_type, word_type>, SFINAE>;

    //! \copydoc FpSemigroupInterface::normal_form
    //!
    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, lots of allocations
    [[nodiscard]] value_type normal_form(value_type const& w);

    template <typename SFINAE = word_type>
    [[nodiscard]] auto normal_form(word_type const& w)
        -> std::enable_if_t<!std::is_same_v<value_type, word_type>, SFINAE>;

    ////////////////////////////////////////////////////////////////////////
    // Kambites - member functions - public
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because number_of_pieces_unsafe isn'Word
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
    //! \par Parameters
    //! (None)
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
    [[nodiscard]] size_t small_overlap_class();

    //! Returns the Ukkonen suffix tree object used to compute pieces.
    //!
    //! This function returns a reference to the Ukkonen generalised suffix
    //! tree object containing the relation words of a Kambites object, that
    //! is used to determine the pieces, and decompositions of the relation
    //! words.
    //!
    //! \parameters (None)
    //!
    //! \returns A const reference to a \ref Ukkonen object.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] auto const& ukkonen() noexcept {
      run();
      return _suffix_tree;
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

    void validate_word(word_type const& w) const override {
      if constexpr (std::is_same_v<
                        typename decltype(_presentation)::letter_type,
                        letter_type>) {
        _presentation.validate_word(w.cbegin(), w.cend());
      } else {
        ToString   to_string(_presentation.alphabet());
        value_type ww = to_string(w);
        _presentation.validate_word(ww.cbegin(), ww.cend());
      }
    }

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

    internal_type const& X(size_t i) const;
    internal_type const& Y(size_t i) const;
    internal_type const& Z(size_t i) const;
    internal_type const& XY(size_t i) const;
    internal_type const& YZ(size_t i) const;
    internal_type const& XYZ(size_t i) const;

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

    size_t clean_overlap_prefix(internal_type_iterator const& first,
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

  template <typename Word>
  Kambites(Presentation<Word> const&) -> Kambites<Word>;

  template <typename Range,
            typename Word1,
            typename Word2 = typename Kambites<Word1>::value_type>
  std::vector<std::vector<Word2>> partition(Kambites<Word1>& k, Range r) {
    static_assert(
        std::is_same_v<std::decay_t<typename Range::output_type>, Word2>);
    using return_type = std::vector<std::vector<Word2>>;

    if (!r.is_finite) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                              "found an infinite range");
    }

    return_type result;

    std::unordered_map<Word2, size_t> map;
    size_t                            index = 0;

    while (!r.at_end()) {
      auto next = r.get();
      if (k.presentation().contains_empty_word() || !next.empty()) {
        auto next_nf        = k.normal_form(next);
        auto [it, inserted] = map.emplace(next_nf, index);
        if (inserted) {
          result.emplace_back();
          index++;
        }
        size_t index_of_next_nf = it->second;
        result[index_of_next_nf].push_back(next);
      }
      r.next();
    }
    return result;
  }

  // There's no non_trivial_classes(Kambites k1, Kambites k2) because it's
  // unclear how this could be computed (because they always define infinite
  // semigroups/monoids), so we can't just do non_trivial_classes(k1,
  // kambites::normal_forms(k2)) (as in ToddCoxeter) because there are
  // infinitely many normal_forms.

}  // namespace libsemigroups

#include "kambites.tpp"

#endif  // LIBSEMIGROUPS_KAMBITES_HPP_
