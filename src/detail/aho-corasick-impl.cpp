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
        : _height(), _link(), _parent(), _parent_letter(), _terminal() {
      init(parent, a);
    }

    AhoCorasickImpl::Node& AhoCorasickImpl::Node::init(index_type  i,
                                                       letter_type a) noexcept {
      _height = i == UNDEFINED ? 0 : UNDEFINED;
      if (_parent == root || _parent == UNDEFINED) {
        _link = root;
      } else {
        _link = UNDEFINED;
      }
      _parent        = i;
      _parent_letter = a;
      _terminal      = false;
      _suffix_link_sources.clear();

      // Cannot set _link or _height here because we don't have access to the
      // relevant info here.
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // AhoCorasickImpl class
    ////////////////////////////////////////////////////////////////////////

    AhoCorasickImpl::AhoCorasickImpl()
        : _all_nodes({Node()}),
          _children(0, 1, UNDEFINED),
          _active_nodes_index({root}),
          _inactive_nodes_index(),
          _node_indices_to_update() {}

    AhoCorasickImpl& AhoCorasickImpl::init() {
      init(0);
      return *this;
    }

    AhoCorasickImpl::AhoCorasickImpl(AhoCorasickImpl const&) = default;
    AhoCorasickImpl::AhoCorasickImpl(AhoCorasickImpl&&)      = default;

    AhoCorasickImpl& AhoCorasickImpl::operator=(AhoCorasickImpl const&)
        = default;
    AhoCorasickImpl& AhoCorasickImpl::operator=(AhoCorasickImpl&&) = default;

    AhoCorasickImpl::AhoCorasickImpl(size_t num_letters)
        : _all_nodes({Node()}),
          _children(num_letters, 1, UNDEFINED),
          _active_nodes_index({root}),
          _inactive_nodes_index(),
          _node_indices_to_update() {}

    AhoCorasickImpl& AhoCorasickImpl::init(size_t num_letters) {
      LIBSEMIGROUPS_ASSERT(!_all_nodes.empty());
      LIBSEMIGROUPS_ASSERT(!_active_nodes_index.empty());

      _children.init(num_letters, _all_nodes.size(), UNDEFINED);
      size_t const old_num_inactive_nodes = _inactive_nodes_index.size();
      _inactive_nodes_index.resize(old_num_inactive_nodes
                                   + _active_nodes_index.size() - 1);
      std::copy_if(_active_nodes_index.begin(),
                   _active_nodes_index.end(),
                   _inactive_nodes_index.begin() + old_num_inactive_nodes,
                   [](auto val) { return val != root; });
      std::sort(_inactive_nodes_index.begin(),
                _inactive_nodes_index.end(),
                std::greater{});
      _active_nodes_index.clear();
      _active_nodes_index.insert(root);
      _all_nodes[0].init();
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.size()
                               + _inactive_nodes_index.size()
                           == _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_children.number_of_rows() == _all_nodes.size());

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

    [[nodiscard]] bool AhoCorasickImpl::terminal_no_checks(index_type i) const {
      LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
      return _all_nodes[i].terminal();
    }

    [[nodiscard]] AhoCorasickImpl::index_type
    AhoCorasickImpl::new_active_node_no_checks(index_type  parent_index,
                                               letter_type a) {
      LIBSEMIGROUPS_ASSERT(parent_index < _all_nodes.size());
      LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent_index) == 1);

      if (_inactive_nodes_index.empty()) {
        size_t const old_nodes_size         = _all_nodes.size();
        size_t const old_num_inactive_nodes = _inactive_nodes_index.size();

        _all_nodes.resize(2 * old_nodes_size);
        _inactive_nodes_index.resize(old_num_inactive_nodes + old_nodes_size);
        std::iota(_inactive_nodes_index.begin() + old_num_inactive_nodes,
                  _inactive_nodes_index.end(),
                  index_type(old_nodes_size));
        std::sort(_inactive_nodes_index.begin(),
                  _inactive_nodes_index.end(),
                  std::greater{});
        _children.add_rows(old_nodes_size);
        return new_active_node_no_checks(parent_index, a);
      }

      index_type new_node_index = _inactive_nodes_index.back();
      _inactive_nodes_index.pop_back();
      _active_nodes_index.insert(new_node_index);
      _all_nodes[new_node_index].init(parent_index, a);
      std::fill(_children.begin_row(new_node_index),
                _children.end_row(new_node_index),
                UNDEFINED);

      // Set the suffix link and height of new node
      auto&      new_node   = _all_nodes[new_node_index];
      index_type link_index = traverse_no_checks(
          suffix_link_no_checks(new_node.parent()), new_node.parent_letter());
      LIBSEMIGROUPS_ASSERT(link_index != UNDEFINED);
      new_node.suffix_link(link_index);
      new_node.height(_all_nodes[new_node.parent()].height() + 1);

      // We have to collect the node indices to update, and then update them,
      // because we must traverse the suffix link sources here, and so we cannot
      // change them at the same time.
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
      // set new_node_index as child of parent
      _children.set(parent_index, a, new_node_index);

      return new_node_index;
    }

    void AhoCorasickImpl::deactivate_node_no_checks(index_type node_index) {
      LIBSEMIGROUPS_ASSERT(node_index < _all_nodes.size());
      // For each active suffix link source <current_source> of <node_index>,
      // push <current_source> to the vector of nodes which need to have their
      // suffix link updated.
      auto& node = _all_nodes[node_index];
      for (auto current_source_index : node.suffix_link_sources()) {
        LIBSEMIGROUPS_ASSERT(_all_nodes[current_source_index].suffix_link()
                             == node_index);
        if (is_active_node(current_source_index)) {
          auto&      current_source    = _all_nodes[current_source_index];
          index_type suffix_link_index = current_source.suffix_link();
          index_type next_suffix_link_index
              = _all_nodes[suffix_link_index].suffix_link();
          while (!is_active_node(next_suffix_link_index)) {
            suffix_link_index = next_suffix_link_index;
            next_suffix_link_index
                = _all_nodes[next_suffix_link_index].suffix_link();
          }
          current_source.suffix_link(next_suffix_link_index);
          add_suffix_link_source(current_source_index, next_suffix_link_index);
        }
      }
      rm_suffix_link_source(node_index, _all_nodes[node_index].suffix_link());

#ifdef LIBSEMIGROUPS_DEBUG
      auto num_removed =
#endif
          _active_nodes_index.erase(node_index);
      LIBSEMIGROUPS_ASSERT(num_removed == 1);
      _inactive_nodes_index.push_back(node_index);
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
#ifdef LIBSEMIGROUPS_DEBUG
      auto [it, inserted] =
#endif
          _all_nodes[target_index].suffix_link_sources().insert(source_index);
      LIBSEMIGROUPS_ASSERT(inserted);
    }

    // Remove <source_index> as a suffix link source of <target_index>, i.e.
    // _all_nodes[source_index].suffix_link() == target_index
    void AhoCorasickImpl::rm_suffix_link_source(index_type source_index,
                                                index_type target_index) {
      LIBSEMIGROUPS_ASSERT(source_index != target_index);

#ifdef LIBSEMIGROUPS_DEBUG
      auto num_erased =
#endif
          _all_nodes[target_index]._suffix_link_sources.erase(source_index);
      LIBSEMIGROUPS_ASSERT(num_erased == 1);
    }

    void
    AhoCorasickImpl::populate_node_indices_to_update(index_type  target_index,
                                                     index_type  new_node_index,
                                                     letter_type a) {
      auto& target = _all_nodes[target_index];

      for (auto current_source_index : target._suffix_link_sources) {
        LIBSEMIGROUPS_ASSERT(current_source_index != new_node_index);
        index_type child_index = _children.get(current_source_index, a);
        if (child_index == UNDEFINED) {
          populate_node_indices_to_update(
              current_source_index, new_node_index, a);
        } else {
          _node_indices_to_update.push_back(child_index);
        }
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
