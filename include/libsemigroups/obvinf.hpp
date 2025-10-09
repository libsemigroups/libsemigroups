//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2025 James D. Mitchell + Reinis Cirpons
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

// This file contains a helper class for checking whether or not a congruence
// defined by generating pairs or finitely presented semigroup or monoid is
// obviously infinite.

#ifndef LIBSEMIGROUPS_OBVINF_HPP_
#define LIBSEMIGROUPS_OBVINF_HPP_

#include <cstddef>  // for size_t
#include <numeric>  // for accumulate
#include <string>   // for string
#include <utility>  // for pair
#include <vector>   // for vector

#include "config.hpp"              // for LIBSEMIGROUPS_EIGEN_ENABLED
#include "ranges.hpp"              // for rx/ranges
#include "types.hpp"               // for word_type etc
#include "word-graph-helpers.hpp"  // for word_graph
#include "word-graph.hpp"          // for is_acyclic
#include "word-range.hpp"          // for ToWord

#include "detail/eigen.hpp"  // for eigen
#include "detail/uf.hpp"     // for Duf

namespace libsemigroups {
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN

  namespace detail {
    template <typename Rewriter, typename ReductionOrder>
    class KnuthBendixImpl;  // forward decl
    class ToddCoxeterImpl;  // forward decl
  }                         // namespace detail

  template <typename Word>
  class Congruence;  // forward decl

  template <typename Word>
  class Kambites;  // forward decl

  template <typename Word>
  class Presentation;  // forward decl
#endif

  namespace presentation {
    template <typename Word>
    void change_alphabet(Presentation<Word>&, Word const&);
  }

  //! \defgroup obvinf_group Obviously infinite
  //!
  //! \brief Functions for checking if a finitely presented semigroup or monoid
  //! is obviously infinite.
  //!
  //! This page collects the documentation for the functionality in
  //! `libsemigroups` for checking if a finitely presented semigroup or monoid
  //! is obviously infinite.
  //!
  //! If `libsemigroups` is compiled with the flag `--enable-eigen`, then the
  //! functions and classes documented on this page may make use of the Eigen
  //! library for linear algebra (see \cite Guennebaud2010aa).

  //! \ingroup obvinf_group
  //!
  //! \brief Class for checking if a finitely presented semigroup or monoid is
  //! obviously infinite.
  //!
  //! This class implements a number of checks whether or not a finitely
  //! presented semigroup or monoid is infinite. These checks are all decidable,
  //! and always return an answer within an amount of time that is linear in the
  //! size of the input.
  //!
  //! These checks are:
  //!
  //!  1. For every generator there is at least one side of one relation that
  //!     consists solely of that generator. If this condition is not met, then
  //!     there is a generator of infinite order.
  //!
  //!  2. The number of occurrences of every generator is not preserved by the
  //!     relations. Otherwise, it is not possible to use the relations to
  //!     reduce the number of occurrences of a generator in a word, and so
  //!     there are infinitely many distinct words.
  //!
  //!  3. The number of generators on the left hand side of a relation is not
  //!     the same as the number of generators on the right hand side for at
  //!     least one generator. Otherwise the relations preserve the length of
  //!     any word and so there are infinitely many distinct words.
  //!
  //!  4. There are at least as many relations as there are generators.
  //!     Otherwise we can find a surjective homomorphism onto an infinite
  //!     subsemigroup of the rationals under addition.
  //!
  //!  5. The checks 2., 3. and 4. are a special case of a more general matrix
  //!     based condition. We construct a matrix whose columns correspond to
  //!     generators and rows correspond to relations. The (i, j)-th entry is
  //!     the number of occurrences of the j-th generator in the left hand side
  //!     of the i-th relation minus the number of occurrences of it on the
  //!     right hand side. If this matrix has a non-trivial kernel, then we can
  //!     construct a surjective homomorphism onto an infinite subsemigroup of
  //!     the rationals under addition. So we check that the matrix is full
  //!     rank.
  //!
  //!  6. The presentation is not that of a free product. To do this we consider
  //!     a graph whose vertices are generators and an edge connects two
  //!     generators if they occur on either side of the same relation. If this
  //!     graph is disconnected then the presentation is a free product and is
  //!     therefore infinite. Note that we currently do not consider the case
  //!     where the identity occurs in the presentation.
  //!
  //! \sa \ref is_obviously_infinite.
  // TODO(1) there are definitely some assumptions about the calls to the member
  // functions of an IsObviouslyInfinite (see for example the function
  // is_obviously_infinite for a Presentation). These should be documented.
  //
  // TODO(1) this class should be more generic, like detail::CongruenceCommon
  // and its derived classes, allowing arbitrary iterators of rules to be added
  class IsObviouslyInfinite {
    // The default constructor is private since an object that is default
    // constructed isn't usable with the current public API.
    IsObviouslyInfinite() = default;

   public:
    //! Alias for `std::vector<word_type>::const_iterator`.
    using const_iterator_word_type =
        typename std::vector<word_type>::const_iterator;

    //! \brief Alias for
    //! `std::vector< std::pair<std::string, std::string>>::const_iterator`.
    //!
    //! Alias for
    //! `std::vector< std::pair<std::string, std::string>>::const_iterator`.
    using const_iterator_pair_string = typename std::vector<
        std::pair<std::string, std::string>>::const_iterator;

    //! Alias for `std::vector<std::string>::const_iterator`.
    using const_iterator_string =
        typename std::vector<std::string>::const_iterator;

    //! \brief Construct from alphabet size.
    //!
    //! Constructs an empty IsObviouslyInfinite object representing a finitely
    //! presented semigroup or monoid with \p n generators.
    //!
    //! \param n the number of generators.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    explicit IsObviouslyInfinite(size_t n);

    //! Re-initialize the object as if it had just been constructed.
    //!
    //! Calling this function puts it into the same state that it would have
    //! been in if it had just been newly constructed with the same parameter
    //! \p n.
    //!
    //! This function exists to allow reuse of the memory allocated within the
    //! object.
    //!
    //! \param n the number of generators.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    IsObviouslyInfinite& init(size_t n);

    //! \brief Construct from alphabet.
    //!
    //! Constructs an empty IsObviouslyInfinite object representing a finitely
    //! presented semigroup or monoid with alphabet \p lphbt.
    //!
    //! \param lphbt the alphabet to use.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    explicit IsObviouslyInfinite(std::string const& lphbt)
        : IsObviouslyInfinite(lphbt.size()) {}

    //! Re-initialize the object as if it had just been constructed.
    //!
    //! Calling this function puts it into the same state that it would have
    //! been in if it had just been newly constructed with the same parameter
    //! \p n.
    //!
    //! This function exists to allow reuse of the memory allocated within the
    //! object.
    //!
    //! \param lphbt the alphabet to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    IsObviouslyInfinite& init(std::string const& lphbt) {
      return init(lphbt.size());
    }

    //! Deleted
    IsObviouslyInfinite(IsObviouslyInfinite const&) = delete;

    //! Deleted
    IsObviouslyInfinite(IsObviouslyInfinite&&) = delete;

    //! Deleted
    IsObviouslyInfinite& operator=(IsObviouslyInfinite const&) = delete;

    //! Deleted
    IsObviouslyInfinite& operator=(IsObviouslyInfinite&&) = delete;

    ~IsObviouslyInfinite();

    //! \brief Add rules from iterators to \ref word_type.
    //!
    //! This function adds the rules described by the iterators \p first and
    //! \p last.
    //!
    //! \param first iterator pointing at the left-hand-side of the first rule
    //! to add.
    //!
    //! \param last iterator pointing one beyond the right-hand-side of the last
    //! rule to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does not check its arguments.
    IsObviouslyInfinite& add_rules_no_checks(word_type const&,
                                             const_iterator_word_type first,
                                             const_iterator_word_type last);

    //! \brief Add rules from iterators to std::string.
    //!
    //! This function adds the rules described by the iterators \p first and
    //! \p last. The rules are translated to \ref word_type objects using the
    //! position of each character in the 1st argument \p lphbt.
    //!
    //! \param lphbt the alphabet to use.
    //!
    //! \param first iterator pointing at the left-hand-side of the first rule
    //! to add.
    //!
    //! \param last iterator pointing one beyond the right-hand-side of the last
    //! rule to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does not check its arguments.
    template <typename Char>
    IsObviouslyInfinite& add_rules_no_checks(
        std::basic_string<Char> const&                                lphbt,
        typename std::vector<std::basic_string<Char>>::const_iterator first,
        typename std::vector<std::basic_string<Char>>::const_iterator last) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto matrix_start = _matrix.rows();
      _matrix.conservativeResize(matrix_start + (last - first) / 2,
                                 Eigen::NoChange);
      _matrix.block(matrix_start, 0, (last - first) / 2, _matrix.cols())
          .setZero();
#else
      auto matrix_start = 0;
      std::fill(_matrix.begin(), _matrix.end(), 0);
#endif

      v4::ToWord stw(lphbt);
      word_type  lhs, rhs;
      for (auto it = first; it < last; ++it) {
        stw(lhs, *it++);  // lhs changed in-place
        stw(rhs, *it);    // rhs changed in-place
        private_add_rule(matrix_start + (it - first) / 2, lhs, rhs);
      }
      _nr_letter_components = _letter_components.number_of_blocks();
      return *this;
    }

    //! \brief Add rules from iterators to std::vector of std::string.
    //!
    //! This function adds the rules described by the iterators \p first and
    //! \p last. The rules are translated to \ref word_type objects using the
    //! position of each character in the 1st argument \p lphbt.
    //!
    //! \param lphbt the alphabet to use.
    //!
    //! \param first iterator pointing at the left-hand-side of the first rule
    //! to add.
    //!
    //! \param last iterator pointing one beyond the right-hand-side of the last
    //! rule to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does not check its arguments.
    IsObviouslyInfinite& add_rules_no_checks(
        char const*                                       lphbt,
        typename std::vector<std::string>::const_iterator first,
        typename std::vector<std::string>::const_iterator last) {
      std::vector<std::string> copy(first, last);
      return add_rules_no_checks(std::string(lphbt), copy.begin(), copy.end());
    }

    //! \brief Add rules from iterators to \ref word_type.
    //!
    //! This function adds the rules described by the iterators \p first and
    //! \p last.
    //!
    //! \param lphbt unused (for consistency of interface only).
    //! \param first iterator pointing at the left-hand-side of the first rule
    //! to add.
    //! \param last iterator pointing one beyond the right-hand-side of the last
    //! rule to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does not check its arguments.
    template <typename Char>
    IsObviouslyInfinite&
    add_rules_no_checks(std::basic_string<Char> const& lphbt,
                        const_iterator_word_type       first,
                        const_iterator_word_type       last) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto matrix_start = _matrix.rows();
      _matrix.conservativeResize(matrix_start + (last - first) / 2,
                                 Eigen::NoChange);
      _matrix.block(matrix_start, 0, (last - first) / 2, _matrix.cols())
          .setZero();
#else
      auto matrix_start = 0;
      std::fill(_matrix.begin(), _matrix.end(), 0);
#endif

      v4::ToWord              to_word(lphbt);
      word_type               lhs, rhs;
      std::basic_string<Char> tmp;
      for (auto it = first; it < last; ++it) {
        // changes lhs in-place
        tmp.assign(it->begin(), it->end());
        to_word(lhs, tmp);
        ++it;
        // changes rhs in-place
        tmp.assign(it->begin(), it->end());
        to_word(rhs, tmp);
        private_add_rule(matrix_start + (it - first) / 2, lhs, rhs);
      }
      _nr_letter_components = _letter_components.number_of_blocks();
      return *this;
    }

    //! \brief Add rules from iterators to std::pair of std::string.
    //!
    //! This function adds the rules described by the iterators \p first and
    //! \p last. The rules are translated to \ref word_type objects using the
    //! position of each character in the 1st argument \p lphbt.
    //!
    //! \param lphbt the alphabet to use.
    //!
    //! \param first iterator pointing at the left-hand-side of the first rule
    //! to add.
    //!
    //! \param last iterator pointing one beyond the right-hand-side of the last
    //! rule to add.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \warning
    //! This function does not check its arguments.
    IsObviouslyInfinite& add_rules_no_checks(std::string const&         lphbt,
                                             const_iterator_pair_string first,
                                             const_iterator_pair_string last);

    //! \brief Returns whether or not the finitely presented semigroup or monoid
    //! is obviously infinite.
    //!
    //! This function returns \c true if the finitely presented semigroup or
    //! monoid defined using the alphabet used to construct an
    //! IsObviouslyInfinite object, and with relations added to the
    //! IsObviouslyInfinite object by \ref add_rules_no_checks, is obviously
    //! infinite.
    //!
    //! \returns Whether or not the finitely presented semigroup or monoid is
    //! obviously infinite.
    bool result() const;

    // TODO(1) certificate() returning why the thing is obviously infinite

   private:
    void private_add_rule(size_t const, word_type const&, word_type const&);

    inline void letters_in_word(size_t row, word_type const& w, int64_t adv) {
      for (size_t const& x : w) {
        matrix(row, x) += adv;
        _seen[x] = true;
      }
    }

    inline void plus_letters_in_word(size_t row, word_type const& w) {
      letters_in_word(row, w, 1);
    }

    inline void minus_letters_in_word(size_t row, word_type const& w) {
      letters_in_word(row, w, -1);
    }

    inline int64_t& matrix(size_t row, size_t col) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      return _matrix(row, col);
#else
      (void) row;
      return _matrix[col];
#endif
    }

    inline bool matrix_row_sums_to_0(size_t row) {
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      return _matrix.row(row).sum() == 0;
#else
      (void) row;
      return std::accumulate(_matrix.cbegin(), _matrix.cend(), 0) == 0;
#endif
    }

    // letter_type i belongs to "preserve" if there exists a relation where
    // the number of occurrences of i is not the same on both sides of the
    // relation letter_type i belongs to "unique" if there is a relation
    // where one side consists solely of i.
    bool              _empty_word;
    detail::Duf<>     _letter_components;
    size_t            _nr_gens;
    size_t            _nr_letter_components;
    size_t            _nr_relations;
    bool              _preserve_length;
    std::vector<bool> _preserve;
    std::vector<bool> _seen;
    std::vector<bool> _unique;

#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
    Eigen::Matrix<int64_t, Eigen::Dynamic, Eigen::Dynamic> _matrix;
#else
    std::vector<int64_t> _matrix;
#endif
  };

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if the finitely presented semigroup or monoid
  //! defined by a Presentation object is obviously infinite or not.
  //!
  //! This function returns \c true if the finitely presented semigroup or
  //! monoid defined by the Presentation object \p p is obviously infinite. This
  //! function exists to make it simpler to call an IsObviouslyInfinite
  //! object a single time.
  //!
  //! \tparam Word the type of the words in the presentation \p p.
  //!
  //! \param p the presentation.
  //!
  //! \returns Whether or not the presentation defines an obviously infinite
  //! semigroup or monoid.
  //!
  //! \throws LibsemigroupsException If the presentation \p p is not valid.
  //!
  //! \note If this function returns \c false, it is still possible that
  //! semigroup or monoid defined by \p p is infinite.
  template <typename Word>
  bool is_obviously_infinite(Presentation<Word> const& p) {
    p.throw_if_bad_alphabet_or_rules();
    if (p.alphabet().empty()) {
      return false;
    }
    auto it
        = std::max_element(std::begin(p.alphabet()), std::end(p.alphabet()));

    if (*it != p.alphabet().size() - 1) {
      auto copy_p = p;
      presentation::change_alphabet(
          copy_p,
          rx::seq<typename Presentation<Word>::letter_type>(0)
              | rx::take(p.alphabet().size()) | rx::to_vector());
      IsObviouslyInfinite ioi(copy_p.alphabet().size());
      ioi.add_rules_no_checks(
          p.alphabet(), copy_p.rules.cbegin(), copy_p.rules.cend());
      return ioi.result();
    }

    IsObviouslyInfinite ioi(p.alphabet().size());
    ioi.add_rules_no_checks(p.alphabet(), p.rules.cbegin(), p.rules.cend());
    return ioi.result();
  }

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if the finitely presented semigroup or monoid
  //! defined by a Presentation object is obviously infinite or not.
  //!
  //! This function returns \c true if the finitely presented semigroup or
  //! monoid defined by the Presentation object \p p is obviously infinite. This
  //! function exists to make it simpler to call an IsObviouslyInfinite
  //! object a single time.
  //!
  //! \param p the presentation.
  //!
  //! \returns Whether or not the presentation defines an obviously infinite
  //! semigroup or monoid.
  //!
  //! \throws LibsemigroupsException If the presentation \p p is not valid.
  //!
  //! \note If this function returns \c false, it is still possible that the
  //! finitely presented semigroup or monoid defined by \p p is infinite.
  template <>
  bool is_obviously_infinite(Presentation<std::string> const& p);

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if the quotient of a finitely presented
  //! semigroup or monoid defined by a ToddCoxeterImpl object is obviously
  //! infinite or not.
  //!
  //! This function returns \c true if the quotient of the finitely presented
  //! semigroup or monoid defined by the ToddCoxeterImpl object \p tc is
  //! obviously infinite; \c false is returned if it is not.
  //!
  //! This function exists to make it simpler to call an
  //! IsObviouslyInfinite object a single time, and uses some information from
  //! the (possible incomplete) ToddCoxeterImpl object to assist in this
  //! determination.
  //!
  //! \param tc the ToddCoxeterImpl instance.
  //!
  //! \returns Whether or not the quotient defined by a ToddCoxeterImpl instance
  //! is obviously infinite.
  //!
  //! \note If this function returns \c false, it is still possible that the
  //! quotient defined by the ToddCoxeterImpl object \p tc is infinite.
  bool is_obviously_infinite(detail::ToddCoxeterImpl const& tc);

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if a congruence obviously has infinite many
  //! classes.
  //!
  //! This function returns \c true if the quotient of the finitely presented
  //! semigroup or monoid defined by the \ref_congruence object \p c is
  //! obviously infinite; \c false is returned if it is not.
  //!
  //! This function exists to make it simpler to call an IsObviouslyInfinite
  //! object a single time, and uses some information from the (possible
  //! incomplete) \ref_congruence object to assist in this determination.
  //!
  //! \param c the \ref_congruence instance.
  //!
  //! \returns Whether or not the congruence obviously has infinitely many
  //! classes.
  //!
  //! \note If this function returns \c false, it is still possible that the
  //! congruence has infinitely many classes.
  // This function is implemented in cong-class.tpp
  template <typename Word>
  bool is_obviously_infinite(Congruence<Word>& c);

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if the finitely presented semigroup or
  //! monoid defined by a \ref_kambites object obviously has infinite many
  //! classes.
  //!
  //! This function returns \c true if the finitely presented semigroup or
  //! monoid defined by a \ref_kambites object is obviously infinite; \c false
  //! is returned if it is not.
  //!
  //! This function exists to make it simpler to call an IsObviouslyInfinite
  //! object a single time, and uses some information from the (possible
  //! incomplete) \ref_kambites object to assist in this determination.
  //!
  //! \param k the \ref_kambites instance.
  //!
  //! \returns Whether or not the finitely presented semigroup or
  //! monoid defined by a \ref_kambites object is obviously infinite.
  //!
  //! \note If this function returns \c false, it is still possible that the
  //! finitely presented semigroup or monoid defined by \p k is infinite.
  template <typename Word>
  bool is_obviously_infinite(Kambites<Word>& k) {
    if (k.finished() && k.small_overlap_class() >= 3) {
      return true;
    }
    auto const&         p = k.presentation();
    IsObviouslyInfinite ioi(p.alphabet().size());
    ioi.add_rules_no_checks(p.alphabet(), p.rules.cbegin(), p.rules.cend());
    ioi.add_rules_no_checks(p.alphabet(),
                            k.internal_generating_pairs().cbegin(),
                            k.internal_generating_pairs().cend());
    if (ioi.result()) {
      return true;
    }
    return k.small_overlap_class() >= 3;
  }

  //! \ingroup obvinf_group
  //!
  //! \brief Function for checking if the quotient of a finitely presented
  //! semigroup or monoid defined by a \ref_knuth_bendix object is obviously
  //! infinite or not.
  //!
  //! This function returns \c true if the quotient of the finitely presented
  //! semigroup or monoid defined by the \ref_knuth_bendix object \p kb is
  //! obviously infinite; \c false is returned if it is not.
  //!
  //! This function exists to make it simpler to call an IsObviouslyInfinite
  //! object a single time, and uses some information from the (possible
  //! incomplete) \ref_knuth_bendix object to assist in this determination.
  //!
  //! \param kb the \ref_knuth_bendix instance.
  //!
  //! \returns Whether or not the quotient defined by a \ref_knuth_bendix
  //! instance is obviously infinite.
  //!
  //! \note If this function returns \c false, it is still possible that the
  //! quotient defined by the \ref_knuth_bendix object \p kb is infinite.
  template <typename Rewriter, typename ReductionOrder>
  bool
  is_obviously_infinite(detail::KnuthBendixImpl<Rewriter, ReductionOrder>& kb) {
    if (kb.finished()) {
      return !v4::word_graph::is_acyclic(kb.gilman_graph());
    }
    auto const& p = kb.internal_presentation();
    if (p.alphabet().empty()) {
      return false;
    }
    IsObviouslyInfinite ioi(p.alphabet().size());
    ioi.add_rules_no_checks(p.alphabet(), p.rules.cbegin(), p.rules.cend());
    ioi.add_rules_no_checks(p.alphabet(),
                            kb.internal_generating_pairs().cbegin(),
                            kb.internal_generating_pairs().cend());
    return ioi.result();
  }

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_OBVINF_HPP_
