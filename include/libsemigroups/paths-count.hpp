//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 James D. Mitchell
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

// This file contains declarations related to counting paths in
// WordGraphs.

#ifndef LIBSEMIGROUPS_PATHS_COUNT_HPP_
#define LIBSEMIGROUPS_PATHS_COUNT_HPP_

#include <algorithm>  // for min
#include <cstddef>    // for size_t
#include <cstdint>    // for uint64_t
#include <iterator>   // for distance
#include <numeric>    // for accum...
#include <vector>     // for vector

#include "detail/containers.hpp"  // for Dynam...
#include "detail/eigen.hpp"       // for eigenstuff

#include "config.hpp"              // for LIBSEMIGROUPS_EIGEN_ENABLED
#include "constants.hpp"           // for POSITIVE_INFINITY
#include "debug.hpp"               // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"           // for LIBSEMIGROUPS_EXCEPTION
#include "word-graph-helpers.hpp"  // for is_acyclic etc
#include "word-graph.hpp"          // for WordGraph
#include "word-range.hpp"          // for number_of_words

namespace libsemigroups::v4 {
  // TODO(v4) Turn this into a doxygen comment once it is moved out of the v4
  // namespace.
  // \ingroup word_graph_group
  //
  // \brief Namespace containing helper functions for the Paths class.
  //
  // This namespace contains helper functions for the Paths class.
  namespace paths {
    //! An enum for specifying the algorithm to the functions
    //! count().
    enum class algorithm {
      //! Use a depth-first-search.
      dfs = 0,
      //! Use the adjacency matrix and matrix multiplication.
      matrix,
      //! Use a dynamic programming approach for acyclic word graphs.
      acyclic,
      //! Try to utilise some corner cases.
      trivial,
      //! The function count() tries to decide which algorithm is
      //! best.
      automatic
    };

    //! \relates Paths
    //! Returns the algorithm used by count().
    //!
    //! \param wg the WordGraph.
    //! \param source the source node.
    //!
    //! \returns A value of type algorithm.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant
    template <typename Node1, typename Node2>
    [[nodiscard]] algorithm count_algorithm(WordGraph<Node1> const& wg,
                                            Node2 source) noexcept {
      (void) wg;
      (void) source;
      return algorithm::acyclic;
    }

    //! \relates Paths
    //! Returns the number of paths from a source node.
    //!
    //! \param wg the WordGraph.
    //! \param source the source node.
    //!
    //! \returns A value of type `uint64_t`.
    //!
    //! \throws LibsemigroupsException if \p source is not a node in the
    //! word graph.
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    //!
    //! \note If `libsemigroups` is compiled with the flag `--enable-eigen`,
    //! then this function makes use of the Eigen library for linear algebra
    //! (see
    //! \cite Guennebaud2010aa).
    //!
    //! \warning If the number of paths exceeds \f$2 ^{64}\f$, then the return
    //! value of this function will not be correct.
    template <typename Node1, typename Node2>
    [[nodiscard]] uint64_t count(WordGraph<Node1> const& wg, Node2 source);

    //! \relates Paths
    //! Returns the algorithm used by count().
    //!
    //! Returns the algorithm used by count() to compute the number
    //! of paths originating at the given source node with length in the range
    //! \f$[min, max)\f$.
    //!
    //! \param wg the WordGraph.
    //! \param source the source node.
    //! \param min the minimum length of paths to count.
    //! \param max the maximum length of paths to count.
    //!
    //! \returns A value of type algorithm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    // Not noexcept because v4::word_graph::topological_sort is not.
    template <typename Node1, typename Node2>
    [[nodiscard]] algorithm count_algorithm(WordGraph<Node1> const& wg,
                                            Node2                   source,
                                            size_t                  min,
                                            size_t                  max);

    //! \relates Paths
    //! Returns the number of paths starting at a given node with length in a
    //! given range.
    //!
    //! \param wg the WordGraph.
    //! \param source the first node.
    //! \param min the minimum length of a path.
    //! \param max the maximum length of a path.
    //! \param lgrthm the algorithm to use (defaults to:
    //! algorithm::automatic).
    //!
    //! \returns
    //! A value of type \c uint64_t.
    //!
    //! \throws LibsemigroupsException if:
    //! * \p source is not a node in the word graph.
    //! * the algorithm specified by \p lgrthm is not applicable.
    //!
    //! \complexity
    //! The complexity depends on the value of \p lgrthm as follows:
    //! * algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of
    //! paths in
    //!   the word graph starting at \p source
    //! * algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is
    //! the
    //!   number of nodes and \f$k\f$ equals \p max.
    //! * algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
    //! number
    //!   of nodes and \f$m\f$ is the out-degree of the word graph (only valid
    //!   if the subgraph induced by the nodes reachable from \p source is
    //!   acyclic)
    //! * algorithm::trivial: at worst \f$O(nm)\f$ where \f$n\f$ is the
    //! number
    //!   of nodes and \f$m\f$ is the out-degree of the word graph (only valid
    //!   in some circumstances)
    //! * algorithm::automatic: attempts to select the fastest
    //! algorithm of the
    //!   preceding algorithms and then applies that.
    //!
    //! \note If `libsemigroups` is compiled with the flag `--enable-eigen`,
    //! then this function makes use of the Eigen library for linear algebra
    //! (see
    //! \cite Guennebaud2010aa).
    //!
    //! \warning If \p lgrthm is algorithm::automatic, then it is not
    //! always the case that the fastest algorithm is used.
    //!
    //! \warning If the number of paths exceeds 2 ^ 64, then return value of
    //! this function will not be correct.
    // not noexcept for example detail::count_trivial can throw
    template <typename Node1, typename Node2>
    [[nodiscard]] uint64_t count(WordGraph<Node1> const& wg,
                                 Node2                   source,
                                 size_t                  min,
                                 size_t                  max,
                                 algorithm lgrthm = algorithm::automatic);

    //! \relates Paths
    //!
    //! Returns the \ref algorithm used by count().
    //!
    //! Returns the \ref algorithm used by count() to compute
    //! the number of paths originating at the given source node and ending at
    //! the given target node with length in the range \f$[min, max)\f$.
    //!
    //! \param wg the WordGraph.
    //! \param source the source node.
    //! \param target the target node.
    //! \param min the minimum length of paths to count.
    //! \param max the maximum length of paths to count.
    //!
    //! \returns A value of type \ref algorithm.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \complexity
    //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
    //! is the out-degree of the word graph.
    // Not noexcept because v4::word_graph::topological_sort isn't
    template <typename Node1, typename Node2>
    [[nodiscard]] algorithm count_algorithm(WordGraph<Node1> const& wg,
                                            Node2                   source,
                                            Node2                   target,
                                            size_t                  min,
                                            size_t                  max);

    //! \relates Paths
    //!
    //! Returns the number of paths between a pair of nodes with length in a
    //! given range.
    //!
    //! \param wg the WordGraph.
    //! \param source the first node.
    //! \param target the last node.
    //! \param min the minimum length of a path.
    //! \param max the maximum length of a path.
    //! \param lgrthm the algorithm to use (defaults to:
    //! algorithm::automatic).
    //!
    //! \returns
    //! A value of type `uint64_t`.
    //!
    //! \throws LibsemigroupsException if:
    //! * \p source is not a node in the word graph.
    //! * \p target is not a node in the word graph.
    //! * the algorithm specified by \p lgrthm is not applicable.
    //!
    //! \complexity
    //! The complexity depends on the value of \p lgrthm as follows:
    //! * algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of
    //!   paths in the word graph starting at \p source
    //! * algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is
    //!   the number of nodes and \f$k\f$ equals \p max.
    //! * algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
    //!   number of nodes and \f$m\f$ is the out-degree of the word graph
    //!   (only valid if the subgraph induced by the nodes reachable from
    //!   \p source is acyclic)
    //! * algorithm::trivial: constant (only valid in some circumstances)
    //! * algorithm::automatic: attempts to select the fastest algorithm
    //!   of the preceding algorithms and then applies that.
    //!
    //! \note If `libsemigroups` is compiled with the flag `--enable-eigen`,
    //! then this function makes use of the Eigen library for linear algebra
    //! (see \cite Guennebaud2010aa).
    //!
    //! \warning If \p lgrthm is algorithm::automatic, then it is not
    //! always the case that the fastest algorithm is used.
    //!
    //! \warning If the number of paths exceeds \f$2 ^{64}\f$, then the return
    //! value of this function will not be correct.
    // not noexcept because cbegin_pstilo isn't
    template <typename Node1, typename Node2>
    [[nodiscard]] uint64_t count(WordGraph<Node1> const& wg,
                                 Node2                   source,
                                 Node2                   target,
                                 size_t                  min,
                                 size_t                  max,
                                 algorithm lgrthm = algorithm::automatic);
  }  // namespace paths
}  // namespace libsemigroups::v4

#include "paths-count.tpp"

#endif  // LIBSEMIGROUPS_PATHS_COUNT_HPP_
