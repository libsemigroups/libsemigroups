//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

// This file contains the implementation of the Dot class.

#include "libsemigroups/dot.hpp"

namespace libsemigroups {
  namespace {
    void append_attrs(std::string&                              s,
                      std::map<std::string, std::string> const& map,
                      bool include_brace = true) {
      if (map.empty()) {
        s += "\n";
        return;
      }

      s += "  ";

      if (include_brace) {
        s += "[";
      }
      std::string_view sep = "";
      for (auto const& attr : map) {
        if (!attr.second.empty()) {
          s += fmt::format(
              "{}{}=\"{}\"",
              sep,
              attr.first,
              attr.second == "__NONE__" ? "" : attr.second);  // TODO(2) fixme
        } else {
          s += fmt::format("{}\n  ", attr.first);
        }
        sep = ", ";
      }
      if (include_brace) {
        s += "]";
      }
      s += "\n";
    }
  }  // namespace

  Dot& Dot::add_subgraph(Dot&& subgraph) {
    subgraph.kind(Kind::subgraph)
        .add_attr("label", subgraph.name())
        .name(fmt::format("cluster_{}", subgraph.name()));

    for (auto& node : subgraph.nodes()) {
      auto old_name = node.name;
      node.name     = subgraph.name() + "_" + node.name;
      add_node(node.name);
      node.add_attr("label", old_name);
    }

    for (auto& edge : subgraph.edges()) {
      edge.head = subgraph.name() + "_" + edge.head;
      edge.tail = subgraph.name() + "_" + edge.tail;
      edge.add_attr("constraint", "false");
    }

    _subgraphs.push_back(std::move(subgraph));
    return *this;
  }

  Dot& Dot::add_subgraph(Dot const& subgraph) {
    return add_subgraph(Dot(subgraph));
  }

  std::string Dot::to_string() const {
    std::string result(fmt::format("{}", _kind));
    if (!_name.empty()) {
      result += " ";
    }
    result += _name + " {\n";
    append_attrs(result, _attrs, false);

    for (Dot const& sg : _subgraphs) {
      result += sg.to_string() + "\n";
    }

    constexpr std::string_view cluster = "cluster";

    for (auto const& [_, node] : _nodes) {
      if (_kind == Kind::subgraph
          || !detail::is_prefix(node.name.begin(),
                                node.name.end(),
                                cluster.begin(),
                                cluster.end())) {
        result += fmt::format("  {}", node.name);
        append_attrs(result, node.attrs);
      }
    }

    for (auto const& edge : _edges) {
      result += fmt::format("  {} {} {}", edge.head, edge_string(), edge.tail);
      append_attrs(result, edge.attrs);
    }
    result += "}";
    return result;
  }

  void Dot::throw_if_not_node(std::string const& s) {
    if (!is_node(s)) {
      LIBSEMIGROUPS_EXCEPTION("there is no node named \"{}\"!", s);
    }
  }

  std::string_view Dot::edge_string() const noexcept {
    constexpr std::string_view edge = "--";
    constexpr std::string_view arc  = "->";

    return _kind == Kind::graph ? edge : arc;
  }

  void Dot::add_or_replace_attr(std::map<std::string, std::string>& attrs,
                                std::string const&                  key,
                                std::string const&                  val) {
    auto it = attrs.find(key);
    if (it == attrs.end()) {
      attrs.emplace(key, val);
    } else {
      it->second = val;
    }
  }

  std::string to_human_readable_repr(Dot const& d) {
    return fmt::format(
        "<dot {} \"{}\" with {} nodes, {} edges, and {} subgraphs>",
        d.kind(),
        d.name(),
        d.nodes() | rx::count(),
        d.edges().size(),
        d.subgraphs().size());
  }

  std::string to_human_readable_repr(Dot::Node const& n) {
    return fmt::format("<dot node \"{}\">", n.name);
  }

  std::string to_human_readable_repr(Dot::Edge const& e) {
    return fmt::format("<dot edge \"{}\" -> \"{}\">", e.head, e.tail);
  }

  std::string to_human_readable_repr(Dot::Kind const&, std::string const& sep) {
    return fmt::format("<enum Dot{}Kind>", sep);
  }

}  // namespace libsemigroups
