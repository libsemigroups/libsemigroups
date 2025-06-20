//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

// This file contains the implementation of a trie with suffix links for use by
// the Aho-Corasick dictionary search algorithm

#ifndef LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
#define LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_

#include <memory>         // for allocator_traits<>::value_type
#include <set>            // for set
#include <stack>          // for stack
#include <stddef.h>       // for size_t
#include <string>         // for string
#include <unordered_map>  // for unordered_map
#include <vector>         // for vector

#include "libsemigroups/constants.hpp"  // for Undefined, operator!=, UNDEFINED, operator==
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/ranges.hpp"     // for rx::iterator_range
#include "libsemigroups/types.hpp"      // for letter_type, word_type

#include "containers.hpp"  // DynamicArray2

// TODO(2) make nodes accessible as indices of some list (numbered nodes).
// Make sure this address the badness of traversals (lots of different data
// types and it just feels a bit hacky)
// TODO(2) is it worthwhile storing a pointer to the terminal nodes beneath
// each node? If this can be updated quickly, it would save a lot of time in
// overlap/confluence checking. One compromise is to have a pointer to the rules
// any given node is contained within. This could be updated easily when adding
// new rules, but more care would be needed when removing rules.
// TODO(1) change names from set_X and get_X to X(val) and X(). e.g.
// set_suffix_link('a') -> suffix_link('a')
// TODO(2) add something that gets a ranges element to find all terminal nodes.
// TODO(2) change all_nodes[i] to node_no_checks(i);

namespace libsemigroups {
  namespace detail {

    class AhoCorasickImpl {
     public:
      using index_type = size_t;

      static constexpr const index_type root = 0;

     private:
      //  Implements the node of a trie
      class Node {
       private:
        mutable index_type _link;
        mutable size_t     _height;

        index_type  _parent;
        letter_type _parent_letter;
        bool        _terminal;
        index_type  _this_index;

       public:
        Node() : Node(UNDEFINED, UNDEFINED, UNDEFINED) {}

        // TODO to cpp
        Node(index_type this_index, index_type parent, letter_type a)
            : _link(),
              _height(),

              _parent(),
              _parent_letter(),
              _terminal(),
              _this_index() {
          init(this_index, parent, a);
        }

        Node(const Node&)            = default;
        Node& operator=(const Node&) = default;
        Node(Node&&)                 = default;
        Node& operator=(Node&&)      = default;

        Node& init(index_type  this_index,
                   index_type  parent,
                   letter_type a) noexcept;

        Node& init() noexcept {
          return init(UNDEFINED, UNDEFINED, UNDEFINED);
        }

        [[nodiscard]] size_t height() const noexcept {
          return _height;
        }

        [[nodiscard]] index_type suffix_link() const noexcept {
          return _link;
        }

        Node const& suffix_link(index_type val) const noexcept {
          _link = val;
          return *this;
        }

        void clear_suffix_link() const noexcept;

        [[nodiscard]] index_type index() const noexcept {
          return _this_index;
        }

        [[nodiscard]] bool is_terminal() const noexcept {
          return _terminal;
        }

        Node& terminal(bool val) noexcept {
          _terminal = val;
          return *this;
        }

        [[nodiscard]] index_type parent() const noexcept {
          return _parent;
        }

        [[nodiscard]] letter_type parent_letter() const noexcept {
          return _parent_letter;
        }

        void height(size_t val) const noexcept {
          _height = val;
        }
      };  // class Node

      std::vector<Node>                 _all_nodes;
      detail::DynamicArray2<index_type> _children;
      std::set<index_type>              _active_nodes_index;
      std::stack<index_type>            _inactive_nodes_index;

      mutable bool _valid_links;

     public:
      AhoCorasickImpl();
      AhoCorasickImpl& init();

      explicit AhoCorasickImpl(size_t num_letters);
      AhoCorasickImpl& init(size_t num_letters);

      AhoCorasickImpl(AhoCorasickImpl const&) = default;

      AhoCorasickImpl& operator=(AhoCorasickImpl const&) = default;

      AhoCorasickImpl(AhoCorasickImpl&&) = default;

      AhoCorasickImpl& operator=(AhoCorasickImpl&&) = default;

      ~AhoCorasickImpl();

      size_t alphabet_size() const noexcept {
        return _children.number_of_cols();
      }

      AhoCorasickImpl& increase_alphabet_size_by(size_t val) {
        size_t c = _children.number_of_cols();
        _children.add_cols(val);
        for (; c < _children.number_of_cols(); ++c) {
          std::fill(
              _children.begin_column(c), _children.end_column(c), UNDEFINED);
        }
        return *this;
      }

      [[nodiscard]] size_t number_of_nodes() const noexcept {
        LIBSEMIGROUPS_ASSERT(_children.number_of_rows()
                             == _active_nodes_index.size());
        return _active_nodes_index.size();
      }

      [[nodiscard]] rx::iterator_range<std::set<index_type>::const_iterator>
      active_nodes() const {
        return rx::iterator_range(cbegin_nodes(), cend_nodes());
      }

      [[nodiscard]] std::set<index_type>::const_iterator
      cbegin_nodes() const noexcept {
        return _active_nodes_index.cbegin();
      }

      [[nodiscard]] std::set<index_type>::const_iterator
      cend_nodes() const noexcept {
        return _active_nodes_index.cend();
      }

      [[nodiscard]] std::set<index_type>::iterator
      begin_nodes() const noexcept {
        return _active_nodes_index.begin();
      }

      [[nodiscard]] std::set<index_type>::iterator end_nodes() const noexcept {
        return _active_nodes_index.end();
      }

      template <typename Iterator>
      index_type add_word(Iterator first, Iterator last);

      template <typename Iterator>
      index_type add_word_no_checks(Iterator first, Iterator last);

      template <typename Iterator>
      index_type rm_word(Iterator first, Iterator last);

      template <typename Iterator>
      index_type rm_word_no_checks(Iterator first, Iterator last);

      // TODO(2) Should this be templated?
      [[nodiscard]] index_type traverse_no_checks(index_type  current,
                                                  letter_type a) const;

      [[nodiscard]] index_type traverse(index_type  current,
                                        letter_type a) const {
        throw_if_node_index_not_active(current);
        return traverse_no_checks(current, a);
      }

      [[nodiscard]] size_t height_no_checks(index_type i) const;

      [[nodiscard]] size_t height(index_type i) const {
        throw_if_node_index_not_active(i);
        return height_no_checks(i);
      }

      [[nodiscard]] index_type suffix_link_no_checks(index_type current) const;

      [[nodiscard]] index_type suffix_link(index_type current) const {
        throw_if_node_index_not_active(current);
        return suffix_link_no_checks(current);
      }

      [[nodiscard]] Node const& node_no_checks(index_type i) const {
        LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
        return _all_nodes[i];
      }

      [[nodiscard]] Node const& node(index_type i) const {
        throw_if_node_index_out_of_range(i);
        return node_no_checks(i);
      }

      [[nodiscard]] index_type child_no_checks(index_type  parent,
                                               letter_type letter) const {
        LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
        return _children.get(parent, letter);
      }

      [[nodiscard]] index_type child(index_type  parent,
                                     letter_type letter) const {
        throw_if_node_index_not_active(parent);
        return child_no_checks(parent, letter);
      }

      void throw_if_node_index_out_of_range(index_type i) const;

      void throw_if_node_index_not_active(index_type i) const;

      // TODO add no_checks.
      [[nodiscard]] size_t number_of_children(index_type i) const noexcept {
        return _children.number_of_cols()
               - std::count(
                   _children.cbegin_row(i), _children.cend_row(i), UNDEFINED);
      }

      template <typename Iterator>
      [[nodiscard]] index_type traverse_trie(Iterator first,
                                             Iterator last) const;

     private:
      [[nodiscard]] index_type new_active_node_no_checks(index_type  parent,
                                                         letter_type a);

      [[nodiscard]] index_type new_active_node(index_type  parent,
                                               letter_type a) {
        throw_if_node_index_not_active(parent);
        return new_active_node_no_checks(parent, a);
      }

      void deactivate_node_no_checks(index_type i);

      void deactivate_node(index_type i) {
        throw_if_node_index_not_active(i);
        deactivate_node_no_checks(i);
      }

      void clear_suffix_links() const;
    };
  }  // namespace detail
}  // namespace libsemigroups

#include "aho-corasick-impl.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
