//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains an implementation of the Schreier-Sims algorithm, as
// described in Section 4.4.2 of:
//
// D. Holt (with B. Eick and E. O'Brien), Handbook of computational group
// theory, CRC Press, 2004.
//
// and as implemented in the kernel module of the Digraphs package for GAP in
// C:
//
// https://github.com/digraphs/Digraphs/blob/master/src/schreier-sims.c
//
// by Wilf A. Wilson.

// TODO(later)
//
// 0. check use of:
//    * element_type []
//    * const_value_type []
//    * reference        []
//    * const_element_reference  []
//    * internal_element_type []
//    * internal_const_element_type []
//    * internal_reference []
//    * internal_const_reference []
// 1. iterator to the elements (Finn requires this)
// 2. stabilizer member func
// 3. change base
// 4. random version
// 5. try it with Digraphs
// 6. only member functions that cause mathematical changes should be non-const

// THIS FILE CAN BE USED AS HEADER ONLY

#ifndef LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
#define LIBSEMIGROUPS_SCHREIER_SIMS_HPP_

#include <array>          // for array
#include <cstddef>        // for size_t
#include <string>         // for operator+, basic_string
#include <unordered_set>  // for unordered_set

#include "adapters.hpp"          // for action, degree, inverse
#include "bruidhinn-traits.hpp"  // for detail::BruidhinnTraits
#include "config.hpp"            // for LIBSEMIGROUPS_HPCOMBI_ENABLED
#include "containers.hpp"        // for Array2, StaticVector2
#include "debug.hpp"             // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"         // for LIBSEMIGROUPS_EXCEPTION
#include "int-range.hpp"         // for IntegralRange
#include "stl.hpp"               // for EqualTo
#include "transf.hpp"            // for Perm
#include "types.hpp"             // for SmallestInteger

namespace libsemigroups {

  //! Defined in ``schreier-sims.hpp``.
  //!
  //! This is a traits class for use with the class template SchreierSims.
  //!
  //! This traits class contains stateless types which are used to adapt any
  //! class representing a permutation for the implementation of the
  //! Schreier-Sims algorithm in the SchreierSims class template.
  //!
  //! \tparam N the largest point not fixed by the permutations in the
  //! permutation group to be represented by a SchreierSims instance.
  //! \tparam TPointType the type of the points acted on.
  //! \tparam TElementType the type of the group elements acting on
  //! \c TPointType.
  template <size_t N, typename TPointType, typename TElementType>
  struct SchreierSimsTraits {
    //! The type of indices to be used by a SchreierSims instance.
    using index_type = size_t;

    //! Type of the object containing all points acted on.
    using domain_type = IntegralRange<TPointType, 0, N>;

    //! Type of the points acted on.
    //!
    //! The type of the points acted on by the group represented by \c this,
    //! which is the same as the template parameter \c TPointType.
    using point_type = TPointType;

    //! Type of the elements.
    using element_type = TElementType;

    //! \copydoc libsemigroups::ImageRightAction
    using Action = ::libsemigroups::ImageRightAction<TElementType, TPointType>;

    //! \copydoc libsemigroups::Degree
    using Degree = ::libsemigroups::Degree<element_type>;

    //! \copydoc libsemigroups::EqualTo
    using EqualTo = ::libsemigroups::EqualTo<element_type>;

    //! \copydoc libsemigroups::IncreaseDegree
    using Inverse = ::libsemigroups::Inverse<element_type>;

    //! \copydoc libsemigroups::One
    using One = ::libsemigroups::One<element_type>;

    //! \copydoc libsemigroups::Product
    using Product = ::libsemigroups::Product<element_type>;

    //! \copydoc libsemigroups::Swap
    using Swap = ::libsemigroups::Swap<element_type>;
  };

  //! Defined in ``schreier-sims.hpp``.
  //!
  //! This class implements a deterministic version of the Schreier-Sims
  //! algorithm acting on a relatively small number of points (< 1000).
  //!
  //! \tparam N the largest point not fixed by the permutations in the
  //! permutation group to be represented by this.
  //! \tparam TPointType the type of the points acted on (default:
  //! the member \c type of SmallestInteger with template parameter \p N).
  //! \tparam TElementType the type of the group elements acting on
  //! \c TPointType (default: the member \c type of \ref LeastPerm with template
  //! parameter \p N).
  //! \tparam TTraits the type of traits object (default: SchreierSimsTraits
  //! with template parameters \c N, \c TPointType, and \c TElementType).
  //!
  //! \sa SchreierSimsTraits.
  //!
  //! \par Example
  //! \code
  //!  SchreierSims<5> S;
  //!  using Perm = decltype(S)::element_type;
  //!  S.add_generator(Perm({1, 0, 2, 3, 4}));
  //!  S.add_generator(Perm({1, 2, 3, 4, 0}));
  //!  S.size(); // 120
  //!  \endcode
  template <size_t N,
            typename TPointType   = typename SmallestInteger<N>::type,
            typename TElementType = LeastPerm<N>,
            typename TTraits = SchreierSimsTraits<N, TPointType, TElementType>>
  class SchreierSims final : private detail::BruidhinnTraits<TElementType> {
    static_assert(
        std::is_same<TPointType, typename TTraits::point_type>::value,
        "incompatible point types, TTraits::point_type and TPointType "
        "must be the same");
    static_assert(
        std::is_same<TElementType, typename TTraits::element_type>::value,
        "incompatible element types, TTraits::element_type and TElementType "
        "must be the same");

    using const_element_reference =
        typename detail::BruidhinnTraits<TElementType>::const_reference;
    using internal_element_type =
        typename detail::BruidhinnTraits<TElementType>::internal_value_type;
    using internal_const_element_type = typename detail::BruidhinnTraits<
        TElementType>::internal_const_value_type;

   public:
    //! Type of the elements.
    //!
    //! The type of the elements of a SchreierSims instance with const removed,
    //! and if \c TElementType is a pointer to const, then the second const is
    //! also removed.
    using element_type =
        typename detail::BruidhinnTraits<TElementType>::value_type;

    //! Type of the points acted on.
    //!
    //! Also the template parameter \p TPointType.
    using point_type = TPointType;

    //! Type of the object containing all points acted on.
    using domain_type = typename TTraits::domain_type;

    //! Type of indices.
    using index_type = typename TTraits::index_type;

    //! Alias for \p TTraits::Action.
    //!
    //! See Action for further details.
    using Action = typename TTraits::Action;

    //! \copydoc libsemigroups::Degree
    using Degree = typename TTraits::Degree;
    //! \copydoc libsemigroups::EqualTo
    using EqualTo = typename TTraits::EqualTo;
    //! \copydoc libsemigroups::Inverse
    using Inverse = typename TTraits::Inverse;
    //! \copydoc libsemigroups::One
    using One = typename TTraits::One;
    //! \copydoc libsemigroups::Product
    using Product = typename TTraits::Product;
    //! \copydoc libsemigroups::Swap
    using Swap = typename TTraits::Swap;

   private:
    struct InternalEqualTo : private detail::BruidhinnTraits<TElementType> {
      bool operator()(internal_const_element_type x,
                      internal_const_element_type y) const {
        return EqualTo()(this->to_external_const(x),
                         this->to_external_const(y));
      }
    };

   public:
    //! Default constructor.
    //!
    //! Construct a SchreierSims object representing the trivial group.
    //!
    //! \complexity
    //! \f$O(N ^ 2)\f$ where \p N is the first template parameter.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    SchreierSims()
        : _base(),
          _base_size(0),
          _domain(),
          _finished(false),
          _one(this->to_internal(One()(N))),
          _orbits(),
          _orbits_lookup(),
          _strong_gens(),
          _tmp_element1(this->internal_copy(_one)),
          _tmp_element2(this->internal_copy(_one)),
          _transversal(),
          _inversal() {
      init();
    }

    ~SchreierSims() {
      clear();
      this->internal_free(_one);
      this->internal_free(_tmp_element1);
      this->internal_free(_tmp_element2);
    }

    //! Default move constructor.
    SchreierSims(SchreierSims&&) = default;

    //! Deleted.
    SchreierSims(SchreierSims const&) = delete;

    //! Deleted.
    SchreierSims& operator=(SchreierSims const&) = delete;

    //! Deleted.
    SchreierSims& operator=(SchreierSims&&) = delete;

    //! Add a generator.
    //!
    //! \param x a const reference to the generator to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if the degree of \p x is not equal to
    //! the first template parameter \c N.
    //!
    //! \complexity
    //! Constant
    void add_generator(const_element_reference x) {
      if (!has_valid_degree(x)) {
        LIBSEMIGROUPS_EXCEPTION(
            "generator degree incorrect, expected %llu, got %llu",
            uint64_t(N),
            uint64_t(Degree()(x)));
      } else if (!contains(x)) {
        _finished = false;
        _strong_gens.push_back(0, this->internal_copy(_tmp_element2));
      }
    }

    //! Get a generator.
    //!
    //! \param index the index of the generator we want.
    //!
    //! \returns
    //! A const reference to the generator of \c this with index \p index.
    //!
    //! \throws LibsemigroupsException if the \p index is out of bounds.
    //!
    //! \complexity
    //! Constant.
    const_element_reference generator(index_type index) const {
      return strong_generator(0, index);
    }

    //! The number of generators.
    //!
    //! \returns
    //! The number of generators, a value of \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    size_t number_of_generators() const noexcept {
      return number_of_strong_generators(0);
    }

    //! The number of strong generators at a given depth.
    //!
    //! \param depth the depth.
    //!
    //! \returns
    //! The number of strong generators, a value of \c size_t, at depth \p
    //! depth of the stabiliser chain.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    size_t number_of_strong_generators(index_type depth) const {
      if (_base_size == 0) {
        return 0;
      } else if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "depth out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(_base_size),
            uint64_t(depth));
      }
      return _strong_gens.size(depth);
    }

    //! Get a strong generator.
    //!
    //! \param depth the depth.
    //! \param index the index of the generator we want.
    //!
    //! \returns
    //! A const reference to the strong generator of \c this at depth \p depth
    //! and with index \p index.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds.
    //! \throws LibsemigroupsException if the \p index is out of bounds.
    //!
    //! \complexity
    //! Constant.
    const_element_reference strong_generator(index_type depth,
                                             index_type index) const {
      if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "depth out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(_base_size),
            uint64_t(depth));
      }
      if (index >= _strong_gens.size(depth)) {
        LIBSEMIGROUPS_EXCEPTION(
            "index out of bounds, expected value in range (0, %llu], got %llu",
            uint64_t(_strong_gens.size(depth)),
            uint64_t(index));
      }
      return this->to_external_const(_strong_gens.at(depth, index));
    }

    //! Get a transversal element.
    //!
    //! \param depth the depth.
    //! \param pt the image of the base point under the traversal.
    //!
    //! \returns
    //! A const reference to the transversal element of \c this at depth
    //! \p depth moving the corresponding basepoint to the point \p pt.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds.
    //! \throws LibsemigroupsException if \p pt is not in the orbit of the
    //! basepoint.
    //!
    //! \complexity
    //! Constant.
    const_element_reference transversal_element(index_type depth,
                                                point_type pt) {
      if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "depth out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(_base_size),
            uint64_t(depth));
      }
      if (pt >= N) {
        LIBSEMIGROUPS_EXCEPTION(
            "pt out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(N),
            uint64_t(pt));
      }
      if (!_orbits_lookup[depth][pt]) {
        LIBSEMIGROUPS_EXCEPTION("no element maps %llu to %llu at depth %llu",
                                uint64_t(_base[depth]),
                                uint64_t(pt),
                                uint64_t(depth));
      }
      return this->to_external_const(_transversal[depth][pt]);
    }

    //! Get an inversal element.
    //!
    //! \param depth the depth.
    //! \param pt the point to map to the base point under the inversal.
    //!
    //! \returns
    //! A const reference to the insversal element of \c this at depth
    //! \p depth moving the corresponding point \p pt to the basepoint.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds.
    //! \throws LibsemigroupsException if \p pt is not in the orbit of the
    //! basepoint.
    //!
    //! \complexity
    //! Constant.
    const_element_reference inversal_element(index_type depth, point_type pt) {
      if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "depth out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(_base_size),
            uint64_t(depth));
      }
      if (pt >= N) {
        LIBSEMIGROUPS_EXCEPTION(
            "pt out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(N),
            uint64_t(pt));
      }
      if (!_orbits_lookup[depth][pt]) {
        LIBSEMIGROUPS_EXCEPTION("no element maps %llu to %llu at depth %llu",
                                uint64_t(_base[depth]),
                                uint64_t(pt),
                                uint64_t(depth));
      }
      return this->to_external_const(_inversal[depth][pt]);
    }

    //! Check if a point is in the orbit of a basepoint.
    //!
    //! \param depth the depth.
    //! \param pt the point.
    //!
    //! \returns
    //! A boolean indicating if the point \p pt is in the orbit of the
    //! basepoint of \c this at depth \p depth.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds or if
    //! \p pt is out of bounds.
    //!
    //! \complexity
    //! Constant.
    bool orbits_lookup(index_type depth, point_type pt) {
      if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "depth out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(_base_size),
            uint64_t(depth));
      }
      if (pt >= N) {
        LIBSEMIGROUPS_EXCEPTION(
            "pt out of bounds, expected value in range [0, %llu), got %llu",
            uint64_t(N),
            uint64_t(pt));
      }
      return _orbits_lookup[depth][pt];
    }

    //! Check if any generators have been added so far.
    //!
    //! \returns \c true if `number_of_generators() == 0` and \c false
    //! otherwise.
    //!
    //! \parameters
    //! (None)
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // Not noexcept because StaticVector2::size isn't
    bool empty() {
      return _strong_gens.size(0) == 0;
    }

    //! Returns the size of the group represented by this.
    //!
    //! \returns the size, a value of \c uint64_t.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    uint64_t size() {
      if (empty()) {
        return 1;
      }
      run();
      uint64_t out = 1;
      for (index_type i = 0; i < _base_size; i++) {
        out *= _orbits.size(i);
      }
      return out;
    }

    //! Sift an element through the stabiliser chain.
    //!
    //! \param x a const reference to a group element.
    //!
    //! \returns A value of type \ref element_type.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is not equal to
    //! the first template parameter \c N.
    element_type sift(const_element_reference x) {
      if (!has_valid_degree(x)) {
        LIBSEMIGROUPS_EXCEPTION(
            "element degree incorrect, expected %llu, got %llu",
            uint64_t(N),
            uint64_t(Degree()(x)));
      }
      element_type cpy = this->external_copy(x);
      Swap()(cpy, this->to_external(_tmp_element2));
      internal_sift();  // changes _tmp_element2 in place
      Swap()(cpy, this->to_external(_tmp_element2));
      return cpy;
    }

    //! Test membership of an element.
    //!
    //! \param x a const reference to the possible element.
    //!
    //! \returns A \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \note
    //! Returns \c false if the degree of \p x is not equal to the first
    //! template parameter \c N.
    bool contains(const_element_reference x) {
      if (!has_valid_degree(x)) {
        return false;
      }
      run();
      element_type cpy = this->external_copy(x);
      Swap()(cpy, this->to_external(_tmp_element2));
      this->external_free(cpy);
      internal_sift();  // changes _tmp_element2 in place
      return InternalEqualTo()(_tmp_element2, _one);
    }

    //! Returns a const reference to the identity.
    //!
    //! \returns A \c bool.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    const_element_reference identity() const {
      return this->to_external_const(_one);
    }

    //! Reset to the trivial group.
    //!
    //! Removes all generators, and orbits, and resets \c this so that it
    //! represents the trivial group.
    //!
    //! \returns
    //! (None)
    //!
    //! \complexity
    //! \f$O(N ^ 2)\f$ where \p N is the first template parameter.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    void clear() {
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < N; ++index) {
          if (_orbits_lookup[depth][index]) {
            this->internal_free(_transversal[depth][index]);
            this->internal_free(_inversal[depth][index]);
          }
        }
      }
      std::unordered_set<internal_element_type> deleted;
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < _strong_gens.size(depth); ++index) {
          if (deleted.find(_strong_gens.at(depth, index)) == deleted.end()) {
            this->internal_free(_strong_gens.at(depth, index));
            deleted.insert(_strong_gens.at(depth, index));
          }
        }
      }
      _strong_gens.clear();
      _orbits.clear();
      init();
    }

    //! Check if the stabiliser chain is fully enumerated.
    //!
    //! \returns
    //! \c true if the stabiliser chain is fully enumerated and \c false
    //! otherwise.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    bool finished() const {
      return _finished;
    }

    //! Add a base point to the stabiliser chain.
    //!
    //! \param pt the base point to add.
    //!
    //! \returns
    //! (None)
    //!
    //! \throws LibsemigroupsException if \p pt is out of range.
    //! \throws LibsemigroupsException if finished() returns \c true.
    //! \throws LibsemigroupsException if \p pt is already a base point.
    //!
    //! \complexity
    //! Linear in the current number of base points.
    void add_base_point(point_type pt) {
      if (pt >= N) {
        LIBSEMIGROUPS_EXCEPTION(
            "base point out of range, expected value in [0, %llu), got %llu",
            uint64_t(N),
            uint64_t(pt));
      } else if (finished()) {
        LIBSEMIGROUPS_EXCEPTION("cannot add further base points");
      } else {
        for (size_t i = 0; i < _base_size; ++i) {
          if (_base[i] == pt) {
            LIBSEMIGROUPS_EXCEPTION(
                "duplicate base point, %llu is already a base point",
                uint64_t(pt));
          }
        }
      }
      internal_add_base_point(pt);
    }

    //! Get a base point.
    //!
    //! \param index the index of the base point.
    //!
    //! \returns the base point with index \c index.
    //!
    //! \throws LibsemigroupsException if \p index is out of range.
    //!
    //! \complexity
    //! Constant.
    point_type base(index_type index) const {
      if (index >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION(
            "index out of bounds, expected value in range (0, %llu], got %llu",
            uint64_t(_base_size),
            uint64_t(index));
      }
      return _base[index];
    }

    //! Get the size of the current base.
    //!
    //! \returns
    //! A \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \parameters
    //! (None)
    size_t base_size() const noexcept {
      return _base_size;
    }

    //! Run the Schreier-Sims algorithm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(N^2\log^3|G|+|T|N^2\log|G|)\f$ time and
    //! \f$O(N^2\log|G|+|T|N)\f$ space,
    //! where \p N is the first template parameter, \f$|G|\f$ is the size of
    //! the group and \f$|T|\f$ is the number of generators of the group.
    //!
    //! \parameters
    //! (None)
    //!
    //! \returns
    //! (None)
    void run() {
      if (_finished || _strong_gens.size(0) == 0) {
        return;
      }

      for (index_type j = 0; j < _strong_gens.size(0); j++) {
        internal_const_element_type x = _strong_gens.at(0, j);
        index_type                  k = 0;
        while (k < _base_size
               && Action()(_base[k], this->to_external_const(x)) == _base[k]) {
          ++k;
        }
        if (k == _base_size) {  // all base points fixed
          point_type pt = *first_non_fixed_point(x);
          internal_add_base_point(pt);
        }
      }

      index_type first = _strong_gens.size(0) - 1;
      LIBSEMIGROUPS_ASSERT(first < N);
      for (index_type i = 1; i < _base_size + 1; i++) {
        point_type beta      = _base[i - 1];
        index_type old_first = _strong_gens.size(i);
        // set up the strong generators
        for (index_type j = first; j < _strong_gens.size(i - 1); j++) {
          internal_element_type x = _strong_gens.at(i - 1, j);
          if (beta == Action()(beta, this->to_external_const(x))) {
            _strong_gens.push_back(i, x);
          }
        }
        first = old_first;
        // find the orbit of <beta> under strong_gens[i - 1]
        orbit_enumerate(i - 1);
      }
      for (int i = _base_size - 1; i >= 0; --i) {
      start:
        for (auto it = _orbits.cbegin(i); it < _orbits.cend(i); ++it) {
          point_type beta = *it;
          for (index_type m = 0; m < _strong_gens.size(i); m++) {
            Product()(this->to_external(_tmp_element1),
                      this->to_external_const(_transversal[i][beta]),
                      this->to_external_const(_strong_gens.at(i, m)));
            point_type delta = Action()(
                beta, this->to_external_const(_strong_gens.at(i, m)));
            LIBSEMIGROUPS_ASSERT(
                delta
                == Action()(_base[i], this->to_external_const(_tmp_element1)));
            if (!InternalEqualTo()(_tmp_element1, _transversal[i][delta])) {
              Product()(this->to_external(_tmp_element2),
                        this->to_external_const(_tmp_element1),
                        this->to_external_const(_inversal[i][delta]));
              LIBSEMIGROUPS_ASSERT(
                  _base[i]
                  == Action()(_base[i],
                              this->to_external_const(_tmp_element2)));
              // internal_sift changes _tmp_element2 in-place
              index_type depth     = internal_sift();
              bool       propagate = false;
              if (depth < _base_size) {
                propagate = true;
              } else if (!InternalEqualTo()(_tmp_element2, _one)) {
                propagate = true;
                internal_add_base_point(*first_non_fixed_point(_tmp_element2));
              }
              if (propagate) {
                LIBSEMIGROUPS_ASSERT(i + 1 <= static_cast<int>(depth));
                _strong_gens.push_back(i + 1,
                                       this->internal_copy(_tmp_element2));
                orbit_add_gen(i + 1, _tmp_element2);
                for (index_type l = i + 2; l <= depth; l++) {
                  _strong_gens.push_back(l, _strong_gens.back(i + 1));
                  orbit_add_gen(l, _tmp_element2);
                  // add generator to orbit of base[l]
                }
                i = depth;
                goto start;
              }
            }
          }
        }
      }
      _finished = true;
    }

   private:
    void init() {
      _base_size = 0;
      _finished  = false;
      _orbits_lookup.fill(false);
    }

    bool has_valid_degree(const_element_reference x) const {
      return
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
          std::is_same<HPCombi::Perm16, element_type>::value ||
#endif
          Degree()(x) == N;
    }

    void internal_add_base_point(point_type pt) {
      LIBSEMIGROUPS_ASSERT(_base_size < N);
      _base[_base_size] = pt;
      _orbits.push_back(_base_size, pt);
      _orbits_lookup[_base_size][pt] = true;
      _transversal[_base_size][pt]   = this->internal_copy(_one);
      _inversal[_base_size][pt]      = this->internal_copy(_one);
      _base_size++;
    }

    void orbit_enumerate(index_type depth, index_type first = 0) {
      LIBSEMIGROUPS_ASSERT(depth < _base_size);
      for (index_type i = first; i < _orbits.size(depth); i++) {
        for (auto it = _strong_gens.cbegin(depth);
             it < _strong_gens.cend(depth);
             ++it) {
          orbit_add_point(depth, *it, _orbits.at(depth, i));
        }
      }
    }

    void orbit_add_gen(index_type depth, internal_element_type gen) {
      LIBSEMIGROUPS_ASSERT(depth < _base_size);
      // Apply the new generator to existing points in orbits[depth].
      index_type old_size_orbit = _orbits.size(depth);
      for (index_type i = 0; i < old_size_orbit; i++) {
        orbit_add_point(depth, gen, _orbits.at(depth, i));
      }
      orbit_enumerate(depth, old_size_orbit);
    }

    void orbit_add_point(index_type            depth,
                         internal_element_type x,
                         point_type            pt) {
      point_type img = Action()(pt, this->to_external_const(x));
      if (!_orbits_lookup[depth][img]) {
        _orbits.push_back(depth, img);
        _orbits_lookup[depth][img] = true;
        _transversal[depth][img]   = this->internal_copy(_one);
        Product()(this->to_external(_transversal[depth][img]),
                  this->to_external_const(_transversal[depth][pt]),
                  this->to_external_const(x));
        _inversal[depth][img] = this->to_internal(
            Inverse()(this->to_external_const(_transversal[depth][img])));
      }
    }

    // Changes _tmp_element2 in-place, and returns the depth reached in the
    // sifting.
    index_type internal_sift() {
      for (index_type depth = 0; depth < _base_size; ++depth) {
        point_type beta
            = Action()(_base[depth], this->to_external_const(_tmp_element2));
        if (!_orbits_lookup[depth][beta]) {
          return depth;
        }
        Product()(this->to_external(_tmp_element1),
                  this->to_external_const(_tmp_element2),
                  this->to_external_const(_inversal[depth][beta]));
        Swap()(this->to_external(_tmp_element2),
               this->to_external(_tmp_element1));
      }
      return _base_size;
    }

    typename domain_type::const_iterator
    first_non_fixed_point(internal_const_element_type x) {
      for (auto it = _domain.cbegin(); it < _domain.cend(); ++it) {
        if (*it != Action()(*it, this->to_external_const(x))) {
          return it;
        }
      }
      // It is currently not possible to add the identity as a generator since
      // add_generator checks containment and every group contains its identity
      // element.
      LIBSEMIGROUPS_ASSERT(false);
      return _domain.cend();
    }

    std::array<point_type, N>                       _base;
    index_type                                      _base_size;
    domain_type                                     _domain;
    bool                                            _finished;
    internal_element_type                           _one;
    detail::StaticVector2<point_type, N>            _orbits;
    detail::Array2<bool, N>                         _orbits_lookup;
    detail::StaticVector2<internal_element_type, N> _strong_gens;
    internal_element_type                           _tmp_element1;
    internal_element_type                           _tmp_element2;
    detail::Array2<internal_element_type, N>        _transversal;
    detail::Array2<internal_element_type, N>        _inversal;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
