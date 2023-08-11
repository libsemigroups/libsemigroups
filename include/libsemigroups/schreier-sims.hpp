//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

// This file contains a declaration of the Schreier-Sims algorithm, as
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
//
// It also contains the declaration of a backtrack search for computing
// the intersection of two permutations given by Schreier-Sims algorithm by
// Reinis Cirpons.

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

// TODO:
// * move to tpp
// * noexcept
// * code coverage (can't do it on MacBook Pro at present)

#ifndef LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
#define LIBSEMIGROUPS_SCHREIER_SIMS_HPP_

#include <array>          // for array
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <iterator>       // for distance
#include <memory>         // for make_unique
#include <string>         // for operator+, basic_string
#include <type_traits>    // for is_same
#include <unordered_set>  // for unordered_set

#include "adapters.hpp"   // for action, degree, inverse
#include "config.hpp"     // for LIBSEMIGROUPS_HPCOMBI_ENABLED
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "transf.hpp"     // for Perm
#include "types.hpp"      // for SmallestInteger

#include "detail/bruidhinn-traits.hpp"  // for BruidhinnTraits
#include "detail/containers.hpp"        // for Array2, StaticTriVector2
#include "detail/int-range.hpp"         // for IntRange

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
  //! \tparam Point the type of the points acted on.
  //! \tparam Element the type of the group elements acting on
  //! \c Point.
  template <size_t N, typename Point, typename Element>
  struct SchreierSimsTraits {
    //! The type of indices to be used by a SchreierSims instance.
    using index_type = size_t;

    //! Type of the object containing all points acted on.
    using domain_type = detail::IntRange<Point>;

    //! Type of the points acted on.
    //!
    //! The type of the points acted on by the group represented by \c this,
    //! which is the same as the template parameter \c Point.
    using point_type = Point;

    //! Type of the elements.
    using element_type = Element;

    //! \copydoc libsemigroups::ImageRightAction
    using Action = ::libsemigroups::ImageRightAction<element_type, Point>;

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
  //! \tparam Point the type of the points acted on (default:
  //! the member \c type of SmallestInteger with template parameter \p N).
  //! \tparam Element the type of the group elements acting on
  //! \c Point (default: the member \c type of \ref LeastPerm with template
  //! parameter \p N).
  //! \tparam Traits the type of traits object (default: SchreierSimsTraits
  //! with template parameters \c N, \c Point, and \c Element).
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
            typename Point   = typename SmallestInteger<N>::type,
            typename Element = LeastPerm<N>,
            typename Traits  = SchreierSimsTraits<N, Point, Element>>
  class SchreierSims : private detail::BruidhinnTraits<Element> {
    static_assert(std::is_same<Point, typename Traits::point_type>::value,
                  "incompatible point types, Traits::point_type and Point "
                  "must be the same");
    static_assert(
        std::is_same<Element, typename Traits::element_type>::value,
        "incompatible element types, Traits::element_type and Element "
        "must be the same");

    using internal_element_type =
        typename detail::BruidhinnTraits<Element>::internal_value_type;
    using internal_const_element_type =
        typename detail::BruidhinnTraits<Element>::internal_const_value_type;

   public:
    //! Type of the elements.
    //!
    //! The type of the elements of a SchreierSims instance with const removed,
    //! and if \c Element is a pointer to const, then the second const is
    //! also removed.
    using element_type = typename detail::BruidhinnTraits<Element>::value_type;

    // TODO doc
    using const_element_reference =
        typename detail::BruidhinnTraits<Element>::const_reference;

    // TODO doc
    using element_reference =
        typename detail::BruidhinnTraits<Element>::reference;

    //! Type of the points acted on.
    //!
    //! Also the template parameter \p Point.
    using point_type = Point;

    //! Type of the object containing all points acted on.
    using domain_type = typename Traits::domain_type;

    //! Type of indices.
    using index_type = typename Traits::index_type;

    //! Alias for \p Traits::Action.
    //!
    //! See Action for further details.
    using Action = typename Traits::Action;
    //! \copydoc libsemigroups::Degree
    using Degree = typename Traits::Degree;
    //! \copydoc libsemigroups::EqualTo
    using EqualTo = typename Traits::EqualTo;
    //! \copydoc libsemigroups::Inverse
    using Inverse = typename Traits::Inverse;
    //! \copydoc libsemigroups::One
    using One = typename Traits::One;
    //! \copydoc libsemigroups::Product
    using Product = typename Traits::Product;
    //! \copydoc libsemigroups::Swap
    using Swap = typename Traits::Swap;

   private:
    // TODO replace Array2 by TriArray2 everywhere below
    std::array<point_type, N>                          _base;
    index_type                                         _base_size;
    domain_type                                        _domain;
    bool                                               _finished;
    internal_element_type                              _one;
    detail::StaticTriVector2<point_type, N>            _orbits;
    detail::Array2<bool, N>                            _orbits_lookup;
    detail::StaticTriVector2<internal_element_type, N> _strong_gens;
    mutable internal_element_type                      _tmp_element1;
    mutable internal_element_type                      _tmp_element2;
    detail::Array2<internal_element_type, N>           _transversal;
    detail::Array2<internal_element_type, N>           _inversal;

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
    // TODO to tpp
    SchreierSims()
        : _base(),
          _base_size(0),
          _domain(0, N),
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
    SchreierSims& init() {
      clear();
      _base_size = 0;
      _finished  = false;
      _orbits_lookup.fill(false);
      return *this;
    }

    // TODO to tpp
    ~SchreierSims() {
      clear();
      this->internal_free(_one);
      this->internal_free(_tmp_element1);
      this->internal_free(_tmp_element2);
    }

    //! Default move constructor.
    SchreierSims(SchreierSims&&) = default;

    // TODO doc
    // TODO to tpp
    // TODO this requires more tests
    SchreierSims(SchreierSims const& that)
        : _base(that._base),
          _base_size(that._base_size),
          _domain(that._domain),
          _finished(that._finished),
          _one(this->internal_copy(that._one)),
          _orbits(that._orbits),
          _orbits_lookup(that._orbits_lookup),
          _strong_gens(),
          _tmp_element1(this->internal_copy(_one)),
          _tmp_element2(this->internal_copy(_one)),
          _transversal(),
          _inversal() {
      init(that);
    }

    // TODO doc
    // TODO to tpp
    // TODO this requires more tests
    SchreierSims& operator=(SchreierSims const& that) {
      _base          = that._base;
      _base_size     = that._base_size;
      _domain        = that._domain;
      _finished      = that._finished;
      _one           = this->internal_copy(that._one);
      _orbits        = that._orbits;
      _orbits_lookup = that._orbits_lookup;
      _tmp_element1  = this->internal_copy(_one);
      _tmp_element2  = this->internal_copy(_one);

      _strong_gens.clear();
      _transversal.clear();
      _inversal.clear();

      init(that);

      return *this;
    }

    // TODO doc
    SchreierSims& operator=(SchreierSims&&) = default;

    //! Add a generator.
    //!
    //! This functions adds the argument \p x as a new generator if and only if
    //! \p x is not already an element of the group represented by the
    //! Schreier-Sims object.
    //!
    //! \param x a const reference to the generator to add.
    //!
    //! \returns
    //! \c true if \p x is added as a generator and \c false if it is not.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is not equal to
    //! the first template parameter \c N.
    //!
    //! \complexity
    //! Constant
    // TODO to tpp
    bool add_generator(const_element_reference x) {
      throw_if_bad_degree(x);
      if (contains(x)) {
        return false;
      }
      _finished = false;
      // FIXME push_back x not _tmp_element2!!
      _strong_gens.push_back(0, this->internal_copy(_tmp_element2));
      return true;
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
    [[nodiscard]] const_element_reference generator(index_type index) const {
      return strong_generator(0, index);
    }

    // TODO(doc)
    [[nodiscard]] const_element_reference
    generator_no_checks(index_type index) const noexcept {
      return strong_generator_no_checks(0, index);
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
    [[nodiscard]] size_t number_of_generators() const noexcept {
      if (_base_size == 0) {
        return 0;
      }
      LIBSEMIGROUPS_ASSERT(!_strong_gens.empty());
      return number_of_strong_generators_no_checks(0);
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
    [[nodiscard]] size_t number_of_strong_generators(index_type depth) const {
      throw_if_bad_depth(depth);
      return number_of_strong_generators_no_checks(depth);
    }

    // TODO doc
    [[nodiscard]] size_t
    number_of_strong_generators_no_checks(index_type depth) const noexcept {
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
    [[nodiscard]] const_element_reference
    strong_generator(index_type depth, index_type index) const {
      throw_if_bad_depth(depth);
      if (index >= _strong_gens.size(depth)) {
        LIBSEMIGROUPS_EXCEPTION("the 2nd argument is out of bounds, expected "
                                "value in range (0, {}], got {}",
                                _strong_gens.size(depth),
                                index);
      }
      return strong_generator_no_checks(depth, index);
    }

    // TODO(doc)
    [[nodiscard]] const_element_reference
    strong_generator_no_checks(index_type depth,
                               index_type index) const noexcept {
      return this->to_external_const(_strong_gens.at(depth, index));
    }

    // TODO(doc)
    [[nodiscard]] const_element_reference
    transversal_element_no_checks(index_type depth,
                                  point_type pt) const noexcept {
      return this->to_external_const(_transversal[depth][pt]);
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
    [[nodiscard]] const_element_reference
    transversal_element(index_type depth, point_type pt) const {
      throw_if_bad_depth(depth);
      throw_if_point_gt_degree(pt);
      throw_if_point_not_in_orbit(depth, pt);
      return transversal_element_no_checks(depth, pt);
    }

    // TODO(doc)
    [[nodiscard]] const_element_reference
    inversal_element_no_checks(index_type depth, point_type pt) const noexcept {
      return this->to_external_const(_inversal[depth][pt]);
    }

    //! Get an inverse of a transversal element.
    //!
    //! \param depth the depth.
    //! \param pt the point to map to the base point under the
    //! inverse_transversal_element.
    //!
    //! \returns
    //! A const reference to the inverse_transversal_element element of \c this
    //! at depth \p depth moving the corresponding point \p pt to the basepoint.
    //!
    //! \throws LibsemigroupsException if the \p depth is out of bounds.
    //! \throws LibsemigroupsException if \p pt is not in the orbit of the
    //! basepoint.
    //!
    //! \complexity
    //! Constant.
    // TODO to tpp
    [[nodiscard]] const_element_reference
    inverse_transversal_element(index_type depth, point_type pt) const {
      throw_if_bad_depth(depth);
      throw_if_point_gt_degree(pt);
      throw_if_point_not_in_orbit(depth, pt);
      return inversal_element_no_checks(depth, pt);
    }

    [[nodiscard]] bool orbit_lookup_no_checks(index_type depth,
                                              point_type pt) const noexcept {
      return _orbits_lookup[depth][pt];
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
    // TODO to tpp
    [[nodiscard]] bool orbit_lookup(index_type depth, point_type pt) const {
      throw_if_bad_depth(depth);
      throw_if_point_gt_degree(pt);
      return orbit_lookup_no_checks(depth, pt);
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
    // Not noexcept because StaticTriVector2::size isn't
    [[nodiscard]] bool empty() const {
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
    [[nodiscard]] uint64_t size() {
      // TODO(later) check if product overflows?
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

    // TODO noexcept?
    // TODO doc
    // TODO tests
    void sift_inplace_no_checks(element_reference x) const {
      // changes x in place, and uses _tmp_element1
      internal_sift(this->to_internal(x));
    }

    // TODO doc
    // TODO tests
    void sift_inplace(element_reference x) const {
      throw_if_bad_degree(x);
      sift_inplace_no_checks(x);
    }

    // TODO noexcept?
    const_element_reference sift_no_checks(const_element_reference x) const {
      this->to_external(_tmp_element2) = x;
      // changes x in place, and uses _tmp_element1
      internal_sift(_tmp_element2);
      return this->to_external_const(_tmp_element2);
    }

    //! Sift an element through the stabiliser chain.
    //!
    //! \param x a const reference to a group element.
    //!
    //! \returns A value of type \ref element_type.
    //!
    //! \throws LibsemigroupsException if the degree of \p x is not equal to
    //! the first template parameter \c N.
    [[nodiscard]] const_element_reference
    sift(const_element_reference x) const {
      throw_if_bad_degree(x);
      return sift_no_checks(x);
    }

    // TODO doc
    // At present I think this will always return false if finished()
    // isn't true, because we either run to the end or haven't done anything at
    // all
    [[nodiscard]] bool const_contains(const_element_reference x) const {
      if (!is_valid_degree(Degree()(x))) {
        return false;
      }
      auto const& y = sift_no_checks(x);
      return internal_equal_to(this->to_internal_const(y), _one);
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
    [[nodiscard]] bool contains(const_element_reference x) {
      if (is_valid_degree(Degree()(x))) {
        run();
        return const_contains(x);
      } else {
        return false;
      }
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
    // TODO noexcept?
    [[nodiscard]] const_element_reference identity() const {
      return this->to_external_const(_one);
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
    // TODO noexcept
    [[nodiscard]] bool finished() const {
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
      throw_if_point_gt_degree(pt);
      if (finished()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the Schreier-Sims algorithm has been run to completion already, "
            "cannot add further base points");
      }
      size_t m = std::distance(_base.cbegin(),
                               std::find(_base.cbegin(), _base.cend(), pt));
      if (m < _base_size) {
        LIBSEMIGROUPS_EXCEPTION("the argument {} (a point) equals item {} in "
                                "the existing base, cannot add it again",
                                pt,
                                m);
      }
      // TODO rename add_base_point_no_checks
      internal_add_base_point(pt);
    }

    [[nodiscard]] point_type base_no_checks(index_type index) const noexcept {
      return _base[index];
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
    [[nodiscard]] point_type base(index_type index) const {
      throw_if_bad_depth(index);
      return base_no_checks(index);
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
    [[nodiscard]] size_t base_size() const noexcept {
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
            if (!internal_equal_to(_tmp_element1, _transversal[i][delta])) {
              Product()(this->to_external(_tmp_element2),
                        this->to_external_const(_tmp_element1),
                        this->to_external_const(_inversal[i][delta]));
              LIBSEMIGROUPS_ASSERT(
                  _base[i]
                  == Action()(_base[i],
                              this->to_external_const(_tmp_element2)));
              // internal_sift changes _tmp_element2 in-place
              index_type depth     = internal_sift(_tmp_element2);
              bool       propagate = false;
              if (depth < _base_size) {
                propagate = true;
              } else if (!internal_equal_to(_tmp_element2, _one)) {
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
    ////////////////////////////////////////////////////////////////////////
    // SchreierSims - validation - private
    ////////////////////////////////////////////////////////////////////////

    bool is_valid_degree(point_type x) const {
      return
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
          std::is_same<HPCombi::Perm16, element_type>::value ||
#endif
          x == N;
    }

    void throw_if_bad_degree(const_element_reference x,
                             std::string const&      arg_pos = "1st") const {
      auto M = Degree()(x);
      if (!is_valid_degree(M)) {
        LIBSEMIGROUPS_EXCEPTION("the degree of the {} argument (an element) is "
                                "incorrect, expected {} got {}",
                                arg_pos,
                                N,
                                M);
      }
    }

    void throw_if_bad_depth(size_t             depth,
                            std::string const& arg_pos = "1st") const {
      if (depth >= _base_size) {
        LIBSEMIGROUPS_EXCEPTION("the {} argument (depth) is out of bounds, "
                                "expected a value in range [0, {}) got {}",
                                arg_pos,
                                _base_size,
                                depth);
      }
    }

    void throw_if_point_gt_degree(point_type         pt,
                                  std::string const& arg_pos = "1st") const {
      if (pt >= N) {
        LIBSEMIGROUPS_EXCEPTION("the {} argument (a point) is out of bounds, "
                                "expected a value in range [0, {}) got {}",
                                arg_pos,
                                N,
                                pt);
      }
    }

    void throw_if_point_not_in_orbit(index_type         depth,
                                     point_type         pt,
                                     std::string const& depth_arg_pos = "1st",
                                     std::string const& pt_arg_pos
                                     = "2nd") const {
      LIBSEMIGROUPS_ASSERT(depth < N);
      LIBSEMIGROUPS_ASSERT(pt < N);
      if (!_orbits_lookup[depth][pt]) {
        LIBSEMIGROUPS_EXCEPTION(
            "the {} argument {} (a point) does not belong "
            "to the orbit specified by the {} argument {} (depth)",
            pt_arg_pos,
            pt,
            depth_arg_pos,
            depth);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // SchreierSims - member functions - private
    ////////////////////////////////////////////////////////////////////////

    bool internal_equal_to(internal_const_element_type x,
                           internal_const_element_type y) const {
      return EqualTo()(this->to_external_const(x), this->to_external_const(y));
    }

    // Used by copy constructor and assignment operator.
    void init(SchreierSims const& that) {
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < N; ++index) {
          if (that._orbits_lookup[depth][index]) {
            _transversal[depth][index]
                = this->internal_copy(that._transversal[depth][index]);
            _inversal[depth][index]
                = this->internal_copy(that._inversal[depth][index]);
          }
        }
      }
      for (size_t depth = 0; depth < N; ++depth) {
        for (size_t index = 0; index < that._strong_gens.size(depth); ++index) {
          _strong_gens.push_back(
              depth, this->internal_copy(that._strong_gens.at(depth, index)));
        }
      }
    }

    // TODO(later): this could be better, especially when use in init() above,
    // we could recycle the memory allocated, instead of freeing everything as
    // below.
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
    index_type internal_sift(internal_element_type x) const {
      LIBSEMIGROUPS_ASSERT(&x != &_tmp_element1);
      for (index_type depth = 0; depth < _base_size; ++depth) {
        point_type beta = Action()(_base[depth], this->to_external_const(x));
        if (!_orbits_lookup[depth][beta]) {
          return depth;
        }
        Product()(this->to_external(_tmp_element1),
                  this->to_external_const(x),
                  this->to_external_const(_inversal[depth][beta]));
        Swap()(this->to_external(x), this->to_external(_tmp_element1));
      }
      return _base_size;
    }

    typename domain_type::const_iterator
    first_non_fixed_point(internal_const_element_type x) const {
      return std::find_if(_domain.cbegin(), _domain.cend(),

      for (auto it = _domain.cbegin(); it < _domain.cend(); ++it) {
        if (*it != Action()(*it, this->to_external_const(x))) {
          return it;
        }
      }
      // It is currently not possible to add the identity as a generator since
      // add_generator checks containment and every group contains its
      // identity element.
      LIBSEMIGROUPS_ASSERT(false);
      return _domain.cend();
    }
  };

  namespace schreier_sims {

    //! Find the intersection of two permutation groups.
    //!
    //! Modifies the first parameter \p T to be the Schreier-Sims object
    //! corresponding to the intersection of \p S1 and \p S2.
    //!
    //! \tparam N the largest point not fixed by the permutations in the
    //! permutation groups.
    //!
    //! \param T an empty Schreier-Sims object that will hold the result.
    //! \param S1 the first semigroup of the intersection.
    //! \param S2 the second group of the intersection.
    //!
    //! \throws LibsemigroupsException if \p T is not empty.
    //!
    // TODO(later) example

    // TODO (from RC):
    // 1. Implement orbit refinement heuristic for intersection.
    // 2. Make the Screier-Sims object during runtime, since we compute the
    //    stabilizers of the intersection already.
    // 3. Refactor for more generality (i.e. so the template parameters N don't
    //    all have to be the same
    //
    // TODO (from JDM):
    // * use the no_checks mem fns of SchreierSims now that they exist

    template <size_t N>
    void intersection(SchreierSims<N>& T,
                      SchreierSims<N>& S1,
                      SchreierSims<N>& S2) {
      // This might not be correct for general traits, i.e. only works for
      // permutations for now.
      using point_type   = typename SchreierSims<N>::point_type;
      using element_type = typename SchreierSims<N>::element_type;
      using One          = typename SchreierSims<N>::One;
      using Product      = typename SchreierSims<N>::Product;

      if (!T.empty()) {
        LIBSEMIGROUPS_EXCEPTION("the parameter T must be empty");
      }

      S1.run();
      S2.run();
      if (S2.base_size() < S1.base_size()) {
        intersection(T, S2, S1);
        return;
      }

      // If N <= 1 then both S1, S2 are trivial.
      if (N <= 1) {
        T.run();
        return;
      }

      // Note that if N-1 points are fixed then the N-th point is also fixed.
      // So if base contains all N points, then we lose nothing by discarding
      // the last point in the base.
      size_t base_size = S1.base_size();
      if (base_size == N) {
        base_size = N - 1;
      }

      auto S2B = std::make_unique<SchreierSims<N>>();
      for (size_t depth = 0; depth < base_size; ++depth) {
        S2B->add_base_point(S1.base(depth));
      }
      for (size_t i = 0; i < S2.number_of_generators(); ++i) {
        S2B->add_generator(S2.generator(i));
      }
      S2B->run();
#ifdef LIBSEMIGROUPS_DEBUG
      for (size_t depth = 0; depth < base_size; ++depth) {
        LIBSEMIGROUPS_ASSERT(S1.base(depth) == S2B->base(depth));
      }
#endif
      // Only need to consider points reachable by both groups.
      // Note that as we traverse the tree these points change!
      // In general, if we are at a node corresponding to elements g and h
      // in the tree and orbits O and P respectively, then the only points we
      // need to consider are O^g intersect P^h.
      // This is not currently implemented! We just use all of the points
      // in the orbits of S1. Implementing it probably requires refactoring
      // the code.
      detail::StaticTriVector2<point_type, N> refined_orbit;
      for (size_t depth = 0; depth < base_size; ++depth) {
        // First point is always base point to make algorithm simpler
        LIBSEMIGROUPS_ASSERT(S1.base(depth) == S2B->base(depth));
        refined_orbit.push_back(depth, S1.base(depth));
        for (point_type pt = 0; pt < N; ++pt) {
          if ((pt != S1.base(depth)) && S1.orbit_lookup(depth, pt)) {
            refined_orbit.push_back(depth, pt);
          }
        }
      }

      // Initially assume that we have traversed the tree to the leaf
      // corresponding to the base and identity element.
      // stab_depth tracks the largest stabiliser we have found thus far.
      size_t                      stab_depth = base_size;
      size_t                      depth      = 0;
      std::array<size_t, N>       state_index;
      std::array<element_type, N> state_elem;
      state_index.fill(0);
      state_elem.fill(One()(N));

      while (stab_depth > 0) {
        for (; depth < base_size; ++depth) {
          // This is a safe memory access as base_size <= N-1, so depth < N-1
          // during the loop and so depth + 1 <= N-1
          LIBSEMIGROUPS_ASSERT(depth + 1 < N);
          Product()(state_elem[depth + 1],
                    S1.transversal_element(
                        depth, refined_orbit.at(depth, state_index[depth])),
                    state_elem[depth]);
        }
        if (S2B->contains(state_elem[depth])) {
          LIBSEMIGROUPS_ASSERT(S1.contains(state_elem[depth]));
          LIBSEMIGROUPS_ASSERT(S2.contains(state_elem[depth]));
          T.add_generator(state_elem[depth]);
          // As soon as we find one, the rest are in a coset of stabiliser, so
          // dont need to look at any more nodes.
          depth = stab_depth;
        }
        // If previous if statement passes then depth = stab_depth > 0 by the
        // while loop invariant. If not, then depth = base_size > 0 due to the
        // for loop before the if statement.
        LIBSEMIGROUPS_ASSERT(depth != 0);
        depth--;

        // Find largest depth that has an unvisited node and increment its
        // index. Adjust stabilizer depth as depths are exhausted.
        for (;; --depth) {
          LIBSEMIGROUPS_ASSERT(depth < base_size);
          state_index[depth]++;
          if (state_index[depth] < refined_orbit.size(depth)) {
            break;
          }
          if (depth < stab_depth) {
            stab_depth = depth;
          }
          state_index[depth] = 0;
          state_elem[depth]  = One()(N);
          if (depth == 0) {
            break;
          }
        }
      }

      T.run();
    }
  }  // namespace schreier_sims
}  // namespace libsemigroups

#include "schreier-sims.tpp"

#endif  // LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
