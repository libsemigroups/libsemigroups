//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Finn Smith
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

// This file contains the declaration of the TwistedBipartition class.

#ifndef LIBSEMIGROUPS_TWISTED_BIPART_HPP_
#define LIBSEMIGROUPS_TWISTED_BIPART_HPP_

// TODO(2)
// * benchmarks
// * use Duf/Suf where possible (later?)
// * template like transformations/pperms etc (later?)

#include <algorithm>         // for max
#include <cstddef>           // for size_t
#include <cstdint>           // for uint32_t, int32_t
#include <cstdlib>           // for abs
#include <initializer_list>  // for initializer_list
#include <string_view>       // for string_view
#include <type_traits>       // for decay_t, false_type, is_signed, true_type
#include <unordered_set>     // for unordered_set
#include <vector>            // for vector

#include "adapters.hpp"   // for Hash
#include "bipart.hpp"     // for Bipartition
#include "constants.hpp"  // for UNDEFINED, POSITIVE_INFINITY
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "types.hpp"      // for enable_if_is_same

#include "detail/fmt.hpp"

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // TwistedBipartitions
  ////////////////////////////////////////////////////////////////////////

  namespace bipartition {

    // TODO: combine this with multiplication to avoid repeated work?
    //! \brief Returns the number of floating components in the product
    //! \f$ab\f$ of two bipartitions \f$a\f$ and \f$b\f$.
    //!
    //! \param a a bipartition.
    //! \param b a bipartition.
    //!
    //! \returns The number of floating components in the product \f$ab\f$.
    //!
    //! \exceptions
    //! \no_except
    [[nodiscard]] size_t number_floating_components(Bipartition const& a,
                                                    Bipartition const& b);

  }  // namespace bipartition

  //! \brief Class for representing twisted bipartitions.
  //!
  //! A twisted bipartition is a bipartition together with a threshold and a
  //! number of floating components. The product of two twisted bipartitions is
  //! defined only when they have the same threshold, and has the same threshold
  //! as the two factors. The underlying bipartition is the product of the
  //! underlying bipartitions of the factors, and the number of floating
  //! components is the sum of the number of floating components in the two
  //! factors and the number of floating components in the product of the
  //! underlying bipartitions of the factors.
  //!
  //! Any twisted bipartition with a higher number of floating components
  //! than its threshold is considered to be equal to zero, and the underlying
  //! bipartition of such a zero is undefined other than
  //! having the correct degree.
  class TwistedBipartition {
   public:
    //! \brief Default constructor.
    //!
    //! Constructs an uninitialized TwistedBipartition with default values.
    TwistedBipartition()
        : _bipartition(), _threshold(0), _floating_components(0) {}

    //! \brief Constructor with explicit values.
    //!
    //! Constructs a TwistedBipartition with the given Bipartition, threshold,
    //! and floating components.
    //!
    //! \param bipartition the underlying Bipartition.
    //! \param threshold the threshold value.
    //! \param floating_components the number of floating components.
    TwistedBipartition(Bipartition bipartition,
                       size_t      threshold,
                       size_t      floating_components)
        : _bipartition(bipartition),
          _threshold(threshold),
          _floating_components(floating_components) {}

    //! \brief Copy constructor.
    TwistedBipartition(TwistedBipartition const& other) = default;

    //! \brief Move constructor.
    TwistedBipartition(TwistedBipartition&& other) noexcept = default;

    //! \brief Copy assignment operator.
    TwistedBipartition& operator=(TwistedBipartition const& other) = default;

    //! \brief Move assignment operator.
    TwistedBipartition& operator=(TwistedBipartition&& other) noexcept
        = default;

    //! \brief Destructor.
    ~TwistedBipartition() = default;

    //! \brief Compare TwistedBipartitions for equality.
    [[nodiscard]] bool operator==(TwistedBipartition const& other) const;

    //! \brief Compare TwistedBipartitions for inequality.
    [[nodiscard]] bool operator!=(TwistedBipartition const& other) const;

    //! \brief Compare TwistedBipartitions for less.
    [[nodiscard]] bool operator<(TwistedBipartition const& other) const;

    //! \brief Return the hash value of the TwistedBipartition.
    [[nodiscard]] size_t hash_value() const;

    //! \brief Return the degree of the underlying Bipartition.
    [[nodiscard]] size_t degree() const noexcept {
      return _bipartition.degree();
    }

    //! \brief Return the rank of the underlying Bipartition.
    [[nodiscard]] size_t rank() const {
      return _bipartition.rank();
    }

    //! \brief Modify the current TwistedBipartition in-place to contain the
    //! product of two TwistedBipartitions.
    void product_inplace_no_checks(TwistedBipartition const& x,
                                   TwistedBipartition const& y,
                                   size_t                    thread_id = 0);

    //! \brief Return the underlying Bipartition.
    //!
    //! Warning: if the TwistedBipartition is zero, the returned
    //! Bipartition is undefined other than having the correct degree.
    [[nodiscard]] Bipartition const& bipartition() const noexcept {
      return _bipartition;
    }

    //! \brief Return the threshold value.
    [[nodiscard]] size_t threshold() const noexcept {
      return _threshold;
    }

    //! \brief Return the number of floating components.
    [[nodiscard]] size_t floating_components() const noexcept {
      return _floating_components;
    }

    [[nodiscard]] bool is_zero() const noexcept {
      return floating_components() > threshold();
    }

   private:
    Bipartition _bipartition;
    size_t      _threshold;
    size_t      _floating_components;
  };

  //! \brief Multiply two TwistedBipartitions.
  [[nodiscard]] TwistedBipartition operator*(TwistedBipartition const& x,
                                             TwistedBipartition const& y);

  //! \brief Compare TwistedBipartitions.
  [[nodiscard]] inline bool operator<=(TwistedBipartition const& x,
                                       TwistedBipartition const& y) {
    return x < y || x == y;
  }

  //! \brief Compare TwistedBipartitions.
  [[nodiscard]] inline bool operator>(TwistedBipartition const& x,
                                      TwistedBipartition const& y) {
    return y < x;
  }

  //! \brief Compare TwistedBipartitions.
  [[nodiscard]] inline bool operator>=(TwistedBipartition const& x,
                                       TwistedBipartition const& y) {
    return y <= x;
  }

  template <typename Return, typename Container>
  [[nodiscard]] enable_if_is_same<Return, TwistedBipartition>
  make(Container const& cont, size_t threshold, size_t floating_components) {
    detail::throw_if_bad_args<Bipartition>(cont);
    TwistedBipartition result(cont, threshold, floating_components);
    bipartition::throw_if_invalid(result.bipartition());
    return result;
  }

  //! \ingroup make_bipart_group
  //!
  //! \brief Check arguments, construct a twisted bipartition, and check it.
  //!
  //! \copydoc make(Container const&)
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, TwistedBipartition>
  make(std::initializer_list<uint32_t> const& cont,
       size_t                                 threshold,
       size_t                                 floating_components) {
    return make<TwistedBipartition, std::initializer_list<uint32_t>>(
        cont, threshold, floating_components);
  }

  //! \ingroup make_bipart_group
  //!
  //! \copydoc make(Container const&)
  template <typename Return>
  [[nodiscard]] enable_if_is_same<Return, TwistedBipartition>
  make(std::initializer_list<std::vector<int32_t>> const& cont,
       size_t                                             threshold,
       size_t                                             floating_components) {
    return make<TwistedBipartition,
                std::initializer_list<std::vector<int32_t>>>(
        cont, threshold, floating_components);
  }

  [[nodiscard]] std::string to_human_readable_repr(TwistedBipartition const& x,
                                                   std::string_view braces
                                                   = "{}",
                                                   size_t max_width = 72);

  ////////////////////////////////////////////////////////////////////////
  // Adapters - TwistedBipartitions
  ////////////////////////////////////////////////////////////////////////

  //! Returns the approximate time complexity of multiplication.
  //!
  //! In the case of a TwistedBipartition of degree *n* the value *n ^ 2* is
  //! returned.
  template <>
  struct Complexity<TwistedBipartition> {
    //! Call operator.
    //!
    //! \param x a const reference to a TwistedBipartition.
    //!
    //! \returns
    //! A value of type `size_t` representing the complexity of multiplying the
    //! parameter \p x by another TwistedBipartition of the same degree.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t
    operator()(TwistedBipartition const& x) const noexcept {
      return x.degree() * x.degree();
    }
  };

  //! Returns the degree of a TwistedBipartition.
  template <>
  struct Degree<TwistedBipartition> {
    [[nodiscard]] size_t
    operator()(TwistedBipartition const& x) const noexcept {
      return x.degree();
    }
  };

  //! Returns the hash value of a TwistedBipartition.
  template <>
  struct Hash<TwistedBipartition> {
    [[nodiscard]] size_t operator()(TwistedBipartition const& x) const {
      return x.hash_value();
    }
  };

  //! Returns the identity TwistedBipartition of a given degree.
  template <>
  struct One<TwistedBipartition> {
    [[nodiscard]] TwistedBipartition
    operator()(TwistedBipartition const& x) const {
      return TwistedBipartition(Bipartition::one(x.degree()), x.threshold(), 0);
    }
  };

  //! Performs the product of two TwistedBipartitions.
  template <>
  struct Product<TwistedBipartition> {
    void operator()(TwistedBipartition&       xy,
                    TwistedBipartition const& x,
                    TwistedBipartition const& y,
                    size_t                    thread_id = 0) {
      xy.product_inplace_no_checks(x, y, thread_id);
    }
  };

  //! Increases the degree of a TwistedBipartition (no-op for now).
  template <>
  struct IncreaseDegree<TwistedBipartition> {
    void operator()(TwistedBipartition&, size_t) {}
  };

  template <>
  struct LambdaValue<TwistedBipartition> {
    using type = std::pair<size_t, Blocks>;
  };

  template <>
  struct RhoValue<TwistedBipartition> {
    using type = std::pair<size_t, Blocks>;
  };

  //! The Rank of a TwistedBipartition is a size_t representing the
  //! linearisation of the lexicographic order on (-floating_components,
  //! rank of underlying bipartition). The specific value is not meaningful;
  //! you are more likely to want floating_components() and rank().
  //! This value is NOT the same as rank().
  template <>
  struct Rank<TwistedBipartition> {
    inline size_t operator()(TwistedBipartition const& x) const;
  };

  //! The lambda value of a TwistedBipartition is a pair consisting of the
  //! number of floating components and the right blocks of the underlying
  //! bipartition.
  template <>
  struct Lambda<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>& res,
                           TwistedBipartition const&  x) const noexcept;
  };

  //! The rho value of a TwistedBipartition is a pair consisting of the
  //! number of floating components and the left blocks of the underlying
  //! bipartition.
  template <>
  struct Rho<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>& res,
                           TwistedBipartition const&  x) const noexcept;
  };

  template <>
  struct ImageRightAction<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>&       res,
                           std::pair<size_t, Blocks> const& pt,
                           TwistedBipartition const&        x) const noexcept;
  };

  template <>
  struct ImageLeftAction<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>&       res,
                           std::pair<size_t, Blocks> const& pt,
                           TwistedBipartition const&        x) const noexcept;
  };

}  // namespace libsemigroups
#include "twisted-bipart.tpp"
#endif  // LIBSEMIGROUPS_TWISTED_BIPART_HPP_
