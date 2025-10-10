//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

// This file contains declarations of the iterator classes for
// paths in an WordGraph.

#include <cstddef>      // for size_t, ptrdiff_t
#include <iterator>     // for forward_iterator_tag
#include <tuple>        // for tie
#include <type_traits>  // for integral_constant<>::value
#include <vector>       // for vector, vector<>::const...

#include "libsemigroups/constants.hpp"           // for UNDEFINED, Undefined
#include "libsemigroups/detail/iterator.hpp"     // for default_postfix_incre...
#include "libsemigroups/types.hpp"               // for word_type
#include "libsemigroups/word-graph-helpers.hpp"  // for word_graph
#include "libsemigroups/word-graph.hpp"

#ifndef LIBSEMIGROUPS_DETAIL_PATH_ITERATORS_HPP_
#define LIBSEMIGROUPS_DETAIL_PATH_ITERATORS_HPP_

namespace libsemigroups {
  namespace detail {
    ////////////////////////////////////////////////////////////////////////
    // pilo = Path And (terminal) Node In Lex Order
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    class const_pilo_iterator {
     public:
      using node_type       = Node;
      using label_type      = typename WordGraph<Node>::label_type;
      using value_type      = word_type;
      using size_type       = typename std::vector<value_type>::size_type;
      using difference_type = typename std::vector<value_type>::difference_type;
      using const_pointer   = typename std::vector<value_type>::const_pointer;
      using pointer         = typename std::vector<value_type>::pointer;
      using const_reference = typename std::vector<value_type>::const_reference;
      using reference       = const_reference;
      using iterator_category = std::forward_iterator_tag;

     private:
      value_type             _edges;
      WordGraph<Node> const* _word_graph;
      label_type             _edge;
      size_t                 _min;
      size_t                 _max;
      std::vector<node_type> _nodes;

     public:
      const_pilo_iterator() noexcept;
      const_pilo_iterator(const_pilo_iterator const&);
      const_pilo_iterator(const_pilo_iterator&&) noexcept;
      const_pilo_iterator& operator=(const_pilo_iterator const&);
      const_pilo_iterator& operator=(const_pilo_iterator&&) noexcept;
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

      // TODO(1) to tpp
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

      [[nodiscard]] WordGraph<Node> const& word_graph() const noexcept {
        return *_word_graph;
      }
    };

    template <typename Node>
    inline void swap(const_pilo_iterator<Node>& x,
                     const_pilo_iterator<Node>& y) noexcept {
      x.swap(y);
    }

    // Note that while the complexity of this is bad, it repeatedly does depth
    // first searches, and so will examine every node and edge of the graph
    // multiple times (if u -a-> v belongs to a path of length 10, then it will
    // be traversed 10 times). But the performance of this iterator is
    // dominated by memory allocation (when creating
    // iterators, at least), and so this doesn't seem that bad.
    template <typename Node>
    class const_pislo_iterator {
     public:
      using node_type       = Node;
      using value_type      = word_type;
      using size_type       = typename std::vector<value_type>::size_type;
      using difference_type = typename std::vector<value_type>::difference_type;
      using const_pointer   = typename std::vector<value_type>::const_pointer;
      using pointer         = typename std::vector<value_type>::pointer;
      using const_reference = typename std::vector<value_type>::const_reference;
      using reference       = const_reference;
      using iterator_category = std::forward_iterator_tag;

     private:
      size_type                         _length;
      detail::const_pilo_iterator<Node> _it;
      size_type                         _max;
      node_type                         _source;

     public:
      // None of the constructors are noexcept because the corresponding
      // constructors for detail::const_pilo_iterator are not.

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

      // noexcept because comparison of detail::const_pilo_iterator is noexcept
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
        if (_it == cend_pilo(_it.word_graph()) && _length == UNDEFINED) {
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

    // PSTILO = Path Source Target In Lex Order
    template <typename Node>
    class const_pstilo_iterator {
     public:
      using node_type       = Node;
      using label_type      = typename WordGraph<node_type>::label_type;
      using value_type      = word_type;
      using size_type       = typename std::vector<value_type>::size_type;
      using difference_type = typename std::vector<value_type>::difference_type;
      using const_pointer   = typename std::vector<value_type>::const_pointer;
      using pointer         = typename std::vector<value_type>::pointer;
      using const_reference = typename std::vector<value_type>::const_reference;
      using reference       = const_reference;
      using iterator_category = std::forward_iterator_tag;

     private:
      // TODO(1) maybe try replacing _can_reach_target with std::unordered_set,
      // then we can use the output of ancestors_of directly
      std::vector<bool>      _can_reach_target;
      value_type             _edges;
      WordGraph<Node> const* _word_graph;
      label_type             _edge;
      size_t                 _min;
      size_t                 _max;
      std::vector<node_type> _nodes;
      node_type              _target;

     public:
      const_pstilo_iterator() noexcept;
      const_pstilo_iterator(const_pstilo_iterator const&);
      const_pstilo_iterator(const_pstilo_iterator&&) noexcept;
      const_pstilo_iterator& operator=(const_pstilo_iterator const&);
      const_pstilo_iterator& operator=(const_pstilo_iterator&&) noexcept;
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
      size_t                             _count;
      detail::const_pislo_iterator<Node> _it;
      node_type                          _target;
      detail::const_pislo_iterator<Node> _end;  // TODO(2) remove?

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
          : _count(source == UNDEFINED
                       ? 0
                       : number_of_paths(*ptr, source, target, min, max)),
            _it(ptr, source, min, max),
            _target(target),
            _end(cend_pislo(*ptr)) {
        operator++();
      }

      // noexcept because comparison of detail::const_pilo_iterator is noexcept
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

  }  // namespace detail
}  // namespace libsemigroups

#include "path-iterators.tpp"
#endif  // LIBSEMIGROUPS_DETAIL_PATH_ITERATORS_HPP_
