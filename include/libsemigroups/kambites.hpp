//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021 James D. Mitchell + Maria Tsalakou
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

// TODO(later)
// * check code coverage
// * check iwyu
// * check const
// * check final
// * check noexcept
// * C(3)?
// * allow arbitrary alphabet orders
// * implement cbegin_normal_forms, cend_normal_forms

#ifndef LIBSEMIGROUPS_KAMBITES_HPP_
#define LIBSEMIGROUPS_KAMBITES_HPP_

#include <stdint.h>  // for uint64_t

#include <algorithm>    // for min
#include <cstddef>      // for size_t
#include <functional>   // for equal_to
#include <memory>       // for shared_ptr, unique_ptr, make_shared
#include <string>       // for string
#include <type_traits>  // for is_base_of, is_trivial
#include <utility>      // for pair
#include <vector>       // for vector

#include "adapters.hpp"      // for One
#include "cong-intf.hpp"     // for CongruenceInterface
#include "constants.hpp"     // for UNDEFINED, POSITIVE_INFINITY
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "fpsemi-intf.hpp"   // for FpSemigroupInterface
#include "froidure-pin.hpp"  // for FroidurePin, FroidurePinTraits
#include "int-range.hpp"
#include "order.hpp"  // for lexicographical_compare
#include "string-view.hpp"
#include "string.hpp"       // for is_prefix
#include "suffix-tree.hpp"  // for SuffixTree
#include "types.hpp"        // for word_type, tril, letter_type
#include "uf.hpp"           // for Duf<>
#include "word.hpp"         // for word_to_string

namespace libsemigroups {
  class FroidurePinBase;  // Forward decl
  namespace detail {
    class KE;  // Forward decl
  }

  namespace fpsemigroup {

    //! Defined in ``kambites.hpp``.
    //!
    //! On this page we describe the functionality relating to the algorithms
    //! for small overlap monoids by
    //! [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038) and the
    //! authors of ``libsemigroups``.
    //!
    //! This page describes the implementation in the class
    //! fpsemigroup::Kambites which uses the FpSemigroupInterface, which is
    //! also documented on this page.
    // TODO(later) example
    template <typename T>
    class Kambites final : public FpSemigroupInterface {
     public:
      ////////////////////////////////////////////////////////////////////////
      // Kambites - aliases - public
      ////////////////////////////////////////////////////////////////////////

      //! The type of strings used by a Kambites instance.
      using string_type = std::string;

      //! The template parameter \p T.
      using internal_type = T;

     private:
      using internal_type_iterator = typename internal_type::const_iterator;

     public:
      ////////////////////////////////////////////////////////////////////////
      // Kambites - Constructors and destructor - public
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
      Kambites()
          :  // Mutable
            _class(UNDEFINED),
            _complements(),
            _have_class(false),
            _XYZ_data(),
            // Non-mutable
            _relation_words(),
            _suffix_tree() {}

      //! Default copy constructor.
      Kambites(Kambites const&);

      //! Default move constructor.
      Kambites(Kambites&&) = default;

      //! Default copy assignment operator.
      Kambites& operator=(Kambites const&) = default;

      //! Default move assignment operator.
      Kambites& operator=(Kambites&&) = default;

      ~Kambites();

      ////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - pure virtual member functions - public
      ////////////////////////////////////////////////////////////////////////

      //! \copydoc FpSemigroupInterface::size
      //!
      //! \throws LibsemigroupsException if the small overlap class is not at
      //! least \f$4\f$.
      // Not noexcept, throws
      uint64_t size() override {
        validate_small_overlap_class();
        return POSITIVE_INFINITY;
      }

#ifndef DOXYGEN_SHOULD_SKIP_THIS
      using FpSemigroupInterface::equal_to;
#endif

      //! \copydoc FpSemigroupInterface::equal_to
      //!
      //! \throws LibsemigroupsException if the small overlap class is not at
      //! least \f$4\f$.
      // Not noexcept, throws
      bool equal_to(string_type const& u, string_type const& v) override {
        validate_small_overlap_class();
        // Words aren't validated, the below returns false if they contain
        // letters not in the alphabet.
        return wp_prefix(internal_type(u), internal_type(v), internal_type());
      }

      // Not noexcept, equal_to above throws
      //! \copydoc FpSemigroupInterface::equal_to
      //!
      //! \throws LibsemigroupsException if the small overlap class is not at
      //! least \f$4\f$.
      bool equal_to(string_type&& u, string_type&& v) {
        string_type uu = u;
        string_type vv = v;
        return equal_to(uu, vv);
      }

      //! \copydoc FpSemigroupInterface::normal_form
      //!
      //! \throws LibsemigroupsException if the small overlap class is not at
      //! least \f$4\f$.
      // Not noexcept, lots of allocations
      string_type normal_form(string_type const& w) override;

      ////////////////////////////////////////////////////////////////////////
      // Kambites - member functions - public
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because number_of_pieces_unsafe isn't
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
      size_t small_overlap_class() const;

      //! Returns the number of normal forms with length in a given range.
      //!
      //! \param min the minimum length of a normal form to count
      //! \param max one larger than the maximum length of a normal form to
      //! count.
      //!
      //! \returns
      //! A value of type `uint64_t`.
      //!
      //! \throws LibsemigroupsException if the small overlap class is not at
      //! least \f$4\f$.
      //!
      //! \complexity
      //! Assuming that \c this has been run until finished, the complexity of
      //! this function is at worst \f$O(mnk ^ 6)\f$ where \f$m\f$ is the
      //! number of letters in the alphabet, \f$n\f$ is the number of normal
      //! forms with length in the range \f$[min, max)\f$, and \f$k\f$ is the
      //! parameter \p max.
      // Not noexcept because FroidurePin::run isn't
      uint64_t number_of_normal_forms(size_t min, size_t max);

      //! Returns the minimum number of pieces required to factorise the
      //! \f$i\f$-th relation word.
      //!
      //! \param i the index of the relation word
      //!
      //! \returns
      //! A value of type `size_t`.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! The current implementation has complexity no worse than \f$O(m)\f$
      //! where \f$m\f$ is the sum of the lengths of the words occurring in the
      //! relations of the semigroup.
      // Returns the number of pieces in the i-th relation word
      // Not noexcept, throws
      size_t number_of_pieces(size_t i) const;

     private:
      ////////////////////////////////////////////////////////////////////////
      // Kambites - validation functions - private
      ////////////////////////////////////////////////////////////////////////

      // Throws exception if the index i is not in the range [0,
      // _relation_words.size())
      //
      // Not noexcept, throws
      void validate_relation_word_index(size_t i) const;

      // Throws exception if the small_overlap_class is < 4.
      //
      // Not noexcept, throws
      void validate_small_overlap_class() const;

      ////////////////////////////////////////////////////////////////////////
      // Kambites - XYZ functions - private
      ////////////////////////////////////////////////////////////////////////

      void really_init_XYZ_data(size_t i) const;

      // init_XYZ_data is split into the function that really does the work
      // (really_init_XYZ_data) which is called once, and init_XYZ_data which
      // can be called very often.
      inline void init_XYZ_data(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        if (_XYZ_data.empty()) {
          _XYZ_data.resize(_relation_words.size());
        }
        if (!_XYZ_data[i].is_initialized) {
          really_init_XYZ_data(i);
        }
      }

      internal_type const& X(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].X;
      }

      internal_type const& Y(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].Y;
      }

      internal_type const& Z(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].Z;
      }

      internal_type const& XY(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].XY;
      }

      internal_type const& YZ(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].YZ;
      }

      internal_type const& XYZ(size_t i) const {
        LIBSEMIGROUPS_ASSERT(i < _relation_words.size());
        LIBSEMIGROUPS_ASSERT(finished_impl());
        init_XYZ_data(i);
        return _XYZ_data[i].XYZ;
      }

      ////////////////////////////////////////////////////////////////////////
      // Kambites - helpers - private
      ////////////////////////////////////////////////////////////////////////

      // Returns the index of the relation word r_i = X_iY_iZ_i if [first,
      // last) = X_iY_iw for some w. If no such exists, then UNDEFINED is
      // returned.
      // Not noexcept because is_prefix isn't
      size_t relation_prefix(internal_type_iterator const& first,
                             internal_type_iterator const& last) const;

      // Returns the index of the relation word r_i = X_iY_iZ_i such that
      // X_iY_i is a clean overlap prefix of <s>, i.e. <s> = X_iY_iw for some
      // w, and there's no factor of <s> of the form X_jY_j starting before the
      // beginning of Y_i. If no such exists, UNDEFINED is returned.
      // Not noexcept because relation_prefix isn't
      inline size_t clean_overlap_prefix(internal_type const& s) const {
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
      std::pair<size_t, size_t> clean_overlap_prefix_mod(internal_type const& s,
                                                         size_t n) const;

      // If x + [first, last) = aX_sY_sw words a, w and for some index s, where
      // X_s = X_s'X_s'', x = aX_s', and [first, last) = X_s''Y_sw, then this
      // function returns a tuple <t> where:
      //
      // 1. std::get<0>(t) is the index <s>
      //
      // 2. std::get<1>(t) is an iterator <it1> into <x>, such that
      //    [it1, x.cend()) = X_s'
      //
      // 3. std::get<2>(t) is an iterator <it2> into [first, last), such that
      //    [it2, last) = w
      //
      // If no such relation word exists, then
      //
      //    (UNDEFINED, first.cend(), second.cend())
      //
      // is returned.
      //
      // Not noexcept because relation_prefix isn't
      std::tuple<size_t, internal_type_iterator, internal_type_iterator>
      p_active(internal_type const&          x,
               internal_type_iterator const& first,
               internal_type_iterator const& last) const {
        // The following should hold but can't be checked when internal_type is
        // MultiStringView.
        // LIBSEMIGROUPS_ASSERT(x.cend() < first || x.cbegin() >= last);
        internal_type y = x;
        y.append(first, last);
        for (auto it = y.cbegin(); it < y.cbegin() + x.size(); ++it) {
          size_t i = relation_prefix(it, y.cend());
          if (i != UNDEFINED) {
            size_t n = it - y.cbegin();
            y.erase(y.begin() + x.size(), y.end());
            return std::make_tuple(
                i, x.cbegin() + n, first + (XY(i).size() - (x.size() - n)));
          }
        }
        return std::make_tuple(UNDEFINED, x.cend(), last);
      }

      // Returns a word equal to w in this, starting with the piece p, no checks
      // are performed. Used in the normal_form function.
      // Not noexcept because detail::is_prefix isn't
      void replace_prefix(internal_type& w, internal_type const& p) const;

      ////////////////////////////////////////////////////////////////////////
      // Kambites - complement helpers - private
      ////////////////////////////////////////////////////////////////////////

      // Returns j among the complements of i such that [first,
      // last) is a prefix of X_jY_jZ_j, and UNDEFINED otherwise.
      size_t prefix_of_complement(size_t                        i,
                                  internal_type_iterator const& first,
                                  internal_type_iterator const& last) const;

      // Helper for the above
      size_t prefix_of_complement(size_t i, internal_type const& w) const {
        return prefix_of_complement(i, w.cbegin(), w.cend());
      }

      // Returns the index j of a complement of X_iY_iZ_i such that X_jY_j is a
      // prefix of w. Otherwise, UNDEFINED is returned.
      size_t complementary_XY_prefix(size_t i, internal_type const& w) const;

      // Returns j such that w is Z_j-active for some Z_j in the
      // complements of Z_i.  Otherwise it returns UNDEFINED.
      //
      // See also: p_active.
      size_t Z_active_complement(size_t i, internal_type const& w) const;

      // Returns j such that w is Z_j-active for some Z_j in the
      // proper (j != i) complements of Z_i.  Otherwise it returns UNDEFINED.
      size_t Z_active_proper_complement(size_t i, internal_type const& w) const;

      size_t
      Z_active_proper_complement(size_t                        i,
                                 internal_type_iterator const& first,
                                 internal_type_iterator const& last) const;

      ////////////////////////////////////////////////////////////////////////
      // Kambites - static functions - private
      ////////////////////////////////////////////////////////////////////////

      static size_t complementary_relation_word(size_t i) {
        return (i % 2 == 0 ? i + 1 : i - 1);
      }

      static void pop_front(std::string& x) {
        x.erase(x.begin());
      }

      static void pop_front(detail::MultiStringView& x) {
        x.pop_front();
      }

      ////////////////////////////////////////////////////////////////////////
      // Kambites - main functions - private
      ////////////////////////////////////////////////////////////////////////

      // copy if const_reference, or rrvalue reference
      template <typename S>
      struct ForwardingTypeHelper {
        using type = std::conditional_t<
            std::is_rvalue_reference<S>::value
                || (std::is_reference<S>::value
                    && std::is_const<std::remove_reference_t<S>>::value),
            std::remove_const_t<std::remove_reference_t<S>>,
            std::remove_reference_t<S>&>;
      };

      template <typename S>
      using ForwardingType = typename ForwardingTypeHelper<S>::type;

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
      // this doesn't seem worth it at present.
      bool wp_prefix(internal_type u, internal_type v, internal_type p) const;

      // Implementational detail
      // Not noexcept because nothing else is and lots allocations
      void normal_form_inner(size_t&        r,
                             internal_type& v,
                             internal_type& w) const;

      ////////////////////////////////////////////////////////////////////////
      // FpSemigroupInterface - pure virtual member functions - private
      ////////////////////////////////////////////////////////////////////////

      void run_impl() override {
        small_overlap_class();
      }

      bool finished_impl() const override {
        return _have_class && small_overlap_class() >= 4;
      }

      void add_rule_impl(std::string const& u, std::string const& v) override;

      std::shared_ptr<FroidurePinBase> froidure_pin_impl() override;

      bool is_obviously_infinite_impl() override {
        return small_overlap_class() >= 3;
      }

      ////////////////////////////////////////////////////////////////////////
      // Kambites - inner classes - private
      ////////////////////////////////////////////////////////////////////////

      // Data structure for caching the regularly accessed parts of the
      // relation words.
      struct RelationWords {
        using internal_type          = Kambites::internal_type;
        bool          is_initialized = false;
        internal_type X;
        internal_type Y;
        internal_type Z;
        internal_type XY;
        internal_type YZ;
        internal_type XYZ;
      };

      // Data structure for caching the complements of each relation word.
      //
      // We say that a relation word u' is a *complement* of a relation word u
      // if there are relation words u = r_1,r_2, ..., r_n = u'$ such that
      // either (r_i,r_{i+1}) \in R or (r_{i+1},r_i) \in R for $1 \leq i \leq
      // n$. We say that $u'$ is a *proper complement* of the relation word u
      // if it is a complement of u and u is not equal to u'.
      class Complements {
       public:
        Complements() = default;
        void init(std::vector<string_type> const&);

        std::vector<size_t> const& of(size_t i) const {
          LIBSEMIGROUPS_ASSERT(i < _lookup.size());
          LIBSEMIGROUPS_ASSERT(_lookup[i] < _complements.size());
          return _complements[_lookup[i]];
        }

       private:
        std::vector<std::vector<size_t>> _complements;
        std::vector<size_t>              _lookup;
      };

      ////////////////////////////////////////////////////////////////////////
      // Kambites - data members - private
      ////////////////////////////////////////////////////////////////////////

      mutable size_t                     _class;
      mutable Complements                _complements;
      mutable bool                       _have_class;
      mutable std::vector<RelationWords> _XYZ_data;

      std::vector<string_type> _relation_words;
      detail::SuffixTree       _suffix_tree;
    };

    ////////////////////////////////////////////////////////////////////////
    // Kambites - IMPLEMENTATION
    ////////////////////////////////////////////////////////////////////////

    ////////////////////////////////////////////////////////////////////////
    // Kambites - constructor and destructor impl - public
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    Kambites<T>::Kambites(Kambites const&) = default;

    template <typename T>
    Kambites<T>::~Kambites() = default;

    ////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual functions impl - public
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    typename Kambites<T>::string_type
    Kambites<T>::normal_form(string_type const& w0) {
      validate_small_overlap_class();
      size_t        r = UNDEFINED;
      internal_type v, w(w0);
      while (!w.empty()) {
        if (r == UNDEFINED) {
          normal_form_inner(r, v, w);
          continue;
        }

        size_t rb
            = Z_active_proper_complement(r, w.cbegin() + Z(r).size(), w.cend());
        if (rb == UNDEFINED || Z(r) == Z(rb)) {
          normal_form_inner(r, v, w);
          continue;
        }
        LIBSEMIGROUPS_ASSERT(detail::is_prefix(w, Z(r)));
        size_t                 s;
        internal_type_iterator it_zrb, it_wp;
        std::tie(s, it_zrb, it_wp)
            = p_active(Z(rb), w.cbegin() + Z(r).size(), w.cend());
        LIBSEMIGROUPS_ASSERT(s < _relation_words.size());
        // we could just use w.erase(w.begin(), it_wp); if not for the
        // if-statement in the next line requiring the old w.
        internal_type wp(it_wp, w.cend());
        if (!wp_prefix(wp, wp, Z(s))) {
          normal_form_inner(r, v, w);
          continue;
        }
        size_t sb = prefix_of_complement(s, it_zrb, Z(rb).cend());
        if (sb != UNDEFINED) {
          // line 10
          replace_prefix(wp, Z(s));
          wp.erase(wp.begin(), wp.begin() + Z(s).size());
          internal_type b(X(sb).cbegin() + (Z(rb).cend() - it_zrb),
                          X(sb).cend());  // b
          if (wp_prefix(internal_type(w0),
                        v + Z(r) + b + YZ(sb) + wp,
                        internal_type())) {
            // line 12
            v += Z(r);
            v += b;
            v += Y(sb);
            w = Z(sb);
            w += wp;
            r = sb;
            continue;
          }
        }
        // line 16
        v += Z(r);
        v.append(w.cbegin() + Z(r).size(), it_wp);
        std::swap(w, wp);
        r = s;
      }
      return v;
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - member functions impl - public
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    size_t Kambites<T>::small_overlap_class() const {
      if (!_have_class) {
        size_t result = POSITIVE_INFINITY;
        for (auto const& w : _relation_words) {
          result = std::min(
              result, _suffix_tree.number_of_pieces(w.cbegin(), w.cend()));
        }
        _have_class = true;
        _class      = result;
      }
      return _class;
    }

    template <typename T>
    uint64_t Kambites<T>::number_of_normal_forms(size_t mn, size_t mx) {
      validate_small_overlap_class();
      if (mn >= mx) {
        return 0;
      }
      mn = std::max(size_t(1),
                    mn);  // FroidurePin's have no words of length 0
      froidure_pin()->run_until([this, mx]() {
        return this->froidure_pin()->current_max_word_length() == mx;
      });
      auto r     = IntegralRange<size_t>(0, froidure_pin()->current_size());
      auto first = std::find_if(r.cbegin(), r.cend(), [this, mn](size_t i) {
        return this->froidure_pin()->current_length(i) == mn;
      });
      auto last  = std::find_if(first, r.cend(), [this, mx](size_t i) {
        return this->froidure_pin()->current_length(i) == mx;
      });
      return last - first;
    }

    template <typename T>
    size_t Kambites<T>::number_of_pieces(size_t i) const {
      validate_relation_word_index(i);
      return _suffix_tree.number_of_pieces(_relation_words[i].cbegin(),
                                           _relation_words[i].cend());
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - validation functions impl - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void Kambites<T>::validate_relation_word_index(size_t i) const {
      if (i >= _relation_words.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value in the range [0, %llu), found %llu",
            uint64_t(_relation_words.size()),
            uint64_t(i));
      }
    }

    template <typename T>
    void Kambites<T>::validate_small_overlap_class() const {
      if (small_overlap_class() < 4) {
        LIBSEMIGROUPS_EXCEPTION(
            "small overlap class must be at least 4, but found %llu",
            uint64_t(small_overlap_class()));
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - XYZ functions impl - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void Kambites<T>::really_init_XYZ_data(size_t i) const {
      size_t const X_size = _suffix_tree.maximal_piece_prefix(
          _relation_words[i].cbegin(), _relation_words[i].cend());
      size_t const Z_size = _suffix_tree.maximal_piece_suffix(
          _relation_words[i].cbegin(), _relation_words[i].cend());

      _XYZ_data[i].is_initialized = true;
      _XYZ_data[i].X              = internal_type(_relation_words[i].cbegin(),
                                     _relation_words[i].cbegin() + X_size);
      _XYZ_data[i].Y   = internal_type(_relation_words[i].cbegin() + X_size,
                                     _relation_words[i].cend() - Z_size);
      _XYZ_data[i].Z   = internal_type(_relation_words[i].cend() - Z_size,
                                     _relation_words[i].cend());
      _XYZ_data[i].XY  = internal_type(_relation_words[i].cbegin(),
                                      _relation_words[i].cend() - Z_size);
      _XYZ_data[i].YZ  = internal_type(_relation_words[i].cbegin() + X_size,
                                      _relation_words[i].cend());
      _XYZ_data[i].XYZ = internal_type(_relation_words[i]);
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - helpers impl - private
    ////////////////////////////////////////////////////////////////////////

    // O(number of relation words * (last - first))
    // See explanation above.
    //
    // TODO(later): we tried multiple things here to try and improve this, but
    // none of them were better than the current function. Two things that we
    // never tried or didn't get to work were:
    // 1) Binary search
    // 2) Using the original suffix tree (without the unique characters)
    template <typename T>
    size_t
    Kambites<T>::relation_prefix(internal_type_iterator const& first,
                                 internal_type_iterator const& last) const {
      for (size_t i = 0; i < _relation_words.size(); ++i) {
        if (detail::is_prefix(first,
                              last,
                              _relation_words[i].cbegin(),
                              _relation_words[i].cend() - Z(i).size())) {
          return i;
        }
      }
      return UNDEFINED;
    }

    // See explanation above.
    // Complexity: O(max|Y| * (last - first))
    template <typename T>
    size_t Kambites<T>::clean_overlap_prefix(
        internal_type_iterator const& first,
        internal_type_iterator const& last) const {
      size_t i = relation_prefix(first, last);
      if (i == UNDEFINED) {
        return UNDEFINED;
      }
      for (auto it = first + X(i).size() + 1; it < first + XY(i).size(); ++it) {
        if (relation_prefix(it, last) != UNDEFINED) {
          return UNDEFINED;
        }
      }
      return i;
    }

    // See explanation above.
    template <typename T>
    std::pair<size_t, size_t>
    Kambites<T>::clean_overlap_prefix_mod(internal_type const& w,
                                          size_t               n) const {
      size_t     i = 0, j = 0;
      auto       first = w.cbegin();
      auto const last  = w.cend();
      for (; i < n; ++i) {
        j = clean_overlap_prefix(first, last);
        if (j != UNDEFINED) {
          break;
        }
        first++;
      }
      return std::make_pair(i, j);
    }

    // See explanation above.
    template <typename T>
    void Kambites<T>::replace_prefix(internal_type&       w,
                                     internal_type const& p) const {
      LIBSEMIGROUPS_ASSERT(wp_prefix(w, w, p));
      if (detail::is_prefix(w, p)) {
        return;
      }

      size_t i, j;
      std::tie(i, j) = clean_overlap_prefix_mod(w, p.size());
      LIBSEMIGROUPS_ASSERT(j != UNDEFINED);

      internal_type u(w.cbegin() + i + XY(j).size(), w.cend());
      replace_prefix(u, Z(j));
      LIBSEMIGROUPS_ASSERT(detail::is_prefix(u, Z(j)));
      u.erase(u.begin(), u.begin() + Z(j).size());

      size_t k = prefix_of_complement(j, p.cbegin() + i, p.cend());
      LIBSEMIGROUPS_ASSERT(k != UNDEFINED);

      w.erase(w.begin() + i, w.end());
      w += XYZ(k);
      w += u;
      LIBSEMIGROUPS_ASSERT(detail::is_prefix(w, p));
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - complement helpers impl - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    size_t Kambites<T>::prefix_of_complement(
        size_t                        i,
        internal_type_iterator const& first,
        internal_type_iterator const& last) const {
      // TODO(later) use binary_search instead
      for (auto const& j : _complements.of(i)) {
        if (detail::is_prefix(XYZ(j).cbegin(), XYZ(j).cend(), first, last)) {
          return j;
        }
      }
      return UNDEFINED;
    }

    template <typename T>
    size_t Kambites<T>::complementary_XY_prefix(size_t               i,
                                                internal_type const& w) const {
      // TODO(later) use binary_search instead
      for (auto const& j : _complements.of(i)) {
        if (detail::is_prefix(w, XY(j))) {
          return j;
        }
      }
      return UNDEFINED;
    }

    template <typename T>
    size_t Kambites<T>::Z_active_complement(size_t               i,
                                            internal_type const& w) const {
      auto const first = w.cbegin();
      auto const last  = w.cend();

      for (auto const& j : _complements.of(i)) {
        if (std::get<0>(p_active(Z(j), first, last)) != UNDEFINED) {
          return j;
        }
      }
      return UNDEFINED;
    }

    template <typename T>
    size_t Kambites<T>::Z_active_proper_complement(
        size_t                        i,
        internal_type_iterator const& first,
        internal_type_iterator const& last) const {
      for (auto const& j : _complements.of(i)) {
        if (i != j && std::get<0>(p_active(Z(j), first, last)) != UNDEFINED) {
          return j;
        }
      }
      return UNDEFINED;
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - main functions impl - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    bool Kambites<T>::wp_prefix(internal_type u,
                                internal_type v,
                                internal_type p) const {
      using detail::is_prefix;

      _complements.init(_relation_words);

      while (!u.empty() && !v.empty()) {
        size_t i = clean_overlap_prefix(u);
        if (i == UNDEFINED) {
          if (u[0] != v[0] || (!p.empty() && u[0] != p[0])) {
            return false;
          }
          pop_front(u);
          pop_front(v);
          if (!p.empty()) {
            pop_front(p);
          }
        } else {
          if (prefix_of_complement(i, p) == UNDEFINED) {  // line 18
            return false;
          }
          size_t j = complementary_XY_prefix(i, v);
          if (j == UNDEFINED) {  // line 20
            return false;
          }
          // At this point u = X_iY_iu' and v = X_jY_jv'
          bool up_start_Z = is_prefix(
              u.cbegin() + XY(i).size(), u.cend(), Z(i).cbegin(), Z(i).cend());
          bool vp_start_Z = is_prefix(
              v.cbegin() + XY(j).size(), v.cend(), Z(j).cbegin(), Z(j).cend());

          if (up_start_Z && vp_start_Z) {  // line 22
            u.erase(u.begin(), u.begin() + XYZ(i).size());
            auto k = Z_active_complement(i, u);
            k      = (k == UNDEFINED ? i : k);
            u      = Z(k) + u;
            v.erase(v.begin(), v.begin() + XYZ(j).size());
            v = Z(k) + v;
            p.clear();
            // line 23
          } else if (i == j) {  // line 26
            if (is_prefix(X(i), p)) {
              p.clear();
            } else {
              p = Z(i);
            }
            u.erase(u.begin(), u.begin() + XY(i).size());
            v.erase(v.begin(), v.begin() + XY(i).size());
          } else if (vp_start_Z) {  // line 30
            u.erase(u.begin(), u.begin() + XY(i).size());
            v.erase(v.begin(), v.begin() + XYZ(j).size());
            v = Z(i) + v;
            p.clear();
          } else if (up_start_Z) {  // line 32
            u.erase(u.begin(), u.begin() + XYZ(i).size());
            u = Z(j) + u;
            v.erase(v.begin(), v.begin() + XY(j).size());
            p.clear();
          } else {  // line 34
            p = detail::maximum_common_suffix(Z(i), Z(j));
            if (!is_prefix(u.cbegin() + XY(i).size(),
                           u.cend(),
                           Z(i).cbegin(),
                           Z(i).cend() - p.size())
                || !is_prefix(v.cbegin() + XY(j).size(),
                              v.cend(),
                              Z(j).cbegin(),
                              Z(j).cend() - p.size())) {
              return false;
            } else {
              u.erase(u.begin(), u.begin() + XYZ(i).size() - p.size());
              v.erase(v.begin(), v.begin() + XYZ(j).size() - p.size());
            }
          }
        }
      }

      return u.empty() && v.empty() && p.empty();
    }

    template <typename T>
    void Kambites<T>::normal_form_inner(size_t&        r,
                                        internal_type& v,
                                        internal_type& w) const {
      size_t i, j;
      std::tie(i, j) = clean_overlap_prefix_mod(w, w.size());
      if (j == UNDEFINED) {
        // line 39
        v += w;
        w.clear();
        return;
      }

      internal_type wp(w.cbegin() + i + XY(j).size(), w.cend());
      v.append(w.cbegin(), w.cbegin() + i);  // a

      if (!wp_prefix(wp, wp, Z(j))) {
        // line 23
        r = UNDEFINED;
        v += XY(j);
        w = std::move(wp);
      } else {
        // line 27
        r = _complements.of(j).front();
        replace_prefix(wp, Z(j));
        v += XY(r);
        w = Z(r);
        w.append(wp.cbegin() + Z(j).size(), wp.cend());
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual functions impl - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void Kambites<T>::add_rule_impl(std::string const& u,
                                    std::string const& v) {
      _have_class = false;
      _relation_words.push_back(u);
      _relation_words.push_back(v);
      _suffix_tree.add_word(u.cbegin(), u.cend());
      _suffix_tree.add_word(v.cbegin(), v.cend());
    }

    template <typename T>
    std::shared_ptr<FroidurePinBase> Kambites<T>::froidure_pin_impl() {
      using KE = detail::KE;
      using froidure_pin_type
          = FroidurePin<KE, FroidurePinTraits<KE, Kambites<T>>>;
      LIBSEMIGROUPS_ASSERT(!alphabet().empty());
      run();
      validate_small_overlap_class();
      auto ptr = std::make_shared<froidure_pin_type>(*this);
      for (size_t i = 0; i < alphabet().size(); ++i) {
        ptr->add_generator(KE(*this, i));
      }
      return ptr;
    }

    ////////////////////////////////////////////////////////////////////////
    // Kambites - inner classes - private
    ////////////////////////////////////////////////////////////////////////

    template <typename T>
    void Kambites<T>::Complements::init(
        std::vector<string_type> const& relation_words) {
      if (relation_words.empty()) {
        return;
      }
      // Initialise the _complements data structure
      if (_complements.empty()) {
        detail::Duf<>                      duf;
        std::unordered_map<size_t, size_t> map;
        duf.resize(relation_words.size());
        for (size_t i = 0; i < relation_words.size() - 1; ++i) {
          if (i % 2 == 0) {
            duf.unite(i, i + 1);
          }
          for (size_t j = i + 1; j < relation_words.size(); ++j) {
            if (relation_words[i] == relation_words[j]) {
              duf.unite(i, j);
            }
          }
        }
        size_t next = 0;
        _complements.resize(duf.number_of_blocks());
        _lookup.resize(relation_words.size());
        for (size_t i = 0; i < relation_words.size(); ++i) {
          std::unordered_map<size_t, size_t>::iterator it;
          bool                                         inserted;
          std::tie(it, inserted) = map.emplace(duf.find(i), next);
          _lookup[i]             = it->second;
          _complements[it->second].push_back(i);
          next += inserted;
        }
        for (auto& v : _complements) {
          std::sort(v.begin(), v.end(), [&relation_words](size_t i, size_t j) {
            return lexicographical_compare(relation_words[i],
                                           relation_words[j]);
          });
        }
      }
    }
  }  // namespace fpsemigroup

  namespace congruence {
    //! Defined in ``kambites.hpp``.
    //!
    //! On this page we describe the functionality relating to the algorithms
    //! for small overlap monoids by
    //! [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038) and the
    //! authors of ``libsemigroups``.
    //!
    //! This page describes the implementation in the class
    //! congruence::Kambites which uses the CongruenceInterface, which is
    //! also documented on this page.
    class Kambites final : public CongruenceInterface {
     public:
      ////////////////////////////////////////////////////////////////////////////
      // Kambites - constructors - public
      ////////////////////////////////////////////////////////////////////////////

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

      //! Construct from fpsemigroup::Kambites.
      //!
      //! This function constructs a new congruence::Kambites object
      //! initialised with existing data in fpsemigroup::Kambites.
      //!
      //! \param copy the fpsemigroup::Kambites.
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      explicit Kambites(fpsemigroup::Kambites<std::string> const& copy);

      //! Default copy constructor.
      Kambites(Kambites const& copy) : Kambites(*copy._k) {}

      //! Deleted.
      Kambites(Kambites&&) = delete;

      //! Deleted.
      Kambites& operator=(Kambites const&) = delete;

      //! Deleted.
      Kambites& operator=(Kambites&&) = delete;

      ~Kambites();

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual member functions - public
      ////////////////////////////////////////////////////////////////////////////

      tril const_contains(word_type const&, word_type const&) const override;
      bool contains(word_type const&, word_type const&) override;

      ////////////////////////////////////////////////////////////////////////////
      // Kambites - member functions - public
      ////////////////////////////////////////////////////////////////////////////

      //! Get the underlying fpsemigroup::Kambites.
      //!
      //! \par Parameters
      //! (None)
      //!
      //! \exceptions
      //! \no_libsemigroups_except
      //!
      //! \complexity
      //! Constant
      //!
      //! \returns
      //! A reference to the underlying fpsemigroup::Kambites of \c this.
      fpsemigroup::Kambites<std::string>& kambites() const {
        return *_k;
      }

     private:
      ////////////////////////////////////////////////////////////////////////////
      // Runner - pure virtual member functions - protected
      ////////////////////////////////////////////////////////////////////////////

      bool finished_impl() const override;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////////

      word_type class_index_to_word_impl(class_index_type) override;
      size_t    number_of_classes_impl() override;
      std::shared_ptr<FroidurePinBase> quotient_impl() override;
      class_index_type word_to_class_index_impl(word_type const&) override;
      void             run_impl() override;

      ////////////////////////////////////////////////////////////////////////////
      // CongruenceInterface - non-pure virtual methods - private
      ////////////////////////////////////////////////////////////////////////////

      void add_pair_impl(word_type const&, word_type const&) override;
      void set_number_of_generators_impl(size_t) override;
      bool is_quotient_obviously_finite_impl() override;
      bool is_quotient_obviously_infinite_impl() override;

      ////////////////////////////////////////////////////////////////////////////
      // Kambites - data - private
      ////////////////////////////////////////////////////////////////////////////

      std::unique_ptr<fpsemigroup::Kambites<std::string>> _k;
    };
  }  // namespace congruence

  // TODO(later) remove code dup with KBE.

  namespace detail {
    // This class is used to wrap libsemigroups::Kambites::string_type into an
    // object that can be used as generators for a FroidurePin object.
    class KE final {
      template <typename T>
      using Kambites    = fpsemigroup::Kambites<T>;
      using string_type = std::string;

      KE(string_type const&);
      KE(string_type&&);

     public:
      KE()          = default;
      KE(KE const&) = default;
      KE(KE&&)      = default;
      KE& operator=(KE const&) = default;
      KE& operator=(KE&&) = default;
      ~KE()               = default;

      template <typename T>
      KE(fpsemigroup::Kambites<T>& k, string_type const& w)
          : _string(k.normal_form(w)) {}

      template <typename T>
      KE(fpsemigroup::Kambites<T>& k, string_type&& w)
          : _string(k.normal_form(std::move(w))) {}

      template <typename T>
      KE(fpsemigroup::Kambites<T>& k, letter_type const& a)
          : KE(k, k.alphabet(a)) {}

      template <typename T>
      KE(fpsemigroup::Kambites<T>& k, word_type const& w) : KE() {
        detail::word_to_string(k.alphabet(), w, _string);
        _string = k.normal_form(_string);
      }

      template <typename T>
      word_type word(Kambites<T> const& k) const {
        return k.string_to_word(_string);
      }

      // Construct from letters or words

      bool operator==(KE const&) const;
      bool operator<(KE const&) const;
      void swap(KE&);

      string_type const& string() const noexcept;

      friend std::ostringstream& operator<<(std::ostringstream& os,
                                            KE const&           KE) {
        os << KE.string();
        return os;
      }

     private:
      string_type _string;
    };

    // The following are not really required but are here as a reminder that
    // KE are used in BruidhinnTraits which depends on the values in the
    // static_asserts below.
    static_assert(!std::is_trivial<KE>::value, "KE is not trivial!!!");
    static_assert(std::integral_constant<bool, (sizeof(KE) <= 32)>::value,
                  "KE's sizeof exceeds 32!!");

  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Adapters for KE class
  ////////////////////////////////////////////////////////////////////////

  template <>
  struct Complexity<detail::KE> {
    constexpr size_t operator()(detail::KE const&) const noexcept {
      return LIMIT_MAX;
    }
  };

  template <>
  struct Degree<detail::KE> {
    constexpr size_t operator()(detail::KE const&) const noexcept {
      return 0;
    }
  };

  template <>
  struct IncreaseDegree<detail::KE> {
    void operator()(detail::KE const&) const noexcept {}
  };

  template <>
  struct One<detail::KE> {
    detail::KE operator()(detail::KE const&) const noexcept {
      return detail::KE();
    }

    detail::KE operator()(size_t = 0) const noexcept {
      return detail::KE();
    }
  };

  template <>
  struct Product<detail::KE> {
    template <typename T>
    void operator()(detail::KE&               xy,
                    detail::KE const&         x,
                    detail::KE const&         y,
                    fpsemigroup::Kambites<T>* kb,
                    size_t) {
      std::string w(x.string());  // string_type
      w += y.string();
      xy = detail::KE(*kb, w);
    }
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template <>
  word_type FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE, fpsemigroup::Kambites<std::string>>>::
      factorisation(detail::KE const& x);

  template <>
  word_type FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE,
                        fpsemigroup::Kambites<detail::MultiStringView>>>::
      factorisation(detail::KE const& x);

  template <>
  tril FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE,
                        fpsemigroup::Kambites<std::string>>>::is_finite() const;

  template <>
  tril FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE,
                        fpsemigroup::Kambites<detail::MultiStringView>>>::
      is_finite() const;
#endif
}  // namespace libsemigroups

////////////////////////////////////////////////////////////////////////
// Specializations of std::hash and std::equal_to
////////////////////////////////////////////////////////////////////////

namespace std {
  template <>
  struct hash<libsemigroups::detail::KE> {
    size_t operator()(libsemigroups::detail::KE const& x) const {
      return hash<string>()(x.string());
    }
  };

  template <>
  struct equal_to<libsemigroups::detail::KE> {
    bool operator()(libsemigroups::detail::KE const& x,
                    libsemigroups::detail::KE const& y) const {
      return x == y;
    }
  };
}  // namespace std

#endif  // LIBSEMIGROUPS_KAMBITES_HPP_
