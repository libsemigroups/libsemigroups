//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_
#define LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_

#include <algorithm>         // for copy, equal, fill
#include <cstddef>           // for size_t
#include <cstdint>           // for uint64_t
#include <initializer_list>  // for initializer_list
#include <iosfwd>            // for ostream
#include <iterator>          // for distance
#include <numeric>           // for inner_product
#include <string>            // for basic_string
#include <string_view>       // for basic_string_view
#include <type_traits>       // for is_same_v, ena...
#include <utility>           // for swap, make_pair
#include <vector>            // for vector

#include "libsemigroups/adapters.hpp"   // for Hash
#include "libsemigroups/constants.hpp"  // for NEGATIVE_INFINITY
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_...
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_...
#include "libsemigroups/is-matrix.hpp"  // for IsMatrix, Matr...

#include "fmt.hpp"                // for format
#include "matrix-exceptions.hpp"  // for throw_if_bad_dim
#include "string.hpp"             // for to_string

namespace libsemigroups::detail {
  // This function is required for exceptions and to_human_readable_repr, so
  // that if we encounter an entry of a matrix (Scalar type), then it can be
  // printed correctly. If we just did fmt::format("{}", val) and val ==
  // POSITIVE_INFINITY, but the type of val is, say, size_t, then this
  // wouldn't use the formatter for PositiveInfinity.
  //
  // Also in fmt v11.1.4 the custom formatter for POSITIVE_INFINITY and
  // NEGATIVE_INFINITY stopped working (and I wasn't able to figure out why)
  template <typename Scalar>
  [[nodiscard]] std::string entry_repr(Scalar a);

  template <typename Container,
            typename Subclass,
            typename TRowView,
            typename Semiring = void>
  class MatrixCommon : MatrixPolymorphicBase {
   public:
    ////////////////////////////////////////////////////////////////////////
    // MatrixCommon - Aliases - public
    ////////////////////////////////////////////////////////////////////////

    using scalar_type            = typename Container::value_type;
    using scalar_reference       = typename Container::reference;
    using scalar_const_reference = typename Container::const_reference;
    using semiring_type          = Semiring;

    using container_type = Container;
    using iterator       = typename Container::iterator;
    using const_iterator = typename Container::const_iterator;

    using RowView = TRowView;

    scalar_type scalar_one() const noexcept {
      return static_cast<Subclass const*>(this)->one_impl();
    }

    scalar_type scalar_zero() const noexcept {
      return static_cast<Subclass const*>(this)->zero_impl();
    }

    Semiring const* semiring() const noexcept {
      return static_cast<Subclass const*>(this)->semiring_impl();
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // MatrixCommon - Semiring arithmetic - private
    ////////////////////////////////////////////////////////////////////////

    scalar_type plus_no_checks(scalar_type x, scalar_type y) const noexcept {
      return static_cast<Subclass const*>(this)->plus_no_checks_impl(y, x);
    }

    scalar_type product_no_checks(scalar_type x, scalar_type y) const noexcept {
      return static_cast<Subclass const*>(this)->product_no_checks_impl(y, x);
    }

   protected:
    ////////////////////////////////////////////////////////////////////////
    // MatrixCommon - Container functions - protected
    ////////////////////////////////////////////////////////////////////////

    // TODO(1) use constexpr-if, not SFINAE
    template <typename SFINAE = container_type>
    auto resize(size_t r, size_t c)
        -> std::enable_if_t<std::is_same_v<SFINAE, std::vector<scalar_type>>> {
      _container.resize(r * c);
    }

    template <typename SFINAE = container_type>
    auto resize(size_t, size_t)
        -> std::enable_if_t<!std::is_same_v<SFINAE, std::vector<scalar_type>>> {
    }

   public:
    ////////////////////////////////////////////////////////////////////////
    // MatrixCommon - Constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    // none of the constructors are noexcept because they allocate
    MatrixCommon()                               = default;
    MatrixCommon(MatrixCommon const&)            = default;
    MatrixCommon(MatrixCommon&&)                 = default;
    MatrixCommon& operator=(MatrixCommon const&) = default;
    MatrixCommon& operator=(MatrixCommon&&)      = default;

    explicit MatrixCommon(std::initializer_list<scalar_type> const& c)
        : MatrixCommon() {
      resize(1, c.size());
      std::copy(c.begin(), c.end(), _container.begin());
    }

    explicit MatrixCommon(std::vector<std::vector<scalar_type>> const& m)
        : MatrixCommon() {
      init(m);
    }

    MatrixCommon(
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixCommon() {
      init(m);
    }

   private:
    // not noexcept because resize isn't
    template <typename T>
    void init(T const& m);

    // not noexcept because init isn't
    void
    init(std::initializer_list<std::initializer_list<scalar_type>> const& m) {
      init<std::initializer_list<std::initializer_list<scalar_type>>>(m);
    }

   public:
    explicit MatrixCommon(RowView const& rv) : MatrixCommon() {
      resize(1, rv.size());
      std::copy(rv.cbegin(), rv.cend(), _container.begin());
    }

    ~MatrixCommon() = default;

    // not noexcept because mem allocate is required
    Subclass one() const;

    ////////////////////////////////////////////////////////////////////////
    // Comparison operators
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because apparently vector::operator== isn't
    bool operator==(MatrixCommon const& that) const {
      return _container == that._container;
    }

    // not noexcept because apparently vector::operator== isn't
    bool operator==(RowView const& that) const {
      return number_of_rows() == 1
             && static_cast<RowView>(*static_cast<Subclass const*>(this))
                    == that;
    }

    // not noexcept because apparently vector::operator< isn't
    bool operator<(MatrixCommon const& that) const {
      return _container < that._container;
    }

    // not noexcept because apparently vector::operator< isn't
    bool operator<(RowView const& that) const {
      return number_of_rows() == 1
             && static_cast<RowView>(*static_cast<Subclass const*>(this))
                    < that;
    }

    // not noexcept because operator== isn't
    template <typename T>
    bool operator!=(T const& that) const {
      static_assert(IsMatrix<T> || std::is_same_v<T, RowView>);
      return !(*this == that);
    }

    // not noexcept because operator< isn't
    template <typename T>
    bool operator>(T const& that) const {
      static_assert(IsMatrix<T> || std::is_same_v<T, RowView>);
      return that < *this;
    }

    // not noexcept because operator< isn't
    template <typename T>
    bool operator>=(T const& that) const {
      static_assert(IsMatrix<T> || std::is_same_v<T, RowView>);
      return that < *this || that == *this;
    }

    // not noexcept because operator< isn't
    template <typename T>
    bool operator<=(T const& that) const {
      static_assert(IsMatrix<T> || std::is_same_v<T, RowView>);
      return *this < that || that == *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Attributes
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because vector::operator[] isn't, and neither is
    // array::operator[]
    scalar_reference operator()(size_t r, size_t c) {
      return this->_container[r * number_of_cols() + c];
    }

    scalar_reference at(size_t r, size_t c) {
      matrix::throw_if_bad_coords(static_cast<Subclass const&>(*this), r, c);
      return this->operator()(r, c);
    }

    // not noexcept because vector::operator[] isn't, and neither is
    // array::operator[]
    scalar_const_reference operator()(size_t r, size_t c) const {
      return this->_container[r * number_of_cols() + c];
    }

    scalar_const_reference at(size_t r, size_t c) const {
      matrix::throw_if_bad_coords(static_cast<Subclass const&>(*this), r, c);
      return this->operator()(r, c);
    }

    // noexcept because number_of_rows_impl is noexcept
    size_t number_of_rows() const noexcept {
      return static_cast<Subclass const*>(this)->number_of_rows_impl();
    }

    // noexcept because number_of_cols_impl is noexcept
    size_t number_of_cols() const noexcept {
      return static_cast<Subclass const*>(this)->number_of_cols_impl();
    }

    // not noexcept because Hash<T>::operator() isn't
    size_t hash_value() const {
      return Hash<Container>()(_container);
    }

    ////////////////////////////////////////////////////////////////////////
    // Arithmetic operators - in-place
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because memory is allocated
    void product_inplace_no_checks(Subclass const& A, Subclass const& B);

    void product_inplace(Subclass const& A, Subclass const& B);

    // not noexcept because iterator increment isn't
    void operator*=(scalar_type a) {
      for (auto it = _container.begin(); it < _container.end(); ++it) {
        *it = product_no_checks(*it, a);
      }
    }

    void plus_inplace_no_checks(Subclass const& that);

    // not noexcept because vector::operator[] and array::operator[] aren't
    void operator+=(Subclass const& that);

    void plus_inplace_no_checks(RowView const& that) {
      LIBSEMIGROUPS_ASSERT(number_of_rows() == 1);
      LIBSEMIGROUPS_ASSERT(number_of_cols() == that.size());
      RowView(*static_cast<Subclass const*>(this)) += that;
    }

    void operator+=(RowView const& that);

    void operator+=(scalar_type a) {
      for (auto it = _container.begin(); it < _container.end(); ++it) {
        *it = plus_no_checks(*it, a);
      }
    }

    // TODO(2) implement operator*=(Subclass const&)

    ////////////////////////////////////////////////////////////////////////
    // Arithmetic operators - not in-place
    ////////////////////////////////////////////////////////////////////////

    Subclass plus_no_checks(Subclass const& y) const {
      Subclass result(*static_cast<Subclass const*>(this));
      result.plus_inplace_no_checks(y);
      return result;
    }

    Subclass operator+(Subclass const& y) const;

    Subclass product_no_checks(Subclass const& y) const {
      Subclass result(*static_cast<Subclass const*>(this));
      result.product_inplace_no_checks(*static_cast<Subclass const*>(this), y);
      return result;
    }

    // not noexcept because product_inplace_no_checks isn't
    Subclass operator*(Subclass const& y) const;

    Subclass operator*(scalar_type a) const {
      Subclass result(*static_cast<Subclass const*>(this));
      result *= a;
      return result;
    }

    Subclass operator+(scalar_type a) const {
      Subclass result(*static_cast<Subclass const*>(this));
      result += a;
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Iterators
    ////////////////////////////////////////////////////////////////////////

    // noexcept because vector::begin and array::begin are noexcept
    iterator begin() noexcept {
      return _container.begin();
    }

    // noexcept because vector::end and array::end are noexcept
    iterator end() noexcept {
      return _container.end();
    }

    // noexcept because vector::begin and array::begin are noexcept
    const_iterator begin() const noexcept {
      return _container.begin();
    }

    // noexcept because vector::end and array::end are noexcept
    const_iterator end() const noexcept {
      return _container.end();
    }

    // noexcept because vector::cbegin and array::cbegin are noexcept
    const_iterator cbegin() const noexcept {
      return _container.cbegin();
    }

    // noexcept because vector::cend and array::cend are noexcept
    const_iterator cend() const noexcept {
      return _container.cend();
    }

    template <typename Iterator>
    std::pair<scalar_type, scalar_type> coords(Iterator const& it) const;

    ////////////////////////////////////////////////////////////////////////
    // Modifiers
    ////////////////////////////////////////////////////////////////////////

    // noexcept because vector::swap and array::swap are noexcept
    void swap(MatrixCommon& that) noexcept {
      std::swap(_container, that._container);
    }

    // noexcept because swap is noexcept, and so too are number_of_rows and
    // number_of_cols
    void transpose_no_checks() noexcept;

    void transpose() {
      matrix::throw_if_not_square(static_cast<Subclass&>(*this));
      transpose_no_checks();
    }

    ////////////////////////////////////////////////////////////////////////
    // Rows
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because there's an allocation
    RowView row_no_checks(size_t i) const;

    RowView row(size_t i) const;

    // not noexcept because there's an allocation
    template <typename T>
    void rows(T& x) const;

    ////////////////////////////////////////////////////////////////////////
    // Friend functions
    ////////////////////////////////////////////////////////////////////////

    friend std::ostream& operator<<(std::ostream& os, MatrixCommon const& x) {
      os << detail::to_string(x);
      return os;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // Private data
    ////////////////////////////////////////////////////////////////////////
    container_type _container;
  };  // class MatrixCommon

  template <typename Scalar>
  class MatrixDynamicDim {
   public:
    MatrixDynamicDim() : _number_of_cols(0), _number_of_rows(0) {}
    MatrixDynamicDim(MatrixDynamicDim const&)            = default;
    MatrixDynamicDim(MatrixDynamicDim&&)                 = default;
    MatrixDynamicDim& operator=(MatrixDynamicDim const&) = default;
    MatrixDynamicDim& operator=(MatrixDynamicDim&&)      = default;

    MatrixDynamicDim(size_t r, size_t c)
        : _number_of_cols(c), _number_of_rows(r) {}

    ~MatrixDynamicDim() = default;

    void swap(MatrixDynamicDim& that) noexcept {
      std::swap(_number_of_cols, that._number_of_cols);
      std::swap(_number_of_rows, that._number_of_rows);
    }

   protected:
    size_t number_of_rows_impl() const noexcept {
      return _number_of_rows;
    }

    size_t number_of_cols_impl() const noexcept {
      return _number_of_cols;
    }

   private:
    size_t _number_of_cols;
    size_t _number_of_rows;
  };

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  struct MatrixStaticArithmetic {
    MatrixStaticArithmetic()                                         = default;
    MatrixStaticArithmetic(MatrixStaticArithmetic const&)            = default;
    MatrixStaticArithmetic(MatrixStaticArithmetic&&)                 = default;
    MatrixStaticArithmetic& operator=(MatrixStaticArithmetic const&) = default;
    MatrixStaticArithmetic& operator=(MatrixStaticArithmetic&&)      = default;

    using scalar_type = Scalar;

   protected:
    static constexpr scalar_type plus_no_checks_impl(scalar_type x,
                                                     scalar_type y) noexcept {
      return PlusOp()(x, y);
    }

    static constexpr scalar_type
    product_no_checks_impl(scalar_type x, scalar_type y) noexcept {
      return ProdOp()(x, y);
    }

    static constexpr scalar_type one_impl() noexcept {
      return OneOp()();
    }

    static constexpr scalar_type zero_impl() noexcept {
      return ZeroOp()();
    }

    static constexpr void const* semiring_impl() noexcept {
      return nullptr;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // RowViews - class for cheaply storing iterators to rows
  ////////////////////////////////////////////////////////////////////////

  template <typename Mat, typename Subclass>
  class RowViewCommon {
    static_assert(IsMatrix<Mat>,
                  "the template parameter Mat must be derived from "
                  "MatrixPolymorphicBase");

   public:
    using const_iterator = typename Mat::const_iterator;
    using iterator       = typename Mat::iterator;

    using scalar_type            = typename Mat::scalar_type;
    using scalar_reference       = typename Mat::scalar_reference;
    using scalar_const_reference = typename Mat::scalar_const_reference;

    using Row         = typename Mat::Row;
    using matrix_type = Mat;

    size_t size() const noexcept {
      return static_cast<Subclass const*>(this)->length_impl();
    }

   private:
    scalar_type plus_no_checks(scalar_type x, scalar_type y) const noexcept {
      return static_cast<Subclass const*>(this)->plus_no_checks_impl(y, x);
    }

    scalar_type product_no_checks(scalar_type x, scalar_type y) const noexcept {
      return static_cast<Subclass const*>(this)->product_no_checks_impl(y, x);
    }

   public:
    RowViewCommon()                                = default;
    RowViewCommon(RowViewCommon const&)            = default;
    RowViewCommon(RowViewCommon&&)                 = default;
    RowViewCommon& operator=(RowViewCommon const&) = default;
    RowViewCommon& operator=(RowViewCommon&&)      = default;

    explicit RowViewCommon(Row const& r)
        : RowViewCommon(const_cast<Row&>(r).begin()) {}

    ~RowViewCommon() = default;

    // Not noexcept because iterator::operator[] isn't
    scalar_const_reference operator[](size_t i) const {
      return _begin[i];
    }

    // Not noexcept because iterator::operator[] isn't
    scalar_reference operator[](size_t i) {
      return _begin[i];
    }

    // Not noexcept because iterator::operator[] isn't
    scalar_const_reference operator()(size_t i) const {
      return (*this)[i];
    }

    // Not noexcept because iterator::operator[] isn't
    scalar_reference operator()(size_t i) {
      return (*this)[i];
    }

    // noexcept because begin() is
    const_iterator cbegin() const noexcept {
      return _begin;
    }

    // not noexcept because iterator arithmetic isn't
    const_iterator cend() const {
      return _begin + size();
    }

    // noexcept because begin() is
    const_iterator begin() const noexcept {
      return _begin;
    }

    // not noexcept because iterator arithmetic isn't
    const_iterator end() const {
      return _begin + size();
    }

    // noexcept because begin() is
    iterator begin() noexcept {
      return _begin;
    }

    // not noexcept because iterator arithmetic isn't
    iterator end() noexcept {
      return _begin + size();
    }

    ////////////////////////////////////////////////////////////////////////
    // Arithmetic operators
    ////////////////////////////////////////////////////////////////////////

    // not noexcept because operator[] isn't
    void plus_inplace_no_checks(RowViewCommon const& x);

    // TODO add tests
    void operator+=(RowViewCommon const& x);

    // not noexcept because operator+= isn't
    Row plus_no_checks(RowViewCommon const& that) const {
      Row result(*static_cast<Subclass const*>(this));
      result.plus_inplace_no_checks(static_cast<Subclass const&>(that));
      return result;
    }

    // TODO add tests
    Row operator+(RowViewCommon const& x);

    // not noexcept because iterator arithmetic isn't
    void operator+=(scalar_type a) {
      for (auto& x : *this) {
        x = plus_no_checks(x, a);
      }
    }

    // not noexcept because iterator arithmetic isn't
    void operator*=(scalar_type a) {
      for (auto& x : *this) {
        x = product_no_checks(x, a);
      }
    }

    // not noexcept because operator*= isn't
    Row operator*(scalar_type a) const {
      Row result(*static_cast<Subclass const*>(this));
      result *= a;
      return result;
    }

    template <typename U>
    bool operator==(U const& that) const {
      // TODO(1) static assert that U is Row or RowView
      return std::equal(begin(), end(), that.begin());
    }

    template <typename U>
    bool operator!=(U const& that) const {
      return !(*this == that);
    }

    template <typename U>
    bool operator<(U const& that) const {
      return std::lexicographical_compare(
          cbegin(), cend(), that.cbegin(), that.cend());
    }

    template <typename U>
    bool operator>(U const& that) const {
      return that < *this;
    }

    void swap(RowViewCommon& that) noexcept {
      std::swap(that._begin, _begin);
    }

    friend std::ostream& operator<<(std::ostream& os, RowViewCommon const& x) {
      os << detail::to_string(x);
      return os;
    }

   protected:
    explicit RowViewCommon(iterator first) : _begin(first) {}

   private:
    iterator _begin;
  };  // class RowViewCommon

  template <typename Mat, typename Subclass>
  void throw_if_bad_dim(RowViewCommon<Mat, Subclass> const& x,
                        RowViewCommon<Mat, Subclass> const& y,
                        std::string_view arg_desc_x = "the 1st argument",
                        std::string_view arg_desc_y = "the 2nd argument");
}  // namespace libsemigroups::detail

#include "matrix-common.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_MATRIX_COMMON_HPP_
