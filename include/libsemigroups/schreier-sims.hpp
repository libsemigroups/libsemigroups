//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2024 James D. Mitchell
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
// * code coverage (can't do it on MacBook Pro at present)

#ifndef LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
#define LIBSEMIGROUPS_SCHREIER_SIMS_HPP_

#include <array>          // for array
#include <cstddef>        // for size_t
#include <cstdint>        // for uint64_t
#include <iterator>       // for distance
#include <memory>         // for make_unique
#include <string>         // for operator+, basic_string
#include <string_view>    // for string_view
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
    using internal_reference =
        typename detail::BruidhinnTraits<Element>::internal_reference;
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
    SchreierSims();

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
    SchreierSims& init();

    ~SchreierSims();

    //! Default move constructor.
    SchreierSims(SchreierSims&&) = default;

    // TODO doc
    SchreierSims& operator=(SchreierSims&&) = default;

    // TODO doc
    // TODO this requires more tests
    SchreierSims(SchreierSims const& that);

    // TODO doc
    // TODO this requires more tests
    SchreierSims& operator=(SchreierSims const& that);

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
    bool add_generator(const_element_reference x);

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

    // Not noexcept because strong_generator_no_checks isn't
    [[nodiscard]] const_element_reference
    generator_no_checks(index_type index) const {
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
    [[nodiscard]] size_t number_of_generators() const noexcept;

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
    // Not noexcept because StaticTriVector2::size(size_t) is not.
    [[nodiscard]] size_t
    number_of_strong_generators_no_checks(index_type depth) const {
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
    strong_generator(index_type depth, index_type index) const;

    // TODO(doc)
    // not noexcept because StaticTriVector2 isn't
    [[nodiscard]] const_element_reference
    strong_generator_no_checks(index_type depth, index_type index) const {
      return this->to_external_const(_strong_gens.at(depth, index));
    }

    // TODO(doc)
    // not noexcept because Array2::operator[] isn't
    [[nodiscard]] const_element_reference
    transversal_element_no_checks(index_type depth, point_type pt) const {
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
    // Not noexcept because throws
    [[nodiscard]] const_element_reference
    transversal_element(index_type depth, point_type pt) const;

    // TODO(doc)
    // Not noexcept because std::array::operator[] isn't
    [[nodiscard]] const_element_reference
    inversal_element_no_checks(index_type depth, point_type pt) const {
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
    [[nodiscard]] const_element_reference
    inverse_transversal_element(index_type depth, point_type pt) const;

    // Not noexcept because std::array::operator[] isn't
    [[nodiscard]] bool orbit_lookup_no_checks(index_type depth,
                                              point_type pt) const {
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
    // not noexcept because run isn't
    [[nodiscard]] uint64_t size();

    // TODO noexcept?
    // TODO doc
    // TODO tests
    // not noexcept because internal_sift isn't
    void sift_inplace_no_checks(element_reference x) const {
      // changes x in place, and uses _tmp_element1
      internal_sift(this->to_internal(x));
    }

    // TODO doc
    // TODO tests
    // not noexcept because can throw
    void sift_inplace(element_reference x) const {
      throw_if_bad_degree(x);
      sift_inplace_no_checks(x);
    }

    // not noexcept because internal_sift isn't
    const_element_reference sift_no_checks(const_element_reference x) const {
      this->to_external(_tmp_element2) = x;
      // changes _tmp_element2 in place, and uses _tmp_element1
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
    // not noexcept because can throw
    [[nodiscard]] const_element_reference
    sift(const_element_reference x) const {
      throw_if_bad_degree(x);
      return sift_no_checks(x);
    }

    // TODO doc
    // At present I think this will always return false if finished()
    // isn't true, because we either run to the end or haven't done anything at
    // all
    // not noexcept because sift_no_checks isn't
    [[nodiscard]] bool const_contains(const_element_reference x) const;

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
    [[nodiscard]] bool contains(const_element_reference x);

    //! Returns a const reference to the identity.
    //!
    //! \returns A \c bool.
    //!
    //! \parameters
    //! (None)
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    [[nodiscard]] const_element_reference identity() const
        noexcept(noexcept(this->to_external_const(_one))) {
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
    [[nodiscard]] bool finished() const noexcept {
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
    // not noexcept because can throw
    void add_base_point(point_type pt);

    // Not noexcept because std::array::operator[] isn't
    [[nodiscard]] point_type base_no_checks(index_type index) const {
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
    // not noexcept because can throw
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
    // not noexcept because it can call mem fns that aren't
    void run();

   private:
    ////////////////////////////////////////////////////////////////////////
    // SchreierSims - validation - private
    ////////////////////////////////////////////////////////////////////////

    bool is_valid_degree(point_type x) const noexcept {
      return
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
          std::is_same<HPCombi::Perm16, element_type>::value ||
#endif
          x == N;
    }

    void throw_if_bad_degree(const_element_reference x,
                             std::string_view        arg_pos = "1st") const;

    void throw_if_bad_depth(size_t           depth,
                            std::string_view arg_pos = "1st") const;

    void throw_if_point_gt_degree(point_type       pt,
                                  std::string_view arg_pos = "1st") const;

    void throw_if_point_not_in_orbit(index_type       depth,
                                     point_type       pt,
                                     std::string_view depth_arg_pos = "1st",
                                     std::string_view pt_arg_pos = "2nd") const;

    ////////////////////////////////////////////////////////////////////////
    // SchreierSims - member functions - private
    ////////////////////////////////////////////////////////////////////////

    bool internal_equal_to(internal_const_element_type x,
                           internal_const_element_type y) const
        noexcept(noexcept(
            EqualTo()(this->to_external_const(x),
                      this->to_external_const(
                          y)) && noexcept(this->to_external_const(x)))) {
      return EqualTo()(this->to_external_const(x), this->to_external_const(y));
    }

    // Used by copy constructor and assignment operator.
    void init_strong_gens_traversal_inversal(SchreierSims const& that);
    void clear();

    void internal_add_base_point(point_type pt);
    void orbit_enumerate(index_type depth, index_type first = 0);
    void orbit_add_gen(index_type depth, internal_element_type gen);
    void orbit_add_point(index_type            depth,
                         internal_element_type x,
                         point_type            pt);
    // Changes _tmp_element2 in-place, and returns the depth reached in the
    // sifting.
    index_type internal_sift(internal_reference x) const;

    typename domain_type::const_iterator
    first_non_fixed_point(internal_const_element_type x) const {
      auto const& y = this->to_external_const(x);
      return std::find_if(_domain.cbegin(), _domain.cend(), [&y](auto pt) {
        return pt != Action()(pt, y);
      });
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
                      SchreierSims<N>& S2);
  }  // namespace schreier_sims
}  // namespace libsemigroups

#include "schreier-sims.tpp"

#endif  // LIBSEMIGROUPS_SCHREIER_SIMS_HPP_
