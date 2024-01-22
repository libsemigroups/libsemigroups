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

#ifndef LIBSEMIGROUPS_DOT_HPP_
#define LIBSEMIGROUPS_DOT_HPP_

#include <array>        // for array
#include <cstddef>      // for size_t
#include <map>          // for map, operator!=, __map_const_...
#include <string>       // for basic_string, operator<, oper...
#include <string_view>  // for basic_string_view
#include <utility>      // for forward, move etc
#include <vector>       // for vector

#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "ranges.hpp"     // for LIBSEMIGROUPS_EXCEPTION

#include "detail/fmt.hpp"     // for format
#include "detail/report.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {
  namespace detail {

    inline std::string const& dot_to_string(std::string const& x) {
      return x;
    }

    inline std::string& dot_to_string(std::string& x) {
      return x;
    }

    inline std::string&& dot_to_string(std::string&& x) {
      return std::move(x);
    }
    // TODO required?
    inline std::string dot_to_string(char const* x) {
      return std::string(x);
    }

    inline std::string dot_to_string(std::string_view x) {
      return std::string(x);
    }

    template <typename Thing>
    std::string dot_to_string(Thing&& thing) {
      return std::to_string(std::forward<Thing>(thing));
    }
  }  // namespace detail

  class Dot {
   public:
    struct Node {
      std::map<std::string, std::string> attrs;
      std::string                        name;

      template <typename Thing>
      explicit Node(Thing&& thing)
          : attrs(), name(detail::dot_to_string(std::forward<Thing>(thing))) {}

      template <typename Thing1, typename Thing2>
      Node& add_attr(Thing1&& key, Thing2&& val) {
        auto key_str = detail::dot_to_string(std::forward<Thing1>(key));
        auto val_str = detail::dot_to_string(std::forward<Thing2>(val));

        add_or_replace_attr(attrs, key_str, val_str);
        return *this;
      }
    };

    struct Edge {
      template <typename Thing1, typename Thing2>
      Edge(Thing1&& f, Thing2&& t)
          : attrs(),
            from(detail::dot_to_string(std::forward<Thing1>(f))),
            to(detail::dot_to_string(std::forward<Thing2>(t))) {}

      template <typename Thing1, typename Thing2>
      Edge& add_attr(Thing1&& key, Thing2&& val) {
        auto key_str = detail::dot_to_string(std::forward<Thing1>(key));
        auto val_str = detail::dot_to_string(std::forward<Thing2>(val));

        add_or_replace_attr(attrs, key_str, val_str);
        return *this;
      }

      std::map<std::string, std::string> attrs;
      std::string                        from;
      std::string                        to;
    };

    enum class Kind { digraph, graph, subgraph };

    static constexpr std::array<std::string_view, 24> colors
        = {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#4604ac",
           "#de0328", "#19801d", "#d881f5", "#00ffff", "#ffff00", "#00ff7f",
           "#ad5867", "#85f610", "#84e9f5", "#f5c778", "#207090", "#764ef3",
           "#7b4c00", "#0000ff", "#b80c9a", "#601045", "#29b7c0", "#839f12"};

    using edge_index = size_t;

   private:
    std::map<std::string, std::string> _attrs;
    std::vector<Edge>                  _edges;
    Kind                               _kind;
    std::string                        _name;
    std::map<std::string, Node>        _nodes;
    std::vector<Dot>                   _subgraphs;

   public:
    Dot() : _attrs(), _edges(), _kind(), _name(), _nodes(), _subgraphs() {}

    Dot& kind(Kind val) {
      _kind = val;
      return *this;
    }

    Dot& name(std::string val) {
      _name = val;
      return *this;
    }

    std::string const& name() const noexcept {
      return _name;
    }

    auto nodes() noexcept {
      return rx::iterator_range(_nodes.begin(), _nodes.end())
             | rx::transform([](auto& pair) -> Node& { return pair.second; });
    }

    auto& edges() {
      return _edges;
    }

    Dot& add_subgraph(Dot&& subgraph) {
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
        edge.from = subgraph.name() + "_" + edge.from;
        edge.to   = subgraph.name() + "_" + edge.to;
        edge.add_attr("constraint", "false");
      }

      _subgraphs.push_back(std::move(subgraph));
      return *this;
    }

    Dot& add_attr(std::string const& key, std::string const& val) {
      add_or_replace_attr(_attrs, key, val);
      return *this;
    }

    Dot& add_attr(std::string const& key) {
      add_or_replace_attr(_attrs, key, "");
      return *this;
    }

    [[nodiscard]] bool is_node(std::string const& name) {
      return _nodes.count(name);
    }

    template <typename Thing>
    [[nodiscard]] bool is_node(Thing&& name) {
      return _nodes.count(name);
      return is_node(detail::dot_to_string(std::forward<Thing>(name)));
    }

    template <typename Thing>
    Node& add_node(Thing&& name) {
      auto name_str       = detail::dot_to_string(std::forward<Thing>(name));
      auto [it, inserted] = _nodes.emplace(name_str, Node(name_str));
      if (!inserted) {
        LIBSEMIGROUPS_EXCEPTION("there is already a node named {}!", name);
      }
      return it->second;
    }

    template <typename Thing1, typename Thing2>
    Edge& add_edge(Thing1&& from, Thing2&& to) {
      auto from_str = detail::dot_to_string(std::forward<Thing1>(from));
      auto to_str   = detail::dot_to_string(std::forward<Thing2>(to));
      throw_if_not_node(from_str);
      throw_if_not_node(to_str);
      _edges.emplace_back(from_str, to_str);
      return _edges.back();
    }

    std::string to_string() const {
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

    static void add_or_replace_attr(std::map<std::string, std::string>& attrs,
                                    std::string const&                  key,
                                    std::string const&                  val) {
      auto it = attrs.find(key);
      if (it == attrs.end()) {
        attrs.emplace(key, val);
      } else {
        it->second = val;
      }
    }

    static void append_attrs(std::string&                              s,
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
          s += fmt::format("{}{}=\"{}\"", sep, attr.first, attr.second);
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
  };

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DOT_HPP_
