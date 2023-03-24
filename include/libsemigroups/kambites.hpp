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

#include <algorithm>    // for min
#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t
#include <functional>   // for equal_to
#include <memory>       // for shared_ptr, unique_ptr, make_shared
#include <string>       // for string
#include <type_traits>  // for is_base_of, is_trivial
#include <utility>      // for pair
#include <vector>       // for vector

#include "adapters.hpp"       // for One
#include "cong-intf-new.hpp"  // for CongruenceInterface
#include "constants.hpp"      // for UNDEFINED, POSITIVE_INFINITY
#include "exception.hpp"      // for LIBSEMIGROUPS_EXCEPTION
#include "froidure-pin.hpp"   // for FroidurePin, FroidurePinTraits
#include "int-range.hpp"
#include "order.hpp"    // for lexicographical_compare
#include "present.hpp"  // for Presentation
#include "string-view.hpp"
#include "string.hpp"   // for is_prefix
#include "types.hpp"    // for word_type, tril, letter_type
#include "uf.hpp"       // for Duf<>
#include "ukkonen.hpp"  // for Ukkonen
#include "words.hpp"    // for word_to_string

namespace libsemigroups {
  class FroidurePinBase;  // Forward decl
  namespace detail {
    class KE;  // Forward decl
  }

  //! Defined in ``kambites.hpp``.
  //!
  //! On this page we describe the functionality relating to the algorithms
  //! for small overlap monoids by
  //! [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038) and the
  //! authors of ``libsemigroups``.
  //!
  //! This page describes the implementation in the class
  //! Kambites which uses the FpSemigroupInterface, which is
  //! also documented on this page.
  // TODO(later) example
  template <typename T>
  class Kambites : public Runner {
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
    Kambites();

    //! Default copy constructor.
    Kambites(Kambites const&);

    //! Default move constructor.
    Kambites(Kambites&&) = default;

    //! Default copy assignment operator.
    Kambites& operator=(Kambites const&) = default;

    //! Default move assignment operator.
    Kambites& operator=(Kambites&&) = default;

    ~Kambites();

    Kambites(Presentation<std::string> const& p) : Runner(), _presentation(p) {
      _presentation.validate();
      for (auto const& r : _presentation.rules) {
        _relation_words.push_back(r);
        _suffix_tree.add_word_no_checks(r.cbegin(), r.cend());
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - public
    ////////////////////////////////////////////////////////////////////////

    //! \copydoc FpSemigroupInterface::size
    //!
    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, throws
    size_t size() {
      validate_small_overlap_class();
      return POSITIVE_INFINITY;
    }

    //! \copydoc FpSemigroupInterface::equal_to
    //!
    //! \throws LibsemigroupsException if the small overlap class is not at
    //! least \f$4\f$.
    // Not noexcept, throws
    bool equal_to(string_type const& u, string_type const& v) {
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
    string_type normal_form(string_type const& w);

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
    auto const& ukkonen() noexcept {
      run();
      return _suffix_tree;
    }

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
             internal_type_iterator const& last) const;

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

    size_t Z_active_proper_complement(size_t                        i,
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
    void normal_form_inner(size_t& r, internal_type& v, internal_type& w) const;

    ////////////////////////////////////////////////////////////////////////
    // FpSemigroupInterface - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void run_impl() override {
      small_overlap_class();
    }

    bool finished_impl() const override {
      return _have_class && small_overlap_class() >= 4;
    }

    // TODO rm
    // void add_rule_impl(std::string const& u, std::string const& v) override;

    // TODO rm
    // std::shared_ptr<FroidurePinBase> froidure_pin_impl() override;

    // TODO impl elsewhere
    // bool is_obviously_infinite_impl() override {
    //   return small_overlap_class() >= 3;
    // }

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

    Presentation<std::string> _presentation;
    std::vector<string_type>  _relation_words;  // TODO this should be required
                                                // any longer
    Ukkonen _suffix_tree;
  };

  // namespace congruence {
  //   //! Defined in ``kambites.hpp``.
  //   //!
  //   //! On this page we describe the functionality relating to the algorithms
  //   //! for small overlap monoids by
  //   //! [Kambites](https://doi.org/10.1016/j.jalgebra.2008.09.038) and the
  //   //! authors of ``libsemigroups``.
  //   //!
  //   //! This page describes the implementation in the class
  //   //! congruence::Kambites which uses the CongruenceInterface, which is
  //   //! also documented on this page.
  //   class Kambites final : public CongruenceInterface {
  //    public:
  //     ////////////////////////////////////////////////////////////////////////////
  //     // Kambites - constructors - public
  //     ////////////////////////////////////////////////////////////////////////////

  //     //! Default constructor.
  //     //!
  //     //! \par Parameters
  //     //! (None)
  //     //!
  //     //! \exceptions
  //     //! \no_libsemigroups_except
  //     //!
  //     //! \complexity
  //     //! Constant
  //     Kambites();

  //     //! Construct from Kambites.
  //     //!
  //     //! This function constructs a new congruence::Kambites object
  //     //! initialised with existing data in Kambites.
  //     //!
  //     //! \param copy the Kambites.
  //     //!
  //     //! \exceptions
  //     //! \no_libsemigroups_except
  //     //!
  //     //! \complexity
  //     //! Constant
  //     explicit Kambites(Kambites<std::string> const& copy);

  //     //! Default copy constructor.
  //     Kambites(Kambites const& copy) : Kambites(*copy._k) {}

  //     //! Deleted.
  //     Kambites(Kambites&&) = delete;

  //     //! Deleted.
  //     Kambites& operator=(Kambites const&) = delete;

  //     //! Deleted.
  //     Kambites& operator=(Kambites&&) = delete;

  //     ~Kambites();

  //     ////////////////////////////////////////////////////////////////////////////
  //     // CongruenceInterface - non-pure virtual member functions - public
  //     ////////////////////////////////////////////////////////////////////////////

  //     tril const_contains(word_type const&, word_type const&) const override;
  //     bool contains(word_type const&, word_type const&) override;

  //     ////////////////////////////////////////////////////////////////////////////
  //     // Kambites - member functions - public
  //     ////////////////////////////////////////////////////////////////////////////

  //     //! Get the underlying Kambites.
  //     //!
  //     //! \par Parameters
  //     //! (None)
  //     //!
  //     //! \exceptions
  //     //! \no_libsemigroups_except
  //     //!
  //     //! \complexity
  //     //! Constant
  //     //!
  //     //! \returns
  //     //! A reference to the underlying Kambites of \c this.
  //     Kambites<std::string>& kambites() const {
  //       return *_k;
  //     }

  //    private:
  //     ////////////////////////////////////////////////////////////////////////////
  //     // Runner - pure virtual member functions - protected
  //     ////////////////////////////////////////////////////////////////////////////

  //     bool finished_impl() const override;

  //     ////////////////////////////////////////////////////////////////////////////
  //     // CongruenceInterface - pure virtual methods - private
  //     ////////////////////////////////////////////////////////////////////////////

  //     word_type class_index_to_word_impl(class_index_type) override;
  //     size_t    number_of_classes_impl() override;
  //     std::shared_ptr<FroidurePinBase> quotient_impl() override;
  //     class_index_type word_to_class_index_impl(word_type const&) override;
  //     void             run_impl() override;

  //     ////////////////////////////////////////////////////////////////////////////
  //     // CongruenceInterface - non-pure virtual methods - private
  //     ////////////////////////////////////////////////////////////////////////////

  //     void add_pair_impl(word_type const&, word_type const&) override;
  //     void set_number_of_generators_impl(size_t) override;
  //     bool is_quotient_obviously_finite_impl() override;
  //     bool is_quotient_obviously_infinite_impl() override;

  //     ////////////////////////////////////////////////////////////////////////////
  //     // Kambites - data - private
  //     ////////////////////////////////////////////////////////////////////////////

  //     std::unique_ptr<Kambites<std::string>> _k;
  //   };
  // }  // namespace congruence

  // TODO(later) remove code dup with KBE.

  namespace detail {
    // This class is used to wrap libsemigroups::Kambites::string_type into an
    // object that can be used as generators for a FroidurePin object.
    class KE final {
      using string_type = std::string;

      KE(string_type const&);
      KE(string_type&&);

     public:
      KE()                     = default;
      KE(KE const&)            = default;
      KE(KE&&)                 = default;
      KE& operator=(KE const&) = default;
      KE& operator=(KE&&)      = default;
      ~KE()                    = default;

      template <typename T>
      KE(Kambites<T>& k, string_type const& w) : _string(k.normal_form(w)) {}

      template <typename T>
      KE(Kambites<T>& k, string_type&& w)
          : _string(k.normal_form(std::move(w))) {}

      template <typename T>
      KE(Kambites<T>& k, letter_type const& a) : KE(k, k.alphabet(a)) {}

      template <typename T>
      KE(Kambites<T>& k, word_type const& w) : KE() {
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
    void operator()(detail::KE&       xy,
                    detail::KE const& x,
                    detail::KE const& y,
                    Kambites<T>*      kb,
                    size_t) {
      std::string w(x.string());  // string_type
      w += y.string();
      xy = detail::KE(*kb, w);
    }
  };

#ifndef DOXYGEN_SHOULD_SKIP_THIS
  template <>
  word_type FroidurePin<detail::KE,
                        FroidurePinTraits<detail::KE, Kambites<std::string>>>::
      factorisation(detail::KE const& x);

  template <>
  word_type FroidurePin<
      detail::KE,
      FroidurePinTraits<detail::KE, Kambites<detail::MultiStringView>>>::
      factorisation(detail::KE const& x);

  template <>
  tril
  FroidurePin<detail::KE,
              FroidurePinTraits<detail::KE, Kambites<std::string>>>::is_finite()
      const;

  template <>
  tril
  FroidurePin<detail::KE,
              FroidurePinTraits<detail::KE,
                                Kambites<detail::MultiStringView>>>::is_finite()
      const;
#endif
}  // namespace libsemigroups

#include "kambites.tpp"

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
