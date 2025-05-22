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

  ////////////////////////////////////////////////////////////////////////
  // Node nested class
  ////////////////////////////////////////////////////////////////////////

  AhoCorasickImpl::Node& AhoCorasickImpl::Node::init(index_type  this_index,
                                                     index_type  parent,
                                                     letter_type a) noexcept {
    _parent        = parent;
    _parent_letter = a;
    _this_index    = this_index;
    _height        = UNDEFINED;
    clear_suffix_link();
    _terminal = false;
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
  }

  // TODO combine with constructor and private default constructor
  AhoCorasickImpl& AhoCorasickImpl::init(size_t num_letters) {
    _all_nodes = {Node()};
    _children.init(num_letters, 1, UNDEFINED);
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
  AhoCorasickImpl::traverse_no_checks(index_type current, letter_type a) const {
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
    size_t cached_height = _all_nodes[i].height();
    if (cached_height != UNDEFINED) {
      return cached_height;
    }

    if (i == root) {
      _all_nodes[i].height(0);
      return 0;
    }

    cached_height = height_no_checks(_all_nodes[i].parent()) + 1;
    _all_nodes[i].height(cached_height);
    return cached_height;
  }

  [[nodiscard]] AhoCorasickImpl::index_type
  AhoCorasickImpl::suffix_link_no_checks(index_type current) const {
    LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
    if (!_valid_links) {
      clear_suffix_links();
      _valid_links = true;
    }
    auto& n = _all_nodes[current];
    if (n.suffix_link() == UNDEFINED) {
      n.suffix_link(traverse_no_checks(suffix_link_no_checks(n.parent()),
                                       n.parent_letter()));
    }
    return n.suffix_link();
  }

  [[nodiscard]] AhoCorasickImpl::index_type
  AhoCorasickImpl::new_active_node_no_checks(index_type parent, letter_type a) {
    LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
    index_type index;
    if (_inactive_nodes_index.empty()) {
      index = _all_nodes.size();
      _all_nodes.emplace_back(index, parent, a);
      _active_nodes_index.insert(index);
      _children.add_rows(1);
    } else {
      index = _inactive_nodes_index.top();
      _inactive_nodes_index.pop();
      _active_nodes_index.insert(index);
      _all_nodes[index].init(index, parent, a);
      std::fill(
          _children.begin_row(index), _children.end_row(index), UNDEFINED);
    }
    return index;
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

  void AhoCorasickImpl::clear_suffix_links() const {
    // TODO(2) determine if it is more efficient to clear the link of all nodes,
    // or just the active ones
    for (auto node = _all_nodes.begin(); node != _all_nodes.end(); ++node) {
      node->clear_suffix_link();
    }
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

}  // namespace libsemigroups
