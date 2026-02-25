//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2021-2025 James D. Mitchell
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

// This file contains the declaration of the Bipartition and Blocks classes.

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

    // TODO: doc
    // TODO: combine this with multiplication to avoid repeated work?
    [[nodiscard]] size_t number_floating_components(Bipartition const& a,
                                                    Bipartition const& b);

  }  // namespace bipartition

  //! \brief Class for representing twisted bipartitions.
  //!
  //! TODO: expand doc
  class TwistedBipartition {
   public:
    //! \brief Default constructor.
    //!
    //! Constructs an uninitialized TwistedBipartition with default values.
    TwistedBipartition()
        : _bipartition(), _threshold(0), _floating_components(0) {}

    // TODO: instead of _is_one, use a flag for whether the
    // TwistedBipartition knows its threshold. Would simplify equality
    // checks and product_inplace_no_checks.

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
                       size_t      floating_components,
                       bool        is_zero = false,
                       bool        is_one  = false)
        : _bipartition(bipartition),
          _threshold(threshold),
          _floating_components(floating_components),
          _is_zero(is_zero),
          _is_one(is_one) {}

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
    [[nodiscard]] bool operator==(TwistedBipartition const& other) const {
      if (_is_zero && other._is_zero) {
        return true;
      }
      if (_is_one && other._is_one) {
        return true;
      }

      if (other._is_one && _floating_components == 0
          && _bipartition == other._bipartition) {
        return true;
      }

      if (_is_one && other._floating_components == 0
          && _bipartition == other._bipartition) {
        return true;
      }

      if (_is_zero != other._is_zero) {
        return false;
      }

      if (_is_one != other._is_one) {
        return false;
      }

      return (_bipartition == other._bipartition
              && _floating_components == other._floating_components);
    }

    //! \brief Compare TwistedBipartitions for inequality.
    [[nodiscard]] bool operator!=(TwistedBipartition const& other) const {
      return !(*this == other);
    }

    //! \brief Compare TwistedBipartitions for less.
    [[nodiscard]] bool operator<(TwistedBipartition const& other) const {
      if ((_is_zero && other._is_zero) || (_is_one && other._is_one)) {
        return false;
      }

      return _bipartition < other._bipartition
             || (_bipartition == other._bipartition
                 && _threshold < other._threshold)
             || (_bipartition == other._bipartition
                 && _threshold == other._threshold
                 && _floating_components < other._floating_components);
    }

    //! \brief Return the hash value of the TwistedBipartition.
    [[nodiscard]] size_t hash_value() const {
      if (_is_zero) {
        return 0;
      }
      return Hash<Bipartition>()(_bipartition)
             ^ std::hash<size_t>()(_floating_components);
    }

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
                                   size_t                    thread_id = 0) {
      _is_zero = false;
      _is_one  = false;
      // TODO: yikes
      _threshold = std::max(x._threshold, y._threshold);

      if (x._is_zero || y._is_zero) {
        set_is_zero(true);
        return;
      }

      if (x._is_one) {
        _bipartition         = y._bipartition;
        _threshold           = y._threshold;
        _floating_components = y._floating_components;
        _is_zero             = y._is_zero;
        _is_one              = y._is_one;
        return;
      }

      if (y._is_one) {
        _bipartition         = x._bipartition;
        _threshold           = x._threshold;
        _floating_components = x._floating_components;
        _is_zero             = x._is_zero;
        _is_one              = x._is_one;
        return;
      }

      _floating_components = x._floating_components + y._floating_components
                             + bipartition::number_floating_components(
                                 x._bipartition, y._bipartition);

      if (_floating_components > _threshold) {
        set_is_zero(true);
        return;
      }

      _bipartition.product_inplace_no_checks(
          x._bipartition, y._bipartition, thread_id);
    }

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
      return _is_zero;
    }

    [[nodiscard]] bool is_one() const noexcept {
      return _is_one;
    }

    void set_is_zero(bool is_zero) noexcept {
      _is_zero = is_zero;
      if (is_zero) {
        _is_one = false;
      }
    }

    void set_is_one(bool is_one) noexcept {
      _is_one = is_one;
      if (is_one) {
        _is_zero = false;
      }
    }

   private:
    Bipartition _bipartition;
    size_t      _threshold;
    size_t      _floating_components;
    bool        _is_zero;
    bool        _is_one;
    // TODO: hacky because adapters usually assume the degree is enough
    // information to e.g. construct the One
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
    // detail::throw_if_bad_args<Bipartition>(cont);
    TwistedBipartition result(cont, threshold, floating_components);
    // bipartition::throw_if_invalid(result);
    return result;
  }

  //! \ingroup make_bipart_group
  //!
  //! \brief Check the arguments, construct a bipartition, and check it.
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
    [[nodiscard]] size_t operator()(TwistedBipartition const&) const noexcept {
      return 0;
      // return x.degree() * x.degree();
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

  // TODO: this is horrible - should operator()(size_t degree) be
  // deprecated?
  //! Returns the identity TwistedBipartition of a given degree.
  template <>
  struct One<TwistedBipartition> {
    [[nodiscard]] TwistedBipartition
    operator()(TwistedBipartition const& x) const {
      return (*this)(x.degree());
    }

    [[nodiscard]] TwistedBipartition operator()(size_t n = 0) const {
      auto x = TwistedBipartition(Bipartition::one(n), 0, 0);
      x.set_is_one(true);
      return x;
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

  template <>
  class RankState<TwistedBipartition, void> {
   public:
    // we need to know the threshold to compute the rank of a One
    using type = size_t;

    //! Default constructor is deleted; we need to know the threshold.
    RankState() noexcept = delete;

    //! Iterator constructor; takes the first valid threshold value.
    template <typename T>
    RankState(T first, T last) {
      if (std::distance(first, last) == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a positive number of generators in the second argument");
      }

      for (auto it = first; it != last; ++it) {
        if (!(it->is_one())) {
          _threshold = it->threshold();
          return;
        }
      }
      LIBSEMIGROUPS_EXCEPTION(
          "expected at least one generator with a valid threshold value");
    }

    type const& get() const {
      return _threshold;
    }

    // Other constructors are deleted.
    //! Deleted.
    RankState(RankState const&) = default;
    //! Deleted.
    RankState(RankState&&) = delete;
    //! Deleted.
    RankState& operator=(RankState const&) = delete;
    //! Deleted.
    RankState& operator=(RankState&&) = delete;

   private:
    type _threshold;
  };

  template <>
  struct Rank<TwistedBipartition> {
    size_t operator()(RankState<TwistedBipartition> const& state,
                      TwistedBipartition const&            x) const {
      if (x.is_zero()) {
        return 0;
      }
      if (x.is_one()) {
        return (state.get() + 2) * x.degree();
      }

      return (state.get() - x.floating_components() + 1) * x.degree()
             + x.rank();
    }
  };

  template <>
  struct Lambda<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>& res,
                           TwistedBipartition const&  x) const noexcept {
      if (x.is_zero()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
        return;
      }
      if (x.is_one()) {
        res.first  = 0;
        res.second = blocks::identity_blocks(x.degree());
        return;
      }

      res.first  = x.floating_components();
      auto* ptr  = x.bipartition().right_blocks_no_checks();
      res.second = std::move(*ptr);
      delete ptr;
      // TODO modify in place instead
    }
  };

  template <>
  struct Rho<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>& res,
                           TwistedBipartition const&  x) const noexcept {
      if (x.is_zero()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
        return;
      }
      if (x.is_one()) {
        res.first  = 0;
        res.second = blocks::identity_blocks(x.degree());
        return;
      }

      res.first  = x.floating_components();
      auto* ptr  = x.bipartition().left_blocks_no_checks();
      res.second = std::move(*ptr);
      delete ptr;
      // TODO modify in place instead
    }
  };

  template <>
  struct ImageRightAction<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>&       res,
                           std::pair<size_t, Blocks> const& pt,
                           TwistedBipartition const&        x) const noexcept {
      // TODO: improve - no need to actually form the projection?

      if (pt.first == UNDEFINED || x.is_zero()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
        return;
      }

      if (x.is_one()) {
        res = pt;
        return;
      }

      Bipartition proj = bipartition::block_projection(pt.second);
      res.first
          = pt.first + x.floating_components()
            + bipartition::number_floating_components(proj, x.bipartition());
      if (res.first > x.threshold()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
      } else {
        auto* ptr  = (proj * x.bipartition()).right_blocks_no_checks();
        res.second = std::move(*ptr);
        delete ptr;
        // TODO modify in place instead
      }
    }
  };

  template <>
  struct ImageLeftAction<TwistedBipartition, std::pair<size_t, Blocks>> {
    inline void operator()(std::pair<size_t, Blocks>&       res,
                           std::pair<size_t, Blocks> const& pt,
                           TwistedBipartition const&        x) const noexcept {
      // TODO: improve as right action

      if (pt.first == UNDEFINED || x.is_zero()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
        return;
      }

      if (x.is_one()) {
        res = pt;
        return;
      }

      Bipartition proj = bipartition::block_projection(pt.second);
      res.first
          = pt.first + x.floating_components()
            + bipartition::number_floating_components(x.bipartition(), proj);
      if (res.first > x.threshold()) {
        res.first  = UNDEFINED;
        res.second = Blocks();
      } else {
        auto* ptr  = (x.bipartition() * proj).left_blocks_no_checks();
        res.second = std::move(*ptr);
        delete ptr;
        // TODO modify in place instead
      }
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TWISTED_BIPART_HPP_
