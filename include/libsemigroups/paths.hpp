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
//

// TODO:
// * doc
// * check code coverage

#ifndef LIBSEMIGROUPS_PATHS_HPP_
#define LIBSEMIGROUPS_PATHS_HPP_

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
#include "constants.hpp"   // for Max, UNDEFINED, Positiv...
#include "debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "order.hpp"       // for order
#include "types.hpp"       // for word_type
#include "word-graph.hpp"  // for WordGraph
#include "words.hpp"       // for number_of_words

#include "detail/containers.hpp"  // for DynamicArray2
#include "detail/iterator.hpp"    // for default_postfix_increment
#include "detail/report.hpp"      // for magic_enum formatter

#include <rx/ranges.hpp>  // for is_input_range

namespace libsemigroups {

  namespace paths {
    //! An enum for specifying the algorithm to the functions number_of_paths().
    enum class algorithm {
      //! Use a depth-first-search.
      dfs = 0,
      //! Use the adjacency matrix and matrix multiplication
      matrix,
      //! Use a dynamic programming approach for acyclic digraphs
      acyclic,
      //! Try to utilise some corner cases.
      trivial,
      //! The function number_of_paths() tries to decide which algorithm is
      //! best.
      automatic
    };
  }  // namespace paths

  ////////////////////////////////////////////////////////////////////////
  // pilo = Path And (terminal) Node In Lex Order
  ////////////////////////////////////////////////////////////////////////

  //! Return type of \ref cbegin_pilo and \ref cend_pilo.
  template <typename Node>
  class const_pilo_iterator {
   public:
    using node_type  = Node;
    using label_type = typename WordGraph<Node>::label_type;

    using value_type        = word_type;
    using size_type         = typename std::vector<value_type>::size_type;
    using difference_type   = typename std::vector<value_type>::difference_type;
    using const_pointer     = typename std::vector<value_type>::const_pointer;
    using pointer           = typename std::vector<value_type>::pointer;
    using const_reference   = typename std::vector<value_type>::const_reference;
    using reference         = const_reference;
    using iterator_category = std::forward_iterator_tag;

   private:
    value_type             _edges;
    WordGraph<Node> const* _digraph;
    label_type             _edge;
    size_t                 _min;
    size_t                 _max;
    std::vector<node_type> _nodes;

   public:
    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17).
    // TODO revise previous statement and the noexcept specifications below
    const_pilo_iterator();
    const_pilo_iterator(const_pilo_iterator const&);
    const_pilo_iterator(const_pilo_iterator&&);
    const_pilo_iterator& operator=(const_pilo_iterator const&);
    const_pilo_iterator& operator=(const_pilo_iterator&&);
    ~const_pilo_iterator();

    const_pilo_iterator(WordGraph<node_type> const* ptr,
                        node_type                   source,
                        size_type                   min,
                        size_type                   max);

    // noexcept because comparison of std::vector<node_type>'s is noexcept
    // because comparison of node_type's is noexcept
    [[nodiscard]] bool
    operator==(const_pilo_iterator const& that) const noexcept {
      return _nodes == that._nodes;
    }

    // noexcept because operator== is noexcept
    [[nodiscard]] bool
    operator!=(const_pilo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return _edges;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &_edges;
    }

    [[nodiscard]] node_type target() const noexcept {
      if (!_nodes.empty()) {
        return _nodes.back();
      } else {
        return UNDEFINED;
      }
    }

    // prefix - not noexcept because std::vector::push_back isn't
    const_pilo_iterator const& operator++();

    // postfix - not noexcept because the prefix ++ isn't
    const_pilo_iterator operator++(int) {
      return detail::default_postfix_increment<const_pilo_iterator>(*this);
    }

    void swap(const_pilo_iterator& that) noexcept;

    [[nodiscard]] WordGraph<Node> const& digraph() const noexcept {
      return *_digraph;
    }
  };

  template <typename Node>
  inline void swap(const_pilo_iterator<Node>& x,
                   const_pilo_iterator<Node>& y) noexcept {
    x.swap(y);
  }

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
  //! \param source the source node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        libsemigroups::POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c const_pilo_iterator pointing to a
  //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
  //! where:
  //! * \c it->first is a libsemigroups::word_type consisting of the edge
  //! labels of the first path (in lexicographical order) from \p source of
  //! length in the range \f$[min, max)\f$; and
  //! * \c it->second is the last node on the path from \p source labelled by
  //! \c it->first, a value of \ref node_type.
  //!
  //! \throws LibsemigroupsException if \p source is not a node in the
  //! digraph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the action digraph represented by \c this contains a cycle that is
  //! reachable from \p source, then there are infinitely many paths starting
  //! at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pilo
  // not noexcept because constructors of const_pilo_iterator aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] const_pilo_iterator<Node1>
  cbegin_pilo(WordGraph<Node1> const& d,
              Node2                   source,
              size_t                  min = 0,
              size_t                  max = POSITIVE_INFINITY) {
    word_graph::validate_node(d, static_cast<Node1>(source));
    return const_pilo_iterator<Node1>(&d, source, min, max);
  }

  //! Returns an iterator for pilo (Path And Node In Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the digraph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pilo
  // not noexcept because constructors of const_pilo_iterator aren't
  template <typename Node>
  [[nodiscard]] const_pilo_iterator<Node> cend_pilo(WordGraph<Node> const& d) {
    return const_pilo_iterator<Node>(&d, 0, 0, 0);
  }

  // Note that while the complexity of this is bad, it repeatedly does depth
  // first searches, and so will examine every node and edge of the graph
  // multiple times (if u -a-> v belongs to a path of length 10, then it will
  // be traversed 10 times). But the performance of this iterator is
  // dominated by memory allocation (when creating
  // iterators, at least), and so this doesn't seem that bad.
  //! Return type of cbegin_pislo and cend_pislo.
  template <typename Node>
  class const_pislo_iterator {
   public:
    using node_type         = Node;
    using value_type        = word_type;
    using size_type         = typename std::vector<value_type>::size_type;
    using difference_type   = typename std::vector<value_type>::difference_type;
    using const_pointer     = typename std::vector<value_type>::const_pointer;
    using pointer           = typename std::vector<value_type>::pointer;
    using const_reference   = typename std::vector<value_type>::const_reference;
    using reference         = const_reference;
    using iterator_category = std::forward_iterator_tag;

   private:
    size_type                 _length;
    const_pilo_iterator<Node> _it;
    size_type                 _max;
    node_type                 _source;

   public:
    // None of the constructors are noexcept because the corresponding
    // constructors for const_pilo_iterator are not.
    const_pislo_iterator();
    const_pislo_iterator(const_pislo_iterator const&);
    const_pislo_iterator(const_pislo_iterator&&);
    const_pislo_iterator& operator=(const_pislo_iterator const&);
    const_pislo_iterator& operator=(const_pislo_iterator&&);
    ~const_pislo_iterator();

    const_pislo_iterator(WordGraph<node_type> const* ptr,
                         node_type                   source,
                         size_type                   min,
                         size_type                   max);

    // noexcept because comparison of const_pilo_iterator is noexcept
    [[nodiscard]] bool
    operator==(const_pislo_iterator const& that) const noexcept {
      return _length == that._length && _it == that._it;
    }

    // noexcept because operator== is noexcept
    [[nodiscard]] bool
    operator!=(const_pislo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return *_it;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &(*_it);
    }

    [[nodiscard]] node_type target() const noexcept {
      if (_it == cend_pilo(_it.digraph()) && _length == UNDEFINED) {
        return UNDEFINED;
      }
      return _it.target();
    }

    // prefix - not noexcept because cbegin_pilo isn't
    const_pislo_iterator const& operator++();

    // postfix - not noexcept because copy constructor isn't
    const_pislo_iterator operator++(int) {
      return detail::default_postfix_increment<const_pislo_iterator>(*this);
    }

    void swap(const_pislo_iterator& that) noexcept;
  };

  template <typename Node>
  inline void swap(const_pislo_iterator<Node>& x,
                   const_pislo_iterator<Node>& y) noexcept {
    x.swap(y);
  }

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
  //! \param source the source node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        libsemigroups::POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c const_pislo_iterator pointing to a
  //! [std::pair](https://en.cppreference.com/w/cpp/utility/pair)
  //! where:
  //! * \c it->first is a libsemigroups::word_type consisting of the edge
  //! labels of the first path (in short-lex order) from \p source of
  //! length in the range \f$[min, max)\f$; and
  //! * \c it->second is the last node on the path from \p source labelled by
  //! \c it->first, a value of \ref node_type.
  //!
  //! \throws LibsemigroupsException if \p source is not a node in the
  //! digraph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the action digraph represented by \c this contains a cycle that is
  //! reachable from \p source, then there are infinitely many paths starting
  //! at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pislo
  // TODO(later) example and what is the complexity?
  // not noexcept because const_pislo_iterator constructors aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] const_pislo_iterator<Node1>
  cbegin_pislo(WordGraph<Node1> const& d,
               Node2                   source,
               size_t                  min = 0,
               size_t                  max = POSITIVE_INFINITY) {
    word_graph::validate_node(d, static_cast<Node1>(source));
    return const_pislo_iterator<Node1>(&d, source, min, max);
  }

  //! Returns an iterator for pislo (Path And Node In Short Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the digraph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pislo
  // not noexcept because const_pislo_iterator constructors aren't
  template <typename Node>
  [[nodiscard]] const_pislo_iterator<Node>
  cend_pislo(WordGraph<Node> const& d) {
    return const_pislo_iterator<Node>(&d, UNDEFINED, 0, 0);
  }

  // PSTILO = Path Source Target In Lex Order
  template <typename Node>
  class const_pstilo_iterator {
   public:
    using node_type         = Node;
    using label_type        = typename WordGraph<node_type>::label_type;
    using value_type        = word_type;
    using size_type         = typename std::vector<value_type>::size_type;
    using difference_type   = typename std::vector<value_type>::difference_type;
    using const_pointer     = typename std::vector<value_type>::const_pointer;
    using pointer           = typename std::vector<value_type>::pointer;
    using const_reference   = typename std::vector<value_type>::const_reference;
    using reference         = const_reference;
    using iterator_category = std::forward_iterator_tag;

   private:
    std::vector<bool>      _can_reach_target;
    value_type             _edges;
    WordGraph<Node> const* _digraph;
    label_type             _edge;
    size_t                 _min;
    size_t                 _max;
    std::vector<node_type> _nodes;
    node_type              _target;

   public:
    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17).
    // TODO revisit noexcept specification
    const_pstilo_iterator();
    const_pstilo_iterator(const_pstilo_iterator const&);
    const_pstilo_iterator(const_pstilo_iterator&&);
    const_pstilo_iterator& operator=(const_pstilo_iterator const&);
    const_pstilo_iterator& operator=(const_pstilo_iterator&&);
    ~const_pstilo_iterator();

    const_pstilo_iterator(WordGraph<Node> const* ptr,
                          node_type              source,
                          node_type              target,
                          size_type              min,
                          size_type              max);

    // noexcept because comparison of std::vector<node_type> is noexcept
    // because comparison of node_type's is noexcept
    [[nodiscard]] bool
    operator==(const_pstilo_iterator const& that) const noexcept {
      return _nodes == that._nodes;
    }

    // noexcept because operator== is noexcept
    [[nodiscard]] bool
    operator!=(const_pstilo_iterator const& that) const noexcept {
      return !(this->operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return _edges;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &_edges;
    }

    [[nodiscard]] node_type target() const noexcept {
      return _target;
    }

    // prefix
    // not noexcept because std::vector::push_back isn't
    const_pstilo_iterator const& operator++();

    // postfix
    // not noexcept because (prefix) operator++ isn't
    const_pstilo_iterator operator++(int) {
      return detail::default_postfix_increment<const_pstilo_iterator>(*this);
    }

    void swap(const_pstilo_iterator& that) noexcept;

   private:
    void init_can_reach_target();
  };

  template <typename Node>
  inline void swap(const_pstilo_iterator<Node>& x,
                   const_pstilo_iterator<Node>& y) noexcept {
    x.swap(y);
  }

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
  //! \param source the first node
  //! \param target the last node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        libsemigroups::POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c const_pstilo_iterator pointing to a
  //! libsemigroups::word_type consisting of the edge labels of the first
  //! path (in lexicographical order) from the node \p source to the node \p
  //! target with length in the range \f$[min, max)\f$ (if any).
  //!
  //! \throws LibsemigroupsException if \p target or \p source is not a node
  //! in the digraph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the action digraph represented by \c this contains a cycle that is
  //! reachable from \p source, then there may be infinitely many paths
  //! starting at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pstilo
  // not noexcept because const_pstilo_iterator constructors aren't
  template <typename Node1, typename Node2>
  [[nodiscard]] const_pstilo_iterator<Node1>
  cbegin_pstilo(WordGraph<Node1> const& d,
                Node2                   source,
                Node2                   target,
                size_t                  min = 0,
                size_t                  max = POSITIVE_INFINITY) {
    // source & target are validated in is_reachable.
    if (!word_graph::is_reachable(d, source, target)) {
      return cend_pstilo(d);
    }
    return const_pstilo_iterator<Node1>(&d, source, target, min, max);
  }

  //! Returns an iterator for PSTILO (Path Source Target In Lex Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the digraph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pstilo
  // not noexcept because const_pstilo_iterator constructors aren't
  template <typename Node>
  [[nodiscard]] const_pstilo_iterator<Node>
  cend_pstilo(WordGraph<Node> const& d) {
    return const_pstilo_iterator<Node>(&d, 0, 0, 0, 0);
  }

  //! Return type of \ref cbegin_pstislo and \ref cend_pstislo.
  template <typename Node>
  class const_pstislo_iterator {
   public:
    using node_type         = Node;
    using value_type        = word_type;
    using reference         = value_type&;
    using const_reference   = value_type const&;
    using difference_type   = std::ptrdiff_t;
    using size_type         = std::size_t;
    using const_pointer     = value_type const*;
    using pointer           = value_type*;
    using iterator_category = std::forward_iterator_tag;

   private:
    const_pislo_iterator<Node> _it;
    node_type                  _target;
    const_pislo_iterator<Node> _end;  // TODO remove?

   public:
    const_pstislo_iterator();
    const_pstislo_iterator(const_pstislo_iterator const&);
    const_pstislo_iterator(const_pstislo_iterator&&);
    const_pstislo_iterator& operator=(const_pstislo_iterator const&);
    const_pstislo_iterator& operator=(const_pstislo_iterator&&);
    ~const_pstislo_iterator();

    const_pstislo_iterator(WordGraph<node_type> const* ptr,
                           node_type                   source,
                           node_type                   target,
                           size_type                   min,
                           size_type                   max)
        : _it(ptr, source, min, max), _target(target), _end(cend_pislo(*ptr)) {
      operator++();
    }

    // noexcept because comparison of const_pilo_iterator is noexcept
    [[nodiscard]] bool
    operator==(const_pstislo_iterator const& that) const noexcept {
      return _it == that._it;
    }

    // noexcept because operator== is noexcept
    [[nodiscard]] bool
    operator!=(const_pstislo_iterator const& that) const noexcept {
      return !(operator==(that));
    }

    [[nodiscard]] const_reference operator*() const noexcept {
      return *_it;
    }

    [[nodiscard]] const_pointer operator->() const noexcept {
      return &(*_it);
    }

    // Will fail if there are no paths!
    [[nodiscard]] node_type target() const noexcept {
      return _target;
    }

    // prefix
    const_pstislo_iterator const& operator++();

    // postfix - not noexcept because the prefix ++ isn't
    const_pstislo_iterator operator++(int) {
      return detail::default_postfix_increment<const_pstislo_iterator>(*this);
    }

    void swap(const_pstislo_iterator& that) noexcept {
      std::swap(_it, that._it);
      std::swap(_target, that._target);
      std::swap(_end, that._end);
    }
  };

  template <typename Node>
  inline void swap(const_pstislo_iterator<Node>& x,
                   const_pstislo_iterator<Node>& y) noexcept {
    x.swap(y);
  }

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
  //! \param source the first node
  //! \param target the last node
  //! \param min the minimum length of a path to enumerate (defaults to \c 0)
  //! \param max the maximum length of a path to enumerate (defaults to
  //!        libsemigroups::POSITIVE_INFINITY).
  //!
  //! \returns
  //! An iterator \c it of type \c const_pstislo_iterator pointing to a
  //! libsemigroups::word_type consisting of the edge labels of the first
  //! path (in short-lex order) from the node \p source to the node \p target
  //! with length in the range \f$[min, max)\f$ (if any).
  //!
  //! \throws LibsemigroupsException if \p target or \p source is not a node
  //! in the digraph.
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a consequence, prefix
  //! incrementing \c ++it the returned  iterator \c it significantly cheaper
  //! than postfix incrementing \c it++.
  //!
  //! \warning
  //! If the action digraph represented by \c this contains a cycle that is
  //! reachable from \p source, then there may be infinitely many paths
  //! starting at \p source, and so \p max should be chosen with some care.
  //!
  //! \sa
  //! cend_pstislo
  // not noexcept because cbegin_pislo isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] const_pstislo_iterator<Node1>
  cbegin_pstislo(WordGraph<Node1> const& d,
                 Node2                   source,
                 Node2                   target,
                 size_t                  min = 0,
                 size_t                  max = POSITIVE_INFINITY) {
    // source & target are validated in is_reachable.
    if (!word_graph::is_reachable(d, source, target)) {
      return cend_pstislo(d);
    }
    return const_pstislo_iterator<Node1>(&d, source, target, min, max);
  }

  //! Returns an iterator for PSTISLO (Path Source Target In Short Lex
  //! Order).
  //!
  //! Returns a forward iterator pointing to one after the last path from any
  //! node in the digraph.
  //!
  //! The iterator returned by this function may still dereferenceable and
  //! incrementable, but may not point to a path in the correct range.
  //!
  //! \sa cbegin_pstislo
  // not noexcept because cend_pislo isn't
  template <typename Node>
  [[nodiscard]] const_pstislo_iterator<Node>
  cend_pstislo(WordGraph<Node> const& d) {
    return const_pstislo_iterator<Node>(&d, UNDEFINED, UNDEFINED, 0, 0);
  }

  //! Returns the \ref algorithm used by number_of_paths().
  //!
  //! \param source the source node.
  //!
  //! \returns A value of type \ref algorithm.
  //!
  //! \exceptions
  //! \noexcept
  //!
  //! \complexity
  //! Constant
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& d, Node2 source) noexcept {
    (void) d;
    (void) source;
    return paths::algorithm::acyclic;
  }

  //! Returns the number of paths from a source node.
  //!
  //! \param source the source node.
  //!
  //! \returns A value of type `uint64_t`.
  //!
  //! \throws LibsemigroupsException if \p source is not a node in the
  //! digraph.
  //!
  //! \complexity
  //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
  //! is the out-degree of the digraph.
  //!
  //! \warning If the number of paths exceeds 2 ^ 64, then return value of
  //! this function will not be correct.
  template <typename Node1, typename Node2>
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& d,
                                         Node2                   source);

  //! Returns the \ref algorithm used by number_of_paths().
  //!
  //! Returns the algorithm used by number_of_paths() to compute the number
  //! of paths originating at the given source node with length in the range
  //! \f$[min, max)\f$.
  //!
  //! \param source the source node
  //! \param min the minimum length of paths to count
  //! \param max the maximum length of paths to count
  //!
  //! \returns A value of type \ref algorithm.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  //!
  //! \complexity
  //! At worst \f$O(nm)\f$ where \f$n\f$ is the number of nodes and \f$m\f$
  //! is the out-degree of the digraph.
  // Not noexcept because word_graph::topological_sort is not.
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& d,
                            Node2                   source,
                            size_t                  min,
                            size_t                  max);

  //! Returns the number of paths starting at a given node with length in a
  //! given range.
  //!
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
  //! * \p source is not a node in the digraph.
  //! * the algorithm specified by \p lgrthm is not applicable.
  //!
  //! \complexity
  //! The complexity depends on the value of \p lgrthm as follows:
  //! * paths::algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths
  //! in
  //!   the digraph starting at \p source
  //! * paths::algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
  //!   number of nodes and \f$k\f$ equals \p max.
  //! * paths::algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //! number
  //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid if
  //!   the subdigraph induced by the nodes reachable from \p source is
  //!   acyclic)
  //! * paths::algorithm::trivial: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //! number
  //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid in
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
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& d,
                                         Node2                   source,
                                         size_t                  min,
                                         size_t                  max,
                                         paths::algorithm        lgrthm
                                         = paths::algorithm::automatic);

  //! Returns the \ref paths::algorithm used by number_of_paths().
  //!
  //! Returns the \ref paths::algorithm used by number_of_paths() to compute
  //! the number of paths originating at the given source node and ending at
  //! the given target node with length in the range \f$[min, max)\f$.
  //!
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
  //! is the out-degree of the digraph.
  // Not noexcept because word_graph::topological_sort isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] paths::algorithm
  number_of_paths_algorithm(WordGraph<Node1> const& d,
                            Node2                   source,
                            Node2                   target,
                            size_t                  min,
                            size_t                  max);

  //! Returns the number of paths between a pair of nodes with length in a
  //! given range.
  //!
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
  //! * \p source is not a node in the digraph.
  //! * \p target is not a node in the digraph.
  //! * the algorithm specified by \p lgrthm is not applicable.
  //!
  //! \complexity
  //! The complexity depends on the value of \p lgrthm as follows:
  //! * paths::algorithm::dfs: \f$O(r)\f$ where \f$r\f$ is the number of paths
  //! in
  //!   the digraph starting at \p source
  //! * paths::algorithm::matrix: at worst \f$O(n ^ 3 k)\f$ where \f$n\f$ is the
  //!   number of nodes and \f$k\f$ equals \p max.
  //! * paths::algorithm::acyclic: at worst \f$O(nm)\f$ where \f$n\f$ is the
  //! number
  //!   of nodes and \f$m\f$ is the out-degree of the digraph (only valid if
  //!   the subdigraph induced by the nodes reachable from \p source is
  //!   acyclic)
  //! * paths::algorithm::trivial: constant (only valid in some circumstances)
  //! * paths::algorithm::automatic: attempts to select the fastest algorithm of
  //! the
  //!   preceding algorithms and then applies that.
  //!
  //! \warning If \p lgrthm is paths::algorithm::automatic, then it is not
  //! always the case that the fastest algorithm is used.
  //!
  //! \warning If the number of paths exceeds 2 ^ 64, then return value of
  //! this function will not be correct.
  // not noexcept because cbegin_pstilo isn't
  template <typename Node1, typename Node2>
  [[nodiscard]] uint64_t number_of_paths(WordGraph<Node1> const& d,
                                         Node2                   source,
                                         Node2                   target,
                                         size_t                  min,
                                         size_t                  max,
                                         paths::algorithm        lgrthm
                                         = paths::algorithm::automatic);

  // Helper class
  template <typename Node>
  class Paths {
   public:
    using node_type   = Node;
    using size_type   = typename WordGraph<Node>::size_type;
    using output_type = word_type const&;

   private:
    using const_iterator = std::variant<const_pstislo_iterator<Node>,
                                        const_pstilo_iterator<Node>,
                                        const_pislo_iterator<Node>,
                                        const_pilo_iterator<Node>>;

    WordGraph<node_type> const* _digraph;
    Order                       _order;
    size_type                   _max;
    size_type                   _min;
    node_type                   _source;
    node_type                   _target;
    mutable const_iterator      _current;
    mutable const_iterator      _end;
    mutable bool                _current_valid;

    bool set_iterator_no_checks() const;
    bool set_iterator() const;

   public:
    output_type get() const {
      set_iterator();
      return std::visit([](auto& it) -> auto const& { return *it; }, _current);
    }

    void next() {
      std::visit([](auto& it) { ++it; }, _current);
    }

    bool at_end() const {
      if (!set_iterator()) {
        return true;
      }
      return _current == _end;
    }

    uint64_t size_hint() const;
    uint64_t size_hint_no_checks() const;

    uint64_t count() const {
      return size_hint();
    }

    static constexpr bool is_finite     = true;  // this isn't always true!
    static constexpr bool is_idempotent = true;

    Paths() {
      init();
    }

    Paths& init();

    Paths(Paths const&)            = default;
    Paths(Paths&&)                 = default;
    Paths& operator=(Paths const&) = default;
    Paths& operator=(Paths&&)      = default;

    explicit Paths(WordGraph<Node> const& digraph) {
      init(digraph);
    }

    Paths& init(WordGraph<Node> const& digraph) {
      init();
      _digraph = &digraph;
      return *this;
    }

    Paths& from(node_type src) {
      return from(this, src);
    }

    [[nodiscard]] node_type from() const noexcept {
      return _source;
    }

    Paths& to(node_type trgt) noexcept {
      return to(this, trgt);
    }

    [[nodiscard]] node_type to() const;

    Paths& min(size_type val) noexcept {
      return min(this, val);
    }

    [[nodiscard]] size_type min() const noexcept {
      return _min;
    }

    Paths& max(size_type val) noexcept {
      return max(this, val);
    }

    [[nodiscard]] size_type max() const noexcept {
      return _max;
    }

    Paths& order(Order val) {
      return order(this, val);
    }

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

   private:
    void throw_if_word_graph_nullptr() const {
      if (_digraph == nullptr) {
        LIBSEMIGROUPS_EXCEPTION("No word graph defined");
      }
    }
  };

  template <typename Node>
  Paths(WordGraph<Node> const&) -> Paths<Node>;

  template <typename Node>
  Paths(WordGraph<Node>&&) -> Paths<Node>;

  template <typename Node>
  class ReversiblePaths : public Paths<Node> {
   private:
    bool _reverse;

    using size_type = typename WordGraph<Node>::size_type;

   public:
    // this isn't always true!
    static constexpr bool is_finite     = Paths<Node>::is_finite;
    static constexpr bool is_idempotent = Paths<Node>::is_idempotent;

    using output_type = word_type;

    ReversiblePaths() {
      init();
    }

    ReversiblePaths& init() {
      Paths<Node>::init();
      _reverse = false;
    }

    ReversiblePaths(ReversiblePaths const&)            = default;
    ReversiblePaths(ReversiblePaths&&)                 = default;
    ReversiblePaths& operator=(ReversiblePaths const&) = default;
    ReversiblePaths& operator=(ReversiblePaths&&)      = default;

    explicit ReversiblePaths(WordGraph<Node> const& digraph) {
      init(digraph);
    }

    ReversiblePaths& init(WordGraph<Node> const& digraph) {
      Paths<Node>::init(digraph);
      return *this;
    }

    ReversiblePaths& from(size_type val) noexcept {
      return Paths<Node>::from(this, val);
    }

    ReversiblePaths& to(size_type val) noexcept {
      return Paths<Node>::to(this, val);
    }

    ReversiblePaths& min(size_type val) noexcept {
      return Paths<Node>::min(this, val);
    }

    ReversiblePaths& max(size_type val) noexcept {
      return Paths<Node>::max(this, val);
    }

    ReversiblePaths& order(Order val) {
      return Paths<Node>::order(this, val);
    }

    ReversiblePaths& reverse(bool val) {
      _reverse = val;
      return *this;
    }

    output_type get() const {
      word_type result = Paths<Node>::get();
      if (_reverse) {
        std::reverse(result.begin(), result.end());
      }
      return result;
    }
  };

  template <typename Node>
  ReversiblePaths(WordGraph<Node> const&) -> ReversiblePaths<Node>;

  template <typename Node>
  ReversiblePaths(WordGraph<Node>&&) -> ReversiblePaths<Node>;

}  // namespace libsemigroups

#include "paths.tpp"

#endif  // LIBSEMIGROUPS_PATHS_HPP_
