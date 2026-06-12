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

#include <algorithm>      // for count
#include <cstddef>        // for size_t, ptrdiff_t
#include <cstdint>        // for uint32_t
#include <iterator>       // for input_iterator_tag
#include <string>         // for basic_string, string
#include <type_traits>    // for decay_t, is_same_v
#include <unordered_set>  // for unordered_set
#include <utility>        // for pair
#include <vector>         // for vector

#include "libsemigroups/constants.hpp"  // for Undefined, operator!=, UNDEFINED, operator==
#include "libsemigroups/debug.hpp"      // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/ranges.hpp"     // for rx::iterator_range
#include "libsemigroups/types.hpp"      // for letter_type, word_type

#include "containers.hpp"  // DynamicArray2
#include "fmt.hpp"         // for format
#include "print.hpp"       // for to_printable

namespace libsemigroups {
  namespace detail {

    class Rule;  // forward decl

    // An AhoCorasickImpl<Value> object represents a hash map like container
    // (implemented using a trie), where the keys in the map must be
    // words consisting of letters in the range {0, ..., n - 1} for some n.
    class AhoCorasickImpl {
     public:
      using index_type = uint32_t;
      using terminal_node_const_iterator
          = std::unordered_set<index_type>::const_iterator;

      static constexpr const index_type root = 0;

     private:
      class Node {
        friend class AhoCorasickImpl;
        ////////////////////////////////////////////////////////////////////////
        // Private data
        ////////////////////////////////////////////////////////////////////////
       private:
        mutable size_t                 _generation;
        uint32_t                       _height;
        index_type                     _link;
        index_type                     _parent;
        letter_type                    _parent_letter;
        std::unordered_set<index_type> _suffix_link_sources;
        // FIXME copying an AhoCorasickImpl will copy the _value verbatim,
        // meaning that if an AhoCorasickImpl is
        // contains in a KnuthBendixImpl, then the Nodes in the AhoCorasickImpl
        // copy will point at the Rule in the original AhoCorasickImpl, and boom
        Rule const* _value;

        Node& init() noexcept {
          return init(UNDEFINED, UNDEFINED);
        }

        Node& init(index_type parent, letter_type a) noexcept;

       public:
        ////////////////////////////////////////////////////////////////////////
        // Constructors/initializers - public
        ////////////////////////////////////////////////////////////////////////

        Node() : Node(UNDEFINED, UNDEFINED) {}
        Node(index_type parent, letter_type a);

        Node(Node const&)            = default;
        Node& operator=(Node const&) = default;
        Node(Node&&)                 = default;
        Node& operator=(Node&&)      = default;

        ~Node() = default;

        ////////////////////////////////////////////////////////////////////////
        // Getters - public
        ////////////////////////////////////////////////////////////////////////

        [[nodiscard]] size_t generation() const noexcept {
          return _generation;
        }

        [[nodiscard]] size_t height() const noexcept {
          return _height;
        }

        [[nodiscard]] index_type suffix_link() const noexcept {
          return _link;
        }

        std::unordered_set<index_type>& suffix_link_sources() noexcept {
          return _suffix_link_sources;
        }

        [[nodiscard]] bool terminal() const noexcept {
          return _value != nullptr;
        }

        [[nodiscard]] index_type parent() const noexcept {
          return _parent;
        }

        [[nodiscard]] letter_type parent_letter() const noexcept {
          return _parent_letter;
        }

        [[nodiscard]] Rule const* value() const noexcept {
          LIBSEMIGROUPS_ASSERT(terminal());
          return _value;
        }

        ////////////////////////////////////////////////////////////////////////
        // Setters - public
        ////////////////////////////////////////////////////////////////////////

        Node const& generation(size_t val) const noexcept {
          _generation = val;
          return *this;
        }

       private:
        ////////////////////////////////////////////////////////////////////////
        // Setters - private
        ////////////////////////////////////////////////////////////////////////

        // All setters of non-mutable members are private to avoid corrupting
        // the objects.

        Node const& height(size_t val) noexcept {
          _height = val;
          return *this;
        }

        Node const& suffix_link(index_type val) noexcept {
          _link = val;
          return *this;
        }

        Node const& value(Rule const* value) {
          LIBSEMIGROUPS_ASSERT(value != nullptr);
          _value = value;
          return *this;
        }
      };  // class Node

      // TODO(1) if we store pointers here instead of Nodes, then inside the
      // Nodes themselves we could store pointers to the parents etc, rather
      // than indices, which would mean we could set the _link and other info in
      // the Node::init() function, also will remove the index <-> Node
      // conversions everywhere.
      std::vector<Node>                 _all_nodes;
      detail::DynamicArray2<index_type> _children;
      std::unordered_set<index_type>    _active_nodes_index;
      std::vector<index_type>           _inactive_nodes_index;
      std::vector<index_type>           _node_indices_to_update;
      std::unordered_set<index_type>    _terminal_nodes_index;
      mutable size_t                    _generation;

      // TODO(1): it seems likely that the positions of the active nodes in
      // _all_nodes will become scattered and disordered over time, and so it'd
      // probably be best to periodically (or maybe always?) compress, and sort
      // the nodes.

     public:
      ////////////////////////////////////////////////////////////////////////
      // Constructors + initializers
      ////////////////////////////////////////////////////////////////////////

      AhoCorasickImpl();
      AhoCorasickImpl& init();

      explicit AhoCorasickImpl(size_t num_letters);
      AhoCorasickImpl& init(size_t num_letters);

      AhoCorasickImpl(AhoCorasickImpl const&);
      AhoCorasickImpl& operator=(AhoCorasickImpl const&);
      AhoCorasickImpl(AhoCorasickImpl&&);
      AhoCorasickImpl& operator=(AhoCorasickImpl&&);

      ~AhoCorasickImpl();

      void increment_generation() const noexcept {
        ++_generation;
      }

      size_t generation() const noexcept {
        return _generation;
      }

      size_t alphabet_size() const noexcept {
        return _children.number_of_cols();
      }

      AhoCorasickImpl& increase_alphabet_size_by(size_t val);

      ////////////////////////////////////////////////////////////////////////
      // New API - somewhat similar mem fns to std::unordered_map
      ////////////////////////////////////////////////////////////////////////

      template <typename Iterator, typename... Args>
      std::pair<index_type, bool> emplace_no_checks(Iterator first,
                                                    Iterator last,
                                                    Rule const*);

      template <typename Iterator, typename... Args>
      std::pair<index_type, bool> emplace(Iterator first,
                                          Iterator last,
                                          Rule const*);

      template <typename Word>
      std::pair<index_type, bool> insert_no_checks(Word const& key,
                                                   Rule const* value) {
        return emplace_no_checks(key.begin(), key.end(), value);
      }

      template <typename Word>
      std::pair<index_type, bool> insert(Word const& key, Rule const* value) {
        return emplace(key.begin(), key.end(), value);
      }

      template <typename Word>
      index_type erase_no_checks(Word const& key);

      template <typename Word>
      index_type erase(Word const& key);

      // TODO(1) rename to begin and change return type to {key, val}, or
      // whatever std::unordered_map implements, or cbegin_keys() or something?
      [[nodiscard]] terminal_node_const_iterator cbegin_terminal_nodes() const {
        return _terminal_nodes_index.cbegin();
      }

      // TODO(1) rename to end and change return type to {key, val}, or whatever
      // std::unordered_map implements
      [[nodiscard]] terminal_node_const_iterator cend_terminal_nodes() const {
        return _terminal_nodes_index.cend();
      }

      // TODO(1) rename to items and change return type to {key, val}, or
      // whatever std::unordered_map implements
      [[nodiscard]] auto terminal_nodes() const {
        return rx::iterator_range(cbegin_terminal_nodes(),
                                  cend_terminal_nodes());
      }

      [[nodiscard]] bool empty() const noexcept {
        return number_of_nodes() == 1;
      }

      ////////////////////////////////////////////////////////////////////////
      // Trie API
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] size_t number_of_nodes() const noexcept {
        LIBSEMIGROUPS_ASSERT(_children.number_of_rows() == _all_nodes.size());
        return _active_nodes_index.size();
      }

      [[nodiscard]] Node const& node_no_checks(index_type i) const {
        LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
        return _all_nodes[i];
      }

      [[nodiscard]] Node const& node(index_type i) const {
        throw_if_node_index_out_of_range(i);
        return node_no_checks(i);
      }

      // The following function is critical for KnuthBendix and so we leave it
      // here to be inlined possibly.
      [[nodiscard]] index_type traverse_no_checks(index_type  current,
                                                  letter_type a) const {
        LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
        index_type next = _children.get(current, a);
        if (next != UNDEFINED) {
          return next;
        } else if (current == root) {
          return root;
        }
        return traverse_no_checks(node_no_checks(current).suffix_link(), a);
      }

      [[nodiscard]] index_type traverse(index_type  current,
                                        letter_type a) const {
        throw_if_node_index_not_active(current);
        return traverse_no_checks(current, a);
      }

      // TODO(1) could wrap this into Node (access _children from inside Node?)
      [[nodiscard]] index_type child_no_checks(index_type  parent,
                                               letter_type letter) const {
        LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
        LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
        return _children.get(parent, letter);
      }

      // TODO(1) could wrap this into Node (access _children from inside Node?)
      [[nodiscard]] index_type child(index_type  parent,
                                     letter_type letter) const {
        throw_if_node_index_not_active(parent);
        return child_no_checks(parent, letter);
      }

      // TODO(1) could wrap this into Node (access _children from inside Node?)
      [[nodiscard]] size_t
      number_of_children_no_checks(index_type i) const noexcept {
        return _children.number_of_cols()
               - std::count(
                   _children.cbegin_row(i), _children.cend_row(i), UNDEFINED);
      }

      // TODO(1) could wrap this into Node (access _children from inside Node?)
      [[nodiscard]] size_t number_of_children(index_type i) const noexcept {
        throw_if_node_index_not_active(i);
        return number_of_children_no_checks(i);
      }

      ////////////////////////////////////////////////////////////////////////
      // Public exceptions
      ////////////////////////////////////////////////////////////////////////

      void throw_if_node_index_out_of_range(index_type i) const;
      void throw_if_node_index_not_active(index_type i) const;

     private:
      template <typename Iterator>
      [[nodiscard]] index_type
      traverse_trie_no_suffix_links_no_checks(Iterator first,
                                              Iterator last) const;

      template <typename Iterator>
      [[nodiscard]] index_type
      traverse_trie_no_suffix_links(Iterator first, Iterator last) const;

      ////////////////////////////////////////////////////////////////////////
      // Activate or deactivate a node
      ////////////////////////////////////////////////////////////////////////

      [[nodiscard]] bool is_active_node(index_type i) {
        return _active_nodes_index.find(i) != _active_nodes_index.end();
      }

      [[nodiscard]] index_type new_active_node_no_checks(index_type  parent,
                                                         letter_type a);

      void deactivate_node_no_checks(index_type i);

      ////////////////////////////////////////////////////////////////////////
      // Update suffix link sources
      ////////////////////////////////////////////////////////////////////////

      // Add <source_index> as a suffix link source of <target_index>, i.e.
      // _all_nodes[source_index].suffix_link() == target_index
      void add_suffix_link_source(index_type source_index,
                                  index_type target_index);

      // Remove <source_index> as a suffix link source of <target_index>, i.e.
      // _all_nodes[source_index].suffix_link() == target_index
      void rm_suffix_link_source(index_type source_index,
                                 index_type target_index);

      void populate_node_indices_to_update(index_type  target_index,
                                           index_type  new_node_index,
                                           letter_type a);
    };  // class AhoCorasickImpl

    namespace aho_corasick_impl {
      template <typename Iterator>
      typename AhoCorasickImpl::index_type
      traverse_trie_no_checks(AhoCorasickImpl const& ac,
                              Iterator               first,
                              Iterator               last);

      template <typename Iterator>
      typename AhoCorasickImpl::index_type
      traverse_trie(AhoCorasickImpl const& ac, Iterator first, Iterator last);

      template <typename Word>
      [[nodiscard]] typename AhoCorasickImpl::index_type
      traverse_trie_no_checks(AhoCorasickImpl& ac, Word const& w) {
        return traverse_trie_no_checks(ac, w.begin(), w.end());
      }

      template <typename Word>
      [[nodiscard]] typename AhoCorasickImpl::index_type
      traverse_trie(AhoCorasickImpl& ac, Word const& w) {
        return traverse_trie(ac, w.begin(), w.end());
      }

      template <typename Iterator>
      [[nodiscard]] auto begin_search_no_checks(AhoCorasickImpl const& ac,
                                                Iterator               first,
                                                Iterator               last);

      template <typename Iterator>
      [[nodiscard]] auto end_search_no_checks(AhoCorasickImpl const& ac,
                                              Iterator,
                                              Iterator);

      template <typename Word>
      [[nodiscard]] auto begin_search_no_checks(AhoCorasickImpl const& ac,
                                                Word const&            w);

      template <typename Word>
      [[nodiscard]] auto end_search_no_checks(AhoCorasickImpl const& ac,
                                              Word const&            w);

    }  // namespace aho_corasick_impl
  }    // namespace detail
}  // namespace libsemigroups

#include "aho-corasick-impl.tpp"

#endif  // LIBSEMIGROUPS_DETAIL_AHO_CORASICK_IMPL_HPP_
