//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2024 Joseph Edwards + James D. Mitchell
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

// This file contains the implementation of the AhoCorasick class.
#include "libsemigroups/aho-corasick.hpp"

#include <array>        // for array
#include <string>       // for basic_string, string, to_string
#include <string_view>  // for basic_string_view, string_view

#include "libsemigroups/constants.hpp"   // for Undefined, UNDEFINED, operator==
#include "libsemigroups/detail/fmt.hpp"  // for format
#include "libsemigroups/dot.hpp"         // for Dot, Dot::Edge, Dot::Node
#include "libsemigroups/types.hpp"       // for word_type, letter_type

namespace libsemigroups {

  AhoCorasick::Node& AhoCorasick::Node::init(index_type  parent,
                                             letter_type a) noexcept {
    _parent        = parent;
    _parent_letter = a;
    _children.clear();
    _height = UNDEFINED;
    clear_suffix_link();
    _terminal = false;
    return *this;
  }

  // Not noexcept because std::unordered_map::count isn't
  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::Node::child(letter_type a) const {
    if (_children.count(a) == 0) {
      return UNDEFINED;
    }
    return static_cast<index_type>(_children[a]);
  }

  void AhoCorasick::Node::clear_suffix_link() const noexcept {
    if (parent() == root || parent() == UNDEFINED) {
      set_suffix_link(root);
    } else {
      set_suffix_link(UNDEFINED);
    }
  }

  AhoCorasick::AhoCorasick()
      : _all_nodes({Node()}), _active_nodes_index(), _inactive_nodes_index() {
    _active_nodes_index.insert(0);
    _valid_links = true;
  }

  AhoCorasick& AhoCorasick::init() {
    _all_nodes = {Node()};
    _active_nodes_index.clear();
    _valid_links = true;
    while (!_inactive_nodes_index.empty()) {
      _inactive_nodes_index.pop();
    }
    _active_nodes_index.insert(0);
    return *this;
  }

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::traverse_no_checks(index_type current, letter_type a) const {
    LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
    index_type next = _all_nodes[current].child(a);
    if (next != UNDEFINED) {
      return next;
    } else if (current == root) {
      return root;
    }
    return traverse_no_checks(suffix_link_no_checks(current), a);
  }

  void AhoCorasick::signature_no_checks(word_type& w, index_type i) const {
    LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
    w.clear();
    while (i != root) {
      w.push_back(_all_nodes[i].parent_letter());
      i = _all_nodes[i].parent();
    }
    std::reverse(w.begin(), w.end());
  }

  [[nodiscard]] size_t AhoCorasick::height_no_checks(index_type i) const {
    LIBSEMIGROUPS_ASSERT(i < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(i) == 1);
    size_t cached_height = _all_nodes[i].height();
    if (cached_height != UNDEFINED) {
      return cached_height;
    }

    if (i == root) {
      _all_nodes[i].set_height(0);
      return 0;
    }

    cached_height = height_no_checks(_all_nodes[i].parent()) + 1;
    _all_nodes[i].set_height(cached_height);
    return cached_height;
  }

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::suffix_link_no_checks(index_type current) const {
    LIBSEMIGROUPS_ASSERT(current < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(current) == 1);
    if (!_valid_links) {
      clear_suffix_links();
      _valid_links = true;
    }
    auto& n = _all_nodes[current];
    if (n.suffix_link() == UNDEFINED) {
      n.set_suffix_link(traverse_no_checks(suffix_link_no_checks(n.parent()),
                                           n.parent_letter()));
    }
    return n.suffix_link();
  }

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::new_active_node_no_checks(index_type parent, letter_type a) {
    LIBSEMIGROUPS_ASSERT(parent < _all_nodes.size());
    LIBSEMIGROUPS_ASSERT(_active_nodes_index.count(parent) == 1);
    index_type index;
    if (_inactive_nodes_index.empty()) {
      index = _all_nodes.size();
      _all_nodes.emplace_back(parent, a);
      _active_nodes_index.insert(index);
    } else {
      index = _inactive_nodes_index.top();
      _inactive_nodes_index.pop();
      _active_nodes_index.insert(index);
      _all_nodes[index].init(parent, a);
    }
    return index;
  }

  void AhoCorasick::deactivate_node_no_checks(index_type i) {
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

  void AhoCorasick::clear_suffix_links() const {
    // TODO determine if it is more efficient to clear the link of all nodes,
    // or just the active ones
    for (auto node = _all_nodes.begin(); node != _all_nodes.end(); ++node) {
      node->clear_suffix_link();
    }
  }

  void AhoCorasick::validate_node_index(index_type i) const {
    if (i >= _all_nodes.size()) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid index, expected value in range [0, {}), found {}",
          _all_nodes.size(),
          i);
    }
  }

  void AhoCorasick::validate_active_node_index(index_type i) const {
    validate_node_index(i);
    if (_active_nodes_index.count(i) != 1) {
      LIBSEMIGROUPS_EXCEPTION(
          "invalid index, expected an index of an active node, found {}", i);
    }
  }

  [[nodiscard]] Dot dot(AhoCorasick& ac) {
    auto to_word = [](word_type const& w) {
      if (w.empty()) {
        return std::string("&#949;");
      }
      std::string result;
      for (auto a : w) {
        result += std::to_string(a);
      }
      return result;
    };

    using index_type = AhoCorasick::index_type;
    Dot result;
    result.kind(Dot::Kind::digraph).add_attr("node [shape=\"box\"]");

    word_type w;
    for (index_type n = 0; n < ac.number_of_nodes(); ++n) {
      ac.signature_no_checks(w, n);
      auto& node = result.add_node(n).add_attr("label", to_word(w));
      if (ac.node_no_checks(n).is_terminal()) {
        node.add_attr("peripheries", "2");
      }
    }

    for (index_type n = 0; n < ac.number_of_nodes(); ++n) {
      for (auto [label, child] : ac.node_no_checks(n).children()) {
        result.add_edge(n, child)
            .add_attr("color", result.colors[label])
            .add_attr("label", label);
      }
      result.add_edge(n, ac.suffix_link_no_checks(n))
          .add_attr("color", "black")
          .add_attr("style", "dashed")
          .add_attr("constraint", "false");
    }
    return result;
  }

}  // namespace libsemigroups
