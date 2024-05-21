//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-24 James D. Mitchell
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

// This file contains the declaration of the partial transformation class and
// its subclasses.

// TODO(later)
// * benchmarks
// * add some tests for PTransf themselves
// * allocator

// TODO(now)
// * repr
// * use "f" as the variable name for transformations

#ifndef LIBSEMIGROUPS_TRANSF_HPP_
#define LIBSEMIGROUPS_TRANSF_HPP_

#include <algorithm>         // for sort, max_element, unique
#include <array>             // for array
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t, uint32_t
#include <initializer_list>  // for initializer_list
#include <iterator>          // for distance
#include <limits>            // for numeric_limits
#include <numeric>           // for iota
#include <string_view>
#include <tuple>          // for tuple_size
#include <type_traits>    // for enable_if_t
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "config.hpp"  // for LIBSEMIGROUPS_HPCOMBI_ENABLED

#include "adapters.hpp"   // for Hash etc
#include "bitset.hpp"     // for BitSet
#include "constants.hpp"  // for UNDEFINED, Undefined
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "hpcombi.hpp"    // for HPCombi::Transf16
#include "types.hpp"      // for SmallestInteger

#include "detail/stl.hpp"  // for is_specialization_of_v

namespace libsemigroups {
  //! \defgroup elements_group Elements
  //!
  //! This page contains an overview summary of the functionality in
  //! `libsemigroups` for defining elements of semigroups and monoids.
  //!
  //! * \ref transf_group

  namespace detail {
    //! Empty base class for polymorphism.
    //!
    //! \sa IsDerivedFromPTransf
    struct PTransfPolymorphicBase {};
  }  // namespace detail

  //! \defgroup transf_group Transformations
  //!
  //! Defined in `transf.hpp`.
  //!
  //! This page contains the documentation for functionality in
  //! `libsemigroups` for various partial transformations.
  //!
  //! A *partial transformation* \f$f\f$ is just a function defined on a
  //! subset of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called
  //! the *degree*  of *f*.  A partial transformation is stored as a container
  //! of the images of \f$\{0, 1, \ldots, n -1\}\f$, i.e. \f$((0)f, (1)f,
  //! \ldots, (n - 1)f)\f$ where the value \ref UNDEFINED is used to
  //! indicate that \f$(i)f\f$ is, you guessed it, undefined (i.e. not among
  //! the points where \f$f\f$ is defined).
  // TODO example

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! derived from PTransfPolymorphicBase.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsDerivedFromPTransf
      = std::is_base_of_v<detail::PTransfPolymorphicBase, T>;

  namespace detail {

    template <typename T>
    struct IsStaticHelper : std::false_type {};

    template <typename T>
    struct IsDynamicHelper : std::false_type {};

  }  // namespace detail

  //! \ingroup transf_group
  //!
  //! \brief Base class for partial transformations.
  //!
  //! This is a class template for partial transformations.
  //!
  //! \tparam Point the type of image values (must be an unsigned integer
  //! type).
  //!
  //! \tparam Container the type of the container holding the image values.
  template <typename Point, typename Container>
  class PTransfBase : public detail::PTransfPolymorphicBase {
    static_assert(std::is_integral_v<Point>,
                  "template parameter Point must be an integral type");
    static_assert(!std::numeric_limits<Point>::is_signed,
                  "template parameter Point must be unsigned");

   public:
    //! Type of the image values.
    //!
    //! Also the template parameter \c Scalar.
    using point_type = Point;

    //! Type of the underlying container.
    //!
    //! In this case, this is `std::vector<point_type>`.
    using container_type = Container;

    // Required by python bindings
    //! Returns the value used to represent \"undefined\".
    //!
    //! This static function returns the value of type \ref point_type used to
    //! represent an \"undefined\" value.
    //!
    //! \returns
    //! A value of type \ref point_type.
    //!
    //! \exception
    //! \noexcept
    [[nodiscard]] static point_type undef() noexcept {
      return static_cast<point_type>(UNDEFINED);
    }

    //! Default constructor.
    //!
    //! Constructs an uninitialized partial transformation of degree \c 0.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Constant.
    PTransfBase() = default;

    //! Construct from container.
    //!
    //! Constructs a partial transformation initialized using the
    //! container \p cont as follows: the image of the point \c i under
    //! the partial transformation is the value in position \c i of the
    //! container \p cont.
    //!
    //! \param cont the container.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in the size of the container \p cont.
    //!
    //! \warning
    //! No checks on the validity of \p cont are performed.
    explicit PTransfBase(Container&& cont) : _container(std::move(cont)) {}

    //! Construct from container.
    //!
    //! Constructs a partial transformation initialized using the
    //! container \p cont as follows: the image of the point \c i under
    //! the partial transformation is the value in position \c i of the
    //! container \p cont.
    //!
    //! \param cont the container.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in the size of the container \p cont.
    //!
    //! \warning
    //! No checks on the validity of \p cont are performed.
    explicit PTransfBase(Container const& cont) : _container(cont) {}

    //! Construct from an initializer list.
    //!
    //! Constructs a partial transformation initialized using the
    //! container \p cont as follows: the image of the point \c i under
    //! the partial transformation is the value in position \c i of the
    //! container \p cont. The values in the initializer list must be
    //! convertible to point_type or equal to \ref UNDEFINED.
    //!
    //! \param cont the initializer list.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in the size of the initializer list \p cont.
    //!
    //! \warning
    //! No checks on the validity of \p cont are performed.
    //!
    //! \sa
    //! \ref make
    template <typename T>
    explicit PTransfBase(std::initializer_list<T> cont) : PTransfBase() {
      static_assert(std::is_same_v<T, Undefined>
                        || std::is_convertible_v<T, point_type>,
                    "the template parameter T must be Undefined or "
                    "convertible to point_type!");
      resize(_container, cont.size());
      std::copy(cont.begin(), cont.end(), _container.begin());
    }

    // TODO doc
    template <typename T>
    explicit PTransfBase(std::vector<T> const& cont) : PTransfBase() {
      static_assert(std::is_same_v<T, Undefined>
                        || std::is_convertible_v<T, point_type>,
                    "the template parameter T must be Undefined or "
                    "convertible to point_type!");
      resize(_container, cont.size());
      std::copy(cont.begin(), cont.end(), _container.begin());
    }

    //! Construct from a container and validates.
    //!
    //! Constructs a partial transformation initialized using the
    //! container \p cont as follows: the image of the point \c i under
    //! the partial transformation is the value in position \c i of the
    //! container \p cont.
    //!
    //! \param cont the container.
    //!
    //! \throw LibsemigroupsException if any of the following hold:
    //! * the size of \p cont is incompatible with \ref container_type.
    //! * any value in \p cont exceeds `cont.size()` and is not equal to
    //!   libsemigroups::UNDEFINED.
    //!
    //! \complexity
    //! Linear in the size of the container \p cont.
    template <typename Subclass, typename ContainerAgain = Container>
    [[nodiscard]] static Subclass make(ContainerAgain&& cont);

    // TODO(doc)
    template <typename Subclass, typename ContainerAgain = Container>
    [[nodiscard]] static Subclass make(ContainerAgain const& cont);

    //! Construct from an initializer list.
    //!
    //! \sa
    //! \ref make
    template <typename Subclass>
    [[nodiscard]] static Subclass
    make(std::initializer_list<point_type> const& cont) {
      return PTransfBase::make<Subclass, std::initializer_list<point_type>>(
          cont);
    }

    //! Make an empty instance of \c Subclass with degree 0.
    template <typename Subclass>
    [[nodiscard]] static Subclass make() {
      return Subclass();
    }

    //! Default copy constructor
    PTransfBase(PTransfBase const&) = default;

    //! Default move constructor
    PTransfBase(PTransfBase&&) = default;

    //! Default copy assignment operator.
    PTransfBase& operator=(PTransfBase const&) = default;

    //! Default move assignment operator.
    PTransfBase& operator=(PTransfBase&&) = default;

    //! Compare for less.
    //!
    //! Returns \c true if `*this` is less than \p that by comparing the
    //! image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator<(PTransfBase const& that) const {
      return _container < that._container;
    }

    //! Compare for greater.
    //!
    //! Returns \c true if `*this` is greater than \p that by comparing the
    //! image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator>(PTransfBase const& that) const {
      return that < *this;
    }

    //! Compare for equality.
    //!
    //! Returns \c true if `*this` equals \p that by comparing the
    //! image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator==(PTransfBase const& that) const {
      return _container == that._container;
    }

    //! Compare for less than or equal.
    //!
    //! Returns \c true if `*this` is less than or equal to \p that by
    //! comparing the image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator<=(PTransfBase const& that) const {
      return _container < that._container || _container == that._container;
    }

    //! Compare for greater than or equal.
    //!
    //! Returns \c true if `*this` is greater than or equal to \p that by
    //! comparing the image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator>=(PTransfBase const& that) const {
      return that <= *this;
    }

    //! Compare for inequality.
    //!
    //! Returns \c true if `*this` does not equal \p that by comparing the
    //! image values of `*this` and \p that.
    //!
    //! \param that the partial transformation for comparison.
    //!
    //! \returns
    //! A value of type \c bool.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! At worst linear in degree().
    [[nodiscard]] bool operator!=(PTransfBase const& that) const {
      return !(*this == that);
    }

    //! Get a reference to the image of a point.
    //!
    //! Returns a reference to the image of \p i.
    //!
    //! \param i the point.
    //!
    //! \returns
    //! A reference to a \ref point_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No bound checks are performed on \p i.
    [[nodiscard]] point_type& operator[](size_t i) {
      return _container[i];
    }

    //! Get a const reference to the image of a point.
    //!
    //! Returns a const reference to the image of \p i.
    //!
    //! \param i the point.
    //!
    //! \returns
    //! A const reference to a \ref point_type.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    //!
    //! \warning
    //! No bound checks are performed on \p i.
    [[nodiscard]] point_type const& operator[](size_t i) const {
      return _container[i];
    }

    //! Get a reference to the image of a point.
    //!
    //! Returns a reference to the image of \p i.
    //!
    //! \param i the point.
    //!
    //! \returns
    //! A reference to a \ref point_type.
    //!
    //! \throws std::out_of_range if \p i is out of range.
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] point_type& at(size_t i) {
      return _container.at(i);
    }

    //! Get a const reference to the image of a point.
    //!
    //! Returns a const reference to the image of \p i.
    //!
    //! \param i the point.
    //!
    //! \returns
    //! A const reference to a \ref point_type.
    //!
    //! \throws std::out_of_range if \p i is out of range.
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] point_type const& at(size_t i) const {
      return _container.at(i);
    }

    //! Multiply by another partial transformation.
    //!
    //! Returns a newly constructed partial transformation holding the
    //! product of `*this` and `that`.
    //!
    //! \tparam Subclass
    //! A class derived from libsemigroups::PTransfPolymorphicBase.
    //!
    //! \param that a partial transformation.
    //!
    //! \returns
    //! A value of type \c Subclass
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in degree().
    // TODO(later) other operators
    template <typename Subclass>
    [[nodiscard]] Subclass operator*(Subclass const& that) const {
      static_assert(IsDerivedFromPTransf<Subclass>,
                    "the template parameter Subclass must be derived from "
                    "PTransfPolymorphicBase");
      Subclass xy(that.degree());
      xy.product_inplace(*static_cast<Subclass const*>(this), that);
      return xy;
    }

    //! Type of iterators point to image values.
    using iterator = typename Container::iterator;

    //! Type of const iterators point to image values.
    using const_iterator = typename Container::const_iterator;

    //! Returns a \ref const_iterator (random access iterator) pointing at the
    //! first image value.
    //!
    //! \returns
    //! A const iterator to the first image value.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] const_iterator cbegin() const noexcept {
      return _container.cbegin();
    }

    //! Returns a \ref const_iterator (random access
    //! iterator) pointing one past the last image value.
    //!
    //! \returns
    //! A const iterator pointing one past the last image value.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] const_iterator cend() const noexcept {
      return _container.cend();
    }

    //! \copydoc cbegin()
    [[nodiscard]] const_iterator begin() const noexcept {
      return _container.begin();
    }

    //! \copydoc cend()
    [[nodiscard]] const_iterator end() const noexcept {
      return _container.end();
    }

    //! Returns an \ref iterator (random access iterator) pointing at the
    //! first image value.
    //!
    //! \returns
    //! An iterator to the first image value.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] iterator begin() noexcept {
      return _container.begin();
    }

    //! Returns an \ref iterator (random access
    //! iterator) pointing one past the last image value.
    //!
    //! \returns
    //! An iterator pointing one past the last image value.
    //!
    //! \complexity
    //! Constant.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] iterator end() noexcept {
      return _container.end();
    }

    //! Returns the number of distinct image values.
    //!
    //! The *rank* of a partial transformation is the number of its distinct
    //! image values, not including \ref libsemigroups::UNDEFINED.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in degree().
    [[nodiscard]] size_t rank() const {
      auto vals = std::unordered_set<point_type>(cbegin(), cend());
      return (vals.find(UNDEFINED) == vals.end() ? vals.size()
                                                 : vals.size() - 1);
    }

    //! Returns a hash value.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \no_libsemigroups_except_detail
    //!
    //! \complexity
    //! Linear in degree().
    // not noexcept because Hash<T>::operator() isn't
    [[nodiscard]] size_t hash_value() const {
      return Hash<Container>()(_container);
    }

    //! Swap with another partial transformation.
    //!
    //! \param that the partial transformation to swap with.
    //!
    //! \exceptions
    //! \noexcept
    void swap(PTransfBase& that) noexcept {
      std::swap(_container, that._container);
    }

    //! Returns the degree of a partial transformation.
    //!
    //! The *degree* of a partial transformation is the number of points used
    //! in its definition, which is equal to the size of the underlying
    //! container.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t degree() const noexcept {
      return _container.size();
    }

    //! Returns the identity transformation on the given number of points.
    //!
    //! This function returns a newly constructed partial transformation with
    //! degree equal to the degree of \c N that fixes every value from \c 0
    //! to \c N.
    //!
    //! \tparam Subclass
    //! A class derived from libsemigroups::PTransfPolymorphicBase.
    //!
    //! \param N the degree of the identity being constructed.
    //!
    //! \returns
    //! A value of type \c Subclass.
    template <typename Subclass>
    [[nodiscard]] static Subclass one(size_t N) {
      static_assert(IsDerivedFromPTransf<Subclass>,
                    "the template parameter Subclass must be derived from "
                    "PTransfPolymorphicBase");
      Subclass result(N);
      std::iota(result.begin(), result.end(), 0);
      return result;
    }

   protected:
    static void resize(container_type& c, size_t N, point_type val = 0);
    void        resize(size_t N, point_type val = 0) {
      resize(_container, N, val);
    }

   private:
    template <typename T>
    static void validate_args(T const& cont);

    Container _container;
  };

  ////////////////////////////////////////////////////////////////////////
  // Helper variable templates
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! derived from StaticPTransf.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsStatic = detail::IsStaticHelper<T>::value;

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! derived from DynamicPTransf.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsDynamic = detail::IsDynamicHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // DynamicPTransf
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Partial transformations with dynamic degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! This is a class for partial transformations where the number of points
  //! acted on (the degree) can be set at run time.
  //!
  //! \tparam Scalar a unsigned integer type.
  //!
  //! \sa PTransfBase for more information.
  template <typename Scalar>
  class DynamicPTransf : public PTransfBase<Scalar, std::vector<Scalar>> {
    using base_type = PTransfBase<Scalar, std::vector<Scalar>>;

   public:
    //! \brief Type of the image values.
    //!
    //! Type of the image values.
    //!
    //! Also the template parameter \c Scalar.
    using point_type = Scalar;

    //! \brief Type of the underlying container.
    //!
    //! Type of the underlying container.
    //!
    //! In this case, this is `std::vector<point_type>`.
    using container_type = std::vector<point_type>;

    // Documented in PTransfBase
    using PTransfBase<point_type, container_type>::PTransfBase;

    // Documented in PTransfBase
    using base_type::begin;

    // Documented in PTransfBase
    using base_type::end;

    //! Returns the degree of a transformation.
    //!
    //! The *degree* of a transformation is the number of points used
    //! in its definition, which is equal to the size of the underlying
    //! container.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \exceptions
    //! \noexcept
    using base_type::degree;

    //! Construct with given degree.
    //!
    //! Constructs a partial transformation of degree \p n with the image of
    //! every point set to \ref UNDEFINED.
    //!
    //! \param n the degree
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the parameter \p n.
    explicit DynamicPTransf(size_t n) : base_type() {
      resize(n, UNDEFINED);
    }

    //! Increase the degree in-place.
    //!
    //! Increases the degree of \c this in-place, leaving existing values
    //! unaltered.
    //!
    //! \param m the number of points to add.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst linear in the sum of the parameter \p m and degree().
    DynamicPTransf& increase_degree_by(size_t m) {
      resize(degree() + m);
      std::iota(end() - m, end(), degree() - m);
      return *this;
    }

   protected:
    using base_type::resize;
  };

  ////////////////////////////////////////////////////////////////////////
  // StaticPTransf
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Partial transformations with static degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! Static partial transformations.
  //!
  //! This is a class for partial transformations where the number of points
  //! acted on (the degree) is set at compile time.
  //!
  //! \tparam Scalar an unsigned integer type.
  template <size_t N, typename Scalar>
  class StaticPTransf : public PTransfBase<Scalar, std::array<Scalar, N>> {
    using base_type = PTransfBase<Scalar, std::array<Scalar, N>>;

   public:
    //! \copydoc PTransfBase::point_type
    using point_type = Scalar;

    //! Type of the underlying container.
    //!
    //! In this case, this is `std::array<point_type, N>`.
    using container_type = std::array<Scalar, N>;

    // TODO(later) This is currently undocumentable. The doc is available in
    // PTransfBase but they aren't present in the doxygen xml output.
    using PTransfBase<point_type, container_type>::PTransfBase;

    //! \copydoc PTransfBase<point_type, container_type>::begin
    using base_type::begin;

    //! \copydoc PTransfBase<point_type, container_type>::end
    using base_type::end;

    //! Default constructor.
    //!
    //! Constructs a partial transformation of degree equal to the template
    //! parameter \p N with the image of every point set to \ref UNDEFINED.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the template parameter \p N.
    StaticPTransf() : base_type() {
      std::fill(begin(), end(), UNDEFINED);
    }

    //! Construct with given degree.
    //!
    //! This function constructs the same partial transformation as the default
    //! constructor, and exists only for consistency of interface with
    //! DynamicPTransf.
    //!
    //! \param n the degree of the partial transformation being constructed.
    //!
    //! \throws LibsemigroupsException if \p n is not equal to the class
    //! template parameter \p N.
    explicit StaticPTransf(size_t n);

    //! \copydoc PTransfBase<Scalar,Container>::degree
    [[nodiscard]] constexpr size_t degree() const noexcept {
      return N;
    }

    //! Increase the degree in-place.
    //!
    //! This doesn't make sense for this type, and it throws every time.
    //!
    //! \throws LibsemigroupsException every time.
    StaticPTransf& increase_degree_by(size_t) {
      // do nothing can't increase the degree
      LIBSEMIGROUPS_EXCEPTION("cannot increase the degree of a StaticPTransf!");
      return *this;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // PTransf
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Partial transformations with static or dynamic degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! This alias equals either DynamicPTransf or StaticPTransf depending on
  //! the template parameters \p N and \p Scalar.
  //!
  //! If \p N is \c 0 (the default), then \c PTransf is \ref
  //! DynamicPTransf. In this case the default value of \p Scalar is \c
  //! uint32_t. If \p N is not \c 0, then \c PTransf is \ref StaticPTransf,
  //! and the default value of \p Scalar is the smallest integer type able to
  //! hold \c N. See also SmallestInteger.
  //!
  //! \tparam N  the degree (default: \c 0)
  //! \tparam Scalar an unsigned integer type (the type of the image values)
  template <
      size_t N = 0,
      typename Scalar
      = std::conditional_t<N == 0, uint32_t, typename SmallestInteger<N>::type>>
  using PTransf = std::
      conditional_t<N == 0, DynamicPTransf<Scalar>, StaticPTransf<N, Scalar>>;

  namespace detail {
    template <typename T>
    struct IsPTransfHelper : std::false_type {};

    template <typename Scalar>
    struct IsPTransfHelper<DynamicPTransf<Scalar>> : std::true_type {};

    template <size_t N, typename Scalar>
    struct IsPTransfHelper<StaticPTransf<N, Scalar>> : std::true_type {};

    template <size_t N, typename Scalar>
    struct IsStaticHelper<StaticPTransf<N, Scalar>> : std::true_type {};

    template <typename Scalar>
    struct IsDynamicHelper<DynamicPTransf<Scalar>> : std::true_type {};

    // TODO to tpp file
    //! No doc
    template <typename T>
    [[nodiscard]] std::string ptransf_repr(T const&         f,
                                           std::string_view prefix,
                                           std::string_view braces) {
      if (braces.size() != 2) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 3rd argument (braces) must have size 2, found {}",
            braces.size());
      }
      return fmt::format("{}({}{}{})",
                         prefix,
                         braces[0],
                         fmt::join(f.cbegin(), f.cend(), ", "),
                         braces[1]);
    }
  }  // namespace detail

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! either \ref DynamicPTransf or \ref StaticPTransf for any template
  //! parameters.
  //!
  //! \tparam T a type.
  //!
  //! \sa libsemigroups::IsStatic and libsemigroups::IsDynamic
  template <typename T>
  static constexpr bool IsPTransf = detail::IsPTransfHelper<T>::value;

  //! \ingroup transf_group
  //! \brief Validate a partial transformation.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar an unsigned integer type (the type of the image values)
  //!
  //! \param f the partial transformation to validate.
  //!
  //! \throw LibsemigroupsException if any of the following hold:
  //! * the size of \p cont is incompatible with `T::container_type`.
  //! * any value in \p cont exceeds `cont.size()` and is not equal to \ref
  //!   libsemigroups::UNDEFINED.
  //!
  //! \complexity
  //! Linear in degree().
  template <typename T>
  auto validate(T const& f) -> std::enable_if_t<IsPTransf<T>> {
    size_t const M = f.degree();
    for (auto const& val : f) {
      // the type of "val" is an unsigned int, and so we don't check for val
      // being less than 0
      if (val >= M && val != UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION("image value out of bounds, expected value in "
                                "[{}, {}), found {}",
                                0,
                                M,
                                val);
      }
    }
  }

  //! \ingroup transf_group
  //!
  //! \brief Convert a partial transformation to a string.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param f the partial transformation
  //! \param braces the type of braces to use (default: "{}")
  //!
  //! \returns A string containing a string representation of the argument \p f.
  template <size_t N, typename Scalar>
  [[nodiscard]] std::string to_string(PTransf<N, Scalar> const& f,
                                      std::string_view          braces = "{}") {
    return detail::ptransf_repr(f, "PTransf", braces);
  }

  //! \ingroup transf_group
  //! \brief Insert a partial transformation into a string stream.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param os the string stream
  //! \param f the partial transformation
  //!
  //! \returns A reference to the 1st argument \p os.
  //!
  //! \sa to_string
  template <size_t N, typename Scalar>
  std::ostringstream& operator<<(std::ostringstream&       os,
                                 PTransf<N, Scalar> const& f) {
    os << to_string(f);
    return os;
  }

  //! \ingroup transf_group
  //! \brief Insert a partial transformation into a std::ostream.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param os the std::ostream
  //! \param f the partial transformation
  //!
  //! \returns A reference to the 1st argument \p os.
  //!
  //! \sa to_string
  template <size_t N, typename Scalar>
  std::ostream& operator<<(std::ostream& os, PTransf<N, Scalar> const& f) {
    os << to_string(f);
    return os;
  }

  ////////////////////////////////////////////////////////////////////////
  // Transf
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Transformations with static or dynamic degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! A *transformation* \f$f\f$ is just a function defined on the
  //! whole of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$
  //! called the *degree* of \f$f\f$.  A transformation is stored as a
  //! container of the images of \f$\{0, 1, \ldots, n - 1\}\f$, i.e.
  //! \f$((0)f, (1)f, \ldots, (n - 1)f)\f$.
  //!
  //! If \p N is \c 0 (the default), then the degree of a \ref Transf instance
  //! can be defined at runtime, and if \p N is not \c 0, then the degree is
  //! fixed at compile time.
  //!
  //! If \p N is \c 0, then the default value of \p Scalar is \c uint32_t. If
  //! \p N is not \c 0, then the default value of \p Scalar is the smallest
  //! integer type able to hold \c N. See also SmallestInteger.
  //!
  //! \tparam N  the degree (default: \c 0)
  //! \tparam Scalar an unsigned integer type (the type of the image values)
  //!
  //! \note
  //! Transf has the same member functions as
  //! \ref StaticPTransf and \ref DynamicPTransf, this isn't currently
  //! reflected by the contents of this page.
  template <
      size_t N = 0,
      typename Scalar
      = std::conditional_t<N == 0, uint32_t, typename SmallestInteger<N>::type>>
  class Transf : public PTransf<N, Scalar> {
    using base_type = PTransf<N, Scalar>;

   public:
    //! Type of the image values.
    //!
    //! Also the template parameter \c Scalar.
    using point_type = Scalar;

    //! Type of the underlying container.
    //!
    //! In this case, this is PTransf<N, Scalar>::container_type.
    using container_type = typename base_type::container_type;

    using PTransf<N, Scalar>::PTransf;

    using base_type::degree;

    //! Construct from a container and validate.
    //!
    //! Constructs an transformation initialized using the
    //! container \p cont as follows: the image of the point \c i under
    //! the transformation is the value in position \c i of the container \p
    //! cont.
    //!
    //! \tparam T the type of the container \p cont.
    //!
    //! \param cont the container.
    //!
    //! \throw LibsemigroupsException if any of the following hold:
    //! * the size of \p cont is incompatible with \ref container_type.
    //! * any value in \p cont exceeds `cont.size()` or is equal to \ref
    //!    UNDEFINED.
    //!
    //! \complexity
    //! Linear in the size of the container \p cont.
    template <typename T>
    [[nodiscard]] static Transf make(T&& cont) {
      return base_type::template make<Transf>(std::forward<T>(cont));
    }

    //! Construct from a container and validate.
    //!
    //! \sa \ref make
    [[nodiscard]] static Transf make(std::initializer_list<point_type>&& cont) {
      return make<std::initializer_list<point_type>>(std::move(cont));
    }

    //! Make an empty Transf with degree 0.
    [[nodiscard]] static Transf make() {
      return base_type::template make<Transf>();
    }

    //! Multiply two transformations and store the product in \c this.
    //!
    //! Replaces the contents of \c this by the product of \p x and \p y.
    //!
    //! \param x a transformation.
    //! \param y a transformation.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in PTransf::degree.
    //!
    //! \warning
    //! No checks are made on whether or not the parameters are compatible. If
    //! \p x and \p y have different degrees, then bad things will happen.
    void product_inplace(Transf const& x, Transf const& y);

    //! Returns the identity transformation on the given number of points.
    //!
    //! This function returns a newly constructed transformation with
    //! degree equal to \p M that fixes every value from \c 0 to \p M.
    //!
    //! \param M the degree.
    //!
    //! \returns
    //! A value of type \c Transf.
    //!
    // TODO(doc) exception thrown if Transf is static and M <> degree()
    [[nodiscard]] static Transf one(size_t M) {
      return base_type::template one<Transf>(M);
    }
  };

  namespace detail {
    template <typename T>
    struct IsTransfHelper : std::false_type {};

    template <size_t N, typename Scalar>
    struct IsTransfHelper<Transf<N, Scalar>> : std::true_type {};

    template <size_t N, typename Scalar>
    struct IsStaticHelper<Transf<N, Scalar>>
        : IsStaticHelper<PTransf<N, Scalar>> {};

    template <size_t N, typename Scalar>
    struct IsDynamicHelper<Transf<N, Scalar>>
        : IsDynamicHelper<PTransf<N, Scalar>> {};
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Transf helpers
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! \ref Transf for any template parameters.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsTransf = detail::IsTransfHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // Transf validate
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //! \brief Validate a transformation.
  //!
  //! \tparam N the number of points
  //! \tparam Scalar the type of the points
  //!
  //! \param x the transformation.
  //!
  //! \throw LibsemigroupsException if the image of any point exceeds \c
  //! x.degree() or is equal to \ref UNDEFINED.
  //!
  //! \complexity
  //! Linear in the size of the container \c x.degree().
  template <size_t N, typename Scalar>
  void validate(Transf<N, Scalar> const& x);

  //! \ingroup transf_group
  //!
  //! \brief Convert a transformation to a string.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param f the transformation
  //! \param braces the type of braces to use (default: "{}")
  //!
  //! \returns A string containing a string representation of the argument \p f.
  // TODO to tpp
  template <size_t N, typename Scalar>
  [[nodiscard]] std::string to_string(Transf<N, Scalar> const& f,
                                      std::string_view         braces = "{}") {
    std::string type;
    switch (sizeof(Scalar)) {
      case 1:
        type = "uint8_t";
        break;
      case 2:
        type = "uint16_t";
        break;
      case 4:
        type = "uint32_t";
        break;
      case 8:
        type = "uint64_t";
        break;
    }
    return detail::ptransf_repr(
        f, fmt::format("Transf<{}, {}>", N, type), braces);
  }

  ////////////////////////////////////////////////////////////////////////
  // PPerm
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Partial permutations with static or dynamic degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! A *partial permutation* \f$f\f$ is just an injective partial
  //! transformation, which is stored as a container of the images of \f$\{0, 1,
  //! \ldots, n - 1\}\f$, i.e.  i.e. \f$((0)f, (1)f, \ldots, (n - 1)f)\f$
  //! where the value \ref UNDEFINED is used to indicate that \f$(i)f\f$ is
  //! undefined (i.e. not among the points where \f$f\f$ is defined).
  //!
  //! If \p N is \c 0 (the default), then the degree of a \ref PPerm instance
  //! can be defined at runtime, and if \p N is not \c 0, then the degree is
  //! fixed at compile time.
  //!
  //! If \p N is \c 0, then the default value of \p Scalar is \c uint32_t. If
  //! \p N is not \c 0, then the default value of \p Scalar is the smallest
  //! integer type able to hold \c N. See also SmallestInteger.
  //!
  //! \tparam N  the degree (default: \c 0)
  //! \tparam Scalar an unsigned integer type (the type of the image values)
  //!
  //! \note
  //! PPerm has the same member functions as \ref StaticPTransf and \ref
  //! DynamicPTransf, this isn't current reflected by the contents of this
  //! page.
  template <
      size_t N = 0,
      typename Scalar
      = std::conditional_t<N == 0, uint32_t, typename SmallestInteger<N>::type>>
  class PPerm : public PTransf<N, Scalar> {
    using base_type = PTransf<N, Scalar>;

   public:
    //! Type of the image values.
    //!
    //! Also the template parameter \c Scalar.
    using point_type = Scalar;

    //! Type of the underlying container.
    //!
    //! In this case, this is \c PTransf<N, Scalar>::container_type.
    using container_type = typename base_type::container_type;

    // Currently no way to document these
    using PTransf<N, point_type>::PTransf;

    // Currently no way to document these
    using base_type::degree;
    using base_type::undef;

    //! Construct from image list and validate
    //!
    //! Constructs a partial perm \f$f\f$ of degree \c M such that \f$f(i) =
    //! cont[i]\f$ for every value in the range \f$[0, M)\f$ where \f$M\f$ is
    //! \c cont.size()
    //!
    //! \param cont list of images or \ref UNDEFINED
    //!
    //! \complexity
    //! Linear in the size of \p cont.
    //!
    //! \throws LibsemigroupsException if any of the following fail to hold:
    //! * the size of \p cont is incompatible with \ref container_type.
    //! * any value in \p cont exceeds `cont.size()` and is not equal to \ref
    //!   UNDEFINED.
    //! * there are repeated values in \p cont.
    template <typename T>
    [[nodiscard]] static PPerm make(T&& cont) {
      return base_type::template make<PPerm>(std::forward<T>(cont));
    }

    //! Construct from image list and validate
    //!
    //! Constructs a partial perm \f$f\f$ of degree \c M such that \f$f(i) =
    //! cont[i]\f$ for every value in the range \f$[0, M)\f$ where \f$M\f$ is
    //! \c cont.size()
    //!
    //! \param cont list of images or \ref UNDEFINED
    //!
    //! \complexity
    //! Linear in the size of \p cont.
    //!
    //! \throws LibsemigroupsException if any of the following fail to hold:
    //! * the size of \p cont is incompatible with \ref container_type.
    //! * any value in \p cont exceeds `cont.size()` and is not equal to
    //!   \ref UNDEFINED.
    //! * there are repeated values in \p cont.
    [[nodiscard]] static PPerm make(std::initializer_list<point_type>&& cont) {
      return make<std::initializer_list<point_type>>(std::move(cont));
    }

    //! Make an empty PPerm of degree 0.
    [[nodiscard]] static PPerm make() {
      return base_type::template make<PPerm>();
    }

    //! Construct from domain, range, and degree, and validate
    //!
    //! Constructs a partial perm of degree \p M such that `f[dom[i]] =
    //! ran[i]` for all \c i and which is \ref UNDEFINED on every other value
    //! in the range \f$[0, M)\f$.
    //!
    //! \param dom the domain
    //! \param ran the range
    //! \param M the degree
    //!
    //! \complexity
    //! Linear in the size of \p dom.
    //!
    //! \throws LibsemigroupsException if any of the following fail to hold:
    //! * the value \p M is not compatible with the template parameter \p N
    //! * \p dom and \p ran do not have the same size
    //! * any value in \p dom or \p ran is greater than \p M
    //! * there are repeated entries in \p dom or \p ran.
    template <typename OtherScalar>
    [[nodiscard]] static PPerm make(std::vector<OtherScalar> const& dom,
                                    std::vector<OtherScalar> const& ran,
                                    size_t                          M);

    //! \copydoc make(std::vector<OtherScalar> const&, std::vector<OtherScalar>
    //! const&, size_t const)
    [[nodiscard]] static PPerm make(std::initializer_list<Scalar> const& dom,
                                    std::initializer_list<Scalar> const& ran,
                                    size_t                               M) {
      return make(std::vector<Scalar>(dom), std::vector<Scalar>(ran), M);
    }

    //! Construct from domain, range, and degree.
    //!
    //! Constructs a partial perm of degree \p M such that `(dom[i])f =
    //! ran[i]` for all \c i and which is \ref UNDEFINED on every other value
    //! in the range \f$[0, M)\f$.
    //!
    //! \param dom the domain
    //! \param ran the range
    //! \param M the degree
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the size of \p dom.
    //!
    //! \warning
    //! No checks whatsoever are performed on the validity of the arguments.
    //!
    //! \sa
    //! \ref make.
    // Note: we use vectors here not container_type (which might be array),
    // because the length of dom and ran might not equal degree().
    template <typename OtherScalar>
    PPerm(std::vector<OtherScalar> const& dom,
          std::vector<OtherScalar> const& ran,
          size_t                          M);

    //! Construct from domain, range, and degree.
    //!
    //! Constructs a partial perm of degree \p M such that `(dom[i])f =
    //! ran[i]` for all \c i and which is \ref UNDEFINED on every other value
    //! in the range \f$[0, M)\f$.
    //!
    //! \param dom the domain
    //! \param ran the range
    //! \param M the degree
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in the size of \p dom.
    //!
    //! \warning
    //! No checks whatsoever are performed on the validity of the arguments.
    //!
    //! \sa
    //! \ref make.
    PPerm(std::initializer_list<point_type> dom,
          std::initializer_list<point_type> ran,
          size_t                            M)
        : PPerm(std::vector<point_type>(dom), std::vector<point_type>(ran), M) {
    }

    //! Multiply two partial perms and store the product in \c this.
    //!
    //! Replaces the contents of \c this by the product of \p x and \p y.
    //!
    //! \param x a partial perm.
    //! \param y a partial perm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in degree().
    //!
    //! \warning
    //! No checks are made on whether or not the parameters are compatible. If
    //! \p x and \p y have different degrees, then bad things will happen.
    void product_inplace(PPerm const& x, PPerm const& y);

    //! Returns the identity partial perm on degree() points.
    //!
    //! This function returns a newly constructed partial perm with degree
    //! equal to the degree of \c this that fixes every value from \c 0 to
    //! degree().
    //!
    //! \returns
    //! A value of type \c PPerm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except

    //! Returns the identity partial perm on the given number of points.
    //!
    //! This function returns a newly constructed partial perm with
    //! degree equal to \p M that fixes every value from \c 0 to \p M.
    //!
    //! \param M the degree.
    //!
    //! \returns
    //! A value of type \c PPerm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in \p M.
    [[nodiscard]] static PPerm one(size_t M) {
      return base_type::template one<PPerm>(M);
    }

   private:
    static void validate_args(std::vector<point_type> const& dom,
                              std::vector<point_type> const& ran,
                              size_t                         deg = N);
  };

  namespace detail {
    //! No doc
    template <typename Iterator>
    void validate_no_duplicates(
        Iterator                                                    first,
        Iterator                                                    last,
        std::unordered_map<std::decay_t<decltype(*first)>, size_t>& seen) {
      seen.clear();
      for (auto it = first; it != last; ++it) {
        if (*it != UNDEFINED) {
          auto [pos, inserted] = seen.emplace(*it, seen.size());
          if (!inserted) {
            LIBSEMIGROUPS_EXCEPTION(
                "duplicate image value, found {} in position {}, first "
                "occurrence in position {}",
                *it,
                std::distance(first, it),
                pos->second);
          }
        }
      }
    }

    //! No doc
    template <typename Iterator>
    void validate_no_duplicates(Iterator first, Iterator last) {
      std::unordered_map<std::decay_t<decltype(*first)>, size_t> seen;
      validate_no_duplicates(first, last, seen);
    }
  }  // namespace detail

  //! \ingroup transf_group
  //!
  //! \brief Convert a partial perm to a string.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param f the partial perm
  //! \param braces the type of braces to use (default: "{}")
  //!
  //! \returns A string containing a string representation of the argument \p f.
  // TODO to tpp
  // TODO improve
  template <size_t N, typename Scalar>
  [[nodiscard]] std::string to_string(PPerm<N, Scalar> const& f,
                                      std::string_view        braces = "{}") {
    return detail::ptransf_repr(f, "PPerm", braces);
  }

  ////////////////////////////////////////////////////////////////////////
  // PPerm helpers
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename T>
    struct IsPPermHelper : std::false_type {};

    template <size_t N, typename Scalar>
    struct IsPPermHelper<PPerm<N, Scalar>> : std::true_type {};

    template <size_t N, typename Scalar>
    struct IsStaticHelper<PPerm<N, Scalar>>
        : IsStaticHelper<PTransf<N, Scalar>> {};

    template <size_t N, typename Scalar>
    struct IsDynamicHelper<PPerm<N, Scalar>>
        : IsDynamicHelper<PTransf<N, Scalar>> {};

  }  // namespace detail

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! \ref PPerm for any template parameters.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsPPerm = detail::IsPPermHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // PPerm validate
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //! \brief Validate a partial perm.
  //!
  //! \tparam T the type of the partial perm to validate.
  //!
  //! \param x the partial perm.
  //!
  //! \throw LibsemigroupsException if:
  //! * the image of any point in \p x exceeds \c x.degree() and is not equal
  //!   to \ref UNDEFINED; or
  //! * \p x is not injective
  //!
  //! \complexity
  //! Linear in the size of the container \c x.degree().
  template <size_t N, typename Scalar>
  void validate(PPerm<N, Scalar> const& x) {
    validate(static_cast<PTransf<N, Scalar> const&>(x));
    detail::validate_no_duplicates(x.begin(), x.end());
  }

  ////////////////////////////////////////////////////////////////////////
  // Perm
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Permutations with static or dynamic degree.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! A *permutation* \f$f\f$ is an injective transformation defined on the
  //! whole of \f$\{0, 1, \ldots, n - 1\}\f$ for some integer \f$n\f$ called
  //! the *degree* of \f$f\f$. A permutation is stored as a vector of the
  //! images of \f$(0, 1, \ldots, n - 1)\f$, i.e. \f$((0)f, (1)f, \ldots, (n -
  //! 1)f)\f$.
  //!
  //! If \p N is \c 0 (the default), then the degree of a \ref PPerm instance
  //! can be defined at runtime, and if \p N is not \c 0, then the degree is
  //! fixed at compile time.
  //!
  //! If \p N is \c 0, then the default value of \p Scalar is \c uint32_t. If
  //! \p N is not \c 0, then the default value of \p Scalar is the smallest
  //! integer type able to hold \c N. See also SmallestInteger.
  //!
  //! \tparam N  the degree (default: \c 0)
  //! \tparam Scalar an unsigned integer type (the type of the image values)
  //!
  //! \note
  //! Perm has the same member functions as \ref StaticPTransf and \ref
  //! DynamicPTransf, this isn't current reflected by the contents of this
  //! page.
  template <
      size_t N = 0,
      typename Scalar
      = std::conditional_t<N == 0, uint32_t, typename SmallestInteger<N>::type>>
  class Perm : public Transf<N, Scalar> {
    using base_type = PTransf<N, Scalar>;

   public:
    //! Type of the image values.
    //!
    //! Also the template parameter \c Scalar.
    using point_type = Scalar;

    //! Type of the underlying container.
    //!
    //! In this case, this is \c PTransf<N, Scalar>::container_type.
    using container_type = typename PTransf<N, point_type>::container_type;

    // Currently no way to document these
    using Transf<N, Scalar>::Transf;

    // Currently no way to document these
    using base_type::degree;

    //! Construct from image list and validate
    //!
    //! Constructs a permutation \f$f\f$ of degree \c M such that \f$f(i) =
    //! cont[i]\f$ for every value in the range \f$[0, M)\f$ where \f$M\f$ is
    //! \c cont.size()
    //!
    //! \param cont list of images
    //!
    //! \complexity
    //! Linear in the size of \p cont.
    //!
    //! \throws LibsemigroupsException if any of the following fail to hold:
    //! * the size of \p cont is incompatible with \ref container_type.
    //! * any value in \p cont exceeds `cont.size()`
    //! * there are repeated values in \p cont.
    template <typename T>
    [[nodiscard]] static Perm make(T&& cont) {
      return base_type::template make<Perm>(std::forward<T>(cont));
    }

#ifndef PARSED_BY_DOXYGEN
    // We don't document this, because it's basically identical to the
    // function template above, and because it confuses the doc system.
    [[nodiscard]] static Perm make(std::initializer_list<point_type>&& cont) {
      return make<std::initializer_list<point_type>>(std::move(cont));
    }
#endif

    //! Returns the identity permutation on the given number of points.
    //!
    //! This function returns a newly constructed permutation with
    //! degree equal to \p M that fixes every value from \c 0 to \p M.
    //!
    //! \param M the degree.
    //!
    //! \returns
    //! A value of type \c PPerm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Linear in \p M.
    [[nodiscard]] static Perm one(size_t M) {
      return base_type::template one<Perm>(M);
    }
  };

  //! \ingroup transf_group
  //!
  //! \brief Convert a perm to a string.
  //!
  //! \tparam N  the degree
  //! \tparam Scalar the type of the image values
  //!
  //! \param f the permutation
  //! \param braces the type of braces to use (default: "{}")
  //!
  //! \returns A string containing a string representation of the argument \p f.
  // TODO improve
  template <size_t N, typename Scalar>
  [[nodiscard]] std::string to_string(Perm<N, Scalar> const& f,
                                      std::string_view       braces = "{}") {
    return detail::ptransf_repr(f, "Perm", braces);
  }

  ////////////////////////////////////////////////////////////////////////
  // Perm helpers
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename T>
    struct IsPermHelper : std::false_type {};

    template <size_t N, typename Scalar>
    struct IsPermHelper<Perm<N, Scalar>> : std::true_type {};
  }  // namespace detail

  //! \ingroup transf_group
  //! \brief Helper variable template.
  //!
  //! The value of this variable is \c true if the template parameter \p T is
  //! \ref Perm for any template parameters.
  //!
  //! \tparam T a type.
  template <typename T>
  static constexpr bool IsPerm = detail::IsPermHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // Perm validate
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //! \brief Validate a permutation.
  //!
  //! \tparam T the type of the permutation to validate.
  //!
  //! \param x the permutation.
  //!
  //! \throw LibsemigroupsException if:
  //! * the image of any point in \p x exceeds \c x.degree()
  //! * \p x is not injective
  //!
  //! \complexity
  //! Linear in the size of the container \c x.degree().
  template <size_t N, typename Scalar>
  auto validate(Perm<N, Scalar> const& x) {
    validate(static_cast<Transf<N, Scalar> const&>(x));
    detail::validate_no_duplicates(x.begin(), x.end());
  }

  ////////////////////////////////////////////////////////////////////////
  // Helper functions
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup transf_group
  //!
  //! \brief Replace the contents of a vector by the set of images of a
  //! partial transformation.
  //!
  //! Replaces the contents of the 2nd argument \p im by those values `f[i]`
  //! where:
  //! * \f$i\in \{0, \ldots, n - 1\}\f$ where \f$n\f$ is PTransfBase::degree()
  //! of the 1st parameter \p f;
  //! * `f[i] != UNDEFINED`.
  //!
  //! \tparam T the type of the 1st argument (Transf, libsemigroups::PTransf,
  //! PPerm, or Perm)
  //! \tparam Point the type of the values in the 2nd argument
  //! (typically T::point_type where \c T is the 1st template parameter.
  //!
  //! \param f the partial transformation whose image is sought.
  //! \param im vector to store the result.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! \f$O(n\log(n))\f$ where \f$n\f$ equals PTransfBase::degree() of \p x.
  //!
  //! \sa \ref domain
  template <typename T, typename Point>
  void image(T const& f, std::vector<Point>& im);

  //! \ingroup transf_group
  //!
  //! \brief Returns the set of images of a partial transformation.
  //!
  //! Returns a std::vector containing those values `f[i]` such that:
  //! * \f$i\in \{0, \ldots, n - 1\}\f$ where \f$n\f$ is PTransfBase::degree()
  //! of the 1st parameter \p f; and
  //! * `f[i] != UNDEFINED`.
  //!
  //! \tparam T the type of the 1st argument (Transf, libsemigroups::PTransf,
  //! PPerm, or Perm)
  //!
  //! \param f the partial transformation whose image is sought.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! \f$O(n\log(n))\f$ where \f$n\f$ equals PTransfBase::degree() of \p x.
  //!
  //! \sa \ref domain
  template <typename T>
  [[nodiscard]] std::vector<typename T::point_type> image(T const& f);

  //! \ingroup transf_group
  //!
  //! \brief Replace the contents of a vector by the set of points where a
  //! partial transformation is defined.
  //!
  //! Replaces the contents of the 2nd argument \p dom by those values `i`
  //! such that:
  //! * \f$i\in \{0, \ldots, n - 1\}\f$ where \f$n\f$ is PTransfBase::degree()
  //! of the 1st parameter \p f; and
  //! * `f[i] != UNDEFINED`.
  //!
  //! \tparam T the type of the 1st argument (Transf, libsemigroups::PTransf,
  //! PPerm, or Perm)
  //! \tparam Point the type of the values in the 2nd argument
  //! (typically T::point_type where \c T is the 1st template parameter.
  //!
  //! \param f the partial transformation whose image is sought.
  //! \param dom vector to store the result.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! \f$O(n)\f$ where \f$n\f$ equals PTransfBase::degree() of \p f.
  //!
  //! \sa \ref image
  template <typename T, typename Point>
  void domain(T const& f, std::vector<Point>& dom);

  //! \ingroup transf_group
  //!
  //! \brief Returns the set of points where a partial transformation is
  //! defined.
  //!
  //! Returns a std::vector containing those values `i` such that:
  //! * \f$i\in \{0, \ldots, n - 1\}\f$ where \f$n\f$ is PTransfBase::degree()
  //! of the 1st parameter \p f; and
  //! * `f[i] != UNDEFINED`.
  //!
  //! \tparam T the type of the 1st argument (Transf, libsemigroups::PTransf,
  //! PPerm, or Perm)
  //! \param f the partial transformation whose image is sought.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! \f$O(n)\f$ where \f$n\f$ equals PTransfBase::degree() of \p x.
  //!
  //! \sa \ref image
  template <typename T>
  [[nodiscard]] std::vector<typename T::point_type> domain(T const& f);

  //! \ingroup transf_group
  //! \brief Returns the identity transformation of same degree as a sample.
  //!
  //! This function returns a newly constructed partial transformation with
  //! degree equal to the degree of \p f that fixes every value from \c 0
  //! to f.degree().
  //!
  //! \tparam T the type of the transformation being constructed (Perm, PPerm,
  //! Transf, or PTransf).
  //!
  //! \returns
  //! A value of type \c T.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  template <typename T>
  [[nodiscard]] T one(T const& f) {
    static_assert(IsDerivedFromPTransf<T>,
                  "the template parameter T must be derived from "
                  "PTransfPolymorphicBase");
    return T::one(f.degree());
  }

  //! \ingroup transf_group
  //! \brief Returns the right one of a partial perm.
  //!
  //! This function returns a newly constructed partial perm with degree equal
  //! to \p N that fixes every value in the image of the argument \p f, and is
  //! \ref UNDEFINED on any other values.
  //!
  //! \returns
  //! A value of type PPerm<N, Scalar>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  template <size_t N, typename Scalar>
  [[nodiscard]] PPerm<N, Scalar> right_one(PPerm<N, Scalar> const& x);
  // TODO void pass by reference version

  //! \ingroup transf_group
  //! \brief Returns the left one of a partial perm.
  //!
  //! This function returns a newly constructed partial perm with degree equal
  //! to \p N that fixes every value in the domain of \p f, and is \ref
  //! UNDEFINED on any other values.
  //!
  //! \returns
  //! A value of type PPerm<N, Scalar>.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  template <size_t N, typename Scalar>
  [[nodiscard]] PPerm<N, Scalar> left_one(PPerm<N, Scalar> const& x);
  // TODO void pass by reference version

  //! \ingroup transf_group
  //! \brief Replace contents of a partial perm with the inverse of another.
  //!
  //! This function inverts \p from into \c to. The *inverse* of a partial
  //! permutation \f$f\f$ is the partial perm \f$g\f$ such that \f$fgf = f\f$
  //! and \f$gfg = g\f$.
  //!
  //! \param from the partial perm to invert.
  //! \param to the partial perm to hold the inverse of \p from.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  // Put the inverse of this into that
  template <size_t N, typename Scalar>
  void inverse(PPerm<N, Scalar> const& from, PPerm<N, Scalar>& to);

  //! \ingroup transf_group
  //! \brief Returns the inverse of a partial perm.
  //!
  //! This function returns a newly constructed inverse of \p f. The *inverse*
  //! of a partial permutation \f$f\f$ is the partial perm \f$g\f$ such that
  //! \f$fgf = f\f$ and \f$gfg = g\f$.
  //!
  //! \tparam N the degree of \p f
  //! \tparam Scalar the type of points
  //!
  //! \param f the partial perm to invert.
  //!
  //! \returns
  //! A value of type \c PPerm.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  //!
  //! \sa right_one and left_one
  template <size_t N, typename Scalar>
  [[nodiscard]] PPerm<N, Scalar> inverse(PPerm<N, Scalar> const& f) {
    PPerm<N, Scalar> to(f.degree());
    inverse(f, to);
    return to;
  }

  //! \ingroup transf_group
  //! \brief Returns the inverse of a permutation.
  //!
  //! This function inverts \p from into \c to. The *inverse* of a permutation
  //! \f$f\f$ is the permutation \f$g\f$ such that \f$fg = gf\f$ is the identity
  //! permutation of degree \f$n\f$.
  //!
  //! \tparam N the degree of \p f
  //! \tparam Scalar the type of points of \p f
  //!
  //! \param from the permutation to invert.
  //! \param to the permutation to hold the inverse of \p from.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  template <size_t N, typename Scalar>
  void inverse(Perm<N, Scalar> const& from, Perm<N, Scalar>& to);

  //! \ingroup transf_group
  //! \brief Returns the inverse of a permutation.
  //!
  //! This function returns a newly constructed inverse of \p f. The *inverse*
  //! of a permutation \f$f\f$ is the permutation \f$g\f$ such that \f$fg =
  //! gf\f$ is the identity permutation of degree \f$N\f$.
  //!
  //! \tparam N the degree of \p f
  //! \tparam Scalar the type of points of \p f
  //!
  //! \param f the permutation to invert.
  //!
  //! \returns
  //! A value of type \c PPerm.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! Linear in degree()
  template <size_t N, typename Scalar>
  [[nodiscard]] Perm<N, Scalar> inverse(Perm<N, Scalar> const& f);

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  struct Degree<T, std::enable_if_t<IsDerivedFromPTransf<T>>> {
    [[nodiscard]] constexpr size_t operator()(T const& x) const noexcept {
      return x.degree();
    }
  };

  template <typename T>
  struct One<T, std::enable_if_t<IsDerivedFromPTransf<T>>> {
    [[nodiscard]] T operator()(T const& x) const {
      return (*this)(x.degree());
    }

    [[nodiscard]] T operator()(size_t N) const {
      return T::one(N);
    }
  };

  template <size_t N, typename Scalar>
  struct Inverse<Perm<N, Scalar>> {
    [[nodiscard]] Perm<N, Scalar> operator()(Perm<N, Scalar> const& x) {
      return inverse(x);
    }
  };

  template <typename Subclass>
  struct Product<Subclass, std::enable_if_t<IsDerivedFromPTransf<Subclass>>> {
    void
    operator()(Subclass& xy, Subclass const& x, Subclass const& y, size_t = 0) {
      xy.product_inplace(x, y);
    }
  };

  template <typename T>
  struct Hash<T, std::enable_if_t<IsDerivedFromPTransf<T>>> {
    [[nodiscard]] constexpr size_t operator()(T const& x) const {
      return x.hash_value();
    }
  };

  template <typename T>
  struct Complexity<T, std::enable_if_t<IsDerivedFromPTransf<T>>> {
    [[nodiscard]] constexpr size_t operator()(T const& x) const noexcept {
      return x.degree();
    }
  };

  //! Specialization of the adapter IncreaseDegree for type derived from
  //! PTransfPolymorphicBase.
  //!
  //! \sa IncreaseDegree.
  template <typename T>
  struct IncreaseDegree<T, std::enable_if_t<IsDerivedFromPTransf<T>>> {
    //! Returns \p x->increase_degree_by(\p n).
    inline void operator()(T& x, size_t n) const {
      x.increase_degree_by(n);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // ImageRight/LeftAction - Transf
  ////////////////////////////////////////////////////////////////////////

  // Equivalent to OnSets in GAP
  // Slowest
  // works for T = std::vector and T = StaticVector1
  //! Specialization of the adapter ImageRightAction for instances of
  //! Transformation and std::vector.
  //!
  //! \sa ImageRightAction
  template <size_t N, typename Scalar, typename T>
  struct ImageRightAction<Transf<N, Scalar>, T> {
    //! Stores the image set of \c pt under \c x in \p res.
    void operator()(T& res, T const& pt, Transf<N, Scalar> const& x) const;
  };

  // Fastest, but limited to at most degree 64
  //! Specialization of the adapter ImageRightAction for instances of
  //! Transformation and BitSet<N> (\f$0 \leq N leq 64\f$).
  //!
  //! \sa ImageRightAction
  template <size_t N, typename Scalar, size_t M>
  struct ImageRightAction<Transf<N, Scalar>, BitSet<M>> {
    //! Stores the image set of \c pt under \c x in \p res.
    void operator()(BitSet<M>&               res,
                    BitSet<M> const&         pt,
                    Transf<N, Scalar> const& x) const {
      res.reset();
      // Apply the lambda to every set bit in pt
      pt.apply([&x, &res](size_t i) { res.set(x[i]); });
    }
  };

  // OnKernelAntiAction
  // T = StaticVector1<S> or std::vector<S>
  //! Specialization of the adapter ImageLeftAction for instances of
  //! Transformation and std::vector.
  //!
  //! \sa ImageLeftAction
  template <size_t N, typename Scalar, typename T>
  struct ImageLeftAction<Transf<N, Scalar>, T> {
    //! Stores the image of \p pt under the left action of \p x in \p res.
    void operator()(T& res, T const& pt, Transf<N, Scalar> const& x) const;
  };

  ////////////////////////////////////////////////////////////////////////
  // Lambda/Rho - Transformation
  ////////////////////////////////////////////////////////////////////////

  // This currently limits the use of Konieczny to transformation of degree at
  // most 64 with the default traits class, since we cannot know the degree at
  // compile time, only at run time.
  //! Specialization of the adapter LambdaValue for instances of
  //! Transformation. Note that the the type chosen here limits the Konieczny
  //! algorithm to Transformations of degree at most 64 (or 32 on 32-bit
  //! systems).
  //!
  //! \sa LambdaValue.
  template <size_t N, typename Scalar>
  struct LambdaValue<Transf<N, Scalar>> {
    //! For transformations, \c type is the largest BitSet available,
    //! representing the image set.
    using type = BitSet<BitSet<1>::max_size()>;
  };

  // Benchmarks indicate that using std::vector yields similar performance to
  // using StaticVector1's.
  //! Specialization of the adapter RhoValue for instances of Transformation.
  //!
  //! \sa RhoValue.
  template <size_t N, typename Scalar>
  struct RhoValue<Transf<N, Scalar>> {
    //! For Transf<N, Scalar>s, \c type is std::vector<T>, representing the
    //! kernel of the Transformations.
    using type = std::vector<Scalar>;
  };

  // T = std::vector or StaticVector1
  //! Specialization of the adapter Lambda for instances of Transformation and
  //! std::vector.
  //!
  //! \sa Lambda.
  template <size_t N, typename Scalar, typename T>
  struct Lambda<Transf<N, Scalar>, T> {
    // not noexcept because std::vector::resize isn't (although
    // StaticVector1::resize is).
    //! Modifies \p res to contain the image set of \p x; that is, \p res[i]
    //! will be \c true if and only if `x[j] = i` for some \f$j\f$.
    void operator()(T& res, Transf<N, Scalar> const& x) const;
  };

  //! Specialization of the adapter Lambda for instances of Transformation and
  //! BitSet<N>.
  //!
  //! \sa Lambda.
  template <size_t N, typename Scalar, size_t M>
  struct Lambda<Transf<N, Scalar>, BitSet<M>> {
    // not noexcept because it can throw
    //! Modifies \p res to contain the image set of \p x; that is, \p res[i]
    //! will be \c true if and only if `x[j] = i` for some \f$j\f$.
    void operator()(BitSet<M>& res, Transf<N, Scalar> const& x) const;
  };

  // T = std::vector<S> or StaticVector1<S, N>
  //! Specialization of the adapter Rho for instances of Transf<N, Scalar> and
  //! std::vector<S> or detail::StaticVector1<S, N>
  //!
  //! \sa Rho.
  template <size_t N, typename Scalar, typename T>
  struct Rho<Transf<N, Scalar>, T> {
    //! Replace the contents of the first argument with the rho-value of a
    //! transformation.
    //!
    //! \param res the container for the result.
    //! \param x the transf.
    //!
    //! \complexity
    //! Linear in `x.degree()`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // not noexcept because std::vector::resize isn't (although
    // StaticVector1::resize is).
    void operator()(T& res, Transf<N, Scalar> const& x) const;
  };

  //! Specialization of the adapter Rank for instances of Transf<N, Scalar>.
  //!
  //! \sa Rank.
  template <size_t N, typename Scalar>
  struct Rank<Transf<N, Scalar>> {
    //! Operator returning the rank of a Transf<N, Scalar>.
    //!
    //! \param x the transf.
    //!
    //! \returns
    //! A value of type \c size_t.
    //!
    //! \complexity
    //! See Transf::rank.
    //!
    //! \exceptions
    //! See Transf::rank.
    [[nodiscard]] size_t operator()(Transf<N, Scalar> const& x) const {
      return x.rank();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // ImageRight/LeftAction - PPerm
  ////////////////////////////////////////////////////////////////////////

  // Slowest
  //! Specialization of the adapter ImageRightAction for instances of
  //! PPerm.
  //!
  //! \sa ImageRightAction
  template <size_t N, typename Scalar>
  struct ImageRightAction<PPerm<N, Scalar>, PPerm<N, Scalar>> {
    //! Stores the idempotent \f$(xy) ^ {-1}xy\f$ in \p res.
    void operator()(PPerm<N, Scalar>&       res,
                    PPerm<N, Scalar> const& pt,
                    PPerm<N, Scalar> const& x) const noexcept {
      res.product_inplace(pt, x);
      res = right_one(res);
    }
  };

  // Faster than the above, but slower than the below
  // works for T = std::vector and T = StaticVector1
  //! Specialization of the adapter ImageRightAction for instances of
  //! PPerm and std::vector.
  //!
  //! \sa ImageRightAction
  template <size_t N, typename Scalar, typename T>
  struct ImageRightAction<PPerm<N, Scalar>, T> {
    //! Stores the image set of \c pt under \c x in \p res.
    void operator()(T& res, T const& pt, PPerm<N, Scalar> const& x) const;
  };

  // Fastest, but limited to at most degree 64
  //! Specialization of the adapter ImageRightAction for instances of
  //! PPerm and BitSet<N> (\f$0 \leq N \leq 64\f$).
  //!
  //! \sa ImageRightAction
  template <size_t N, typename Scalar, size_t M>
  struct ImageRightAction<PPerm<N, Scalar>, BitSet<M>> {
    //! Stores the image set of \c pt under \c x in \p res.
    void operator()(BitSet<M>&              res,
                    BitSet<M> const&        pt,
                    PPerm<N, Scalar> const& x) const;
  };

  // Slowest
  //! Specialization of the adapter ImageLeftAction for instances of
  //! PPerm.
  //!
  //! \sa ImageLeftAction.
  template <size_t N, typename Scalar>
  struct ImageLeftAction<PPerm<N, Scalar>, PPerm<N, Scalar>> {
    //! Stores the idempotent \f$xy(xy) ^ {-1}\f$ in \p res.
    void operator()(PPerm<N, Scalar>&       res,
                    PPerm<N, Scalar> const& pt,
                    PPerm<N, Scalar> const& x) const noexcept {
      res.product_inplace(x, pt);
      res = left_one(res);
    }
  };

  // Fastest when used with BitSet<N>.
  // works for T = std::vector and T = BitSet<N>
  // Using BitSet<N> limits this to size 64. However, if we are trying to
  // compute a LeftAction object, then the max size of such is 2 ^ 64, which
  // is probably not achievable. So, for higher degrees, we will only be able
  // to compute relatively sparse LeftActions (i.e. not containing the
  // majority of the 2 ^ n possible subsets), in which case using vectors or
  // StaticVector1's might be not be appreciable slower anyway. All of this is
  // to say that it probably isn't worthwhile trying to make BitSet's work for
  // more than 64 bits.
  //! Specialization of the adapter ImageLeftAction for instances of
  //! PPerm and std::vector or BitSet<N> (\f$0 \leq N \leq 64\f$).
  //!
  //! \sa ImageLeftAction.
  template <size_t N, typename Scalar, typename T>
  struct ImageLeftAction<PPerm<N, Scalar>, T> {
    void operator()(T& res, T const& pt, PPerm<N, Scalar> const& x) const {
      //! Stores the inverse image set of \c pt under \c x in \p res.
      static PPerm<N, Scalar> xx(x.degree());
      inverse(x, xx);  // invert x into xx
      ImageRightAction<PPerm<N, Scalar>, T>()(res, pt, xx);
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Lambda/Rho - PPerm
  ////////////////////////////////////////////////////////////////////////

  // This currently limits the use of Konieczny to partial perms of degree at
  // most 64 with the default traits class, since we cannot know the degree
  // at compile time, only at run time.
  //! Specialization of the adapter LambdaValue for instances of PPerm.
  //! Note that the the type chosen here limits the Konieczny algorithm to
  //! PPerms of degree at most 64 (or 32 on 32-bit systems).
  //!
  //! \sa LambdaValue.
  template <size_t N, typename Scalar>
  struct LambdaValue<PPerm<N, Scalar>> {
    //! For PPerms, \c type is BitSet<N>, representing the image of the
    //! PPerms.
    using type = BitSet<BitSet<1>::max_size()>;
  };

  //! Specialization of the adapter RhoValue for instances of PPerm.
  //! Note that the the type chosen here limits the Konieczny algorithm to
  //! PPerms of degree at most 64 (or 32 on 32-bit systems).
  //!
  //! \sa RhoValue.
  template <size_t N, typename Scalar>
  struct RhoValue<PPerm<N, Scalar>> {
    //! For PPerms, \c type is BitSet<N>, representing the domain of the
    //! PPerms.
    using type = typename LambdaValue<PPerm<N, Scalar>>::type;
  };

  //! Specialization of the adapter Lambda for instances of PPerm and
  //! BitSet<N>.
  //!
  //! \sa Lambda.
  template <size_t N, typename Scalar, size_t M>
  struct Lambda<PPerm<N, Scalar>, BitSet<M>> {
    //! Modifies \p res to contain the image set of \p x; that is, \p res[i]
    //! will be \c true if and only if `x[j] = i` for some \f$j\f$.
    void operator()(BitSet<M>& res, PPerm<N, Scalar> const& x) const;
  };

  //! Specialization of the adapter Rho for instances of PPerm and
  //! BitSet<N>.
  //!
  //! \sa Rho.
  template <size_t N, typename Scalar, size_t M>
  struct Rho<PPerm<N, Scalar>, BitSet<M>> {
    //! Modifies \p res to contain the domain of \p x; that is, \p res[i]
    //! will be \c true if and only if `x[i] != UNDEFINED`.
    void operator()(BitSet<M>& res, PPerm<N, Scalar> const& x) const;
  };

  //! Specialization of the adapter Rank for instances of PPerm.
  //!
  //! \sa Rank and PPerm::rank.
  template <size_t N, typename Scalar>
  struct Rank<PPerm<N, Scalar>> {
    //! Returns the rank of \p x.
    //!
    //! The rank of a PPerm is the number of points in the image.
    [[nodiscard]] size_t operator()(PPerm<N, Scalar> const& x) const {
      return x.rank();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Perm
  ////////////////////////////////////////////////////////////////////////

  //! Specialization of the adapter ImageRightAction for instances of
  //! Permutation.
  //!
  //! \sa ImageRightAction.
  // TODO(later) this could work for everything derived from PTransf
  template <size_t N, typename Scalar, typename T>
  struct ImageRightAction<Perm<N, Scalar>,
                          T,
                          std::enable_if_t<std::is_integral_v<T>>> {
    //! Stores the image of \p pt under the action of \p p in \p res.
    void operator()(T&                     res,
                    T const&               pt,
                    Perm<N, Scalar> const& p) const noexcept {
      LIBSEMIGROUPS_ASSERT(pt < p.degree());
      res = p[pt];
    }

    //! Returns the image of \p pt under the action of \p p.
    [[nodiscard]] T operator()(T pt, Perm<N, Scalar> const& x) {
      return x[pt];
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Helpers
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <size_t N>
    struct LeastTransfHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
      using type = std::conditional_t<N >= 17, Transf<N>, HPCombi::Transf16>;
#else
      using type = Transf<N>;
#endif
    };

    template <size_t N>
    struct LeastPPermHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
      using type = std::conditional_t<N >= 17, PPerm<N>, HPCombi::PPerm16>;
#else
      using type = PPerm<N>;
#endif
    };

    template <size_t N>
    struct LeastPermHelper {
#ifdef LIBSEMIGROUPS_HPCOMBI_ENABLED
      using type = std::conditional_t<N >= 17, Perm<N>, HPCombi::Perm16>;
#else
      using type = Perm<N>;
#endif
    };
  }  // namespace detail

  //! \ingroup transf_group
  //!
  //! \brief Type of transformations using the least memory for a given degree.
  //!
  //! Helper for getting the type of the least size, and fastest,
  //! transformation in ``libsemigroups`` or ``HPCombi`` that are defined on
  //! at most \c N points.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! \tparam N the maximum number of points the transformations will be
  //! defined on.
  template <size_t N>
  using LeastTransf = typename detail::LeastTransfHelper<N>::type;

  //! \ingroup transf_group
  //!
  //! \brief Type of partial perms using the least memory for a given degree.
  //!
  //! Helper for getting the type of the least size, and fastest, partial perm
  //! in ``libsemigroups`` or ``HPCombi`` that are defined on at most \c N
  //! points.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! \tparam N the maximum number of points the partial perms will be defined
  //! on.
  template <size_t N>
  using LeastPPerm = typename detail::LeastPPermHelper<N>::type;

  //! \ingroup transf_group
  //!
  //! \brief Type of perms using the least memory for a given degree.
  //!
  //! Helper for getting the type of the least size, and fastest, permutation
  //! in ``libsemigroups`` or ``HPCombi`` that are defined on at most \c N
  //! points.
  //!
  //! Defined in ``transf.hpp``.
  //!
  //! \tparam N the maximum number of points the permutations will be defined
  //! on.
  template <size_t N>
  using LeastPerm = typename detail::LeastPermHelper<N>::type;

}  // namespace libsemigroups

#include "transf.tpp"

#endif  // LIBSEMIGROUPS_TRANSF_HPP_
