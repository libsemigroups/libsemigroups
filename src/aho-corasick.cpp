//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 Joe Edwards + James D. Mitchell
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
