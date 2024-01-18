//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2024 Joe Edwards + James D. Mitchell
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

#include "libsemigroups/dot.hpp"

namespace libsemigroups {

  AhoCorasick::Node& AhoCorasick::Node::init(index_type parent, letter_type a) {
    _parent        = parent;
    _parent_letter = a;
    _children.clear();
    _height = UNDEFINED;
    clear_suffix_link();
    _terminal = false;
    return *this;
  }

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::Node::child(letter_type a) const noexcept {
    if (_children.count(a) == 0) {
      return UNDEFINED;
    }
    return static_cast<index_type>(_children[a]);
  }

  void AhoCorasick::Node::clear_suffix_link() const {
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

  void AhoCorasick::signature(word_type& w, index_type i) const {
    w.clear();
    while (i != root) {
      w.push_back(_all_nodes[i].parent_letter());
      i = _all_nodes[i].parent();
    }
    std::reverse(w.begin(), w.end());
  }

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::suffix_link(index_type current) const {
    if (!_valid_links) {
      clear_suffix_links();
      _valid_links = true;
    }
    auto& n = _all_nodes[current];
    if (n.suffix_link() == UNDEFINED) {
      n.set_suffix_link(traverse(suffix_link(n.parent()), n.parent_letter()));
    }
    return n.suffix_link();
  }

  AhoCorasick::index_type AhoCorasick::new_active_node(index_type  parent,
                                                       letter_type a) {
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

  [[nodiscard]] AhoCorasick::index_type
  AhoCorasick::traverse(index_type current, letter_type a) const {
    index_type next = _all_nodes[current].child(a);
    if (next != UNDEFINED) {
      return next;
    } else if (current == root) {
      return root;
    }
    return traverse(suffix_link(current), a);
  }

  void AhoCorasick::clear_suffix_links() const {
    // TODO determine if it is more efficient to clear the link of all nodes,
    // or just the active ones
    for (auto node = _all_nodes.begin(); node != _all_nodes.end(); ++node) {
      node->clear_suffix_link();
    }
  }

  Dot dot(AhoCorasick& ac) {
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
      ac.signature(w, n);
      auto& node = result.add_node(n).add_attr("label", to_word(w));
      if (ac.node(n).is_terminal()) {
        node.add_attr("peripheries", "2");
      }
    }

    for (index_type n = 0; n < ac.number_of_nodes(); ++n) {
      for (auto [label, child] : ac.node(n).children()) {
        result.add_edge(n, child)
            .add_attr("color", result.colors[label])
            .add_attr("label", label);
      }
      result.add_edge(n, ac.suffix_link(n))
          .add_attr("color", "black")
          .add_attr("style", "dashed")
          .add_attr("constraint", "false");
    }
    return result;
  }
}  // namespace libsemigroups
