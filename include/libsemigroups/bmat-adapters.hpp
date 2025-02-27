//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020-2024 James D. Mitchell
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

// TODO(0): almost all parameters and t-parameters are not documented in this
// file.

#ifndef LIBSEMIGROUPS_BMAT_ADAPTERS_HPP_
#define LIBSEMIGROUPS_BMAT_ADAPTERS_HPP_

#include <cstddef>      // for size_t
#include <iterator>     // for distance
#include <type_traits>  // for enable_if_t
#include <utility>      // for move
#include <vector>       // for vector, vector<>::ref...

#include "action.hpp"     // for RightAction
#include "adapters.hpp"   // for ImageRightAction
#include "bitset.hpp"     // for BitSet, IsBitSet
#include "debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "matrix.hpp"     // for BMat

#include "libsemigroups/detail/containers.hpp"  // for StaticVector1

namespace libsemigroups {
  //! \defgroup adapters_bmat_group Adapters for BMat
  //!
  //! This page contains the documentation of the functionality in
  //! `libsemigroups` that adapts \ref BMat types for use with the
  //! algorithms in `libsemigroups`.
  //!
  //! Further adapters for \ref BMat8 and other matrix objects are available
  //! \ref adapters_bmat8_group "here" and \ref adapters_matrix_group "here".
  //!
  //! @{
  ////////////////////////////////////////////////////////////////////////
  // ImageRight/LeftAction - BMat
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialisation of the ImageRightAction adapter for \ref BMat and
  //! containers of `BitSet`.
  //!
  //! Defined in `bmat-adapters.hpp`.
  //!
  //! Specialization of the \ref ImageRightAction adapter for \ref BMat and
  //! containers of \ref BitSet.
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true), and the `value_type` of the template type `Container` must be a
  //! bit set (`IsBitSet<typename Container::value_type>>` must be true). If
  //! not, template substitution will fail.
  //!
  //! \sa ImageRightAction.
  // T = StaticVector1<BitSet<N>, N> or std::vector<BitSet<N>>
  // Possibly further container when value_type is BitSet.
  template <typename Mat, typename Container>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct ImageRightAction<
      Mat,
      Container,
      std::enable_if_t<IsBMat<Mat> && IsBitSet<typename Container::value_type>>>
#else
  struct ImageRightAction<Mat, Container>
#endif
  {
    // TODO(now) Are pt and x the right way round in the doc?
    //! \brief Store the image of \p pt under the right action of \p x.
    //!
    //! Modifies \p res in-place to hold the image of \p pt under the right
    //! action of \p x.
    // TODO(0) parameters not documented
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
      auto tmp = matrix::bitset_row_basis<Mat>(res);
      std::swap(res, tmp);
    }
  };

  //! \brief Specialisation of the ImageLeftAction adapter for \ref BMat.
  //!
  //! Defined in `bmat-adapters.hpp`.
  //!
  //! Specialization of the ImageLeftAction adapter for \ref BMat
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa ImageLeftAction.
  //!
  // TODO(0) doc tparams
  template <typename Mat, typename Container>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct ImageLeftAction<Mat, Container, std::enable_if_t<IsBMat<Mat>>>
#else
  struct ImageLeftAction<Mat, Container>
#endif
  {
    //! \brief Store the image of \p pt under the left action of \p x.
    //!
    //! Modifies \p res in-place to hold the image of \p pt under the left
    //! action of \p x.
    // TODO(0) doc parameters
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

  //! \brief Specialisation of the LambdaValue adapter for \ref BMat.
  //!
  //! Defined in `bmat-adapters.hpp`.
  //!
  //! Specialization of the LambdaValue adapter for \ref BMat
  //!
  //! \note
  //! The the type chosen here limits the Konieczny algorithm to BMats of degree
  //! at most 64 (or 32 on 32-bit systems).
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa LambdaValue.
  // This currently limits the use of Konieczny to matrices of dimension at
  // most 64 with the default traits class, since we cannot know the
  // dimension of the matrices at compile time, only at run time.
  // TODO(0) doc tparams
  template <typename Mat>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct LambdaValue<Mat, std::enable_if_t<IsBMat<Mat>>>
#else
  struct LambdaValue<Mat>
#endif
  {
    static constexpr size_t N = BitSet<1>::max_size();

    //! \brief The type of Lambda Values.
    //!
    //! For BMats, \c type is `StaticVector1<BitSet<N>, N>`, where \c N is the
    //! maximum width of BitSet on the system. This represents the column space
    //! basis of the BMats.
    using type = detail::StaticVector1<BitSet<N>, N>;
  };

  //! \brief Specialization of the RhoValue adapter for \ref BMat.
  //!
  //! Specialization of the RhoValue adapter for \ref BMat.
  //!
  //! \note
  //! The the type chosen here limits the Konieczny algorithm to BMats of degree
  //! at most 64 (or 32 on 32-bit systems).
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa RhoValue.
  // TODO(0) doc tparams
  template <typename Mat>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct RhoValue<Mat, std::enable_if_t<IsBMat<Mat>>>
#else
  struct RhoValue<Mat>
#endif
  {
    //! \brief The type of Rho Values.
    //!
    //! For BMats, \c type is `StaticVector1<BitSet<N>, N>`, where \c N is the
    //! maximum width of \ref BitSet on the system. This represents the column
    //! space basis of a \ref BMat.
    using type = typename LambdaValue<Mat>::type;
  };

  // Benchmarks show that the following is the fastest (i.e. duplicating the
  // code from ImageRightAction, and using StaticVector1).  T =
  // StaticVector1<BitSet>, std::vector<BitSet>, StaticVector1<std::bitset>,
  // std::vector<std::bitset>

  //! \brief Specialization of the Lambda adapter for \ref BMat.
  //!
  //! Specialization of the Lambda adapter for instances of \ref BMat and
  //! `std::vector<BitSet<N>>` or `detail::StaticVector1<BitSet<N>>`.
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa Lambda.
  // TODO(0) doc tparameters
  template <typename Mat, typename Container>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct Lambda<Mat, Container, std::enable_if_t<IsBMat<Mat>>>
#else
  struct Lambda<Mat, Container>
#endif
  {
    //! \brief Modifies \p res in-place to contain the row space basis of \p x.
    //!
    //! Modifies \p res in-place to contain the row space basis of \p x.
    // TODO(0) doc parameters
    void operator()(Container& res, Mat const& x) const {
      using value_type = typename Container::value_type;
      size_t const N   = value_type().size();
      if (x.number_of_rows() > N) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected matrix of dimension at most {}, found {}",
            N,
            x.number_of_rows());
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
      auto tmp = matrix::bitset_row_basis<Mat>(res);
      std::swap(res, tmp);
    }
  };

  // T = StaticVector1<BitSet>, std::vector<BitSet>,
  // StaticVector1<std::bitset>, std::vector<std::bitset>

  //! \brief Specialization of the Rho adapter for \ref Transf,
  //! `std::vector<BitSet<N>>` and `StaticVector1<BitSet<N>>`.
  //!
  //! Specialization of the Rho adapter for instances of \ref Transf and
  //! `std::vector<BitSet<N>>` or `StaticVector1<BitSet<N>>`.
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa Rho.
  template <typename Mat, typename Container>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct Rho<Mat, Container, std::enable_if_t<IsBMat<Mat>>>
#else
  struct Rho<Mat, Container>
#endif
  {
    //! \brief Modifies \p res in-place to contain the column space basis of \p
    //! x.
    //!
    //! Modifies \p res in-place to contain the column space basis of \p x.
    void operator()(Container& res, Mat const& x) const noexcept {
      const_cast<Mat*>(&x)->transpose();
      Lambda<Mat, Container>()(res, x);
      const_cast<Mat*>(&x)->transpose();
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Rank - BMat
  ////////////////////////////////////////////////////////////////////////

  //! \brief Specialisation of the ImageRightAction adapter for \ref BMat and
  //! `BitSet`.
  //!
  //! Defined in `bmat-adapters.hpp`.
  //!
  //! Specialization of the ImageRightAction adapter for \ref BMat and
  //! `BitSet`.
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa ImageRightAction.
  template <size_t N, typename Mat>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct ImageRightAction<Mat, BitSet<N>, std::enable_if_t<IsBMat<Mat>>>
#else
  struct ImageRightAction<Mat, BitSet<N>>
#endif
  {
    //! No doc
    using result_type = BitSet<N>;

    //! \brief Store the image of \p pt under the right action of \p x.
    //!
    //! Modifies \p res in-place to hold the image of \p pt under the right
    //! action of \p x.
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

  //! \brief Specialisation of the RankState adapter for \ref BMat.
  //!
  //! Defined in `bmat-adapters.hpp`.
  //!
  //! Specialization of the RankState adapter for \ref BMat.
  //!
  //! \warning
  //! The template type `Mat` must be a \ref BMat type (\ref IsBMat<Mat> must be
  //! true). If not, template substitution will fail.
  //!
  //! \sa RankState.
  template <typename Mat>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  class RankState<Mat, std::enable_if_t<IsBMat<Mat>>>
#else
  class RankState<Mat>
#endif
  {
   public:
    //! \brief The maximum size of a bit set.
    //!
    //! The maximum size of \ref BitSet.
    using MaxBitSet = BitSet<BitSet<1>::max_size()>;

    //! \brief Type of the `RankState`
    using type = RightAction<Mat, MaxBitSet, ImageRightAction<Mat, MaxBitSet>>;

    //! Deleted.
    RankState() = delete;

    //! Deleted.
    RankState(RankState const&) = delete;

    //! Deleted.
    RankState(RankState&&) = delete;

    //! Deleted.
    RankState& operator=(RankState const&) = delete;

    //! Deleted.
    RankState& operator=(RankState&&) = delete;

    //! \brief Construct a `RankState` instance using iterators.
    //!
    //! Construct a `RankState` instance using an iterator of generators.
    //!
    //! \tparam T the iterator type of the parameters.
    //!
    //! \param first iterator pointing to the first generator.
    //! \param last iterator pointing one beyond the last generator.
    //!
    //! \throws LibsemigroupsException if the std::distance between `first`
    //! and `last` is 0.
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

    //! \brief Returns the row orbit.
    //!
    //! Returns the row orbit.
    type const& get() const {
      _orb.run();
      LIBSEMIGROUPS_ASSERT(_orb.finished());
      return _orb;
    }

   private:
    mutable type _orb;
  };

  //! \brief Specialization of the Rank adapter for \ref BMat.
  //!
  //! Specialization of the Rank adapter for instances of \ref BMat
  //!
  //! \warning
  //! The template type `Mat` must satisfy \ref IsBMat<Mat>.
  //! If not, template substitution will fail.
  //!
  //! \sa Rank.
  template <typename Mat>
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
  struct Rank<Mat, RankState<Mat>, std::enable_if_t<IsBMat<Mat>>>
#else
  struct Rank<Mat, RankState<Mat>>
#endif
  {
    //! \brief Returns the rank of \p x.
    //!
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
  //! @}
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_BMAT_ADAPTERS_HPP_
