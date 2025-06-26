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

    AhoCorasickImpl::Node::Node(index_type parent, letter_type a)
        : _first_suffix_link_source(),
          _height(),
          _link(),
          _next_node_same_suffix_link(),
          _parent(),
          _parent_letter(),
          _terminal() {
      init(parent, a);
    }

    AhoCorasickImpl::Node& AhoCorasickImpl::Node::init(index_type  i,
                                                       letter_type a) noexcept {
      _first_suffix_link_source = UNDEFINED;
      _height                   = i == UNDEFINED ? 0 : UNDEFINED;
      if (_parent == root || _parent == UNDEFINED) {
        _link = root;
      } else {
        _link = UNDEFINED;
      }
      _next_node_same_suffix_link = UNDEFINED;
      _parent                     = i;
      _parent_letter              = a;
      _terminal                   = false;

      // Cannot set _link or _height here because we don't have access to the
      // relevant info here.
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // AhoCorasickImplImpl class
    ////////////////////////////////////////////////////////////////////////

    AhoCorasickImpl::AhoCorasickImpl()
        : _all_nodes({Node()}),
          _children(0, 1, UNDEFINED),
          _active_nodes_index(),
          _inactive_nodes_index() {}

    AhoCorasickImpl& AhoCorasickImpl::init() {
      init(0);
      return *this;
    }

    AhoCorasickImpl::AhoCorasickImpl(size_t num_letters)
        : _all_nodes({Node()}),
          _children(num_letters, 1, UNDEFINED),
          _active_nodes_index(),
          _inactive_nodes_index() {
      _active_nodes_index.insert(0);
    }

    AhoCorasickImpl& AhoCorasickImpl::init(size_t num_letters) {
      _all_nodes = {Node()};
      _children.init(num_letters, 1, UNDEFINED);
      _active_nodes_index.clear();
      _active_nodes_index.insert(0);
      while (!_inactive_nodes_index.empty()) {
        _inactive_nodes_index.pop();
      }

      // TODO maybe better to just deactivate all the nodes, the following
      // causes a seg fault for some reason.
      // _children.init(num_letters, _all_nodes.size(), UNDEFINED);
      // for (auto i : _active_nodes_index) {
      //   if (i != 0) {
      //     _inactive_nodes_index.push(i);
      //   }
      // }
      // _active_nodes_index.clear();
      // _active_nodes_index.insert(0);
      // LIBSEMIGROUPS_ASSERT(_active_nodes_index.size()
      //                          + _inactive_nodes_index.size()
      //                      == _all_nodes.size());
      // LIBSEMIGROUPS_ASSERT(_children.number_of_rows() ==
      // _all_nodes.size());
      return *this;
    }

    AhoCorasickImpl::~AhoCorasickImpl() = default;

    AhoCorasickImpl& AhoCorasickImpl::increase_alphabet_size_by(size_t val) {
      size_t c = _children.number_of_cols();
      _children.add_cols(val);
      for (; c < _children.number_of_cols(); ++c) {
        std::fill(
            _children.begin_column(c), _children.end_column(c), UNDEFINED);
      }
      return *this;
    }

    [[nodiscard]] size_t AhoCorasickImpl::height_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
      return _all_nodes[i].height();
    }

    [[nodiscard]] AhoCorasickImpl::index_type
    AhoCorasickImpl::new_active_node_no_checks(index_type  parent_index,
                                               letter_type a) {
      LIBSEMIGROUPS_ASSERT(parent_index < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent_index) == 1);

      index_type new_node_index;
      if (_inactive_nodes_index.empty()) {
        new_node_index = _all_nodes.size();
        _all_nodes.resize(2 * _all_nodes.size());
        _all_nodes[new_node_index].init(parent_index, a);
        _active_nodes_index.insert(new_node_index);
        for (index_type i = new_node_index + 1; i != _all_nodes.size(); ++i) {
          _inactive_nodes_index.push(i);
        }
        _children.add_rows(_all_nodes.size());
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

      _node_indices_to_update.clear();
      populate_node_indices_to_update(parent_index, new_node_index, a);
      for (index_type node_index : _node_indices_to_update) {
        auto& node = _all_nodes[node_index];
        LIBSEMIGROUPS_ASSERT(node_index != new_node_index);
        LIBSEMIGROUPS_ASSERT(node.suffix_link() != new_node_index);
        rm_suffix_link_source(node_index, node.suffix_link());
        node.suffix_link(new_node_index);
        add_suffix_link_source(node_index, new_node_index);
      }

      // Add new node as a source of its suffix link
      add_suffix_link_source(new_node_index, link_index);

      return new_node_index;
    }

    void AhoCorasickImpl::deactivate_node_no_checks(index_type i) {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      // For each active suffix link source <current_source> of <i>, push
      // <current_source> to the vector of nodes which need to have their
      // suffix link updated.
      index_type current_source_index
          = _all_nodes[i].first_suffix_link_source();
      while (current_source_index != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                             == i);
        if (is_active_node(current_source_index)) {
          _node_indices_to_update.push_back(current_source_index);
        }
        current_source_index
            = _all_nodes[current_source_index].next_node_same_suffix_link();
      }
      rm_suffix_link_source(i, _all_nodes[i].suffix_link());

      // Make the node inactive
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

    void AhoCorasickImpl::throw_if_letter_out_of_range(index_type i) const {
      if (i >= alphabet_size()) {
        LIBSEMIGROUPS_EXCEPTION(
            "expected a value [0, {}), found {}", alphabet_size(), i);
      }
    }

    // Add <source_index> as a suffix link source of <target_index>, i.e.
    // _all_nodes[source_index].suffix_link() == target_index
    void AhoCorasickImpl::add_suffix_link_source(index_type source_index,
                                                 index_type target_index) {
      LIBSEMIGROUPS_ASSERT(source_index != target_index);
      auto& source = _all_nodes[source_index];
      auto& target = _all_nodes[target_index];
      LIBSEMIGROUPS_ASSERT(source_index != target.first_suffix_link_source());
      source.next_node_same_suffix_link(target.first_suffix_link_source());
      target.first_suffix_link_source(source_index);
    }

    // Remove <source_index> as a suffix link source of <target_index>, i.e.
    // _all_nodes[source_index].suffix_link() == target_index
    void AhoCorasickImpl::rm_suffix_link_source(index_type source_index,
                                                index_type target_index) {
      auto& target = _all_nodes[target_index];
      if (target.first_suffix_link_source() == source_index) {
        target.first_suffix_link_source(
            _all_nodes[source_index].next_node_same_suffix_link());
      } else {
        index_type current_source_index = target.first_suffix_link_source();
        index_type prev_source_index;
        LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                             == target_index);
        do {
          prev_source_index = current_source_index;
          current_source_index
              = _all_nodes[current_source_index].next_node_same_suffix_link();
          // The next assertion asserts that source_index is in fact a suffix
          // link source of target_index.
          LIBSEMIGROUPS_ASSERT(current_source_index != UNDEFINED);

          LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                               == target_index);
        } while (current_source_index != source_index);

        index_type new_next_index
            = _all_nodes[current_source_index].next_node_same_suffix_link();

        LIBSEMIGROUPS_ASSERT(prev_source_index != new_next_index);
        _all_nodes[prev_source_index].next_node_same_suffix_link(
            new_next_index);
      }
    }

    void
    AhoCorasickImpl::populate_node_indices_to_update(index_type  target_index,
                                                     index_type  new_node_index,
                                                     letter_type a) {
      index_type current_source_index
          = _all_nodes[target_index].first_suffix_link_source();

      LIBSEMIGROUPS_ASSERT(current_source_index != new_node_index);
      while (current_source_index != UNDEFINED) {
        LIBSEMIGROUPS_ASSERT(current_source_index != new_node_index);
        index_type child_index = _children.get(current_source_index, a);
        if (child_index == UNDEFINED) {
          populate_node_indices_to_update(
              current_source_index, new_node_index, a);
        } else {
          _node_indices_to_update.push_back(child_index);
        }
        current_source_index
            = _all_nodes[current_source_index].next_node_same_suffix_link();
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
