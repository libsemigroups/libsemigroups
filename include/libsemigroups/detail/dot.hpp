//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_DETAIL_DOT_
#define LIBSEMIGROUPS_DETAIL_DOT_

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <fmt/format.h>

namespace libsemigroups {

  class Dot {
   private:
    struct Node {
      Node(std::string const& name) : name(name) {}

      std::string                                  name;
      std::unordered_map<std::string, std::string> attrs;
    };

    struct Edge {
      Edge(std::string const& from, std::string const& to)
          : from(from), to(to), attrs() {}
      std::string                                  from;
      std::string                                  to;
      std::unordered_map<std::string, std::string> attrs;
    };

   public:
    enum class Kind { digraph, graph };

   private:
    std::unordered_map<std::string, std::string> _attrs;
    Kind                                         _kind;
    std::string                                  _name;
    std::unordered_map<std::string, Node>        _nodes;
    std::vector<Edge>                            _edges;

   public:
    static constexpr std::array<std::string_view, 24> colors
        = {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#4604ac",
           "#de0328", "#19801d", "#d881f5", "#00ffff", "#ffff00", "#00ff7f",
           "#ad5867", "#85f610", "#84e9f5", "#f5c778", "#207090", "#764ef3",
           "#7b4c00", "#0000ff", "#b80c9a", "#601045", "#29b7c0", "#839f12"};
    Dot() : _attrs(), _kind(), _name(), _nodes(), _edges() {}

    Dot& kind(Kind val) {
      _kind = val;
      return *this;
    }

    Dot& name(std::string val) {
      _name = val;
      return *this;
    }

    Dot& add_attr(std::string const& key, std::string const& val) {
      add_or_replace_attr(_attrs, key, val);
      return *this;
    }

    [[nodiscard]] bool is_node(std::string const& name) {
      return _nodes.count(name);
    }

    Dot& add_node(std::string const& name) {
      if (!_nodes.emplace(name, Node(name)).second) {
        LIBSEMIGROUPS_EXCEPTION("there is already a node named {}!", name);
      }
      return *this;
    }

    Dot& add_node(size_t n) {
      return add_node(std::to_string(n));
    }

    Dot& add_node_attr(std::string const& name,
                       std::string const& key,
                       std::string const& val) {
      throw_if_not_node(name);
      auto& node = _nodes.find(name)->second;
      add_or_replace_attr(node.attrs, key, val);
      return *this;
    }

    template <typename T,
              typename = std::enable_if_t<!std::is_same_v<T, char const*>>>
    Dot& add_node_attr(T name, std::string const& key, std::string const& val) {
      return add_node_attr(std::to_string(name), key, val);
    }

    Dot& add_edge(std::string const& from, std::string const& to) {
      throw_if_not_node(from);
      throw_if_not_node(to);
      _edges.emplace_back(from, to);
      return *this;
    }

    Dot& add_edge(size_t from, size_t to) {
      return add_edge(std::to_string(from), std::to_string(to));
    }

    // TODO replace by edge index
    Dot& add_edge_attr(std::string const& from,
                       std::string const& to,
                       std::string const& key,
                       std::string const& val) {
      throw_if_not_node(from);
      throw_if_not_node(to);

      auto it = std::find_if(
          _edges.rbegin(), _edges.rend(), [&from, &to](Edge const& e) {
            return e.from == from && e.to == to;
          });
      if (it == _edges.rend()) {
        LIBSEMIGROUPS_EXCEPTION(
            "there is no edge {} {} {}!", from, edge_string(), to);
      }
      add_or_replace_attr(it->attrs, key, val);
      return *this;
    }

    // TODO replace by edge index
    template <typename S, typename T>
    Dot& add_edge_attr(S const&         from,
                       T const&         to,
                       std::string_view key,
                       std::string_view val) {
      return add_edge_attr(std::to_string(from),
                           std::to_string(to),
                           std::string(key),
                           std::string(val));
    }

    std::string to_string() const {
      constexpr std::string_view head = "digraph";
      std::string                result(head);
      if (_kind == Kind::graph) {
        result.erase(result.begin(), result.begin() + 2);
      }
      if (!_name.empty()) {
        result += " ";
      }
      result += _name + " {\n";
      append_attrs(result, _attrs, false);

      for (auto const& [_, node] : _nodes) {
        result += fmt::format("  {}", node.name);
        append_attrs(result, node.attrs);
      }

      for (auto const& edge : _edges) {
        result += fmt::format("  {} {} {}", edge.from, edge_string(), edge.to);
        append_attrs(result, edge.attrs);
      }
      result += "}";
      return result;
    }

   private:
    void throw_if_not_node(std::string const& s) {
      if (!is_node(s)) {
        LIBSEMIGROUPS_EXCEPTION("there is no node {}!", s);
      }
    }

    std::string_view edge_string() const noexcept {
      constexpr std::string_view edge = "--";
      constexpr std::string_view arc  = "->";

      return _kind == Kind::graph ? edge : arc;
    }

    void
    add_or_replace_attr(std::unordered_map<std::string, std::string>& attrs,
                        std::string const&                            key,
                        std::string const&                            val) {
      auto it = attrs.find(key);
      if (it == attrs.end()) {
        attrs.emplace(key, val);
      } else {
        it->second = val;
      }
    }

    void append_attrs(std::string&                                        s,
                      std::unordered_map<std::string, std::string> const& map,
                      bool include_brace = true) const {
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
        s += fmt::format("{}{}=\"{}\"", sep, attr.first, attr.second);
        sep = ", ";
      }
      if (include_brace) {
        s += "]";
      }
      s += "\n";
    }
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DETAIL_DOT_
