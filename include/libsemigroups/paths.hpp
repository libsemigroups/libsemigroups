//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

// This file contains declarations related to iterating through paths in an
// WordGraph.

// TODO(2) check code coverage

#ifndef LIBSEMIGROUPS_PATHS_HPP_
#define LIBSEMIGROUPS_PATHS_HPP_

#if (defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER)))
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#pragma GCC diagnostic ignored "-Wuninitialized"
#endif

#include <algorithm>    // for any_of, for_each, max_element
#include <cstddef>      // for size_t, ptrdiff_t
#include <iterator>     // for distance, forward_iterator_tag
#include <numeric>      // for accumulate
#include <stdint.h>     // for uint64_t
#include <tuple>        // for tie
#include <type_traits>  // for true_type
#include <variant>      // for visit, variant
#include <vector>       // for vector, allocator

#include "config.hpp"      // for LIBSEMIGROUPS_EIGEN_ENA...
#include "constants.hpp"   // for Max, UNDEFINED, Positive...
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "order.hpp"       // for order
#include "ranges.hpp"      // for is_input_range
#include "types.hpp"       // for word_type
#include "word-graph.hpp"  // for WordGraph
#include "words.hpp"       // for number_of_words

#include "detail/containers.hpp"      // for DynamicArray2
#include "detail/iterator.hpp"        // for default_postfix_increment
#include "detail/path-iterators.hpp"  // for default_postfix_increment
#include "detail/report.hpp"          // for magic_enum formatter

namespace libsemigroups {

  //! \defgroup paths_group Paths
  //!
  //! This page contains links to the documentation for the functionality in
  //! ``libsemigroups`` for finding and counting paths in WordGraph objects.

  //! Helper namespace.
  namespace paths {
    //! \ingroup paths_group
    //!
    //! An enum for specifying the algorithm to the functions number_of_paths().
    enum class algorithm {
      //! Use a depth-first-search.
      dfs = 0,
      //! Use the adjacency matrix and matrix multiplication
      matrix,
      //! Use a dynamic programming approach for acyclic word graphs
      acyclic,
      //! Try to utilise some corner cases.
      trivial,
      //! The function number_of_paths() tries to decide which algorithm is
      //! best.
      automatic
    };
  }  // namespace paths

  //! \ingroup paths_group
  //! Returns an iterator for pilo (Path And Node In Lex Order).
  //!
  //! Returns a forward iterator pointing to a pair consisting of the edge
  //! labels of the first path (in lexicographical order) starting at
  //! \p source with length in the range \f$[min, max)\f$ and the last node
  //! of that path.
  //!
  //! If incremented, the iterator will point to the next least edge
  //! labelling of a path (in lexicographical order), and its last node, with
  //! length in the range \f$[min, max)\f$.  Iterators of the type returned
  //! by this function are equal whenever they point to equal objects.
  //!
  //! \param wg the WordGraph
  //! \param source the source node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        \ref POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c const_pilo_iterator pointing to a
  //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
  //! where:
  //! * \c it->first is a \ref word_type consisting of the edge
  //! labels of the first path (in lexicographical order) from \p source of
  //! length in the range \f$[min, max)\f$; and
  //! * \c it->second is the last node on the path from \p source labelled by
  //! \c it->first, a value of \ref WordGraph::node_type.
  //!
  //! \throws LibsemigroupsException if \p source is not a node in the
  //! word graph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the word graph represented by \c this contains a cycle that is
  //! reachable from \p source, then there are infinitely many paths starting
  //! at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pilo
  // not noexcept because constructors of const_pilo_iterator aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] auto cbegin_pilo(WordGraph<Node1> const& wg,
                                 Node2                   source,
                                 size_t                  min = 0,
                                 size_t max = POSITIVE_INFINITY) {
    word_graph::validate_node(wg, static_cast<Node1>(source));
    return detail::const_pilo_iterator<Node1>(&wg, source, min, max);
  }

  //! \ingroup paths_group
  //! Returns an iterator for pilo (Path And Node In Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the word graph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pilo
  // not noexcept because constructors of const_pilo_iterator aren't
  template <typename Node>
  [[nodiscard]] auto cend_pilo(WordGraph<Node> const& wg) {
    return detail::const_pilo_iterator<Node>(&wg, 0, 0, 0);
  }

  //! \ingroup paths_group
  //!
  //! Returns an iterator for pislo (Path And Node In Short Lex Order).
  //!
  //! Returns a forward iterator pointing to a pair consisting of the edge
  //! labels of the first path (in short-lex order) starting at \p source
  //! with length in the range \f$[min, max)\f$ and the last node of that
  //! path.
  //!
  //! If incremented, the iterator will point to the next least edge
  //! labelling of a path (in short-lex order), and its last node, with
  //! length in the range \f$[min, max)\f$.  Iterators of the type returned
  //! by this function are equal whenever they point to equal objects.
  //!
  //! \param wg the WordGraph
  //! \param source the source node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        \ref POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c detail::const_pislo_iterator pointing to a
  //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
  //! where:
  //! * \c it->first is a \ref word_type consisting of the edge
  //! labels of the first path (in short-lex order) from \p source of
  //! length in the range \f$[min, max)\f$; and
  //! * \c it->second is the last node on the path from \p source labelled by
  //! \c it->first, a value of \ref WordGraph::node_type.
  //!
  //! \throws LibsemigroupsException if \p source is not a node in the
  //! word graph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the word graph represented by \c this contains a cycle that is
  //! reachable from \p source, then there are infinitely many paths starting
  //! at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pislo
  // TODO(2) example and what is the complexity?
  // not noexcept because detail::const_pislo_iterator constructors aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] auto cbegin_pislo(WordGraph<Node1> const& wg,
                                  Node2                   source,
                                  size_t                  min = 0,
                                  size_t max = POSITIVE_INFINITY) {
    word_graph::validate_node(wg, static_cast<Node1>(source));
    return detail::const_pislo_iterator<Node1>(&wg, source, min, max);
  }

  //! \ingroup paths_group
  //! Returns an iterator for pislo (Path And Node In Short Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the word graph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pislo
  // not noexcept because detail::const_pislo_iterator constructors aren't
  template <typename Node>
  [[nodiscard]] auto cend_pislo(WordGraph<Node> const& wg) {
    return detail::const_pislo_iterator<Node>(&wg, UNDEFINED, 0, 0);
  }

  //! \ingroup paths_group
  //! Returns an iterator for PSTILO (Path Source Target In Lex Order).
  //!
  //! Returns a forward iterator pointing to the edge labels of the first
  //! path (in lexicographical order) starting at the node \p source and
  //! ending at the node \p target with length in the range \f$[min, max)\f$.
  //!
  //! If incremented, the iterator will point to the next least edge
  //! labelling of a path (in lexicographical order).  Iterators of the type
  //! returned by this function are equal whenever they point to equal
  //! objects.
  //!
  //! \param wg the WordGraph
  //! \param source the first node
  //! \param target the last node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        \ref POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c detail::const_pstilo_iterator pointing to a
  //! \ref word_type consisting of the edge labels of the first
  //! path (in lexicographical order) from the node \p source to the node \p
  //! target with length in the range \f$[min, max)\f$ (if any).
  //!
  //! \throws LibsemigroupsException if \p target or \p source is not a node
  //! in the word graph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the word graph represented by \c this contains a cycle that is
  //! reachable from \p source, then there may be infinitely many paths
  //! starting at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pstilo
  // not noexcept because detail::const_pstilo_iterator constructors aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] auto cbegin_pstilo(WordGraph<Node1> const& wg,
                                   Node2                   source,
                                   Node2                   target,
                                   size_t                  min = 0,
                                   size_t max = POSITIVE_INFINITY) {
    // source & target are validated in is_reachable.
    if (!word_graph::is_reachable(wg, source, target)) {
      return cend_pstilo(wg);
    }
    return detail::const_pstilo_iterator<Node1>(&wg, source, target, min, max);
  }

  //! \ingroup paths_group
  //! Returns an iterator for PSTILO (Path Source Target In Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the word graph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pstilo
  // not noexcept because detail::const_pstilo_iterator constructors aren't
  template <typename Node>
  [[nodiscard]] auto cend_pstilo(WordGraph<Node> const& wg) {
    return detail::const_pstilo_iterator<Node>(&wg, 0, 0, 0, 0);
  }

  //! \ingroup paths_group
  //!
  //! Returns an iterator for PSTISLO (Path Source Target In Short Lex
  //! Order).
  //!
  //! Returns a forward iterator pointing to the edge labels of the first
  //! path (in short-lex order) starting at the node \p source and ending
  //! at the node \p target with length in the range \f$[min, max)\f$.
  //!
  //! If incremented, the iterator will point to the next least edge
  //! labelling of a path (in short-lex order).  Iterators of the type
  //! returned by this function are equal whenever they point to equal
  //! objects.
  //!
  //! \param wg the WordGraph
  //! \param source the first node
  //! \param target the last node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        \ref POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c detail::const_pstislo_iterator pointing to a
  //! \ref word_type consisting of the edge labels of the first
  //! path (in short-lex order) from the node \p source to the node \p target
  //! with length in the range \f$[min, max)\f$ (if any).
  //!
  //! \throws LibsemigroupsException if \p target or \p source is not a node
  //! in the word graph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the word graph represented by \c this contains a cycle that is
  //! reachable from \p source, then there may be infinitely many paths
  //! starting at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pstislo
  // not noexcept because cbegin_pislo isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] auto cbegin_pstislo(WordGraph<Node1> const& wg,
                                    Node2                   source,
                                    Node2                   target,
                                    size_t                  min = 0,
                                    size_t max = POSITIVE_INFINITY) {
    // source & target are validated in is_reachable.
    if (!word_graph::is_reachable(wg, source, target)) {
      return cend_pstislo(wg);
    }
    return detail::const_pstislo_iterator<Node1>(&wg, source, target, min, max);
  }

  //! \ingroup paths_group
  //!
  //! Returns an iterator for PSTISLO (Path Source Target In Short Lex
  //! Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the word graph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pstislo
  // not noexcept because cend_pislo isn't
  template <typename Node>
  [[nodiscard]] auto cend_pstislo(WordGraph<Node> const& wg) {
    return detail::const_pstislo_iterator<Node>(
        &wg, UNDEFINED, UNDEFINED, 0, 0);
  }

  //! \ingroup paths_group
  //! Returns the paths::algorithm used by number_of_paths().
  //!
  //! \param wg the WordGraph
  //! \param source the source node.
  //!
  //! \returns A value of type paths::algorithm.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \complexity
  //! Constant
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& wg, Node2 source) noexcept {
    (void) wg;
    (void) source;
    return paths::algorithm::acyclic;
  }

  //! \ingroup paths_group
  //! Returns the number of paths from a source node.
  //!
  //! \param wg the WordGraph
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
  //! \warning If the number of paths exceeds 2 ^ 64, then return value of
  //! this function will not be correct.
  template <typename Node1, typename Node2>
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& wg,
                                         Node2                   source);

  //! \ingroup paths_group
  //! Returns the paths::algorithm used by number_of_paths().
  //!
  //! Returns the algorithm used by number_of_paths() to compute the number
  //! of paths originating at the given source node with length in the range
  //! \f$[min, max)\f$.
  //!
  //! \param wg the WordGraph
  //! \param source the source node
  //! \param min the minimum length of paths to count
  //! \param max the maximum length of paths to count
  //!
  //! \returns A value of type paths::algorithm.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
  //! is the out-degree of the word graph.
  // Not noexcept because word_graph::topological_sort is not.
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& wg,
                            Node2                   source,
                            size_t                  min,
                            size_t                  max);

  //! \ingroup paths_group
  //! Returns the number of paths starting at a given node with length in a
  //! given range.
  //!
  //! \param wg the WordGraph
  //! \param source the first node
  //! \param min the minimum length of a path
  //! \param max the maximum length of a path
  //! \param lgrthm the algorithm to use (defaults to:
  //! paths::algorithm::automatic).
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
  //! * paths::algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths
  //! in
  //!   the word graph starting at \p source
  //! * paths::algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
  //!   number of nodes and \f$k\f$ equals \p max.
  //! * paths::algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //! number
  //!   of nodes and \f$m\f$ is the out-degree of the word graph (only valid if
  //!   the subgraph induced by the nodes reachable from \p source is
  //!   acyclic)
  //! * paths::algorithm::trivial: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //! number
  //!   of nodes and \f$m\f$ is the out-degree of the word graph (only valid in
  //!   some circumstances)
  //! * paths::algorithm::automatic: attempts to select the fastest algorithm of
  //! the
  //!   preceding algorithms and then applies that.
  //!
  //! \warning If \p lgrthm is paths::algorithm::automatic, then it is not
  //! always the case that the fastest algorithm is used.
  //!
  //! \warning If the number of paths exceeds 2 ^ 64, then return value of
  //! this function will not be correct.
  // not noexcept for example detail::number_of_paths_trivial can throw
  template <typename Node1, typename Node2>
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& wg,
                                         Node2                   source,
                                         size_t                  min,
                                         size_t                  max,
                                         paths::algorithm        lgrthm
                                         = paths::algorithm::automatic);

  //! \ingroup paths_group
  //!
  //! Returns the \ref paths::algorithm used by number_of_paths().
  //!
  //! Returns the \ref paths::algorithm used by number_of_paths() to compute
  //! the number of paths originating at the given source node and ending at
  //! the given target node with length in the range \f$[min, max)\f$.
  //!
  //! \param wg the WordGraph
  //! \param source the source node
  //! \param target the target node
  //! \param min the minimum length of paths to count
  //! \param max the maximum length of paths to count
  //!
  //! \returns A value of type \ref paths::algorithm.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
  //! is the out-degree of the word graph.
  // Not noexcept because word_graph::topological_sort isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& wg,
                            Node2                   source,
                            Node2                   target,
                            size_t                  min,
                            size_t                  max);

  //! \ingroup paths_group
  //!
  //! Returns the number of paths between a pair of nodes with length in a
  //! given range.
  //!
  //! \param wg the WordGraph
  //! \param source the first node
  //! \param target the last node
  //! \param min the minimum length of a path
  //! \param max the maximum length of a path
  //! \param lgrthm the algorithm to use (defaults to:
  //! paths::algorithm::automatic).
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
  //! * paths::algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths
  //!   in the word graph starting at \p source
  //! * paths::algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
  //!   number of nodes and \f$k\f$ equals \p max.
  //! * paths::algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //!   number of nodes and \f$m\f$ is the out-degree of the word graph (only
  //!   valid if the subgraph induced by the nodes reachable from \p source is
  //!   acyclic)
  //! * paths::algorithm::trivial: constant (only valid in some circumstances)
  //! * paths::algorithm::automatic: attempts to select the fastest algorithm of
  //!   the preceding algorithms and then applies that.
  //!
  //! \warning If \p lgrthm is paths::algorithm::automatic, then it is not
  //! always the case that the fastest algorithm is used.
  //!
  //! \warning If the number of paths exceeds 2 ^ 64, then return value of
  //! this function will not be correct.
  // not noexcept because cbegin_pstilo isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& wg,
                                         Node2                   source,
                                         Node2                   target,
                                         size_t                  min,
                                         size_t                  max,
                                         paths::algorithm        lgrthm
                                         = paths::algorithm::automatic);

  //! \ingroup paths_group
  //!
  //! \brief Range for iterating through paths in a WordGraph.
  //!
  //! This class represents a range object that facilitates iterating through
  //! the paths in a WordGraph \ref from a given node (possible \ref to another
  //! node) in a particular \ref order.
  //!
  //! \tparam Node the type of the nodes in the underlying WordGraph.
  //!
  //! So that the Paths class can be used efficiently with the functionality of
  //! rx::ranges, the usual naming conventions in ``libsemigroups`` are not
  //! used for the member functions of Paths. In particular, none of the member
  //! functions check their arguments, but they do not have the suffix
  //! ``_no_checks``.
  //!
  //! For a Paths object to be valid it must have both its source node defined
  //! (using \ref from) and the underlying WordGraph defined (using \ref
  //! init(WordGraph<Node> const&).
  //! This can be verified using \ref is_valid. The following member functions
  //! should only be called if \ref is_valid returns \c true:
  //! * \ref get
  //! * \ref next
  //! * \ref at_end
  //! * \ref size_hint
  //! * \ref count
  template <typename Node>
  class Paths {
   public:
    //! \brief The template parameter \c Node, the type of the nodes in the
    //! underlying WordGraph.
    //!
    //! The template parameter \c Node, the type of the nodes in the
    //! underlying WordGraph.
    using node_type = Node;

    //! \brief Unsigned integer for indexing.
    //!
    //! Unsigned integer for indexing.
    using size_type = typename WordGraph<Node>::size_type;

    //! \brief Alias for const reference to a \ref word_type.
    //!
    //! The output type of this type of range.
    using output_type = word_type const&;

   private:
    using const_iterator = std::variant<detail::const_pstislo_iterator<Node>,
                                        detail::const_pstilo_iterator<Node>,
                                        detail::const_pislo_iterator<Node>,
                                        detail::const_pilo_iterator<Node>>;

    WordGraph<node_type> const* _word_graph;
    Order                       _order;
    size_type                   _max;
    size_type                   _min;
    mutable size_type           _position;
    node_type                   _source;
    node_type                   _target;
    mutable const_iterator      _current;
    mutable const_iterator      _end;
    mutable bool                _current_valid;

    bool set_iterator_no_checks() const;

    // The following init function is private to avoid the case of constructing
    // a Paths object without setting _word_graph
    Paths& init();

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initialization
    ////////////////////////////////////////////////////////////////////////

    //! \brief Deleted.
    //!
    //! To avoid the situation where the underlying WordGraph is not defined, it
    //! is not possible to default construct a Paths object.
    Paths() = delete;

    //! \brief Default copy constructor.
    //!
    //! Default copy constructor.
    Paths(Paths const&) = default;

    //! \brief Default move constructor.
    //!
    //! Default move constructor.
    Paths(Paths&&) = default;

    //! \brief Default copy assignment operator.
    //!
    //! Default copy assignment operator.
    Paths& operator=(Paths const&) = default;

    //! \brief Default move assignment operator.
    //!
    //! Default move assignment operator.
    Paths& operator=(Paths&&) = default;

    //! \brief Construct from a WordGraph.
    //!
    //! This function constructs a Paths object from the WordGraph \p wg.
    //!
    //! \param wg the word graph.
    //!
    //! \warning It is also necessary to set the source node using \ref from
    //! before the object is valid.
    //!
    //! \warning The Paths object only holds a reference to the underlying
    //! WordGraph \p wg, and so \p wg must outlive any Paths object constructed
    //! from \p wg.
    explicit Paths(WordGraph<Node> const& wg) {
      init(wg);
    }

    //! \brief Reinitialize a Paths object.
    //!
    //! This function puts a Paths object back into the same state as
    //! if it had been newly constructs from the WordGraph \p wg.
    //!
    //! \param wg the word graph.
    //!
    //! \warning It is also necessary to set the source node using \ref from
    //! before the object is valid.
    //!
    //! \warning The Paths object only holds a reference to the underlying
    //! WordGraph \p wg, and so \p wg must outlive any Paths object constructed
    //! from \p wg.
    Paths& init(WordGraph<Node> const& wg) {
      init();
      _word_graph = &wg;
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // Validation
    ////////////////////////////////////////////////////////////////////////
    //! TODO
    // TODO to tpp
    void throw_if_not_valid(std::string_view sep = "::") {
      if (_source == UNDEFINED) {
        LIBSEMIGROUPS_EXCEPTION(
            "no source node defined, use Paths{}from to set the source node",
            sep);
      }
    }

    //! TODO
    [[nodiscard]] bool is_valid() const noexcept {
      return _source != UNDEFINED;
    }

    ////////////////////////////////////////////////////////////////////////
    // Functions + members required by rx::ranges
    ////////////////////////////////////////////////////////////////////////

    //! \brief Get the current path in the range.
    //!
    //! Get the current path in the range.
    //!
    //! \returns The current path, a value of \ref output_type.
    //!
    //! \warning It is the responsibility of the caller to ensure that \ref
    //! is_valid returns \c true before calling this function.
    output_type get() const {
      set_iterator_no_checks();
      return std::visit(
          [](auto& it) -> auto const& { return *it; }, _current);
    }

    //! \brief Advance to the next path in the range.
    //!
    //! Advance to the current path in the range. If \ref at_end returns \c
    //! true, then this function does nothing.
    //!
    //! \warning It is the responsibility of the caller to ensure that \ref
    //! is_valid returns \c true before calling this function.
    void next() {
      if (!at_end()) {
        ++_position;
        std::visit([](auto& it) { ++it; }, _current);
      }
    }

    //! \brief Check if the range is exhausted.
    //!
    //! This function returns \c true if there are no more paths in the range,
    //! and \c false otherwise.
    //!
    //! \returns Whether or not the range is exhausted.
    //!
    //! \warning It is the responsibility of the caller to ensure that \ref
    //! is_valid returns \c true before calling this function.
    [[nodiscard]] bool at_end() const {
      if (!set_iterator_no_checks()) {
        return true;
      }
      return _current == _end;
    }

    //! \brief Get the size of the range.
    //!
    //! This function returns the number of paths in the range. The output is
    //! identical to that of \ref count, and is included for compatibility with
    //! [rx::ranges][].
    //!
    //! [rx::ranges]: https://github.com/simonask/rx-ranges
    //!
    //! \returns the number of paths in the range.
    //!
    //! \warning It is the responsibility of the caller to ensure that \ref
    //! is_valid returns \c true before calling this function.
    [[nodiscard]] uint64_t size_hint() const;

    //! \brief Get the size of the range.
    //!
    //! This function returns the number of paths in the range. The output is
    //! identical to that of \ref size_hint, and is included for compatibility
    //! with [rx::ranges][].
    //!
    //! [rx::ranges]: https://github.com/simonask/rx-ranges
    //!
    //! \returns the number of paths in the range.
    //!
    //! \warning It is the responsibility of the caller to ensure that \ref
    //! is_valid returns \c true before calling this function.
    [[nodiscard]] uint64_t count() const {
      return size_hint();
    }

    static constexpr bool is_finite     = true;  // this isn't always true!
    static constexpr bool is_idempotent = true;

    ////////////////////////////////////////////////////////////////////////
    // Settings
    ////////////////////////////////////////////////////////////////////////

    //! \brief Set the source node of every path in the range.
    //!
    //! This function can be used to set the source node (or the "from" node) of
    //! all of the paths in the range.
    //!
    //! \param n the source node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \warning This function must be called at least once for a Paths object
    //! to be valid.
    //!
    //! \exceptions
    //! \noexcept
    Paths& from(node_type n) noexcept {
      return from(this, n);
    }

    //! TODO
    [[nodiscard]] node_type from() const noexcept {
      return _source;
    }

    //! \brief Set the target node of every path in the range.
    //!
    //! This function can be used to set the target node (or the "to" node) of
    //! all of the paths in the range. It is not necessary to set this value.
    //! If it is \ref UNDEFINED, then the range will contain every path \ref
    //! from with every possible target in \ref word_graph.
    //!
    //! \param n the target node.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    Paths& to(node_type n) noexcept {
      return to(this, n);
    }

    //! TODO
    [[nodiscard]] node_type to() const;

    //! \brief Set the minimum length of path in the range.
    //!
    //! This function can be used to set the minimum length of paths that will
    //! be contained in the range. If this function is not called, then the
    //! range will contain paths starting with length \c 0.
    //!
    //! \param val the minimum path length.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    Paths& min(size_type val) noexcept {
      return min(this, val);
    }

    //! TODO
    [[nodiscard]] size_type min() const noexcept {
      return _min;
    }

    //! \brief Set the maximum length of path in the range.
    //!
    //! This function can be used to set the maximum length of path that will
    //! be contained in the range. If this function is not called, then the
    //! range will contain paths of unbounded length (possibly infinitely many).
    //!
    //! \param val the maximum path length.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    Paths& max(size_type val) noexcept {
      return max(this, val);
    }

    //! TODO
    [[nodiscard]] size_type max() const noexcept {
      return _max;
    }

    //! TODO
    Paths& order(Order val) {
      return order(this, val);
    }

    //! TODO
    [[nodiscard]] Order order() const noexcept {
      return _order;
    }

   protected:
    template <typename Subclass>
    Subclass& from(Subclass* obj, node_type src) {
      _current_valid &= (src == _source);
      _source = src;
      return *obj;
    }

    template <typename Subclass>
    Subclass& to(Subclass* obj, node_type trgt) noexcept {
      _current_valid &= (trgt == _target);
      _target = trgt;
      return *obj;
    }

    template <typename Subclass>
    Subclass& min(Subclass* obj, size_type min) noexcept {
      _current_valid &= (min == _min);
      _min = min;
      return *obj;
    }

    template <typename Subclass>
    Subclass& max(Subclass* obj, size_type max) noexcept {
      _current_valid &= (max == _max);
      _max = max;
      return *obj;
    }

    template <typename Subclass>
    Subclass& order(Subclass* obj, Order val);
  };

  //! TODO
  template <typename Node>
  Paths(WordGraph<Node> const&) -> Paths<Node>;

  //! TODO
  template <typename Node>
  Paths(WordGraph<Node>&&) -> Paths<Node>;

  //! \ingroup paths_group
  //!
  //! TODO
  template <typename Node>
  class ReversiblePaths : public Paths<Node> {
   private:
    bool _reverse;

    using size_type = typename WordGraph<Node>::size_type;

    // Private so that we cannot create one of these without the word graph
    // known.
    ReversiblePaths& init() {
      Paths<Node>::init();
      _reverse = false;
    }

   public:
    // this isn't always true!
    //! TODO
    static constexpr bool is_finite = Paths<Node>::is_finite;
    //! TODO
    static constexpr bool is_idempotent = Paths<Node>::is_idempotent;

    //! TODO
    using output_type = word_type;

    //! TODO
    ReversiblePaths() = delete;

    //! TODO
    ReversiblePaths(ReversiblePaths const&) = default;
    //! TODO
    ReversiblePaths(ReversiblePaths&&) = default;
    //! TODO
    ReversiblePaths& operator=(ReversiblePaths const&) = default;
    //! TODO
    ReversiblePaths& operator=(ReversiblePaths&&) = default;

    //! TODO
    explicit ReversiblePaths(WordGraph<Node> const& wg) : Paths<Node>(wg) {}

    //! TODO
    ReversiblePaths& init(WordGraph<Node> const& wg) {
      Paths<Node>::init(wg);
      return *this;
    }

    //! TODO
    ReversiblePaths& from(size_type val) noexcept {
      return Paths<Node>::from(this, val);
    }

    //! TODO
    ReversiblePaths& to(size_type val) noexcept {
      return Paths<Node>::to(this, val);
    }

    //! TODO
    ReversiblePaths& min(size_type val) noexcept {
      return Paths<Node>::min(this, val);
    }

    //! TODO
    ReversiblePaths& max(size_type val) noexcept {
      return Paths<Node>::max(this, val);
    }

    //! TODO
    ReversiblePaths& order(Order val) {
      return Paths<Node>::order(this, val);
    }

    //! TODO
    ReversiblePaths& reverse(bool val) {
      _reverse = val;
      return *this;
    }

    //! TODO
    // TODO to tpp
    output_type get() const {
      word_type result = Paths<Node>::get();
      if (_reverse) {
        std::reverse(result.begin(), result.end());
      }
      return result;
    }
  };

  //! TODO
  template <typename Node>
  ReversiblePaths(WordGraph<Node> const&) -> ReversiblePaths<Node>;

  //! TODO
  template <typename Node>
  ReversiblePaths(WordGraph<Node>&&) -> ReversiblePaths<Node>;

}  // namespace libsemigroups

#include "paths.tpp"

#if (defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER)))
#pragma GCC diagnostic pop
#endif
#endif  // LIBSEMIGROUPS_PATHS_HPP_
