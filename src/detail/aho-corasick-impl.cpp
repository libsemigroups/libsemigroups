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

// This file contains the implementation of the AhoCorasickImpl class.
#include "libsemigroups/detail/aho-corasick-impl.hpp"

#include <algorithm>    // for max, copy, reverse
#include <array>        // for array
#include <string>       // for basic_string, string, to_string
#include <string_view>  // for basic_string_view, string_view

#include "libsemigroups/constants.hpp"   // for Undefined, UNDEFINED, operator==
#include "libsemigroups/debug.hpp"       // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/detail/fmt.hpp"  // for format
#include "libsemigroups/dot.hpp"         // for Dot, Dot::Edge, Dot::Node
#include "libsemigroups/exception.hpp"   // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/types.hpp"       // for word_type, letter_type

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // Node nested class
    ////////////////////////////////////////////////////////////////////////

    AhoCorasickImpl::Node& AhoCorasickImpl::Node::init(index_type  parent,
                                                       letter_type a) noexcept {
      // mutable TODO remove mutability here
      _height = parent == UNDEFINED ? 0 : UNDEFINED;
      clear_suffix_link();

      // non-mutable
      _first_suffix_link_source   = UNDEFINED;
      _next_node_same_suffix_link = UNDEFINED;
      _parent                     = parent;
      _parent_letter              = a;
      _terminal                   = false;

      // Cannot set _link or _height here because we don't have access to the
      // relevant info here.

      return *this;
    }

    // TODO why const?
    void AhoCorasickImpl::Node::clear_suffix_link() const noexcept {
      if (parent() == root || parent() == UNDEFINED) {
        suffix_link(root);
      } else {
        suffix_link(UNDEFINED);
      }
    }

    ////////////////////////////////////////////////////////////////////////
    // AhoCorasickImplImpl class
    ////////////////////////////////////////////////////////////////////////

    AhoCorasickImpl::AhoCorasickImpl() : AhoCorasickImpl(0) {}

    AhoCorasickImpl& AhoCorasickImpl::init() {
      init(0);
      return *this;
    }

    AhoCorasickImpl::AhoCorasickImpl(size_t num_letters)
        : _all_nodes({Node()}),
          _children(num_letters, 1, UNDEFINED),
          _active_nodes_index(),
          _inactive_nodes_index(),
          _valid_links(true) {
      _active_nodes_index.insert(0);
      // TODO use init, and don't dupl code
    }

    AhoCorasickImpl& AhoCorasickImpl::init(size_t num_letters) {
      _all_nodes = {Node()};
      _children.init(num_letters, 1, UNDEFINED);
      // TODO maybe better to just deactivate all the nodes?
      _active_nodes_index.clear();
      _active_nodes_index.insert(0);
      while (!_inactive_nodes_index.empty()) {
        _inactive_nodes_index.pop();
      }
      _valid_links = true;
      return *this;
    }

    AhoCorasickImpl::~AhoCorasickImpl() = default;

    [[nodiscard]] AhoCorasickImpl::index_type
    AhoCorasickImpl::traverse_no_checks(index_type  current,
                                        letter_type a) const {
      LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
      index_type next = _children.get(current, a);
      if (next != UNDEFINED) {
        return next;
      } else if (current == root) {
        return root;
      }
      return traverse_no_checks(suffix_link_no_checks(current), a);
    }

    [[nodiscard]] size_t AhoCorasickImpl::height_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
      return _all_nodes[i].height();
    }

    [[nodiscard]] AhoCorasickImpl::index_type
    AhoCorasickImpl::suffix_link_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
      return _all_nodes[i].suffix_link();
    }

    [[nodiscard]] AhoCorasickImpl::index_type
    AhoCorasickImpl::new_active_node_no_checks(index_type  parent_index,
                                               letter_type a) {
      LIBSEMIGROUPS_ASSERT(parent_index < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent_index) == 1);

      index_type new_node_index;
      if (_inactive_nodes_index.empty()) {
        new_node_index = _all_nodes.size();
        _all_nodes.emplace_back(parent_index, a);
        _active_nodes_index.insert(new_node_index);
        _children.add_rows(1);
      } else {
        new_node_index = _inactive_nodes_index.top();
        _inactive_nodes_index.pop();
        _active_nodes_index.insert(new_node_index);
        _all_nodes[new_node_index].init(parent_index, a);
        std::fill(_children.begin_row(new_node_index),
                  _children.end_row(new_node_index),
                  UNDEFINED);
      }

      // Set the suffix link and height of new node
      auto&      new_node   = _all_nodes[new_node_index];
      index_type link_index = traverse_no_checks(
          suffix_link_no_checks(new_node.parent()), new_node.parent_letter());
      LIBSEMIGROUPS_ASSERT(link_index != UNDEFINED);
      new_node.suffix_link(link_index);
      new_node.height(_all_nodes[new_node.parent()].height() + 1);

      update_suffix_link_sources(parent_index, new_node_index, a);

      // Add new node as a source of its suffix link
      add_suffix_link_source(new_node_index, link_index);

      return new_node_index;
    }

    void AhoCorasickImpl::deactivate_node_no_checks(index_type i) {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
#ifdef LIBSEMIGROUPS_DEBUG
      auto num_removed = _active_nodes_index.erase(i);
      (void) num_removed;
      LIBSEMIGROUPS_ASSERT(num_removed == 1);
#else
      _active_nodes_index.erase(i);
#endif
      _inactive_nodes_index.push(i);
    }

    void AhoCorasickImpl::throw_if_node_index_out_of_range(index_type i) const {
      if (i >= _all_nodes.size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid index, expected value in range [0, {}), found {}",
            _all_nodes.size(),
            i);
      }
    }

    void AhoCorasickImpl::throw_if_node_index_not_active(index_type i) const {
      throw_if_node_index_out_of_range(i);
      if (_active_nodes_index.count(i) != 1) {
        LIBSEMIGROUPS_EXCEPTION(
            "invalid index, expected an index of an active node, found {}", i);
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
