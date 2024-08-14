//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell
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

// TODO(0): document the adapters
// TODO(1) tpp file
// TODO(1): constexpr

#ifndef LIBSEMIGROUPS_MATRIX_HPP_
#define LIBSEMIGROUPS_MATRIX_HPP_

#include <algorithm>      // for min
#include <array>          // for array
#include <bitset>         // for bitset
#include <cstddef>        // for size_t
#include <iosfwd>         // for ostringstream
#include <numeric>        // for inner_product
#include <type_traits>    // for false_type, is_signed, true_type
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set
#include <vector>         // for vector

#include "adapters.hpp"   // for Degree
#include "bitset.hpp"     // for BitSet
#include "config.hpp"     // for PARSED_BY_DOXYGEN
#include "constants.hpp"  // for POSITIVE_INFINITY
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "detail/containers.hpp"  // for StaticVector1
#include "detail/string.hpp"      // for detail::to_string

namespace libsemigroups {

  //! \defgroup matrix_group Matrices
  //!
  //! This page contains links to the documentation of the functionality in
  //! libsemigroups for matrices.
  //!
  //! There are several different classes in ``libsemigroups`` for representing
  //! matrices over various semirings. There are up to three different
  //! representations for every type of matrix:
  //!
  //! 1. those whose dimension and arithmetic operations can be defined at
  //!    compile time: \ref StaticMatrix.
  //! 2. those whose arithmetic operation can be defined at compile time but
  //!    whose dimensions can be set at run time: \ref DynamicMatrix
  //! 3. those whose arithmetic operations and dimensions can be set at run
  //!    time: \ref DynamicMatrix
  //!
  //! It's unlikely that you will want to use the classes described on this
  //! page directly, but rather through the aliases described on the other
  //! matrix pages (such as, for example, BMat).
  //!
  //! # Over specific semirings
  //!
  //! The following matrix classes are provided which define matrices over some
  //! specific semirings:
  //!
  //! * \ref bmat_index_group
  //! * \ref intmat_group
  //! * \ref maxplusmat_group
  //! * \ref minplusmat_group
  //! * \ref maxplustruncmat_group
  //! * \ref minplustruncmat_group
  //! * \ref ntpmat_group
  //! * \ref projmaxplus_group
  //!
  //! # Over arbitrary semirings
  //!
  //! The following general matrix classes are provided which can be used to
  //! define matrices over arbitrary semirings:
  //!
  //! * \ref dynamicmatrix-compile
  //! * \ref dynamicmatrix-run
  //! * \ref StaticMatrix
  //! * \ref variable-templates
  //!
  //! # Row views
  //!
  //! A row view is a lightweight representation of a row of a matrix.  The
  //! following row view classes are provided:
  //!
  //! * \ref dynamicrowview
  //! * \ref staticrowview
  //!
  // TODO(0) incorporate this in the appropriate place
  // Adapters
  // --------
  //
  // There are various specialisations of the adapters described on `this
  // page<Adapters>` for the matrices described on this page:
  //
  // .. toctree::
  //    :maxdepth: 1
  //
  //    adapters/complexity
  //    adapters/degree
  //    adapters/increasedegreeby
  //    adapters/one
  //    adapters/product

  ////////////////////////////////////////////////////////////////////////
  // Detail
  ////////////////////////////////////////////////////////////////////////

  namespace detail {

    template <typename T>
    struct IsStdBitSetHelper : std::false_type {};

    template <size_t N>
    struct IsStdBitSetHelper<std::bitset<N>> : std::true_type {};

    struct MatrixPolymorphicBase {};

    template <typename T>
    struct IsMatrixHelper {
      static constexpr bool value
          = std::is_base_of<detail::MatrixPolymorphicBase, T>::value;
    };
  }  // namespace detail

  template <typename T>
  static constexpr bool IsStdBitSet = detail::IsStdBitSetHelper<T>::value;

  template <typename T>
  constexpr bool IsMatrix = detail::IsMatrixHelper<T>::value;

  ////////////////////////////////////////////////////////////////////////
  // Detail
  ////////////////////////////////////////////////////////////////////////
  namespace detail {
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

      scalar_type plus(scalar_type x, scalar_type y) const noexcept {
        return static_cast<Subclass const*>(this)->plus_impl(y, x);
      }

      scalar_type prod(scalar_type x, scalar_type y) const noexcept {
        return static_cast<Subclass const*>(this)->prod_impl(y, x);
      }

     protected:
      ////////////////////////////////////////////////////////////////////////
      // MatrixCommon - Container functions - protected
      ////////////////////////////////////////////////////////////////////////

      // TODO(1) use constexpr-if, not SFINAE
      template <typename SFINAE = container_type>
      auto resize(size_t r, size_t c)
          -> std::enable_if_t<
              std::is_same<SFINAE, std::vector<scalar_type>>::value> {
        _container.resize(r * c);
      }

      template <typename SFINAE = container_type>
      auto resize(size_t, size_t)
          -> std::enable_if_t<
              !std::is_same<SFINAE, std::vector<scalar_type>>::value> {}

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
      void init(T const& m) {
        size_t const R = m.size();
        if (R == 0) {
          return;
        }
        size_t const C = m.begin()->size();
        resize(R, C);
        for (size_t r = 0; r < R; ++r) {
          auto row = m.begin() + r;
          for (size_t c = 0; c < C; ++c) {
            _container[r * C + c] = *(row->begin() + c);
          }
        }
      }

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
      // TODO(0) identity -> one, hmmmm, this won't work because there's
      // already a mem fn "one", what do?
      Subclass identity() const {
        size_t const n = number_of_rows();
        Subclass     x(semiring(), n, n);
        std::fill(x.begin(), x.end(), scalar_zero());
        for (size_t r = 0; r < n; ++r) {
          x(r, r) = scalar_one();
        }
        return x;
      }

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
        return !(*this == that);
      }

      // not noexcept because operator< isn't
      template <typename T>
      bool operator>(T const& that) const {
        return that < *this;
      }

      ////////////////////////////////////////////////////////////////////////
      // Attributes
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because vector::operator[] isn't, and neither is
      // array::operator[]
      scalar_reference operator()(size_t r, size_t c) {
        return this->_container[r * number_of_cols() + c];
      }

      // not noexcept because vector::operator[] isn't, and neither is
      // array::operator[]
      scalar_const_reference operator()(size_t r, size_t c) const {
        return this->_container[r * number_of_cols() + c];
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
      void product_inplace_no_checks(Subclass const& A, Subclass const& B) {
        LIBSEMIGROUPS_ASSERT(number_of_rows() == number_of_cols());
        LIBSEMIGROUPS_ASSERT(A.number_of_rows() == number_of_rows());
        LIBSEMIGROUPS_ASSERT(B.number_of_rows() == number_of_rows());
        LIBSEMIGROUPS_ASSERT(A.number_of_cols() == number_of_cols());
        LIBSEMIGROUPS_ASSERT(B.number_of_cols() == number_of_cols());
        LIBSEMIGROUPS_ASSERT(&A != this);
        LIBSEMIGROUPS_ASSERT(&B != this);

        // Benchmarking boolean matrix multiplication reveals that using a
        // non-static container_type gives the best performance, when compared
        // to static container_type the performance is more or less the same
        // (but not thread-safe), and there appears to be a performance
        // penalty of about 50% when using static thread_local container_type
        // (when compiling with clang).
        size_t const             N = A.number_of_rows();
        std::vector<scalar_type> tmp(N, 0);

        for (size_t c = 0; c < N; c++) {
          for (size_t i = 0; i < N; i++) {
            tmp[i] = B(i, c);
          }
          for (size_t r = 0; r < N; r++) {
            (*this)(r, c) = std::inner_product(
                A._container.begin() + r * N,
                A._container.begin() + (r + 1) * N,
                tmp.begin(),
                scalar_zero(),
                [this](scalar_type x, scalar_type y) {
                  return this->plus(x, y);
                },
                [this](scalar_type x, scalar_type y) {
                  return this->prod(x, y);
                });
          }
        }
      }

      // not noexcept because iterator increment isn't
      void operator*=(scalar_type a) {
        for (auto it = _container.begin(); it < _container.end(); ++it) {
          *it = prod(*it, a);
        }
      }

      // not noexcept because vector::operator[] and array::operator[] aren't
      void operator+=(Subclass const& that) {
        LIBSEMIGROUPS_ASSERT(that.number_of_rows() == number_of_rows());
        LIBSEMIGROUPS_ASSERT(that.number_of_cols() == number_of_cols());
        for (size_t i = 0; i < _container.size(); ++i) {
          _container[i] = plus(_container[i], that._container[i]);
        }
      }

      void operator+=(RowView const& that) {
        LIBSEMIGROUPS_ASSERT(number_of_rows() == 1);
        RowView(*static_cast<Subclass const*>(this)) += that;
      }

      // TODO(later) uncomment and test (this works, just not tested or used
      // for anything, so because time is short commenting out for now)
      // void operator+=(scalar_type a) {
      //   for (auto it = _container.begin(); it < _container.end(); ++it) {
      //     *it = plus(*it, a);
      //   }
      // }

      // TODO(later) implement operator*=(Subclass const&)

      ////////////////////////////////////////////////////////////////////////
      // Arithmetic operators - not in-place
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because operator+= isn't
      Subclass operator+(Subclass const& y) const {
        Subclass result(*static_cast<Subclass const*>(this));
        result += y;
        return result;
      }

      // not noexcept because product_inplace_no_checks isn't
      Subclass operator*(Subclass const& y) const {
        Subclass result(*static_cast<Subclass const*>(this));
        result.product_inplace_no_checks(*static_cast<Subclass const*>(this),
                                         y);
        return result;
      }

      // TODO(later) implement operator*(Scalar)
      // TODO(later) implement operator+(Scalar)

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

      template <typename U>
      std::pair<scalar_type, scalar_type> coords(U const& it) const {
        static_assert(
            std::is_same<U, iterator>::value
                || std::is_same<U, const_iterator>::value,
            "the parameter it must be of type iterator or const_iterator");
        scalar_type const v = std::distance(_container.begin(), it);
        return std::make_pair(v / number_of_cols(), v % number_of_cols());
      }

      ////////////////////////////////////////////////////////////////////////
      // Modifiers
      ////////////////////////////////////////////////////////////////////////

      // noexcept because vector::swap and array::swap are noexcept
      void swap(MatrixCommon& that) noexcept {
        std::swap(_container, that._container);
      }

      // noexcept because swap is noexcept, and so too are number_of_rows and
      // number_of_cols
      void transpose() noexcept {
        LIBSEMIGROUPS_ASSERT(number_of_rows() == number_of_cols());
        if (number_of_rows() == 0) {
          return;
        }
        auto& x = *this;
        for (size_t r = 0; r < number_of_rows() - 1; ++r) {
          for (size_t c = r + 1; c < number_of_cols(); ++c) {
            std::swap(x(r, c), x(c, r));
          }
        }
      }

      ////////////////////////////////////////////////////////////////////////
      // Rows
      ////////////////////////////////////////////////////////////////////////

      // not noexcept because there's an allocation
      RowView row(size_t i) const {
        if (i >= number_of_rows()) {
          LIBSEMIGROUPS_EXCEPTION(
              "index out of range, expected value in [{}, {}), found {}",
              0,
              number_of_rows(),
              i);
        }
        auto& container = const_cast<Container&>(_container);
        return RowView(static_cast<Subclass const*>(this),
                       container.begin() + i * number_of_cols(),
                       number_of_cols());
      }

      // not noexcept because there's an allocation
      template <typename T>
      void rows(T& x) const {
        auto& container = const_cast<Container&>(_container);
        for (auto itc = container.begin(); itc != container.end();
             itc += number_of_cols()) {
          x.emplace_back(
              static_cast<Subclass const*>(this), itc, number_of_cols());
        }
        LIBSEMIGROUPS_ASSERT(x.size() == number_of_rows());
      }

      ////////////////////////////////////////////////////////////////////////
      // Friend functions
      ////////////////////////////////////////////////////////////////////////

      friend std::ostream& operator<<(std::ostream& os, MatrixCommon const& x) {
        os << detail::to_string(x);
        return os;
      }

      ////////////////////////////////////////////////////////////////////////
      // Validation
      ////////////////////////////////////////////////////////////////////////

     private:
      ////////////////////////////////////////////////////////////////////////
      // Private data
      ////////////////////////////////////////////////////////////////////////
      container_type _container;
    };

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
      MatrixStaticArithmetic()                              = default;
      MatrixStaticArithmetic(MatrixStaticArithmetic const&) = default;
      MatrixStaticArithmetic(MatrixStaticArithmetic&&)      = default;
      MatrixStaticArithmetic& operator=(MatrixStaticArithmetic const&)
          = default;
      MatrixStaticArithmetic& operator=(MatrixStaticArithmetic&&) = default;

      // TODO(later) from here to the end of MatrixStaticArithmetic should be
      // private or protected
      using scalar_type = Scalar;

      static constexpr scalar_type plus_impl(scalar_type x,
                                             scalar_type y) noexcept {
        return PlusOp()(x, y);
      }

      static constexpr scalar_type prod_impl(scalar_type x,
                                             scalar_type y) noexcept {
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
      scalar_type plus(scalar_type x, scalar_type y) const noexcept {
        return static_cast<Subclass const*>(this)->plus_impl(y, x);
      }

      scalar_type prod(scalar_type x, scalar_type y) const noexcept {
        return static_cast<Subclass const*>(this)->prod_impl(y, x);
      }

     public:
      RowViewCommon()                                = default;
      RowViewCommon(RowViewCommon const&)            = default;
      RowViewCommon(RowViewCommon&&)                 = default;
      RowViewCommon& operator=(RowViewCommon const&) = default;
      RowViewCommon& operator=(RowViewCommon&&)      = default;

      explicit RowViewCommon(Row const& r)
          : RowViewCommon(const_cast<Row&>(r).begin()) {}

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
      void operator+=(RowViewCommon const& x) {
        auto& this_ = *this;
        for (size_t i = 0; i < size(); ++i) {
          this_[i] = plus(this_[i], x[i]);
        }
      }

      // not noexcept because iterator arithmeic isn't
      void operator+=(scalar_type a) {
        for (auto& x : *this) {
          x = plus(x, a);
        }
      }

      // not noexcept because iterator arithmeic isn't
      void operator*=(scalar_type a) {
        for (auto& x : *this) {
          x = prod(x, a);
        }
      }

      // not noexcept because operator*= isn't
      Row operator*(scalar_type a) const {
        Row result(*static_cast<Subclass const*>(this));
        result *= a;
        return result;
      }

      // not noexcept because operator+= isn't
      Row operator+(RowViewCommon const& that) const {
        Row result(*static_cast<Subclass const*>(this));
        result += static_cast<Subclass const&>(that);
        return result;
      }

      template <typename U>
      bool operator==(U const& that) const {
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

      friend std::ostream& operator<<(std::ostream&        os,
                                      RowViewCommon const& x) {
        os << detail::to_string(x);
        return os;
      }

     protected:
      explicit RowViewCommon(iterator first) : _begin(first) {}

     private:
      iterator _begin;
    };

    template <typename Container>
    void throw_if_any_row_wrong_size(Container const& m) {
      if (m.size() <= 1) {
        return;
      }
      uint64_t const C  = m.begin()->size();
      auto           it = std::find_if_not(
          m.begin() + 1, m.end(), [&C](typename Container::const_reference r) {
            return r.size() == C;
          });
      if (it != m.end()) {
        LIBSEMIGROUPS_EXCEPTION("invalid argument, expected every item to "
                                "have length {}, found {} in entry {}",
                                C,
                                it->size(),
                                std::distance(m.begin(), it));
      }
    }

    template <typename Scalar>
    void throw_if_any_row_wrong_size(
        std::initializer_list<std::initializer_list<Scalar>> m) {
      throw_if_any_row_wrong_size<
          std::initializer_list<std::initializer_list<Scalar>>>(m);
    }

  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Matrix forward declarations
  ////////////////////////////////////////////////////////////////////////
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  class StaticMatrix;

  template <typename... Args>
  class DynamicMatrix;

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;

  template <typename Semiring, typename Scalar>
  class DynamicMatrix<Semiring, Scalar>;

  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////
  // RowViews - classes for cheaply storing iterators to rows
  ////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////

  ////////////////////////////////////////////////////////////////////////
  // StaticRowViews - static arithmetic
  ////////////////////////////////////////////////////////////////////////

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t C,
            typename Scalar>
  class StaticRowView
      : public detail::RowViewCommon<
            StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>,
            StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
   private:
    using RowViewCommon = detail::RowViewCommon<
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>,
        StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>;
    friend RowViewCommon;

    template <size_t R>
    using StaticMatrix_ = ::libsemigroups::
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>;

   public:
    using scalar_type = Scalar;

    using detail::RowViewCommon<
        StaticMatrix_<1>,
        StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>::RowViewCommon;

    StaticRowView() = default;  // gcc-6 seems to require this

    template <size_t R>
    StaticRowView(StaticMatrix_<R> const*,
                  typename RowViewCommon::iterator it,
                  size_t)
        : RowViewCommon(it) {}

    using RowViewCommon::size;

   private:
    static constexpr size_t length_impl() noexcept {
      return C;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // DynamicRowViews - static arithmetic
  ////////////////////////////////////////////////////////////////////////

  template <typename... Args>
  class DynamicRowView;

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>
      : public detail::RowViewCommon<
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
   private:
    using DynamicMatrix_ = DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;
    using RowViewCommon  = detail::RowViewCommon<
         DynamicMatrix_,
         DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>;
    friend RowViewCommon;

   public:
    using iterator = typename RowViewCommon::iterator;
    using Row      = typename DynamicMatrix_::Row;

    using detail::RowViewCommon<
        DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
        DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>::RowViewCommon;

    DynamicRowView() = default;
    DynamicRowView(DynamicMatrix_ const*, iterator const& it, size_t N)
        : RowViewCommon(it), _length(N) {}

    explicit DynamicRowView(Row const& r)
        : RowViewCommon(r), _length(r.number_of_cols()) {}

   private:
    size_t length_impl() const noexcept {
      return _length;
    }
    size_t _length;
  };

  ////////////////////////////////////////////////////////////////////////
  // DynamicRowViews - dynamic arithmetic
  ////////////////////////////////////////////////////////////////////////

  template <typename Semiring, typename Scalar>
  class DynamicRowView<Semiring, Scalar>
      : public detail::RowViewCommon<DynamicMatrix<Semiring, Scalar>,
                                     DynamicRowView<Semiring, Scalar>> {
   private:
    using DynamicMatrix_ = DynamicMatrix<Semiring, Scalar>;
    friend DynamicMatrix_;
    using RowViewCommon
        = detail::RowViewCommon<DynamicMatrix_,
                                DynamicRowView<Semiring, Scalar>>;
    friend RowViewCommon;

   public:
    using scalar_type = typename DynamicMatrix_::scalar_type;
    using iterator    = typename RowViewCommon::iterator;
    using Row         = typename DynamicMatrix_::Row;

    using detail::RowViewCommon<
        DynamicMatrix<Semiring, Scalar>,
        DynamicRowView<Semiring, Scalar>>::RowViewCommon;

    DynamicRowView() = default;

    DynamicRowView(DynamicMatrix_ const* mat, iterator const& it, size_t)
        : RowViewCommon(it), _matrix(mat) {}

    explicit DynamicRowView(Row const& r) : RowViewCommon(r), _matrix(&r) {}

   private:
    size_t length_impl() const noexcept {
      return _matrix->number_of_cols();
    }

    scalar_type plus_impl(scalar_type x, scalar_type y) const noexcept {
      return _matrix->plus_impl(x, y);
    }

    scalar_type prod_impl(scalar_type x, scalar_type y) const noexcept {
      return _matrix->prod_impl(x, y);
    }

    DynamicMatrix_ const* _matrix;
  };

  ////////////////////////////////////////////////////////////////////////
  // StaticMatrix with compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! Defined in ``matrix.hpp``.
  //!
  //! This is a class for matrices where both the arithmetic operations in
  //! the underlying semiring, and the dimensions of the matrices are known
  //! at compile time.
  //!
  //! \tparam PlusOp a stateless type with a call operator of signature
  //!    `scalar_type operator()(scalar_type, scalar_type)`
  //!    implementing the addition of the semiring.
  //!
  //! \tparam ProdOp a stateless type with a call operator of signature
  //!    `scalar_type operator()(scalar_type, scalar_type)`
  //!    implementing the multiplication of the semiring.
  //!
  //! \tparam ZeroOp a stateless type with a call operator of signature
  //!    `scalar_type operator()()` returning the zero of the semiring
  //!    (the additive identity element).
  //!
  //! \tparam OneOp a stateless type with a call operator of signature
  //!    `scalar_type operator()()` returning the one of the semiring
  //!    (the multiplicative identity element).
  //!
  //! \tparam R  the number of rows of the matrices.
  //! \tparam C  the number of columns of the matrices.
  //!
  //! \tparam Scalar the type of the entries in the matrices (the type of
  //! elements in the underlying semiring)
  //!
  //! \note Certain member functions only work for square matrices and some only
  //!    work for rows.
  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            size_t R,
            size_t C,
            typename Scalar>
  class StaticMatrix
      : public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
        public detail::MatrixCommon<
            std::array<Scalar, R * C>,
            StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>,
            StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>> {
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Aliases - private
    ////////////////////////////////////////////////////////////////////////

    using MatrixCommon = ::libsemigroups::detail::MatrixCommon<
        std::array<Scalar, R * C>,
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>,
        StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>>;
    friend MatrixCommon;

   public:
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Aliases - public
    ////////////////////////////////////////////////////////////////////////

    //! Alias for the template parameter \p Scalar.
    using scalar_type = typename MatrixCommon::scalar_type;

    //! Alias for references to the template parameter \p Scalar.
    using scalar_reference = typename MatrixCommon::scalar_reference;

    //! Alias for const references to the template parameter \p Scalar.
    using scalar_const_reference =
        typename MatrixCommon::scalar_const_reference;

    //! Alias for the type of the rows of a StaticMatrix.
    using Row = StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, 1, C, Scalar>;

    //! Alias for the type of row views of a StaticMatrix.
    using RowView = StaticRowView<PlusOp, ProdOp, ZeroOp, OneOp, C, Scalar>;

    //! Alias for the template parameter \p PlusOp.
    using Plus = PlusOp;

    //! Alias for the template parameter \p ProdOp.
    using Prod = ProdOp;

    //! Alias for the template parameter \p ZeroOp.
    using Zero = ZeroOp;

    //! Alias for the template parameter \p OneOp.
    using One = OneOp;

    static constexpr size_t nr_rows = R;
    static constexpr size_t nr_cols = C;

    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - Constructors + destructor - public
    ////////////////////////////////////////////////////////////////////////

    //! \brief Construct a row (from std::initializer_list).
    //!
    //! Construct a row from an std::initializer_list.
    //!
    //! \param c  the values to be copied into the row.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(n)\f$ where \f$n\f$ is the size of the row being
    //!   constructed.
    //!
    //! \warning
    //!   This constructor only works for rows, i.e. when the template
    //!   parameter `R` is `1`.
    explicit StaticMatrix(std::initializer_list<scalar_type> const& c)
        : MatrixCommon(c) {
      static_assert(R == 1,
                    "cannot construct Matrix from the given initializer list, "
                    "incompatible dimensions");
      LIBSEMIGROUPS_ASSERT(c.size() == C);
    }

    //! \brief Construct a matrix.
    //!
    //! Construct a matrix from an std::initializer_list of
    //! std::initializer_list.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(mn)\f$ where \f$m\f$ is the template parameter `R`
    //!   and \f$n\f$ is the template parameter `C`.
    //!
    //! \par Example
    //! \code
    //!  Mat m({{1, 1}, {0, 0}});
    //! \endcode
    explicit StaticMatrix(
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixCommon(m) {}

    //! \brief Construct a matrix.
    //!
    //! Construct a matrix from an std::vector of std::vector.
    //!
    //! \param m  the values to be copied into the matrix.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //!   \f$O(mn)\f$ where \f$m\f$ is the template parameter `R`
    //!   and \f$n\f$ is the template parameter `C`.
    explicit StaticMatrix(std::vector<std::vector<scalar_type>> const& m)
        : MatrixCommon(m) {}

    //! \brief Construct a row from a row view.
    //!
    //! Construct a row from a row view.
    //!
    //! \param rv  the row view.
    //!
    //! \exceptions
    //!   this function guarantees not to throw a
    //!   LibsemigroupsException.
    //!
    //! \complexity
    //!   \f$O(n)\f$ where \f$n\f$ is the size of the row being
    //!   constructed.
    //!
    //! \warning
    //!   This constructor will only compile for rows, i.e. when the template
    //!   parameter `R` is `1`.
    explicit StaticMatrix(RowView const& rv) : MatrixCommon(rv) {
      static_assert(
          R == 1,
          "cannot construct Matrix with more than one row from RowView!");
    }

    //! \brief Default constructor.
    //!
    //! Default constructor.
    StaticMatrix() = default;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    StaticMatrix(StaticMatrix const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    StaticMatrix(StaticMatrix&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    StaticMatrix& operator=(StaticMatrix const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    StaticMatrix& operator=(StaticMatrix&&) = default;

#ifndef PARSED_BY_DOXYGEN
    // For uniformity of interface, the args do nothing
    StaticMatrix(size_t r, size_t c) : StaticMatrix() {
      (void) r;
      (void) c;
      LIBSEMIGROUPS_ASSERT(r == number_of_rows());
      LIBSEMIGROUPS_ASSERT(c == number_of_cols());
    }

    // For uniformity of interface, the first arg does nothing
    StaticMatrix(void const* ptr, std::initializer_list<scalar_type> const& c)
        : StaticMatrix(c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    StaticMatrix(
        void const*                                                      ptr,
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : StaticMatrix(m) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    explicit StaticMatrix(void const* ptr, RowView const& rv)
        : StaticMatrix(rv) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, no arg used for anything
    StaticMatrix(void const* ptr, size_t r, size_t c) : StaticMatrix(r, c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }
#endif

    ~StaticMatrix() = default;

    //! TODO identity -> one
    static StaticMatrix identity(size_t n = 0) {
      static_assert(C == R, "cannot create non-square identity matrix");
      // If specified the value of n must equal R or otherwise weirdness will
      // ensue...
      LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
      (void) n;
#if defined(__APPLE__) && defined(__clang__) \
    && (__clang_major__ == 13 || __clang_major__ == 14)
      // With Apple clang version 13.1.6 (clang-1316.0.21.2.5) something goes
      // wrong and the value R is optimized away somehow, meaning that the
      // values on the diagonal aren't actually set. This only occurs when
      // libsemigroups is compiled with -O2 or higher.
      size_t volatile m = R;
#else
      size_t m = R;
#endif
      StaticMatrix x(m, m);
      std::fill(x.begin(), x.end(), ZeroOp()());
      for (size_t r = 0; r < m; ++r) {
        x(r, r) = OneOp()();
      }
      return x;
    }

    //! TODO identity -> one
    static StaticMatrix identity(void const* ptr, size_t n = 0) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
      LIBSEMIGROUPS_ASSERT(n == 0 || n == R);
      return identity(n);
    }

    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - member function aliases - public
    ////////////////////////////////////////////////////////////////////////
#ifdef PARSED_BY_DOXYGEN
    //! \brief Returns a reference to the specified entry of the matrix.
    //!
    //! Returns a reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry\param c  the index of the
    //! column of the entry
    //!
    //! \returns  A value of type scalar_reference.
    //!
    //! \exceptions
    //!   this function guarantees not to throw a LibsemigroupsException.
    //!
    //! \complexity
    //!   Constant
    //!
    //! \warning
    //!    No checks on the validity of the parameters `r` and `c` are
    //!    performed.
    scalar_reference operator()(size_t r, size_t c);

    //! \brief Returns a const reference to the specified entry of the matrix.
    //!
    //! Returns a const reference to the specified entry of the matrix.
    //!
    //! \param r  the index of the row of the entry
    //! \param c  the index of the column of the entry
    //!
    //! \returns  A value of type scalar_const_reference.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    //!
    //! \warning
    //! No checks on the validity of the parameters `r` and `c` are
    //! performed.
    scalar_const_reference operator()(size_t r, size_t c) const;

    //! \brief Returns an iterator pointing at the first entry.
    //!
    //! This function returns a random access iterator point at the first entry
    //! of the matrix.
    //!
    //! \returns A value of type `iterator`.
    //!
    //! \complexity
    //! Constant
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    iterator begin() noexcept;

    //! \brief Returns an iterator pointing one beyond the last entry in the
    //! matrix.
    //!
    //! \returns A value of type \ref iterator.
    //!
    //! \complexity  Constant
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    iterator end();

    //! \brief Returns a const iterator pointing at the first entry.
    //!
    //! This function returns a const (random access) iterator pointing at the
    //! first entry in the matrix.
    //!
    //! \returns  A value of type `const_iterator`.
    //!
    //! \complexity
    //! Constant
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //!   The order in which entries in the matrix are iterated over is not
    //!   specified.
    const_iterator cbegin() const noexcept;

    //! \brief Returns a const iterator pointing one beyond the last entry in
    //! the matrix.
    //!
    //! This function returns a const (random access) iterator pointing one
    //! passed the last entry of the matrix.
    //!
    //! \returns A value of type \ref const_iterator.
    //!
    //! \complexity  Constant
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \warning
    //! The order in which entries in the matrix are iterated over is not
    //! specified.
    const_iterator cend();

    //! \brief Equality operator.
    //!
    //! This function returns \c true if `*this` is equal to \p that;
    //! and \c false otherwise.
    //!
    //! \param that  matrix for comparison.
    //!
    //! \returns `true` if `*this` and `that` are equal and `false` if they are
    //! not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is the number of rows and
    //! \f$n\f$ is the number of columns of the matrix.
    bool operator==(StaticMatrix const& that) const;

    //! \brief Equality operator.
    //!
    //! This function returns \c true if `*this` is not equal to \p that;
    //! and \c false otherwise.
    //!
    //! \param that  the row view for comparison.
    //!
    //! \returns `true` if `*this` and `that` are equal and `false` if they
    //! are not.  In particular, if `*this` has more than one row, then
    //! `false` is returned.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is the number of columns of
    //! the matrix.
    bool operator==(RowView const& that) const;

    //! \brief Inequality operator.
    //!
    //! This function returns \c true if `*this` is not equal to \p that; and
    //! \c false otherwise.
    //!
    //! \param that  the matrix or row view for comparison.
    //!
    //! \returns The negation of `operator==(that)`.
    //!
    //! \complexity
    //! see operator==
    bool operator!=(StaticMatrix const& that) const;

    //! \brief Inequality operator.
    //!
    //! This function returns \c true if `*this` is not equal to \p that; and
    //! \c false otherwise.
    //!
    //! \param that  the matrix or row view for comparison.
    //!
    //! \returns The negation of `operator==(that)`.
    //!
    //! \complexity
    //! see operator==
    bool operator!=(RowView const& that) const;

    //! \brief Less than operator.
    //!
    //! This operator defines a total order on the set of matrices of the
    //! same type, the details of which is implementation specific.
    //!
    //! \param that  the matrix for comparison.
    //!
    //! \returns `true` if `*this` is less than `that` and `false` if it is
    //! not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is number_of_rows
    //! and \f$n\f$ is number_of_cols
    bool operator<(StaticMatrix const& that) const;

    //! \brief Less than operator.
    //!
    //! \param that  the row view for comparison.
    //!
    //! \returns `true` if `*this` is less than `that`, and `false`
    //! otherwise. In particular, if `*this` has more than one row, then
    //! `false` is returned.
    //!
    //! \complexity
    //! At worst \f$O(n)\f$ where \f$n\f$ is \ref number_of_cols
    bool operator<(RowView const& that) const;

    //! \brief Greater than operator.
    //!
    //! This operator defines a total order on the set of matrices of the same
    //! type, the details of which is implementation specific.
    //!
    //! \param that  the matrix for comparison.
    //! \returns `true` if `*this` is less than `that` and `false` if it is
    //! not.
    //!
    //! \complexity
    //! At worst \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ is \ref number_of_cols
    bool operator>(StaticMatrix const& that) const;

    //! \brief Get the coordinates of an iterator.
    //!
    //! This function returns a pair containing the row and columns
    //! corresponding to an const_iterator pointing into a matrix.
    //!
    //! \param it the iterator
    //! \returns A value of type `std::pair<scalar_type, scalar_type>`.
    //!
    //! \complexity  Constant
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    std::pair<scalar_type, scalar_type> coords(const_iterator it) const;

    //! \brief Returns the number of rows of the matrix.
    //!
    //! This function returns the number of rows of the matrix.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \complexity
    //! Constant
    //!
    //! \exceptions
    //! \noexcept
    size_t number_of_rows();

    //! \brief Returns the number of columns of the matrix.
    //!
    //! This function returns the number of columns of the matrix.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \complexity
    //! Constant
    //!
    //! \exceptions
    //! \noexcept
    size_t number_of_cols();

    //! \brief Returns the sum of two matrices.
    //!
    //! This function returns the sum of two StaticMatrix objects.
    //!
    //! \param that  the matrix to add to `this`.
    //!
    //! \returns A value of type `StaticMatrix`.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ \ref number_of_cols
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! verified.
    StaticMatrix operator+(StaticMatrix const& that);

    //! \brief Add a matrix to another matrix in-place.
    //!
    //! \param that  the matrix to add to `this`.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is \ref
    //! number_of_cols
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! verified by the implementation.
    void operator+=(StaticMatrix const& that);

    //! \brief Add a row to another row in-place.
    //!
    //! This function adds the row represented by a RowView to another row
    //! in-place.
    //!
    //! \param that  the matrix to add.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$
    //! is \ref number_of_cols
    //!
    //! \warning
    //! This function only works if `this` has a single row, i.e. the
    //! template parameter `R` is `1`, but no checks are performed to ensure
    //! this is the case.
    void operator+=(RowView const& that);

    //! \brief Adds a scalar to every entry of the matrix in-place.
    //!
    //! This function adds a scalar to every entry of the matrix in-place.
    //!
    //! \param a  the Scalar to add to `this`.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows and \f$m\f$ is \ref
    //! number_of_cols
    void operator+=(Scalar a);

    //! \brief Returns the product of two matrices.
    //!
    //! This function returns the product of two matrices.
    //!
    //! \param that the matrix to multiply by `this`.
    //!
    //! \returns A value of type `StaticMatrix`.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ \ref number_of_rows and \f$m\f$ \ref
    //! number_of_cols.
    //!
    //! \warning
    //! The matrices must be of the same dimensions, although this is not
    //! verified.
    StaticMatrix operator*(StaticMatrix const& that);

    //! \brief Multiplies every entry of a matrix by a scalar in-place.
    //!
    //! This function multiplies every entry of a matrix by a scalar in-place.
    //!
    //! \param a the scalar to multiply every entry by.
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is \ref number_of_rows
    //! and \f$m\f$ is \ref number_of_cols.
    void operator*=(scalar_type a);

    //! \brief Multiplies \p x and \p y and stores the result in `this`.
    //!
    //! This function redefines `this` to be the product of \p x and \p y. This
    //! is in-place multiplication to avoid allocation of memory for products
    //! which do not need to be stored for future use.
    //!
    //! \param A  the first matrix to multiply
    //! \param B  the second matrix to multiply
    //!
    //! \complexity
    //! \f$O(n ^ 3)\f$ where \f$n\f$ is \ref number_of_rows or \ref
    //! number_of_cols.
    //!
    //! \warning
    //! This function only applies to matrices with the same number of rows
    //! and columns but this isn't verified.
    void product_inplace_no_checks(StaticMatrix const& x,
                                   StaticMatrix const& y);

    //! \brief Returns a view into a row.
    //!
    //! This function returns a \ref RowView into the row with index \p i.
    //!
    //! \param i  the index of the row
    //!
    //! \returns  A value of type RowView.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! Constant
    // TODO should be row_no_checks
    RowView row(size_t i) const;

    //! \brief Add row views for every row in the matrix to a container.
    //!
    //! This function adds a \ref RowView for each row in the matrix to the
    //! container \p x.
    //!
    //! \tparam T the type of the container.
    //! \param x  a container.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(m)\f$ where \f$m\f$ is the template parameter `R`.
    template <typename T>
    void rows(T& x) const;

    //! \brief Swaps the contents of `*this` with the contents of \p that.
    //!
    //! This function swaps the contents of `*this` with the contents of \p
    //! that.
    //!
    //! \param that  the matrix to swap contents with.
    //!
    //! \complexity
    //! Constant
    //!
    //! \exceptions
    //! \noexcept
    void swap(StaticMatrix& that) noexcept;

    //! \brief Transpose a matrix in-place.
    //!
    //! This function transpose a matrix object in-place.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! \f$O(mn)\f$ where \f$m\f$ is the template parameter `R` and
    //! \f$n\f$ is the template parameter `C`.
    //!
    //! \warning
    //! This only works when the template parameters `R` and `C` are equal
    //! (i.e. for square matrices), but this is not verified.
    void transpose() noexcept;  // TODO -> transpose_no_checks
#endif                          // PARSED_BY_DOXYGEN

   private:
    ////////////////////////////////////////////////////////////////////////
    // StaticMatrix - implementation of MatrixCommon requirements - private
    ////////////////////////////////////////////////////////////////////////

    static constexpr size_t number_of_rows_impl() noexcept {
      return R;
    }
    static constexpr size_t number_of_cols_impl() noexcept {
      return C;
    }
  };  // namespace libsemigroups

  ////////////////////////////////////////////////////////////////////////
  // DynamicMatrix with compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  class DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>
      : public detail::MatrixDynamicDim<Scalar>,
        public detail::MatrixCommon<
            std::vector<Scalar>,
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>,
        public detail::
            MatrixStaticArithmetic<PlusOp, ProdOp, ZeroOp, OneOp, Scalar> {
    using MatrixDynamicDim = ::libsemigroups::detail::MatrixDynamicDim<Scalar>;
    using MatrixCommon     = ::libsemigroups::detail::MatrixCommon<
            std::vector<Scalar>,
            DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>,
            DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>>;
    friend MatrixCommon;

   public:
    using scalar_type      = typename MatrixCommon::scalar_type;
    using scalar_reference = typename MatrixCommon::scalar_reference;
    using scalar_const_reference =
        typename MatrixCommon::scalar_const_reference;

    using Row     = DynamicMatrix;
    using RowView = DynamicRowView<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>;

    DynamicMatrix()                                = default;
    DynamicMatrix(DynamicMatrix const&)            = default;
    DynamicMatrix(DynamicMatrix&&)                 = default;
    DynamicMatrix& operator=(DynamicMatrix const&) = default;
    DynamicMatrix& operator=(DynamicMatrix&&)      = default;

    DynamicMatrix(size_t r, size_t c) : MatrixDynamicDim(r, c), MatrixCommon() {
      resize(number_of_rows(), number_of_cols());
    }

    explicit DynamicMatrix(std::initializer_list<scalar_type> const& c)
        : MatrixDynamicDim(1, c.size()), MatrixCommon(c) {}

    explicit DynamicMatrix(
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), m.begin()->size()), MatrixCommon(m) {}

    explicit DynamicMatrix(std::vector<std::vector<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), m.begin()->size()), MatrixCommon(m) {}

    explicit DynamicMatrix(RowView const& rv)
        : MatrixDynamicDim(1, rv.size()), MatrixCommon(rv) {}

    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(void const* ptr, size_t r, size_t c) : DynamicMatrix(r, c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(void const* ptr, std::initializer_list<scalar_type> const& c)
        : DynamicMatrix(c) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    // For uniformity of interface, the first arg does nothing
    DynamicMatrix(
        void const*                                                      ptr,
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : DynamicMatrix(m) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
    }

    ~DynamicMatrix();

    static DynamicMatrix identity(void const* ptr, size_t n) {
      (void) ptr;
      LIBSEMIGROUPS_ASSERT(ptr == nullptr);
      return identity(n);
    }

    static DynamicMatrix identity(size_t n) {
      DynamicMatrix x(n, n);
      std::fill(x.begin(), x.end(), ZeroOp()());
      for (size_t r = 0; r < n; ++r) {
        x(r, r) = OneOp()();
      }
      return x;
    }

    using MatrixCommon::begin;
    using MatrixCommon::identity;
    using MatrixCommon::number_of_cols;
    using MatrixCommon::number_of_rows;
    using MatrixCommon::resize;

    void swap(DynamicMatrix& that) noexcept {
      static_cast<MatrixDynamicDim&>(*this).swap(
          static_cast<MatrixDynamicDim&>(that));
      static_cast<MatrixCommon&>(*this).swap(static_cast<MatrixCommon&>(that));
    }
  };

  template <typename PlusOp,
            typename ProdOp,
            typename ZeroOp,
            typename OneOp,
            typename Scalar>
  DynamicMatrix<PlusOp, ProdOp, ZeroOp, OneOp, Scalar>::~DynamicMatrix()
      = default;

  ////////////////////////////////////////////////////////////////////////
  // DynamicMatrix with runtime semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  template <typename Semiring, typename Scalar>
  class DynamicMatrix<Semiring, Scalar>
      : public detail::MatrixDynamicDim<Scalar>,
        public detail::MatrixCommon<std::vector<Scalar>,
                                    DynamicMatrix<Semiring, Scalar>,
                                    DynamicRowView<Semiring, Scalar>,
                                    Semiring> {
    using MatrixCommon = detail::MatrixCommon<std::vector<Scalar>,
                                              DynamicMatrix<Semiring, Scalar>,
                                              DynamicRowView<Semiring, Scalar>,
                                              Semiring>;
    friend MatrixCommon;
    using MatrixDynamicDim = ::libsemigroups::detail::MatrixDynamicDim<Scalar>;

   public:
    using scalar_type      = typename MatrixCommon::scalar_type;
    using scalar_reference = typename MatrixCommon::scalar_reference;
    using scalar_const_reference =
        typename MatrixCommon::scalar_const_reference;

    using Row     = DynamicMatrix;
    using RowView = DynamicRowView<Semiring, Scalar>;
    friend RowView;

    DynamicMatrix()                                = delete;
    DynamicMatrix(DynamicMatrix const&)            = default;
    DynamicMatrix(DynamicMatrix&&)                 = default;
    DynamicMatrix& operator=(DynamicMatrix const&) = default;
    DynamicMatrix& operator=(DynamicMatrix&&)      = default;

    DynamicMatrix(Semiring const* semiring, size_t r, size_t c)
        : MatrixDynamicDim(r, c), MatrixCommon(), _semiring(semiring) {
      resize(number_of_rows(), number_of_cols());
    }

    explicit DynamicMatrix(Semiring const*                           semiring,
                           std::initializer_list<scalar_type> const& c)
        : MatrixDynamicDim(1, c.size()), MatrixCommon(c), _semiring(semiring) {}

    explicit DynamicMatrix(
        Semiring const* semiring,
        std::initializer_list<std::initializer_list<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), m.begin()->size()),
          MatrixCommon(m),
          _semiring(semiring) {}

    explicit DynamicMatrix(Semiring const* semiring,
                           std::vector<std::vector<scalar_type>> const& m)
        : MatrixDynamicDim(m.size(), (m.empty() ? 0 : m.begin()->size())),
          MatrixCommon(m),
          _semiring(semiring) {}

    explicit DynamicMatrix(RowView const& rv)
        : MatrixDynamicDim(1, rv.size()),
          MatrixCommon(rv),
          _semiring(rv._matrix->semiring()) {}

    // No static DynamicMatrix::identity(size_t n) because we need a semiring!
    static DynamicMatrix identity(Semiring const* sr, size_t n) {
      DynamicMatrix x(sr, n, n);
      std::fill(x.begin(), x.end(), x.scalar_zero());
      for (size_t r = 0; r < n; ++r) {
        x(r, r) = x.scalar_one();
      }
      return x;
    }

    ~DynamicMatrix();

    using MatrixCommon::begin;
    using MatrixCommon::identity;
    using MatrixCommon::number_of_cols;
    using MatrixCommon::number_of_rows;
    using MatrixCommon::resize;

    void swap(DynamicMatrix& that) noexcept {
      static_cast<MatrixDynamicDim&>(*this).swap(
          static_cast<MatrixDynamicDim&>(that));
      static_cast<MatrixCommon&>(*this).swap(static_cast<MatrixCommon&>(that));
      std::swap(_semiring, that._semiring);
    }

   private:
    scalar_type plus_impl(scalar_type x, scalar_type y) const noexcept {
      return _semiring->plus(x, y);
    }

    scalar_type prod_impl(scalar_type x, scalar_type y) const noexcept {
      return _semiring->prod(x, y);
    }

    scalar_type one_impl() const noexcept {
      return _semiring->scalar_one();
    }

    scalar_type zero_impl() const noexcept {
      return _semiring->scalar_zero();
    }

    Semiring const* semiring_impl() const noexcept {
      return _semiring;
    }

    Semiring const* _semiring;
  };

  template <typename Semiring, typename Scalar>
  DynamicMatrix<Semiring, Scalar>::~DynamicMatrix() = default;

  ////////////////////////////////////////////////////////////////////////
  // Helper structs to check if matrix is static, or has a pointer to a
  // semiring
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename T>
    struct IsStaticMatrixHelper : std::false_type {};

    template <typename PlusOp,
              typename ProdOp,
              typename ZeroOp,
              typename OneOp,
              size_t R,
              size_t C,
              typename Scalar>
    struct IsStaticMatrixHelper<
        StaticMatrix<PlusOp, ProdOp, ZeroOp, OneOp, R, C, Scalar>>
        : std::true_type {};

    template <typename T>
    struct IsMatWithSemiringHelper : std::false_type {};

    template <typename Semiring, typename Scalar>
    struct IsMatWithSemiringHelper<DynamicMatrix<Semiring, Scalar>>
        : std::true_type {};

    template <typename S, typename T = void>
    struct IsTruncMatHelper : std::false_type {};
  }  // namespace detail

  template <typename T>
  constexpr bool IsStaticMatrix = detail::IsStaticMatrixHelper<T>::value;

  template <typename T>
  constexpr bool IsDynamicMatrix = IsMatrix<T> && !IsStaticMatrix<T>;

  template <typename T>
  static constexpr bool IsMatWithSemiring
      = detail::IsMatWithSemiringHelper<T>::value;

  namespace detail {

    template <typename T>
    static constexpr bool IsTruncMat = IsTruncMatHelper<T>::value;

    template <typename Mat>
    void semiring_validate(Mat const& m) {
      if (IsMatWithSemiring<Mat> && m.semiring() == nullptr) {
        LIBSEMIGROUPS_EXCEPTION(
            "the matrix's pointer to a semiring is nullptr!")
      }
    }
  }  // namespace detail

  // TODO(0) move to matrix namespace
  template <typename Mat>
  auto matrix_threshold(Mat const&) noexcept
      -> std::enable_if_t<!detail::IsTruncMat<Mat>, typename Mat::scalar_type> {
    return UNDEFINED;
  }

  template <typename Mat>
  auto matrix_threshold(Mat const&) noexcept
      -> std::enable_if_t<detail::IsTruncMat<Mat> && !IsMatWithSemiring<Mat>,
                          typename Mat::scalar_type> {
    return detail::IsTruncMatHelper<Mat>::threshold;
  }

  template <typename Mat>
  auto matrix_threshold(Mat const& x) noexcept
      -> std::enable_if_t<detail::IsTruncMat<Mat> && IsMatWithSemiring<Mat>,
                          typename Mat::scalar_type> {
    return x.semiring()->threshold();
  }

  ////////////////////////////////////////////////////////////////////////
  // Boolean matrices - compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup bmat_group BMat
  //!
  //! This page describes the functionality for \f$m \times n\f$ boolean
  //! matrices for arbitrary dimensions \f$m\f$ and \f$n\f$. There are two
  //! types of such boolean matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template `BMat<R, C>`: if `R` or `C` has value
  //! `0`, then the dimensions can be set at run time, otherwise the
  //! dimensions are `R` and `C`. The default value of `R` is `0` and of
  //! `C` is `R`.
  //!
  //! The alias `BMat<R, C>` is either StaticMatrix or DynamicMatrix, please
  //! refer to the documentation of these class templates for more details. The
  //! only substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and the latter
  //! should be constructed using the dimensions.
  //!
  //! \par Example
  //!
  //! \code
  //!    BMat<3> m;       // default construct an uninitialized 3 x 3 static
  //!    matrix BMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic
  //!    matrix
  //! \endcode

  //! \ingroup bmat_group
  //!
  //! \brief Function object for addition in the boolean semiring.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()(bool x, bool y) const noexcept` which returns `x || y`;
  //! representing addition in the boolean semiring.
  struct BooleanPlus {
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the boolean semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the boolean semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()(bool x, bool y) const noexcept {
      return x || y;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for multiplication in the boolean semiring.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()(bool x, bool y) const noexcept` which returns `x && y`;
  //! representing multiplication in the boolean semiring.
  struct BooleanProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the boolean
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in the boolean semiring.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()(bool x, bool y) const noexcept {
      return x & y;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for returning the multiplicative identity.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()() const noexcept` which returns `true`; representing
  //! the multiplicative identity of the boolean semiring.
  struct BooleanOne {
    //! \brief Call operator returning the multiplication identity \c true of
    //! the boolean semiring.
    //!
    //! Call operator returning the multiplication identity \c true of
    //! the boolean semiring.
    //!
    //! \returns The value \c true.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()() const noexcept {
      return true;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Function object for returning the additive identity.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `bool operator()() const noexcept` which returns `false`;
  //! representing the additive identity of the boolean semiring.
  struct BooleanZero {
    //! \brief Call operator returning the additive identity \c false of
    //! the boolean semiring.
    //!
    //! Call operator returning the additive identity \c false of
    //! the boolean semiring.
    //!
    //! \returns The value \c false.
    //!
    //! \exceptions
    //! \noexcept
    constexpr bool operator()() const noexcept {
      return false;
    }
  };

  //! \ingroup bmat_group
  //!
  //! \brief Alias for dynamic boolean matrices.
  //!
  //! Alias for the type of dynamic boolean matrices where the dimensions of the
  //! matrices can be defined at run time.
  // The use of `int` rather than `bool` as the scalar type for dynamic
  // boolean matrices is intentional, because the bit iterators implemented in
  // std::vector<bool> entail a significant performance penalty.
  using DynamicBMat
      = DynamicMatrix<BooleanPlus, BooleanProd, BooleanZero, BooleanOne, int>;

  //! \ingroup bmat_group
  //!
  //! \brief Alias for static boolean matrices.
  //!
  //! Alias for the type of static boolean matrices where the dimensions of the
  //! matrices are defined at compile time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  template <size_t R, size_t C>
  using StaticBMat = StaticMatrix<BooleanPlus,
                                  BooleanProd,
                                  BooleanZero,
                                  BooleanOne,
                                  R,
                                  C,
                                  int>;

  //! \ingroup bmat_group
  //!
  //! \brief Alias template for boolean matrices.
  //!
  //! Alias template for boolean matrices.
  //!
  //! \tparam R the number of rows of the matrices. A value of `0` (the default
  //! value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrices. A value of `0` indicates
  //! that the dimensions will be set at run time (the default value is ``R``).
  // FLS + JDM considered adding BMat8 and decided it wasn't a good idea.
  template <size_t R = 0, size_t C = R>
  using BMat
      = std::conditional_t<R == 0 || C == 0, DynamicBMat, StaticBMat<R, C>>;

  namespace detail {
    template <typename T>
    struct IsBMatHelper : std::false_type {};

    template <size_t R, size_t C>
    struct IsBMatHelper<StaticBMat<R, C>> : std::true_type {};

    template <>
    struct IsBMatHelper<DynamicBMat> : std::true_type {};

    template <typename T>
    struct BitSetCapacity {
      static constexpr size_t value = BitSet<1>::max_size();
    };

    template <size_t R, size_t C>
    struct BitSetCapacity<StaticBMat<R, C>> {
      static_assert(R == C, "the number of rows and columns must be equal");
      static constexpr size_t value = R;
    };
  }  // namespace detail

  //! \ingroup bmat_group
  //!
  //! \brief Helper to check if a type is \ref BMat.
  //!
  //! This variable has value `true` if the template parameter `T` is the
  //! same as `BMat<R, C>` for some values of `R` and `C`.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsBMat = detail::IsBMatHelper<T>::value;

  //! \ingroup bmat_group
  //!
  //! \brief Validate that a boolean matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring.
  //!
  //! \throws LibsemigroupsException if TODO(0)
  template <typename Mat>
  auto validate(Mat const& m) -> std::enable_if_t<IsBMat<Mat>> {
    using scalar_type = typename Mat::scalar_type;
    auto it           = std::find_if_not(
        m.cbegin(), m.cend(), [](scalar_type x) { return x == 0 || x == 1; });
    if (it != m.cend()) {
      size_t r, c;
      std::tie(r, c) = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION(
          "invalid entry, expected 0 or 1 but found {} in entry ({}, {})",
          *it,
          r,
          c);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Integer matrices - compile time semiring arithmetic
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup intmat_group Integer matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$m \times n\f$  matrices of
  //! integers for arbitrary dimensions \f$m\f$ and \f$n\f$. There are two
  //! types of such matrices: those whose dimension is known at compile-time,
  //! and those where it is not.  Both types can be accessed via the alias
  //! template IntMat: if `R` or `C` has value `0`, then the dimensions
  //! can be set at run time, otherwise `R` and `C` are the dimension. The
  //! default value of `R` is `0`, and of `C` is `R`.
  //!
  //! The alias \ref IntMat is either StaticMatrix or DynamicMatrix, please
  //! refer to the documentation of these class templates for more details. The
  //! only substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and the latter
  //! should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  // clang-format off
  //! IntMat<3> m;       // default construct an uninitialized 3 x 3 static matrix
  //! IntMat<>  m(4, 4); // construct an uninitialized 4 x 4 dynamic matrix
  //! \endcode
  // clang-format on

  //! \ingroup intmat_group
  //!
  //! \brief Function object for addition in the ring of integers.
  //!
  //! \tparam Scalar TODO
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! which returns the usual sum `x + y` of `x` and `y`; representing
  //! addition in the integer semiring.
  template <typename Scalar>
  struct IntegerPlus {
    // TODO doc
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      return x + y;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for multiplication in the ring of integers.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! which returns the usual product `x * y` of `x` and `y`;
  //! representing multiplication in the integer semiring.
  template <typename Scalar>
  struct IntegerProd {
    // TODO constepxr
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      return x * y;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for returning the additive identity.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns `0`; representing
  //! the additive identity of the integer semiring.
  template <typename Scalar>
  struct IntegerZero {
    // TODO constepxr
    Scalar operator()() const noexcept {
      return 0;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Function object for returning the multiplicative identity.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns `1`; representing
  //! the multiplicative identity of the integer semiring.
  template <typename Scalar>
  struct IntegerOne {
    // TODO doc
    // TODO constepxr
    Scalar operator()() const noexcept {
      return 1;
    }
  };

  //! \ingroup intmat_group
  //!
  //! \brief Alias for dynamic integer matrices.
  //!
  //! Alias for the type of dynamic integer matrices where the dimensions of the
  //! matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <typename Scalar>
  using DynamicIntMat = DynamicMatrix<IntegerPlus<Scalar>,
                                      IntegerProd<Scalar>,
                                      IntegerZero<Scalar>,
                                      IntegerOne<Scalar>,
                                      Scalar>;

  //! \ingroup intmat_group
  //!
  //! \brief Alias for static integer matrices.
  //!
  //! Alias for the type of static integer matrices where the dimensions of the
  //! matrices can be defined at compile time.
  //!
  //! \tparam R the number of rows of the matrices. A value of `0` (the default
  //! value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrices. A value of `0` indicates
  //! that the dimensions will be set at run time (the default value is `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R, size_t C, typename Scalar>
  using StaticIntMat = StaticMatrix<IntegerPlus<Scalar>,
                                    IntegerProd<Scalar>,
                                    IntegerZero<Scalar>,
                                    IntegerOne<Scalar>,
                                    R,
                                    C,
                                    Scalar>;

  //! \ingroup intmat_group
  //!
  //! \brief Alias template for integer matrices.
  //!
  //! Alias template for integer matrices.
  //!
  //! \tparam R the number of rows of the matrices. A value of `0` (the
  //! default value) indicates that the dimensions will be set at run time.
  //!
  //! \tparam C the number of columns of the matrices. A value of `0`
  //! indicates that the dimensions will be set at run time (the default value
  //! is `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using IntMat = std::conditional_t<R == 0 || C == 0,
                                    DynamicIntMat<Scalar>,
                                    StaticIntMat<R, C, Scalar>>;

  //! \ingroup intmat_group
  //!
  //! \brief Validate that an integer matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring. This is always \c true for \ref IntMat objects.
  template <size_t R, size_t C, typename Scalar>
  void validate(StaticIntMat<R, C, Scalar> const&) {}

  //! \copydoc validate(StaticIntMat<R, C, Scalar> const&)
  template <typename Scalar>
  void validate(DynamicIntMat<Scalar> const&) {}

  ////////////////////////////////////////////////////////////////////////
  // Max-plus matrices
  ////////////////////////////////////////////////////////////////////////
  //! \defgroup maxplusmat_group Max-plus matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices over
  //! the max-plus semiring for arbitrary dimension \f$n\f$. There are two types
  //! of such matrices those whose dimension is known at compile-time, and those
  //! where it is not.  Both types can be accessed via the alias template
  //! \ref MaxPlusMat: if `N` has value `0`, then the dimensions can be set at
  //! run time, otherwise `N` is the dimension. The default value of `N` is `0`.
  //!
  //! The alias \ref MaxPlusMat is either StaticMatrix or DynamicMatrix, please
  //! refer to the documentation of these class templates for more details. The
  //! only substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and the latter
  //! should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //! MaxPlusMat<3> m;       // default construct an uninitialized 3 x 3
  //!    static matrix MaxPlusMat<>  m(4, 4); // construct an uninitialized 4 x
  //!    4 dynamic matrix
  //! \endcode
  // TODO(0) fix layout in example above

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for addition in the max-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \oplus y\f$ which is defined by
  //!
  //! \f[
  //!    x\oplus y =
  //!    \begin{cases}
  //!    \max\{x, y\}   & \text{if } x \neq -\infty\text{ and }y \neq -\infty
  //!    \\ \mbox{}
  //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!    \end{cases}
  //! \f]
  //!
  //! representing addition in the max-plus semiring.
  // Static arithmetic
  template <typename Scalar>
  struct MaxPlusPlus {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the max-plus semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \y in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for multiplication in the max-plus semiring.
  //!
  //!  This is a stateless struct with a single call operator of signature:
  //!  `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //!  that returns \f$x \otimes y\f$ which is defined bitset_type
  //!  \f[
  //!     x\otimes y =
  //!     \begin{cases}
  //!     x + y   & \text{if } x \neq -\infty\text{ and }y \neq -\infty \\
  //!     -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!     \end{cases}
  //!  \f]
  //!  representing multiplication in the max-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MaxPlusProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the max-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \y in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return x + y;
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Function object for returning the additive identity of the max-plus
  //! semiring.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()() const noexcept` which returns \f$-\infty\f$;
  //! representing the additive identity of the max-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MaxPlusZero {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for additive identity.
    //!
    //! This function returns the additive identity in the max-plus
    //! semiring.
    //!
    //! \returns The additive identity in the max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()() const noexcept {
      return NEGATIVE_INFINITY;
    }
  };

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias for dynamic max-plus matrices.
  //!
  //! Alias for the type of dynamic max-plus matrices where the dimensions of
  //! the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicMaxPlusMat = DynamicMatrix<MaxPlusPlus<Scalar>,
                                          MaxPlusProd<Scalar>,
                                          MaxPlusZero<Scalar>,
                                          IntegerZero<Scalar>,
                                          Scalar>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias for static max-plus matrices.
  //!
  //! Alias for static max-plus matrices whose arithmetic and dimensions are
  //! defined at compile-time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticMaxPlusMat = StaticMatrix<MaxPlusPlus<Scalar>,
                                        MaxPlusProd<Scalar>,
                                        MaxPlusZero<Scalar>,
                                        IntegerZero<Scalar>,
                                        R,
                                        C,
                                        Scalar>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Alias template for max-plus matrices.
  //!
  //! Alias template for max-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using MaxPlusMat = std::conditional_t<R == 0 || C == 0,
                                        DynamicMaxPlusMat<Scalar>,
                                        StaticMaxPlusMat<R, C, Scalar>>;

  //! \ingroup maxplusmat_group
  //!
  //! \brief Validate that a max-plus matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring. This is always \c true for \ref MaxPlusMat objects.
  template <size_t R, size_t C, typename Scalar>
  void validate(StaticMaxPlusMat<R, C, Scalar> const&) {}

  //! \copydoc validate(StaticMaxPlusMat<R, C, Scalar> const&)
  template <typename Scalar>
  void validate(DynamicMaxPlusMat<Scalar> const&) {}

  ////////////////////////////////////////////////////////////////////////
  // Min-plus matrices
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup minplusmat_group Min-plus matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the min-plus semiring for arbitrary dimension \f$n\f$. There are
  //! two types of such matrices those whose dimension is known at compile-time,
  //! and those where it is not.  Both types can be accessed via the alias
  //! template \ref MinPlusMat: if `N` has value `0`, then
  //! the dimensions can be set at run time, otherwise `N` is the dimension.
  //! The default value of `N` is `0`.
  //!
  //! The alias \ref MinPlusMat is either StaticMatrix or DynamicMatrix, please
  //! refer to the documentation of these class templates for more details. The
  //! only substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and the latter
  //! should be constructed using the dimensions.
  //!
  //! \par Example
  //!
  //! \code
  //!    MinPlusMat<3> m;       // default construct an uninitialized 3 x 3
  //!    static matrix MinPlusMat<>  m(4, 4); // construct an uninitialized 4 x
  //!    4 dynamic matrix
  //!    \endcode
  // TODO fix the example

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for addition in the min-plus semiring.
  //!
  //!  This is a stateless struct with a single call operator of signature:
  //!  `Scalar operator()(Scalar x, Scalar y) const noexcept` that returns \f$x
  //!  \oplus y\f$ which is defined by
  //!  \f[
  //!     x\oplus y =
  //!     \begin{cases}
  //!     \min\{x, y\}   & \text{if } x \neq \infty\text{ and }y \neq \infty \\
  //!     \infty & \text{if } x = \infty \text{ or }y = \infty; \\
  //!     \end{cases}
  //!  \f]
  //!  representing addition in the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  //!
  // Static arithmetic
  template <typename Scalar>
  struct MinPlusPlus {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in the min-plus semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \y in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for multiplication in the min-plus semiring.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //! \f[
  //!     x\otimes y =
  //!     \begin{cases}
  //!     x + y  & \text{if } x \neq \infty\text{ and }y \neq \infty \\
  //!     \infty & \text{if } x = \infty \text{ or }y = \infty; \\
  //!     \end{cases}
  //! \f]
  //!  representing multiplication in the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MinPlusProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in the min-plus
    //! semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \y in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return x + y;
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Function object for returning the additive identity of the min-plus
  //! semiring.
  //!
  //!  This is a stateless struct with a single call operator of signature:
  //!  `Scalar operator()() const noexcept` which returns \f$\infty\f$;
  //!  representing the additive identity of the min-plus semiring.
  //!
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <typename Scalar>
  struct MinPlusZero {
    static_assert(std::is_signed<Scalar>::value,
                  "MinPlus requires a signed integer type as parameter!");
    //! \brief Call operator for additive identity.
    //!
    //! This function returns the additive identity in the min-plus
    //! semiring.
    //!
    //! \returns The additive identity in the min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()() const noexcept {
      return POSITIVE_INFINITY;
    }
  };

  //! \ingroup minplusmat_group
  //!
  //! \brief Alias for dynamic min-plus matrices.
  //!
  //! Alias for the type of dynamic min-plus matrices where the dimensions of
  //! the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicMinPlusMat = DynamicMatrix<MinPlusPlus<Scalar>,
                                          MinPlusProd<Scalar>,
                                          MinPlusZero<Scalar>,
                                          IntegerZero<Scalar>,
                                          Scalar>;

  //! \ingroup minplusmat_group
  //!
  //! \brief Alias for static min-plus matrices.
  //!
  //! Alias for static min-plus matrices whose arithmetic and dimensions are
  //! defined at compile-time.
  //!
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticMinPlusMat = StaticMatrix<MinPlusPlus<Scalar>,
                                        MinPlusProd<Scalar>,
                                        MinPlusZero<Scalar>,
                                        IntegerZero<Scalar>,
                                        R,
                                        C,
                                        Scalar>;
  //! \ingroup minplusmat_group
  //!
  //! \brief Alias template for min-plus matrices.
  //!
  //! Alias template for min-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using MinPlusMat = std::conditional_t<R == 0 || C == 0,
                                        DynamicMinPlusMat<Scalar>,
                                        StaticMinPlusMat<R, C, Scalar>>;

  //! \ingroup minplusmat_group
  //!
  //! \brief Validate that a min-plus matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring. This is always \c true for \ref MinPlusMat objects.
  template <typename Scalar>
  void validate(DynamicMinPlusMat<Scalar> const&) {}

  // \copydoc validate(DynamicMinPlusMat<Scalar> const&)
  template <size_t R, size_t C, typename Scalar>
  void validate(StaticMinPlusMat<R, C, Scalar> const&) {}

  ////////////////////////////////////////////////////////////////////////
  // Max-plus matrices with threshold
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup maxplustruncmat_group Truncated max-plus  matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the finite quotient of the max-plus semiring by the congruence
  //! \f$t = t + 1\f$ for arbitrary \f$n\f$ and \f$t\f$. The value
  //! \f$t\f$ is referred to as the *threshold*.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic operations
  //!    are known at compile-time (i.e. the value of \f$t\f$ is known at
  //!    compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$) are
  //!    to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template
  //! \ref MaxPlusTruncMat if `T` has value `0`,
  //! then the threshold can be set at run time, and if `R` or `C` is `0`,
  //! then the dimension can be set at run time.  The default value of `T` is
  //! `0`, `R` is `0`, and of `C` is `R`.
  //!
  //! The alias \ref MaxPlusTruncMax is either
  //! StaticMatrix or DynamicMatrix, please refer to the
  //! documentation of these class templates for more details. The only
  //! substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and
  //! the latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //!    MaxPlusTruncMat<11, 3> m;       // default construct an uninitialized 3
  //!    x 3 static matrix with threshold 11 MaxPlusTruncMat<11> m(4, 4);    //
  //!    construct an uninitialized 4 x 4 dynamic matrix with threshold 11
  //!    MaxPlusTruncSemiring sr(11);    // construct a truncated max-plus
  //!    semiring with threshold 11 MaxPlusTruncMat<>  m(sr, 5, 5); // construct
  //!    an uninitialized 5 x 5 dynamic matrix with threshold 11 (defined at run
  //!    time)
  //!    \endcode
  // TODO unscramble the example

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Function object for multiplication in truncated max-plus semirings.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //! \f[
  //!    x\otimes y =
  //!    \begin{cases}
  //!    \min\{x + y, T\}   & \text{if } x \neq -\infty\text{ and }y \neq
  //!    -\infty \\
  //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty;
  //!    \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the max-plus semiring by
  //! the congruence \f$T = T + 1\f$.
  //!
  //! \tparam T the threshold (point at which the entries in the max-plus
  //! semiring are truncated).
  //! \tparam Scalar the type of the values in the semiring (must be signed
  //! integer type).
  template <size_t T, typename Scalar>
  struct MaxPlusTruncProd {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in a max-plus
    //! truncated semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \y in truncated max-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= static_cast<Scalar>(T))
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= static_cast<Scalar>(T))
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return std::min(x + y, static_cast<Scalar>(T));
    }
  };

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Class representing a truncated max-plus semiring.
  //!
  //! This class represents the **truncated max-plus semiring** consists of the
  //! integers \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$-\infty\f$. Instances of this
  //! class can be used to define the value of the threshold \f$t\f$ at run
  //! time.
  //!
  //! \tparam Scalar the type of the elements of the semiring. This must be a
  //! signed integer type (defaults to \c int).
  template <typename Scalar = int>
  class MaxPlusTruncSemiring {
    static_assert(std::is_signed<Scalar>::value,
                  "MaxPlus requires a signed integer type as parameter!");

   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    MaxPlusTruncSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    MaxPlusTruncSemiring(MaxPlusTruncSemiring const&) noexcept = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    MaxPlusTruncSemiring(MaxPlusTruncSemiring&&) noexcept = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    MaxPlusTruncSemiring& operator=(MaxPlusTruncSemiring const&) noexcept
        = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    MaxPlusTruncSemiring& operator=(MaxPlusTruncSemiring&&) noexcept = default;

    ~MaxPlusTruncSemiring() = default;

    //! \brief Construct from threshold.
    //!
    //! Construct from threshold.
    //!
    //! \param threshold the threshold.
    //!
    //! \throws LibsemigroupsException if `threshold` is less than zero.
    //!
    //! \complexity
    //! Constant.
    explicit MaxPlusTruncSemiring(Scalar threshold) : _threshold(threshold) {
      if (threshold < 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                                threshold);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns the multiplicative identity in a truncated
    //! max-plus semiring.
    //!
    //! \returns The additive identity in a truncated max-plus semiring (the
    //! value `0`).
    //!
    //! \exceptions
    //! \noexcept
    Scalar scalar_one() const noexcept {
      return 0;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns the additive identity in a truncated max-plus
    //! semiring.
    //!
    //! \returns The additive identity in a truncated max-plus semiring (the
    //! value `NEGATIVE_INFINITY`).
    //!
    //! \exceptions
    //! \noexcept
    Scalar scalar_zero() const noexcept {
      return NEGATIVE_INFINITY;
    }

    //! \brief Multiplication in a truncated max-plus semiring.
    //!
    //! This function returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!    x\otimes y =
    //!    \begin{cases}
    //!    \min\{x + y, t\}   & \text{if } x \neq -\infty\text{ and }y \neq
    //!    -\infty \\
    //!    -\infty & \text{if } x = -\infty \text{ or }y = -\infty; \\
    //!    \end{cases}
    //! \f]
    //! where \f$t\f$ is the threshold; representing multiplication in the
    //! quotient of the max-plus semiring.
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar prod(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY || y == NEGATIVE_INFINITY) {
        return NEGATIVE_INFINITY;
      }
      return std::min(x + y, _threshold);
    }

    //! \brief Addition in a truncated max-plus semiring.
    //!
    //! Returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!     \max\{x, y\}   & \text{if } x \neq -\infty\text{ and }y \neq -\infty
    //!     \\
    //!     -\infty & \text{if } x = -\infty \text{ or }y = -\infty; \\
    //!   \end{cases}
    //! \f]
    //! representing addition in the max-plus semiring (and its quotient).
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == NEGATIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == NEGATIVE_INFINITY);
      if (x == NEGATIVE_INFINITY) {
        return y;
      } else if (y == NEGATIVE_INFINITY) {
        return x;
      }
      return std::max(x, y);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct \ref MaxPlusTruncSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

   public:
    Scalar const _threshold;
  };

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias for dynamic truncated max-plus matrices.
  //!
  //! Alias for the type of dynamic truncated max-plus matrices where the
  //! dimension is defined at run time, but the threshold is defined at
  //! compile-time.
  //!
  //! \tparam T the threshold.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, typename Scalar>
  using DynamicMaxPlusTruncMat = DynamicMatrix<MaxPlusPlus<Scalar>,
                                               MaxPlusTruncProd<T, Scalar>,
                                               MaxPlusZero<Scalar>,
                                               IntegerZero<Scalar>,
                                               Scalar>;

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias for static truncated max-plus matrices.
  //!
  //! Alias for static truncated max-plus matrices where the threshold and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, size_t R, size_t C, typename Scalar>
  using StaticMaxPlusTruncMat = StaticMatrix<MaxPlusPlus<Scalar>,
                                             MaxPlusTruncProd<T, Scalar>,
                                             MaxPlusZero<Scalar>,
                                             IntegerZero<Scalar>,
                                             R,
                                             C,
                                             Scalar>;
  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Alias template for truncated max-plus matrices.
  //!
  //! Alias template for truncated max-plus matrices.
  //!
  //! \tparam T the threshold. A value of `0` indicates that the value will be
  //! set at run time (default: `0`).
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T = 0, size_t R = 0, size_t C = R, typename Scalar = int>
  using MaxPlusTruncMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0,
                         DynamicMatrix<MaxPlusTruncSemiring<Scalar>, Scalar>,
                         DynamicMaxPlusTruncMat<T, Scalar>>,
      StaticMaxPlusTruncMat<T, R, C, Scalar>>;

  namespace detail {
    template <typename T>
    struct IsMaxPlusTruncMatHelper : std::false_type {};

    template <size_t T, size_t R, size_t C, typename Scalar>
    struct IsMaxPlusTruncMatHelper<StaticMaxPlusTruncMat<T, R, C, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <size_t T, typename Scalar>
    struct IsMaxPlusTruncMatHelper<DynamicMaxPlusTruncMat<T, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <typename Scalar>
    struct IsMaxPlusTruncMatHelper<
        DynamicMatrix<MaxPlusTruncSemiring<Scalar>, Scalar>> : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
    };
  }  // namespace detail

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Helper to check if a type is \ref MaxPlusTruncMat.
  //!
  //! This variable has value `true` if the template parameter `T` is the same
  //! as \ref MaxPlusTruncMat for some template parameters; and `false` if it is
  //! not.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMaxPlusTruncMat
      = detail::IsMaxPlusTruncMatHelper<T>::value;

  namespace detail {
    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsMaxPlusTruncMat<T>>>
        : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsMaxPlusTruncMatHelper<T>::threshold;
    };
  }  // namespace detail

  //! \ingroup maxplustruncmat_group
  //!
  //! \brief Validate that a truncated max-plus matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring.
  //!
  //! \throws LibsemigroupsException if TODO(0)
  // TODO(1) to tpp
  template <typename Mat>
  auto validate(Mat const& m) -> std::enable_if_t<IsMaxPlusTruncMat<Mat>> {
    // Check that the semiring pointer isn't the nullptr if it shouldn't be
    detail::semiring_validate(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix_threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == NEGATIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      auto [r, c] = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected values in [0, {}] "
                              "{} {{-{}}} but found {} in entry ({}, {})",
                              t,
                              u8"\u222A",
                              u8"\u221E",
                              *it,
                              r,
                              c);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // Min-plus matrices with threshold
  ////////////////////////////////////////////////////////////////////////

  //! \defgroup minplustruncmat_group Truncated min-plus matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$  matrices
  //! over the finite quotient of the min-plus semiring by the congruence
  //! \f$t = t + 1\f$ for arbitrary \f$n\f$ and \f$t\f$. The value
  //! \f$t\f$ is referred to as the *threshold*.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic operations
  //!    are known at compile-time (i.e. the value of \f$t\f$ is known at
  //!    compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$) are
  //!    to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template \ref
  //! MinPlusTruncMat: if `T` has value `0`, then the threshold can be set at
  //! run time, and if `R` or `C` is `0`, then the dimension can be set at run
  //! time. The default value of `T` is `0`, `R` is `0`, and of `C` is `R`.
  //!
  //! The alias \ref MinPlusTruncMat is either StaticMatrix or DynamicMatrix,
  //! please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of StaticMatrix
  //! and DynamicMatrix is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //!    MinPlusTruncMat<11, 3> m;       // default construct an uninitialized 3
  //!    x 3 static matrix with threshold 11 MinPlusTruncMat<11> m(4, 4);    //
  //!    construct an uninitialized 4 x 4 dynamic matrix with threshold 11
  //!    MinPlusTruncSemiring sr(11);    // construct a truncated min-plus
  //!    semiring with threshold 11 MinPlusTruncMat<>  m(sr, 5, 5); // construct
  //!    an uninitialized 5 x 5 dynamic matrix with threshold 11 (defined at run
  //!    time)
  //! \endcode
  // TODO unscramble the example

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Function object for multiplication in min-plus truncated semirings.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept` that returns \f$x
  //! \otimes y\f$ which is defined by
  //! \f[
  //!    x\otimes y =
  //!    \begin{cases}
  //!    \min\{x + y, T\}   & \text{if } x \neq \infty\text{ and }y \neq
  //!    \infty \\
  //!    \infty & \text{if } x = \infty \text{ or }y = \infty; \\
  //!    \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the min-plus semiring
  //! by the congruence \f$T = T + 1\f$.
  //!
  //! \tparam T the threshold (point at which the entries in the min-plus
  //! semiring are truncated).
  //! \tparam Scalar the type of the values in the semiring.
  template <size_t T, typename Scalar>
  struct MinPlusTruncProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in a truncated
    //! min-plus semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in truncated min-plus semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= static_cast<Scalar>(T))
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= static_cast<Scalar>(T))
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::min(x + y, static_cast<Scalar>(T));
    }
  };

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Class representing a truncated min-plus semiring.
  //!
  //! This class represents the **min-plus truncated semiring** consists of the
  //! integers \f$\{0, \ldots , t\}\f$ for some value \f$t\f$ (called the
  //! **threshold** of the semiring) and \f$\infty\f$. Instances of this class
  //! can be used to define the value of the threshold \f$t\f$ at run time.
  //!
  //! \tparam Scalar the type of the elements of the semiring. This must be an
  //! integral type.
  template <typename Scalar = int>
  class MinPlusTruncSemiring {
    static_assert(std::is_integral<Scalar>::value,
                  "MinPlus requires an integral type as parameter!");

   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    MinPlusTruncSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    MinPlusTruncSemiring(MinPlusTruncSemiring const&) noexcept = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    MinPlusTruncSemiring(MinPlusTruncSemiring&&) noexcept = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    MinPlusTruncSemiring& operator=(MinPlusTruncSemiring const&) noexcept
        = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    MinPlusTruncSemiring& operator=(MinPlusTruncSemiring&&) noexcept = default;

    //! \brief Construct from threshold.
    //!
    //! Construct from threshold.
    //!
    //! \param threshold the threshold.
    //!
    //! \throws LibsemigroupsException if `threshold` is less than zero.
    //!
    //! \complexity
    //! Constant.
    explicit MinPlusTruncSemiring(Scalar threshold) : _threshold(threshold) {
      if (std::is_signed<Scalar>::value && threshold < 0) {
        LIBSEMIGROUPS_EXCEPTION("expected non-negative value, found {}",
                                threshold);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns the multiplicative identity in a truncated
    //! min-plus semiring.
    //!
    //! \returns The multiplicative identity in a truncated min-plus semiring
    //! (the value `0`).
    //!
    //! \exceptions
    //! \noexcept
    Scalar scalar_one() const noexcept {
      return 0;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns the additive identity in a truncated min-plus
    //! semiring.
    //!
    //! \returns The additive identity in a truncated min-plus semiring (the
    //! value `POSITIVE_INFINITY`).
    //!
    //! \exceptions
    //! \noexcept
    // TODO These mem fns (one and zero) aren't needed?
    // TODO constexpr?
    Scalar scalar_zero() const noexcept {
      return POSITIVE_INFINITY;
    }

    //! \brief Multiplication in a truncated min-plus semiring.
    //!
    //! Returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!    x\otimes y =
    //!    \begin{cases}
    //!    \min\{x + y, t\}   & \text{if } x \neq \infty\text{ and }y \neq
    //!    \infty \\
    //!    \infty & \text{if } x = \infty \text{ or }y = \infty; \\
    //!    \end{cases}
    //! \f]
    //! where \f$t\f$ is the threshold; representing multiplication in the
    //! quotient of the min-plus semiring.
    //!
    //! \param x scalar
    //! \param y scalar
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar prod(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY || y == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      return std::min(x + y, _threshold);
    }

    //! \brief Addition in a truncated min-plus semiring.
    //!
    //! Returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!   \min\{x, y\}   & \text{if } x \neq \infty\text{ and }y \neq \infty \\
    //!   \infty & \text{if } x = \infty \text{ or }y = \infty; \\
    //!   \end{cases}
    //! \f]
    //! representing addition in the min-plus truncated semiring (and its
    //! quotient).
    //!
    //! \param x scalar.
    //! \param y scalar.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT((x >= 0 && x <= _threshold)
                           || x == POSITIVE_INFINITY);
      LIBSEMIGROUPS_ASSERT((y >= 0 && y <= _threshold)
                           || y == POSITIVE_INFINITY);
      if (x == POSITIVE_INFINITY) {
        return y;
      } else if (y == POSITIVE_INFINITY) {
        return x;
      }
      return std::min(x, y);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct \ref MinPlusTruncSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

   public:
    Scalar const _threshold;
  };

  // TODO remove?
  template <typename Scalar>
  using DynamicMinPlusTruncMatSR
      = DynamicMatrix<MinPlusTruncSemiring<Scalar>, Scalar>;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias for dynamic truncated min-plus matrices.
  //!
  //! Alias for the type of dynamic truncated min-plus matrices where the
  //! dimension is defined at run time, but the threshold is defined at
  //! compile-time.
  //!
  //! \tparam T the threshold.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, typename Scalar>
  using DynamicMinPlusTruncMat = DynamicMatrix<MinPlusPlus<Scalar>,
                                               MinPlusTruncProd<T, Scalar>,
                                               MinPlusZero<Scalar>,
                                               IntegerZero<Scalar>,
                                               Scalar>;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias for static truncated min-plus matrices.
  //!
  //! Alias for static truncated min-plus matrices where the threshold and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //! \tparam R  the number of rows.
  //! \tparam C  the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t T, size_t R, size_t C, typename Scalar>
  using StaticMinPlusTruncMat = StaticMatrix<MinPlusPlus<Scalar>,
                                             MinPlusTruncProd<T, Scalar>,
                                             MinPlusZero<Scalar>,
                                             IntegerZero<Scalar>,
                                             R,
                                             C,
                                             Scalar>;

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Alias template for truncated min-plus matrices.
  //!
  //! Alias template for truncated min-plus matrices.
  //!
  //! \tparam T the threshold. A value of `0` indicates that the value will be
  //! set at run time (default: `0`).
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T = 0, size_t R = 0, size_t C = R, typename Scalar = int>
  using MinPlusTruncMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0,
                         DynamicMinPlusTruncMatSR<Scalar>,
                         DynamicMinPlusTruncMat<T, Scalar>>,
      StaticMinPlusTruncMat<T, R, C, Scalar>>;

  namespace detail {
    template <typename T>
    struct IsMinPlusTruncMatHelper : std::false_type {};

    template <size_t T, size_t R, size_t C, typename Scalar>
    struct IsMinPlusTruncMatHelper<StaticMinPlusTruncMat<T, R, C, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <size_t T, typename Scalar>
    struct IsMinPlusTruncMatHelper<DynamicMinPlusTruncMat<T, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
    };

    template <typename Scalar>
    struct IsMinPlusTruncMatHelper<DynamicMinPlusTruncMatSR<Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
    };
  }  // namespace detail

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Helper to check if a type is \ref MinPlusTruncMat.
  //!
  //! This variable has value `true` if the template parameter `T` is the same
  //! as \ref MinPlusTruncMat for some template parameters; and `false` if it is
  //! not.
  //!
  //! \tparam T the type to check.
  template <typename T>
  static constexpr bool IsMinPlusTruncMat
      = detail::IsMinPlusTruncMatHelper<T>::value;

  namespace detail {
    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsMinPlusTruncMat<T>>>
        : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsMinPlusTruncMatHelper<T>::threshold;
    };
  }  // namespace detail

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Validate that a truncated min-plus matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring.
  //!
  //! \throws LibsemigroupsException if TODO(0)
  // TODO(1) to tpp
  template <typename Mat>
  auto validate(Mat const& m) -> std::enable_if_t<IsMinPlusTruncMat<Mat>> {
    // Check that the semiring pointer isn't the nullptr if it shouldn't be
    detail::semiring_validate(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix_threshold(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t](scalar_type x) {
      return x == POSITIVE_INFINITY || (0 <= x && x <= t);
    });
    if (it != m.cend()) {
      uint64_t r, c;
      std::tie(r, c) = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected values in [0, {}] "
                              "{} {{{}}} but found {} in entry ({}, {})",
                              t,
                              u8"\u222A",
                              u8"\u221E",
                              *it,
                              r,
                              c);
    }
  }

  ////////////////////////////////////////////////////////////////////////
  // NTP matrices
  ////////////////////////////////////////////////////////////////////////

  // clang-format off

  //! \defgroup ntpmat_group Matrices over the natural numbers quotiented by (t = t + p)
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$ matrices over
  //! the finite quotient of the usual semiring of natural number by the
  //! congruence \f$t = t + p\f$ for arbitrary \f$n\f$, \f$t\f$, and
  //! \f$p\f$. The value \f$t\f$ is referred to as the *threshold* and
  //! \f$p\f$ is called the *period*. The matrices of this type are referred
  //! to by the acronym **ntp** matrices, for "natural threshold period".  The
  //! NTPSemiring has elements \f$\{0, 1, ..., t, t +  1, ..., t
  //! + p - 1\}\f$ where  \f$t\f$, and \f$p\f$ are the threshold and period,
  //! respectively; addition and multiplication in the NTPSemiring is
  //! defined below.
  //!
  //! There are three types of such matrices where:
  //!
  //! 1. the dimension is known at compile-time;
  //! 2. the dimension is to be defined a run time but the arithmetic operations
  //!    are known at compile-time (i.e. the values of \f$t\f$ and \f$p\f$
  //!    are known at compile time)
  //! 3. both the dimension and the arithmetic operations (i.e. \f$t\f$ and
  //!    \f$p\f$) are to be defined a run time.
  //!
  //! All three of these types can be accessed via the alias template \ref
  //! NTPMat:
  //! if `T` and `P` have value `0`, then the threshold and period can be set at
  //! run time, and if `R` or `C` is `0`, then the dimension can be set at run
  //! time.  The default values of `T`, `P`, and `R` are `0`, and the default
  //! value of `C` is `R`.
  //!
  //! The alias \ref NTPMat is either StaticMatrix or DynamicMatrix, please
  //! refer to the documentation of these class templates for more details. The
  //! only substantial difference in the interface of StaticMatrix and
  //! DynamicMatrix is that the former can be default constructed and the latter
  //! should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //!
  //!    NTPMat<11, 2, 3> m;  // default construct an uninitialized 3 x 3 static
  //!    matrix with threshold 11, period 2 NTPMat<11, 2> m(4, 4);  // construct
  //!    an uninitialized 4 x 4 dynamic matrix with threshold 11, period 2
  //!    NTPSemiring sr(11, 2);  // construct an ntp semiring with threshold 11,
  //!    period 2 NTPMat<>  m(sr, 5, 5);  // construct an uninitialized 5 x 5
  //!    dynamic matrix with threshold 11, period 2
  //!    \endcode
  // clang-format on
  // TODO unmangle the code block above

  namespace detail {
    template <size_t T, size_t P, typename Scalar>
    constexpr Scalar thresholdperiod(Scalar x) noexcept {
      if (x > T) {
        return T + (x - T) % P;
      }
      return x;
    }

    template <typename Scalar>
    inline Scalar thresholdperiod(Scalar x,
                                  Scalar threshold,
                                  Scalar period) noexcept {
      if (x > threshold) {
        return threshold + (x - threshold) % period;
      }
      return x;
    }
  }  // namespace detail

  //! \ingroup ntpmat_group
  //!
  //! \brief Function object for addition in ntp semirings.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \oplus y\f$ which is defined by
  //! \f[
  //!    x\oplus y =
  //!    \begin{cases}
  //!    x + y & \text{if } x + y \leq T \\
  //!    T + ((x + y) - T \pmod{P}) & \text{if } x + y > T
  //!    \end{cases}
  //! \f]
  //! representing addition in the quotient of the semiring natural numbers by
  //! the congruence \f$(T = T + P)\f$.
  //!
  //! \tparam T the threshold.
  //! \tparam P the period.
  //! \tparam Scalar the type of the values in the semiring.
  // Static arithmetic
  template <size_t T, size_t P, typename Scalar>
  struct NTPPlus {
    //! \brief Call operator for addition.
    //!
    //! This function returns the sum of its arguments in an ntp semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The sum of \p x and \p y in an ntp semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      return detail::thresholdperiod<T, P>(x + y);
    }
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Function object for multiplication in an ntp semirings.
  //!
  //! This is a stateless struct with a single call operator of signature:
  //! `Scalar operator()(Scalar x, Scalar y) const noexcept`
  //! that returns \f$x \otimes y\f$ which is defined by
  //!
  //! \f[
  //!   x\otimes y =
  //!   \begin{cases}
  //!   xy & \text{if } xy \leq T \\
  //!   T + ((xy - T) \pmod{P}) & \text{if } xy > T
  //!   \end{cases}
  //! \f]
  //! representing multiplication in the quotient of the semiring natural
  //! numbers by the congruence \f$(T = T + P)\f$.
  //!
  //! \tparam T the threshold.
  //! \tparam P the period.
  //! \tparam Scalar the type of the values in the semiring.
  template <size_t T, size_t P, typename Scalar>
  struct NTPProd {
    //! \brief Call operator for multiplication.
    //!
    //! This function returns the product of its arguments in an ntp semiring.
    //!
    //! \param x the first value.
    //! \param y the second value.
    //!
    //! \returns The product of \p x and \p y in an ntp semiring.
    //!
    //! \exceptions
    //! \noexcept
    Scalar operator()(Scalar x, Scalar y) const noexcept {
      return detail::thresholdperiod<T, P>(x * y);
    }
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Class representing an ntp semiring.
  //!
  //! This class represents the **ntp semiring** consists of the integers
  //! \f$\{0, 1, ..., t, t +  1, ..., t + p - 1\}\f$ for some  \f$t\f$ and
  //! \f$p\f$ (called the **threshold** and **period**). Instances of this
  //! class can be used to define the value of the threshold \f$t\f$ and
  //! period \f$p\f$ at run time.
  //!
  //! \tparam Scalar the type of the elements of the semiring.
  // Dynamic arithmetic
  template <typename Scalar = size_t>
  class NTPSemiring {
   public:
    //! \brief Deleted default constructor.
    //!
    //! Deleted default constructor.
    // Deleted to avoid uninitialised values of period and threshold.
    NTPSemiring() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    NTPSemiring(NTPSemiring const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    NTPSemiring(NTPSemiring&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    NTPSemiring& operator=(NTPSemiring const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    NTPSemiring& operator=(NTPSemiring&&) = default;

    ~NTPSemiring() = default;

    //! \brief Construct from threshold and period.
    //!
    //! Construct from threshold and period.
    //!
    //! \param t the threshold (\f$t \geq 0\f$).
    //! \param p the period (\f$p > 0\f$).
    //!
    //! \throws LibsemigroupsException if \p t is less than zero.
    //! \throws LibsemigroupsException if \p p is less than or equal to zero.
    //!
    //! \complexity
    //! Constant.
    NTPSemiring(Scalar t, Scalar p) : _period(p), _threshold(t) {
      if constexpr (std::is_signed<Scalar>::value) {
        if (t < 0) {
          LIBSEMIGROUPS_EXCEPTION(
              "expected non-negative value for 1st argument, found {}", t);
        }
      }
      if (p <= 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected positive value for 2nd argument, found {}", p);
      }
    }

    //! \brief Get the multiplicative identity.
    //!
    //! This function returns \f$1\f$; representing the multiplicative identity
    //! of the quotient of the semiring of natural numbers.
    //!
    //! \returns The multiplicative identity in an ntp semiring (the
    //! value `0`).
    //!
    //! \exceptions
    //! \noexcept
    static constexpr Scalar scalar_one() noexcept {
      return 1;
    }

    //! \brief Get the additive identity.
    //!
    //! This function returns \f$0\f$ representing the additive identity of the
    //! quotient of the semiring of natural numbers.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    static constexpr Scalar scalar_zero() noexcept {
      return 0;
    }

    //! \brief Multiplication in an ntp semiring.
    //!
    //! This function returns \f$x \otimes y\f$ which is defined by
    //! \f[
    //!   x\otimes y =
    //!   \begin{cases}
    //!   xy & \text{if } xy \leq t \\
    //!   t + ((xy - t)\pmod{p}) & \text{if } xy > t
    //!   \end{cases}
    //! \f]
    //! where \f$t\f$ is the \ref threshold and \f$p\f$ is the \ref period;
    //! representing multiplication in the quotient of the semiring of natural
    //! numbers.
    //!
    //! \param x scalar (`0 <= x < threshold() + period()`).
    //! \param y scalar (`0 <= y < threshold() + period()`).
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    // TODO should be _no_checks, also for the other Semirings
    Scalar prod(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + _threshold - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + _threshold - 1);
      return detail::thresholdperiod(x * y, _threshold, _period);
    }

    //! \brief Addition in an ntp semiring.
    //!
    //! This function returns \f$x \oplus y\f$ which is defined by
    //! \f[
    //!   x\oplus y =
    //!   \begin{cases}
    //!   x + y                      & \text{if } x + y \leq t \\
    //!   t + ((x + y - t) \pmod{p}) & \text{if } x + y > t
    //!   \end{cases}
    //! \f]
    //! where \f$t\f$ is the \ref threshold and \f$p\f$ is the \ref period;
    //! representing multiplication in the quotient of the semiring of natural
    //! numbers.
    //!
    //! \param x scalar (\f$0\leq x < t + p\f$).
    //! \param y scalar (\f$0\leq y < t + p\f$).
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar plus(Scalar x, Scalar y) const noexcept {
      LIBSEMIGROUPS_ASSERT(x >= 0 && x <= _period + _threshold - 1);
      LIBSEMIGROUPS_ASSERT(y >= 0 && y <= _period + _threshold - 1);
      return detail::thresholdperiod(x + y, _threshold, _period);
    }

    //! \brief Get the threshold.
    //!
    //! Returns the threshold value used to construct \ref NTPSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar threshold() const noexcept {
      return _threshold;
    }

    //! \brief Get the period.
    //!
    //! Returns the period value used to construct \ref NTPSemiring
    //! instance.
    //!
    //! \returns A value of type `Scalar`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    Scalar period() const noexcept {
      return _period;
    }

   private:
    Scalar _period;
    Scalar _threshold;
  };

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with dynamic threshold and period.
  //!
  //! Alias for ntp matrices with dimensions, threshold, and period defined
  //! at runtime.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicNTPMatWithSemiring = DynamicMatrix<NTPSemiring<Scalar>, Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with static threshold and period.
  //!
  //! Alias for the type of dynamic ntp matrices where the dimension
  //! is defined at run time, but the threshold and period are defined at
  //! compile-time.
  //!
  //! \tparam T       the threshold.
  //! \tparam P       the period.
  //! \tparam Scalar  the type of the entries in the matrix.
  template <size_t T, size_t P, typename Scalar>
  using DynamicNTPMatWithoutSemiring = DynamicMatrix<NTPPlus<T, P, Scalar>,
                                                     NTPProd<T, P, Scalar>,
                                                     IntegerZero<Scalar>,
                                                     IntegerOne<Scalar>,
                                                     Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias for ntp matrices with static threshold and period, and
  //! dimensions.
  //!
  //! Alias for static ntp matrices where the threshold, period, and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam T  the threshold.
  //!
  //! \tparam P  the period.
  //!
  //! \tparam R  the number of rows.
  //!
  //! \tparam C  the number of columns.
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `int`).
  template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
  using StaticNTPMat = StaticMatrix<NTPPlus<T, P, Scalar>,
                                    NTPProd<T, P, Scalar>,
                                    IntegerZero<Scalar>,
                                    IntegerOne<Scalar>,
                                    R,
                                    C,
                                    Scalar>;

  //! \ingroup ntpmat_group
  //!
  //! \brief Alias template for ntp matrices.
  //!
  //! Alias template for truncated min-plus matrices.
  //!
  //! \tparam T the threshold. If both `T` and `P` are `0`, this indicates that
  //!   the value will be set at run time (default: `0`).
  //!
  //! \tparam P the period. If both `T` and `P` are `0`, this indicates that the
  //!   value will be set at run time (default: `0`).
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar the type of the entries in the matrix (default: `size_t`).
  template <size_t T        = 0,
            size_t P        = 0,
            size_t R        = 0,
            size_t C        = R,
            typename Scalar = size_t>
  using NTPMat = std::conditional_t<
      R == 0 || C == 0,
      std::conditional_t<T == 0 && P == 0,
                         DynamicNTPMatWithSemiring<Scalar>,
                         DynamicNTPMatWithoutSemiring<T, P, Scalar>>,
      StaticNTPMat<T, P, R, C, Scalar>>;

  namespace detail {
    template <typename T>
    struct IsNTPMatHelper : std::false_type {};

    template <typename Scalar>
    struct IsNTPMatHelper<DynamicNTPMatWithSemiring<Scalar>> : std::true_type {
      static constexpr Scalar threshold = UNDEFINED;
      static constexpr Scalar period    = UNDEFINED;
    };

    template <size_t T, size_t P, typename Scalar>
    struct IsNTPMatHelper<DynamicNTPMatWithoutSemiring<T, P, Scalar>>
        : std::true_type {
      static constexpr Scalar threshold = T;
      static constexpr Scalar period    = P;
    };

    template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
    struct IsNTPMatHelper<StaticNTPMat<T, P, R, C, Scalar>> : std::true_type {
      static constexpr Scalar threshold = T;
      static constexpr Scalar period    = P;
    };
  }  // namespace detail

  //! \ingroup minplustruncmat_group
  //!
  //! \brief Helper to check if a type is \ref NTPMat.
  //!
  //! This variable has value `true` if the template parameter `U` is the same
  //! as NTPMat for some values of `T`, `P`, `R`, `C`, and `Scalar`; and `false`
  //! if it is not.
  //!
  //! \tparam U the type to check.
  template <typename U>
  static constexpr bool IsNTPMat = detail::IsNTPMatHelper<U>::value;

  namespace detail {
    template <typename T>
    struct IsTruncMatHelper<T, std::enable_if_t<IsNTPMat<T>>> : std::true_type {
      static constexpr typename T::scalar_type threshold
          = IsNTPMatHelper<T>::threshold;
      static constexpr typename T::scalar_type period
          = IsNTPMatHelper<T>::period;
    };

  }  // namespace detail

  namespace matrix {
    // TODO doc
    template <size_t T, size_t P, size_t R, size_t C, typename Scalar>
    constexpr Scalar period(StaticNTPMat<T, P, R, C, Scalar> const&) noexcept {
      return P;
    }

    // TODO doc
    template <size_t T, size_t P, typename Scalar>
    constexpr Scalar
    period(DynamicNTPMatWithoutSemiring<T, P, Scalar> const&) noexcept {
      return P;
    }

    // TODO doc
    template <typename Scalar>
    Scalar period(DynamicNTPMatWithSemiring<Scalar> const& x) noexcept {
      return x.semiring()->period();
    }
  }  // namespace matrix

  //! \ingroup ntpmat_group
  //!
  //! \brief Validate that an ntp matrix is valid.
  //!
  //! This function can be used to validate that a matrix contains values in the
  //! underlying semiring.
  //!
  //! \throws LibsemigroupsException if TODO(0)
  template <typename Mat>
  auto validate(Mat const& m) -> std::enable_if_t<IsNTPMat<Mat>> {
    // Check that the semiring pointer isn't the nullptr if it shouldn't be
    detail::semiring_validate(m);

    using scalar_type   = typename Mat::scalar_type;
    scalar_type const t = matrix_threshold(m);
    scalar_type const p = matrix::period(m);
    auto it = std::find_if_not(m.cbegin(), m.cend(), [t, p](scalar_type x) {
      return (0 <= x && x < p + t);
    });
    if (it != m.cend()) {
      uint64_t r, c;
      std::tie(r, c) = m.coords(it);
      LIBSEMIGROUPS_EXCEPTION("invalid entry, expected values in [0, {}) but "
                              "found {} in entry ({}, {})",
                              p + t,
                              *it,
                              r,
                              c);
    }
  }  // namespace libsemigroups

  ////////////////////////////////////////////////////////////////////////
  // Projective max-plus matrices
  ////////////////////////////////////////////////////////////////////////

  namespace detail {
    template <typename T>
    class ProjMaxPlusMat : MatrixPolymorphicBase {
     public:
      using scalar_type            = typename T::scalar_type;
      using scalar_reference       = typename T::scalar_reference;
      using scalar_const_reference = typename T::scalar_const_reference;
      using semiring_type          = void;

      using container_type = typename T::container_type;
      using iterator       = typename T::iterator;
      using const_iterator = typename T::const_iterator;

      using underlying_matrix_type = T;

      using RowView = typename T::RowView;

      // Note that Rows are never normalised, and that's why we use the
      // underlying matrix Row type and not 1 x n ProjMaxPlusMat's instead
      // (since these will be normalised according to their entries, and
      // this might not correspond to the normalised entries of the matrix).
      using Row = typename T::Row;

      scalar_type scalar_one() const noexcept {
        return _underlying_mat.scalar_one();
      }

      scalar_type scalar_zero() const noexcept {
        return _underlying_mat.scalar_zero();
      }

      ////////////////////////////////////////////////////////////////////////
      // ProjMaxPlusMat - Constructors + destructor - public
      ////////////////////////////////////////////////////////////////////////

      ProjMaxPlusMat() : _is_normalized(false), _underlying_mat() {}
      ProjMaxPlusMat(ProjMaxPlusMat const&)            = default;
      ProjMaxPlusMat(ProjMaxPlusMat&&)                 = default;
      ProjMaxPlusMat& operator=(ProjMaxPlusMat const&) = default;
      ProjMaxPlusMat& operator=(ProjMaxPlusMat&&)      = default;

      ProjMaxPlusMat(size_t r, size_t c)
          : _is_normalized(false), _underlying_mat(r, c) {}

      explicit ProjMaxPlusMat(std::vector<std::vector<scalar_type>> const& m)
          : _is_normalized(false), _underlying_mat(m) {
        normalize();
      }

      ProjMaxPlusMat(
          std::initializer_list<std::initializer_list<scalar_type>> const& m)
          : ProjMaxPlusMat(
                std::vector<std::vector<scalar_type>>(m.begin(), m.end())) {}

      ~ProjMaxPlusMat() = default;

      ProjMaxPlusMat identity() const {
        auto result = ProjMaxPlusMat(_underlying_mat.identity());
        return result;
      }

      static ProjMaxPlusMat identity(size_t n) {
        return ProjMaxPlusMat(T::identity(n));
      }

      ////////////////////////////////////////////////////////////////////////
      // Comparison operators
      ////////////////////////////////////////////////////////////////////////

      bool operator==(ProjMaxPlusMat const& that) const {
        normalize();
        that.normalize();
        return _underlying_mat == that._underlying_mat;
      }

      bool operator!=(ProjMaxPlusMat const& that) const {
        return !(_underlying_mat == that._underlying_mat);
      }

      bool operator<(ProjMaxPlusMat const& that) const {
        normalize();
        that.normalize();
        return _underlying_mat < that._underlying_mat;
      }

      bool operator>(ProjMaxPlusMat const& that) const {
        return that < *this;
      }

      ////////////////////////////////////////////////////////////////////////
      // Attributes
      ////////////////////////////////////////////////////////////////////////

      // The following is commented out because it can't be used safely,
      // i.e. when is the matrix normalised again? scalar_reference
      scalar_reference operator()(size_t r, size_t c) {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat(r, c);
      }

      scalar_const_reference operator()(size_t r, size_t c) const {
        normalize();
        return _underlying_mat(r, c);
      }

      size_t number_of_rows() const noexcept {
        return _underlying_mat.number_of_rows();
      }

      size_t number_of_cols() const noexcept {
        return _underlying_mat.number_of_cols();
      }

      size_t hash_value() const {
        normalize();
        return Hash<T>()(_underlying_mat);
      }

      ////////////////////////////////////////////////////////////////////////
      // Arithmetic operators - in-place
      ////////////////////////////////////////////////////////////////////////

      void product_inplace_no_checks(ProjMaxPlusMat const& A,
                                     ProjMaxPlusMat const& B) {
        _underlying_mat.product_inplace_no_checks(A._underlying_mat,
                                                  B._underlying_mat);
        normalize(true);  // force normalize
      }

      void operator+=(ProjMaxPlusMat const& that) {
        _underlying_mat += that._underlying_mat;
        normalize(true);  // force normalize
      }

      void operator*=(scalar_type a) {
        _underlying_mat *= a;
        normalize(true);  // force normalize
      }

      ////////////////////////////////////////////////////////////////////////
      // Arithmetic operators - not in-place
      ////////////////////////////////////////////////////////////////////////

      ProjMaxPlusMat operator+(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(_underlying_mat + that._underlying_mat);
      }

      ProjMaxPlusMat operator*(ProjMaxPlusMat const& that) const {
        return ProjMaxPlusMat(_underlying_mat * that._underlying_mat);
      }

      ////////////////////////////////////////////////////////////////////////
      // Iterators
      ////////////////////////////////////////////////////////////////////////

      // The following should probably be commented out because I can't
      // currently think how to ensure that the matrix is normalised if it's
      // changed this way.

      iterator begin() noexcept {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat.begin();
      }

      iterator end() noexcept {
        // to ensure the returned value is normalised
        normalize();
        // to ensure that the matrix is renormalised if the returned scalar is
        // assigned.
        _is_normalized = false;
        return _underlying_mat.end();
      }

      const_iterator begin() const noexcept {
        normalize();
        return _underlying_mat.begin();
      }

      const_iterator end() const noexcept {
        normalize();
        return _underlying_mat.end();
      }

      const_iterator cbegin() const noexcept {
        normalize();
        return _underlying_mat.cbegin();
      }

      const_iterator cend() const noexcept {
        normalize();
        return _underlying_mat.cend();
      }

      ////////////////////////////////////////////////////////////////////////
      // Modifiers
      ////////////////////////////////////////////////////////////////////////

      void swap(ProjMaxPlusMat& that) noexcept {
        std::swap(_underlying_mat, that._underlying_mat);
      }

      void transpose() noexcept {
        _underlying_mat.transpose();
      }

      ////////////////////////////////////////////////////////////////////////
      // Rows
      ////////////////////////////////////////////////////////////////////////

      RowView row(size_t i) const {
        normalize();
        return _underlying_mat.row(i);
      }

      template <typename C>
      void rows(C& x) const {
        normalize();
        return _underlying_mat.rows(x);
      }

      ////////////////////////////////////////////////////////////////////////
      // Friend functions
      ////////////////////////////////////////////////////////////////////////

      friend std::ostream& operator<<(std::ostream&         os,
                                      ProjMaxPlusMat const& x) {
        x.normalize();
        os << detail::to_string(x._underlying_mat);
        return os;
      }

      T const& underlying_matrix() const noexcept {
        normalize();
        return _underlying_mat;
      }

     private:
      explicit ProjMaxPlusMat(T&& mat)
          : _is_normalized(false), _underlying_mat(std::move(mat)) {
        normalize();
      }

      void normalize(bool force = false) const {
        if ((_is_normalized && !force)
            || (_underlying_mat.number_of_rows() == 0)
            || (_underlying_mat.number_of_cols() == 0)) {
          _is_normalized = true;
          return;
        }
        scalar_type const n = *std::max_element(_underlying_mat.cbegin(),
                                                _underlying_mat.cend());
        std::for_each(_underlying_mat.begin(),
                      _underlying_mat.end(),
                      [&n](scalar_type& s) {
                        if (s != NEGATIVE_INFINITY) {
                          s -= n;
                        }
                      });
        _is_normalized = true;
      }

      mutable bool _is_normalized;
      mutable T    _underlying_mat;
    };
  }  // namespace detail

  //! \defgroup projmaxplus_group Projective max-plus matrices
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This page describes the functionality for \f$n \times n\f$ projective
  //! matrices over the max-plus semiring. Projective max-plus matrices belong
  //! to the quotient of the monoid of all max-plus matrices by the congruence
  //! where two matrices are related if they differ by a scalar multiple; see
  //! MaxPlusMat.
  //!
  //! There are two types of such matrices those whose dimension is known at
  //! compile-time, and those where it is not.  Both types can be accessed via
  //! the alias template \ref ProjMaxPlusMat: if `R` or `C` has value
  //! `0`, then the dimensions can be set at run time, otherwise `R` and `C` are
  //! the dimensions.  The default value of `R` is `0`, `C` is `R`, and `Scalar`
  //! is `int`.
  //!
  //! Matrices in both these classes are modified when constructed to be in a
  //! normal form which is obtained by subtracting the maximum finite entry in
  //! the matrix from the every finite entry.
  //!
  //! The alias \ref ProjMaxPlusMat is neither StaticMatrix nor DynamicMatrix,
  //! but has the same interface as each of these types. Every instance of \ref
  //! ProjMaxPlusMat wraps a \ref MaxPlusMat.
  //!
  //! \note
  //! The types RowView and Row are the same as those
  //! in the wrapped matrix. This means that a Row object for a
  //! projective max-plus matrix is never normalised, because if they were
  //! they would be normalised according to their entries, and this might not
  //! correspond to the normalised entries of the matrix.
  //!
  //! Please refer to the documentation of these class templates for more
  //! details. The only substantial difference in the interface of StaticMatrix
  //! and DynamicMatrix is that the former can be default constructed and the
  //! latter should be constructed using the dimensions.
  //!
  //! \par Example
  //! \code
  //!    ProjMaxPlusMat<3> m;       // default construct an uninitialized 3 x 3
  //!    static matrix ProjMaxPlusMat<>  m(4, 4); // construct an uninitialized
  //!    4 x 4 dynamic matrix
  //! \endcode
  // TODO unscramble

  //! \ingroup projmaxplus_group
  //!
  //! \brief Alias for static projective max-plus matrices with compile-time
  //! arithmetic and dimensions.
  //!
  //! Alias for static projective max-plus matrices whose arithmetic and
  //! dimensions are defined at compile-time.
  //!
  //! \tparam R the number of rows.
  //! \tparam C the number of columns.
  //! \tparam Scalar the type of the entries in the matrix.
  template <size_t R, size_t C, typename Scalar>
  using StaticProjMaxPlusMat
      = detail::ProjMaxPlusMat<StaticMaxPlusMat<R, C, Scalar>>;

  //! \ingroup projmaxplus_group
  //!
  //! \brief Alias for dynamic projective max-plus matrices with run-time
  //! dimensions.
  //!
  //! Alias for the type of dynamic projective max-plus matrices where the
  //! dimensions of the matrices can be defined at run time.
  //!
  //! \tparam Scalar the type of the entries in the matrix.
  template <typename Scalar>
  using DynamicProjMaxPlusMat
      = detail::ProjMaxPlusMat<DynamicMaxPlusMat<Scalar>>;

  //! \ingroup projmaxplus_group
  //! \brief Alias template for projective max-plus matrices.
  //!
  //! Alias template for projective max-plus matrices.
  //!
  //! \tparam R the number of rows.  A value of `0` indicates that the value
  //! will be set at run time (default: `0`).
  //!
  //! \tparam C the number of columns.  A value of `0` indicates that the value
  //! will be set at run time (default: `R`).
  //!
  //! \tparam Scalar The type of the entries in the matrix (default: `int`).
  template <size_t R = 0, size_t C = R, typename Scalar = int>
  using ProjMaxPlusMat = std::conditional_t<R == 0 || C == 0,
                                            DynamicProjMaxPlusMat<Scalar>,
                                            StaticProjMaxPlusMat<R, C, Scalar>>;

  namespace detail {
    template <typename T>
    struct IsProjMaxPlusMatHelper : std::false_type {};

    template <size_t R, size_t C, typename Scalar>
    struct IsProjMaxPlusMatHelper<StaticProjMaxPlusMat<R, C, Scalar>>
        : std::true_type {};

    template <typename Scalar>
    struct IsProjMaxPlusMatHelper<DynamicProjMaxPlusMat<Scalar>>
        : std::true_type {};
  }  // namespace detail

  //! \ingroup projmaxplus_group
  //!
  //! \brief Helper to check if a type is \ref ProjMaxPlusMat.
  //!
  //! This variable has value `true` if the template parameter `T` is the same
  //! as \ref ProjMaxPlusMat for some values of `R`, `C`, and `Scalar`; and
  //! `false` if it is not.
  template <typename T>
  static constexpr bool IsProjMaxPlusMat
      = detail::IsProjMaxPlusMatHelper<T>::value;

  // \ingroup projmaxplus_group
  //! TODO(0)
  template <typename Mat>
  auto validate(Mat const& m) -> std::enable_if_t<IsProjMaxPlusMat<Mat>> {
    validate(m.underlying_matrix());
  }

  //! \ingroup matrix_group
  //!
  //! \brief Namespace for helper functions for matrices.
  //!
  //! Defined in ``matrix.hpp``.
  //!
  //! This namespace contains various helper functions for the various matrix
  //! classes in ``libsemigroups``. These functions could have been member
  //! functions of the matrix classes but they only use public member functions,
  //! and so they are declared as free functions instead.
  namespace matrix {

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - pow
    ////////////////////////////////////////////////////////////////////////

    // TODO(later) version that changes x in-place
    //! TODO(0)
    template <typename Mat>
    Mat pow(Mat const& x, typename Mat::scalar_type e) {
      using scalar_type = typename Mat::scalar_type;

      if (std::is_signed<scalar_type>::value && e < 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "negative exponent, expected value >= 0, found {}", e);
      } else if (x.number_of_cols() != x.number_of_rows()) {
        LIBSEMIGROUPS_EXCEPTION("expected a square matrix, found {}x{}",
                                x.number_of_rows(),
                                x.number_of_cols());
      }

      if (e == 0) {
        return x.identity();
      }

      auto y = Mat(x);
      if (e == 1) {
        return y;
      }
      auto z = (e % 2 == 0 ? x.identity() : y);

      Mat tmp(x.number_of_rows(), x.number_of_cols());
      while (e > 1) {
        tmp.product_inplace_no_checks(y, y);
        std::swap(y, tmp);
        e /= 2;
        if (e % 2 == 1) {
          tmp.product_inplace_no_checks(z, y);
          std::swap(z, tmp);
        }
      }
      return z;
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - bitset_rows
    ////////////////////////////////////////////////////////////////////////

    // The main function
    //! TODO(0)
    template <typename Mat, size_t R, size_t C, typename Container>
    void bitset_rows(Container&&                          views,
                     detail::StaticVector1<BitSet<C>, R>& result) {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      // std::vector<bool> is used as value_type in the benchmarks
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(std::is_same<value_type, RowView>::value
                        || std::is_same<value_type, std::vector<bool>>::value,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(views.size() <= R);
      LIBSEMIGROUPS_ASSERT(views.empty() || views[0].size() <= C);
      for (auto const& v : views) {
        result.emplace_back(v.cbegin(), v.cend());
      }
    }

    // Helper
    //! TODO(0)
    template <typename Mat, size_t R, size_t C, typename Container>
    auto bitset_rows(Container&& views) {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(std::is_same<value_type, RowView>::value
                        || std::is_same<value_type, std::vector<bool>>::value,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(views.size() <= R);
      LIBSEMIGROUPS_ASSERT(views.empty() || views[0].size() <= C);
      detail::StaticVector1<BitSet<C>, R> result;
      bitset_rows<Mat>(std::forward<Container>(views), result);
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - bitset_row_basis
    ////////////////////////////////////////////////////////////////////////

    // This works with std::vector and StaticVector1, with value_type equal
    // to std::bitset and BitSet.
    //! TODO(0)
    template <typename Mat, typename Container>
    void bitset_row_basis(Container&& rows, std::decay_t<Container>& result) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(rows.size() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(rows.empty()
                           || rows[0].size() <= BitSet<1>::max_size());

      std::sort(rows.begin(), rows.end(), detail::LessBitSet());
      // Remove duplicates
      rows.erase(std::unique(rows.begin(), rows.end()), rows.end());
      for (size_t i = 0; i < rows.size(); ++i) {
        value_type cup;
        cup.reset();
        for (size_t j = 0; j < i; ++j) {
          if ((rows[i] & rows[j]) == rows[j]) {
            cup |= rows[j];
          }
        }
        for (size_t j = i + 1; j < rows.size(); ++j) {
          if ((rows[i] & rows[j]) == rows[j]) {
            cup |= rows[j];
          }
        }
        if (cup != rows[i]) {
          result.push_back(std::move(rows[i]));
        }
      }
    }

    //! TODO(0)
    template <typename Mat, typename Container>
    std::decay_t<Container> bitset_row_basis(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(rows.size() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(rows.empty()
                           || rows[0].size() <= BitSet<1>::max_size());
      std::decay_t<Container> result;
      bitset_row_basis<Mat>(std::forward<Container>(rows), result);
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - rows
    ////////////////////////////////////////////////////////////////////////

    //! TODO(0)
    template <typename Mat, typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    std::vector<typename Mat::RowView> rows(Mat const& x) {
      std::vector<typename Mat::RowView> container;
      x.rows(container);
      return container;
    }

    //! TODO(0)
    template <typename Mat, typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    detail::StaticVector1<typename Mat::RowView, Mat::nr_rows>
    rows(Mat const& x) {
      detail::StaticVector1<typename Mat::RowView, Mat::nr_rows> container;
      x.rows(container);
      return container;
    }

    // Helper
    //! TODO(0)
    template <typename Mat, size_t R, size_t C>
    void bitset_rows(Mat const&                           x,
                     detail::StaticVector1<BitSet<C>, R>& result) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(R <= BitSet<1>::max_size(),
                    "R must be at most BitSet<1>::max_size()!");
      static_assert(C <= BitSet<1>::max_size(),
                    "C must be at most BitSet<1>::max_size()!");
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= C);
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= R);
      bitset_rows<Mat>(std::move(rows(x)), result);
    }

    // Helper
    //! TODO(0)
    template <typename Mat>
    auto bitset_rows(Mat const& x) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      size_t const M = detail::BitSetCapacity<Mat>::value;
      return bitset_rows<Mat, M, M>(std::move(rows(x)));
    }

    //! TODO(0)
    template <typename Mat, size_t M = detail::BitSetCapacity<Mat>::value>
    detail::StaticVector1<BitSet<M>, M> bitset_row_basis(Mat const& x) {
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      detail::StaticVector1<BitSet<M>, M> result;
      bitset_row_basis<Mat>(std::move(bitset_rows(x)), result);
      return result;
    }

    //! TODO(0)
    template <typename Mat, typename Container>
    void bitset_row_basis(Mat const& x, Container& result) {
      using value_type = typename Container::value_type;
      static_assert(IsBMat<Mat>, "IsBMat<Mat> must be true!");
      static_assert(IsBitSet<value_type> || IsStdBitSet<value_type>,
                    "Container::value_type must be BitSet or std::bitset");
      LIBSEMIGROUPS_ASSERT(x.number_of_rows() <= BitSet<1>::max_size());
      LIBSEMIGROUPS_ASSERT(x.number_of_cols() <= BitSet<1>::max_size());
      bitset_row_basis<Mat>(std::move(bitset_rows(x)), result);
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - MaxPlusTruncMat
    ////////////////////////////////////////////////////////////////////////

    //! TODO(0)
    template <typename Mat, typename Container>
    auto row_basis(Container&& views, std::decay_t<Container>& result)
        -> std::enable_if_t<IsMaxPlusTruncMat<Mat>> {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(std::is_same<value_type, typename Mat::RowView>::value,
                    "Container::value_type must be Mat::RowView");
      using scalar_type = typename Mat::scalar_type;
      using Row         = typename Mat::Row;

      if (views.empty()) {
        return;
      }

      LIBSEMIGROUPS_ASSERT(result.empty());

      std::sort(views.begin(), views.end());
      Row tmp1(views[0]);

      for (size_t r1 = 0; r1 < views.size(); ++r1) {
        if (r1 == 0 || views[r1] != views[r1 - 1]) {
          std::fill(tmp1.begin(), tmp1.end(), tmp1.scalar_zero());
          for (size_t r2 = 0; r2 < r1; ++r2) {
            scalar_type max_scalar = matrix_threshold(tmp1);
            for (size_t c = 0; c < tmp1.number_of_cols(); ++c) {
              if (views[r2][c] == tmp1.scalar_zero()) {
                continue;
              }
              if (views[r1][c] >= views[r2][c]) {
                if (views[r1][c] != matrix_threshold(tmp1)) {
                  max_scalar
                      = std::min(max_scalar, views[r1][c] - views[r2][c]);
                }
              } else {
                max_scalar = tmp1.scalar_zero();
                break;
              }
            }
            if (max_scalar != tmp1.scalar_zero()) {
              tmp1 += views[r2] * max_scalar;
            }
          }
          if (tmp1 != views[r1]) {
            result.push_back(views[r1]);
          }
        }
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - BMat
    ////////////////////////////////////////////////////////////////////////

    // This version of row_basis for BMat's is for used for compatibility
    // with the MatrixCommon framework, i.e. so that BMat's exhibit the same
    // interface/behaviour as other matrices.
    //
    // This version takes a container of row views of BMat, converts it to a
    // container of BitSets, computes the row basis using the BitSets, then
    // selects those row views in views that belong to the computed basis.
    //! TODO(0)
    template <typename Mat, typename Container>
    auto row_basis(Container&& views, std::decay_t<Container>& result)
        -> std::enable_if_t<IsBMat<Mat>> {
      using RowView    = typename Mat::RowView;
      using value_type = typename std::decay_t<Container>::value_type;
      // std::vector<bool> is used as value_type in the benchmarks
      static_assert(std::is_same<value_type, RowView>::value
                        || std::is_same<value_type, std::vector<bool>>::value,
                    "Container::value_type must equal Mat::RowView or "
                    "std::vector<bool>!!");

      if (views.empty()) {
        return;
      }

      // Convert RowViews to BitSets
      size_t const M  = detail::BitSetCapacity<Mat>::value;
      auto         br = bitset_rows<Mat, M, M>(std::forward<Container>(views));
      using bitset_type = typename decltype(br)::value_type;

      // Map for converting bitsets back to row views
      std::unordered_map<bitset_type, size_t> lookup;
      LIBSEMIGROUPS_ASSERT(br.size() == views.size());
      for (size_t i = 0; i < br.size(); ++i) {
        lookup.insert({br[i], i});
      }

      // Compute rowbasis using bitsets + convert back to rowviews
      for (auto const& bs : bitset_row_basis<Mat>(br)) {
        auto it = lookup.find(bs);
        LIBSEMIGROUPS_ASSERT(it != lookup.end());
        result.push_back(views[it->second]);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_basis - generic helpers
    ////////////////////////////////////////////////////////////////////////

    //! TODO(0)
    // Row basis of rowspace of matrix <x> appended to <result>
    template <typename Mat,
              typename Container,
              typename = std::enable_if_t<IsMatrix<Mat>>>
    void row_basis(Mat const& x, Container& result) {
      row_basis<Mat>(std::move(rows(x)), result);
    }

    //! TODO(0)
    // Row basis of rowspace of matrix <x>
    template <typename Mat, typename = std::enable_if_t<IsDynamicMatrix<Mat>>>
    std::vector<typename Mat::RowView> row_basis(Mat const& x) {
      std::vector<typename Mat::RowView> container;
      row_basis(x, container);
      return container;
    }

    //! TODO(0)
    // Row basis of rowspace of matrix <x>
    template <typename Mat, typename = std::enable_if_t<IsStaticMatrix<Mat>>>
    detail::StaticVector1<typename Mat::RowView, Mat::nr_rows>
    row_basis(Mat const& x) {
      detail::StaticVector1<typename Mat::RowView, Mat::nr_rows> container;
      row_basis(x, container);
      return container;
    }

    //! TODO(0)
    // Row basis of rowspace of space spanned by <rows>
    template <typename Mat, typename Container>
    std::decay_t<Container> row_basis(Container&& rows) {
      using value_type = typename std::decay_t<Container>::value_type;
      static_assert(IsMatrix<Mat>, "IsMatrix<Mat> must be true!");
      static_assert(std::is_same<value_type, typename Mat::RowView>::value,
                    "Container::value_type must be Mat::RowView");

      std::decay_t<Container> result;
      row_basis<Mat>(std::forward<Container>(rows), result);
      return result;
    }

    ////////////////////////////////////////////////////////////////////////
    // Matrix helpers - row_space_size
    ////////////////////////////////////////////////////////////////////////

    //! TODO(0)
    template <typename Mat, typename = std::enable_if_t<IsBMat<Mat>>>
    size_t row_space_size(Mat const& x) {
      size_t const M                 = detail::BitSetCapacity<Mat>::value;
      auto         bitset_row_basis_ = bitset_row_basis<Mat>(
          std::move(bitset_rows<Mat, M, M>(std::move(rows(x)))));

      std::unordered_set<BitSet<M>> st;
      st.insert(bitset_row_basis_.cbegin(), bitset_row_basis_.cend());
      std::vector<BitSet<M>> orb(bitset_row_basis_.cbegin(),
                                 bitset_row_basis_.cend());
      for (size_t i = 0; i < orb.size(); ++i) {
        for (auto& row : bitset_row_basis_) {
          auto cup = orb[i];
          for (size_t j = 0; j < x.number_of_rows(); ++j) {
            cup.set(j, cup[j] || row[j]);
          }
          if (st.insert(cup).second) {
            orb.push_back(std::move(cup));
          }
        }
      }
      return orb.size();
    }

  }  // namespace matrix

  template <typename Mat,
            typename
            = std::enable_if_t<IsMatrix<Mat> && !IsMatWithSemiring<Mat>>>
  Mat to_matrix(std::initializer_list<
                std::initializer_list<typename Mat::scalar_type>> const& il) {
    detail::throw_if_any_row_wrong_size(il);
    Mat m(il);
    validate(m);
    return m;
  }

  template <typename Mat,
            typename
            = std::enable_if_t<IsMatrix<Mat> && !IsMatWithSemiring<Mat>>>
  Mat to_matrix(std::initializer_list<typename Mat::scalar_type> const& il) {
    Mat m(il);
    validate(m);
    return m;
  }

  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  Mat to_matrix(
      Semiring const* sr,  // TODO to reference
      std::initializer_list<
          std::initializer_list<typename Mat::scalar_type>> const& il) {
    Mat m(sr, il);
    validate(m);
    return m;
  }

  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  Mat to_matrix(Semiring const*                                            sr,
                std::vector<std::vector<typename Mat::scalar_type>> const& v) {
    Mat m(sr, v);
    validate(m);
    return m;
  }

  template <typename Mat,
            typename Semiring,
            typename = std::enable_if_t<IsMatrix<Mat>>>
  Mat to_matrix(Semiring const*                                         sr,
                std::initializer_list<typename Mat::scalar_type> const& il) {
    Mat m(sr, il);
    validate(m);
    return m;
  }

  template <size_t R, size_t C, typename Scalar>
  ProjMaxPlusMat<R, C, Scalar>
  to_matrix(std::initializer_list<std::initializer_list<Scalar>> const& il) {
    return ProjMaxPlusMat<R, C, Scalar>(
        to_matrix<ProjMaxPlusMat<R, C, Scalar>::underlying_matrix_type>(il));
  }

  ////////////////////////////////////////////////////////////////////////
  // Printing etc...
  ////////////////////////////////////////////////////////////////////////

  //! \ingroup matrix_group
  //! TODO(0)
  template <typename S, typename T>
  std::ostringstream& operator<<(std::ostringstream&                os,
                                 detail::RowViewCommon<S, T> const& x) {
    os << "{";
    for (auto it = x.cbegin(); it != x.cend(); ++it) {
      os << *it;
      if (it != x.cend() - 1) {
        os << ", ";
      }
    }
    os << "}";
    return os;
  }

  //! \ingroup matrix_group
  //! TODO(0)
  template <typename T>
  auto operator<<(std::ostringstream& os, T const& x)
      -> std::enable_if_t<IsMatrix<T>, std::ostringstream&> {
    size_t n = 0;
    if (x.number_of_rows() != 1) {
      os << "{";
    }
    for (auto&& r : matrix::rows(x)) {
      os << r;
      if (n != x.number_of_rows() - 1) {
        os << ", ";
      }
      n++;
    }
    if (x.number_of_rows() != 1) {
      os << "}";
    }
    return os;
  }

  ////////////////////////////////////////////////////////////////////////
  // Adapters
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  struct Complexity<T, std::enable_if_t<IsMatrix<T>>> {
    constexpr size_t operator()(T const& x) const noexcept {
      return x.number_of_rows() * x.number_of_rows() * x.number_of_rows();
    }
  };

  template <typename T>
  struct Degree<T, std::enable_if_t<IsMatrix<T>>> {
    constexpr size_t operator()(T const& x) const noexcept {
      return x.number_of_rows();
    }
  };

  template <typename T>
  struct Hash<T, std::enable_if_t<IsMatrix<T>>> {
    constexpr size_t operator()(T const& x) const {
      return x.hash_value();
    }
  };

  template <typename T>
  struct IncreaseDegree<T, std::enable_if_t<IsMatrix<T>>> {
    constexpr void operator()(T&, size_t) const noexcept {
      // static_assert(false, "Cannot increase degree for Matrix");
      LIBSEMIGROUPS_ASSERT(false);
    }
  };

  template <typename T>
  struct One<T, std::enable_if_t<IsMatrix<T>>> {
    inline T operator()(T const& x) const {
      return x.identity();
    }
  };

  template <typename T>
  struct Product<T, std::enable_if_t<IsMatrix<T>>> {
    inline void operator()(T& xy, T const& x, T const& y, size_t = 0) const {
      xy.product_inplace_no_checks(x, y);
    }
  };
}  // namespace libsemigroups

namespace std {
  template <size_t N,
            typename Mat,
            std::enable_if_t<libsemigroups::IsMatrix<Mat>>>
  inline void swap(Mat& x, Mat& y) noexcept {
    x.swap(y);
  }
}  // namespace std

#endif  // LIBSEMIGROUPS_MATRIX_HPP_
