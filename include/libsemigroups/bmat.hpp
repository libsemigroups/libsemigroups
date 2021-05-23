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

#ifndef LIBSEMIGROUPS_BMAT_HPP_
#define LIBSEMIGROUPS_BMAT_HPP_

#include <cstddef>  // for size_t

#include "action.hpp"     // for RightAction
#include "adapters.hpp"   // for ImageRightAction
#include "bitset.hpp"     // for BitSet, IsBitSet
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "matrix.hpp"     // for BMat

namespace libsemigroups {
  // ////////////////////////////////////////////////////////////////////////
  // // ImageRight/LeftAction - BMat
  // ////////////////////////////////////////////////////////////////////////

  // T = StaticVector1<BitSet<N>, N> or std::vector<BitSet<N>>
  // Possibly further container when value_type is BitSet.
  template <typename Container, typename Mat>
  struct ImageRightAction<
      Mat,
      Container,
      std::enable_if_t<
          IsBMat<Mat> && IsBitSet<typename Container::value_type>>> {
    // not noexcept because BitSet<N>::apply isn'Container
    void operator()(Container& res, Container const& pt, Mat const& x) const {
      using value_type = typename Container::value_type;
      res.clear();

      for (auto const& v : pt) {
        value_type cup;
        cup.reset();
        v.apply([&x, &cup](size_t i) {
          for (size_t j = 0; j < x.number_of_rows(); ++j) {
            cup.set(j, cup[j] || x(i, j));
          }
        });
        res.push_back(std::move(cup));
      }
      auto tmp = matrix_helpers::bitset_row_basis<Mat>(res);
      std::swap(res, tmp);
    }
  };

  template <typename Container, typename Mat>
  struct ImageLeftAction<Mat, Container, std::enable_if_t<IsBMat<Mat>>> {
    void operator()(Container& res, Container const& pt, Mat const& x) const {
      const_cast<Mat*>(&x)->transpose();
      try {
        ImageRightAction<Mat, Container>()(res, pt, x);
      } catch (...) {
        const_cast<Mat*>(&x)->transpose();
        throw;
      }
      const_cast<Mat*>(&x)->transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Lambda/Rho - BMat
  ////////////////////////////////////////////////////////////////////////

  // This currently limits the use of Konieczny to matrices of dimension at
  // most 64 with the default traits class, since we cannot know the
  // dimension of the matrices at compile time, only at run time.
  template <typename Mat>
  struct LambdaValue<Mat, std::enable_if_t<IsBMat<Mat>>> {
    static constexpr size_t N = BitSet<1>::max_size();
    using type                = detail::StaticVector1<BitSet<N>, N>;
  };

  //! Specialization of the adapter RhoValue for instances of BMat.
  //! Note that the the type chosen here limits the Konieczny algorithm to
  //! BMats of degree at most 64 (or 32 on 32-bit systems).
  //!
  //! \sa RhoValue.
  template <typename Mat>
  struct RhoValue<Mat, std::enable_if_t<IsBMat<Mat>>> {
    //! For BMats, \c type is StaticVector1<BitSet<N>, N>, where \c N is the
    //! maximum width of BitSet on the system. This represents the column space
    //! basis of the BMats.
    using type = typename LambdaValue<Mat>::type;
  };

  // Benchmarks show that the following is the fastest (i.e. duplicating the
  // code from ImageRightAction, and using StaticVector1).  T =
  // StaticVector1<BitSet>, std::vector<BitSet>, StaticVector1<std::bitset>,
  // std::vector<std::bitset>
  //! Specialization of the adapter Lambda for instances of BMat and
  //! std::vector<BitSet<N>> or StaticVector1<BitSet<N>>.
  //!
  //! \sa Lambda.
  template <typename Container, typename Mat>
  struct Lambda<Mat, Container, std::enable_if_t<IsBMat<Mat>>> {
    //! Modifies \p res to contain the row space basis of \p x.
    void operator()(Container& res, Mat const& x) const {
      using value_type = typename Container::value_type;
      size_t const N   = value_type().size();
      if (x.number_of_rows() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most %llu, found %llu",
            static_cast<uint64_t>(N),
            static_cast<uint64_t>(x.number_of_rows()));
      }
      res.clear();
      for (size_t i = 0; i < x.number_of_rows(); ++i) {
        value_type cup;
        cup.reset();
        for (size_t j = 0; j < x.number_of_rows(); ++j) {
          cup.set(j, x(i, j));
        }
        res.push_back(std::move(cup));
      }
      auto tmp = matrix_helpers::bitset_row_basis<Mat>(res);
      std::swap(res, tmp);
    }
  };

  // T = StaticVector1<BitSet>, std::vector<BitSet>,
  // StaticVector1<std::bitset>, std::vector<std::bitset>
  //! Specialization of the adapter Rho for instances of Transformation and
  //! std::vector<BitSet<N>> or StaticVector1<BitSet<N>>.
  //!
  //! \sa Lambda.
  template <typename Container, typename Mat>
  struct Rho<Mat, Container, std::enable_if_t<IsBMat<Mat>>> {
    //! Modifies \p res to contain the column space basis of \p x.
    void operator()(Container& res, Mat const& x) const noexcept {
      const_cast<Mat*>(&x)->transpose();
      Lambda<Mat, Container>()(res, x);
      const_cast<Mat*>(&x)->transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Rank - BMat
  ////////////////////////////////////////////////////////////////////////

  template <size_t N, typename Mat>
  struct ImageRightAction<Mat, BitSet<N>, std::enable_if_t<IsBMat<Mat>>> {
    using result_type = BitSet<N>;
    void operator()(result_type&       res,
                    result_type const& pt,
                    Mat const&         x) const {
      static thread_local detail::StaticVector1<BitSet<N>, N> x_rows;
      x_rows.clear();
      for (size_t i = 0; i < x.number_of_rows(); ++i) {
        BitSet<N> row(0);
        for (size_t j = 0; j < x.number_of_rows(); ++j) {
          if (x(i, j)) {
            row.set(j, true);
          }
        }
        x_rows.push_back(std::move(row));
      }
      res.reset();
      pt.apply([&res](size_t i) { res |= x_rows[i]; });
    }
  };

  template <typename Mat>
  class RankState<Mat, std::enable_if_t<IsBMat<Mat>>> {
   public:
    using MaxBitSet = BitSet<BitSet<1>::max_size()>;
    using type = RightAction<Mat, MaxBitSet, ImageRightAction<Mat, MaxBitSet>>;

    RankState()                 = delete;
    RankState(RankState const&) = delete;
    RankState(RankState&&)      = delete;
    RankState& operator=(RankState const&) = delete;
    RankState& operator=(RankState&&) = delete;

    template <typename T>
    RankState(T first, T last) {
      static thread_local std::vector<MaxBitSet> seeds;
      LIBSEMIGROUPS_ASSERT(_orb.empty());
      if (std::distance(first, last) == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a positive number of generators in the second argument");
      }
      for (auto it = first; it < last; ++it) {
        _orb.add_generator(*it);
      }
      for (size_t i = 0; i < first->number_of_rows(); ++i) {
        MaxBitSet seed(0);
        seed.set(i, true);
        _orb.add_seed(seed);
      }
    }

    //! Returns the row orbit.
    type const& get() const {
      _orb.run();
      LIBSEMIGROUPS_ASSERT(_orb.finished());
      return _orb;
    }

   private:
    mutable type _orb;
  };

  //! Specialization of the adapter Rank for instances of BMat
  //!
  //! \sa Rank.
  template <typename Mat>
  struct Rank<Mat, RankState<Mat>, std::enable_if_t<IsBMat<Mat>>> {
    //! Returns the rank of \p x.
    //!
    //! The rank of a Mat may be defined as the rank of the
    //! Transformation obtained via the action of the Mat on the row
    //! orbit of the semigroup.
    size_t operator()(RankState<Mat> const& state, Mat const& x) const {
      using bitset_type = BitSet<BitSet<1>::max_size()>;
      using orb_type    = typename RankState<Mat>::type;
      static thread_local std::vector<bool>        seen;
      static thread_local std::vector<bitset_type> x_rows;
      seen.clear();
      x_rows.clear();
      orb_type const& orb = state.get();
      LIBSEMIGROUPS_ASSERT(orb.finished());
      seen.resize(orb.current_size());
      for (size_t i = 0; i < x.number_of_rows(); ++i) {
        bitset_type row(0);
        for (size_t j = 0; j < x.number_of_rows(); ++j) {
          if (x(i, j)) {
            row.set(j, true);
          }
        }
        x_rows.push_back(std::move(row));
      }
      size_t rnk = 0;
      for (size_t i = 0; i < orb.current_size(); ++i) {
        bitset_type const& row = orb[i];
        bitset_type        cup;
        cup.reset();
        row.apply([&cup](size_t j) { cup |= x_rows[j]; });
        size_t pos = orb.position(cup);
        LIBSEMIGROUPS_ASSERT(pos != UNDEFINED);
        if (!seen[pos]) {
          rnk++;
          seen[pos] = true;
        }
      }
      return rnk;
    }
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BMAT_HPP_
