//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 Joseph Edwards
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
#include <cstddef>  // for size_t
#include <map>      // for map
#include <string>   // for string

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for REQUIRE
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/dot.hpp"                 // for Dot
#include "libsemigroups/exception.hpp"           // for LibsemigroupsException
#include "libsemigroups/word-graph-helpers.hpp"  // for word_graph
#include "libsemigroups/word-graph.hpp"          // for WordGraph

#include "libsemigroups/ranges.hpp"  // for Ranges

namespace libsemigroups {
  using namespace rx;
  LIBSEMIGROUPS_TEST_CASE("Dot::Edge",
                          "000",
                          "edge attributes",
                          "[dot][quick]") {
    auto      wg    = v4::make<WordGraph<size_t>>(3, {{0, 1}, {1, 0}, {2, 2}});
    const Dot d     = v4::word_graph::dot(wg);
    auto      edges = d.edges();
    REQUIRE(edges.size() == 6);
    REQUIRE(edges[0].head == "0");
    REQUIRE(edges[0].tail == "0");
    REQUIRE(edges[0].attrs
            == std::map<std::string, std::string>{{"color", "#00ff00"}});
    edges[0].add_attr("style", "dashed");
    REQUIRE(edges[0].attrs
            == std::map<std::string, std::string>{{"color", "#00ff00"},
                                                  {"style", "dashed"}});
    edges[0].attrs["color"] = "blue";
    REQUIRE(edges[0].attrs
            == std::map<std::string, std::string>{{"color", "blue"},
                                                  {"style", "dashed"}});
  }

  LIBSEMIGROUPS_TEST_CASE("Dot::Node",
                          "001",
                          "node attributes",
                          "[dot][quick]") {
    auto      wg    = v4::make<WordGraph<size_t>>(3, {{0, 1}, {1, 0}, {2, 2}});
    const Dot d     = v4::word_graph::dot(wg);
    auto      nodes = d.nodes() | to_vector();
    REQUIRE((nodes.size()) == 3);
    REQUIRE(nodes[0].attrs
            == std::map<std::string, std::string>{{"shape", "box"}});
    nodes[0].add_attr("shape", "circle");
    REQUIRE(nodes[0].attrs
            == std::map<std::string, std::string>{{"shape", "circle"}});
    nodes[0].attrs["color"] = "blue";
    REQUIRE(nodes[0].attrs
            == std::map<std::string, std::string>{{"color", "blue"},
                                                  {"shape", "circle"}});
  }

  LIBSEMIGROUPS_TEST_CASE("Dot", "002", "dot attributes", "[dot][quick]") {
    auto wg = v4::make<WordGraph<size_t>>(3, {{0, 1}, {1, 0}, {2, 2}});
    Dot  d  = v4::word_graph::dot(wg);
    d.add_attr("node [shape=circle]");
    REQUIRE(d.attrs()
            == std::map<std::string, std::string>{{"node [shape=circle]", ""}});
    d.add_attr("splines", "line");
    REQUIRE(d.attrs()
            == std::map<std::string, std::string>{{"node [shape=circle]", ""},
                                                  {"splines", "line"}});
  }

  LIBSEMIGROUPS_TEST_CASE("Dot", "003", "add_node", "[dot][quick]") {
    Dot d;
    d.add_node("cat");
    REQUIRE_THROWS_AS(d.add_node("cat"), LibsemigroupsException);
    auto& n = d.add_node("dog");
    REQUIRE((d.nodes() | count()) == 2);
    REQUIRE(d.to_string() == "digraph {\n\n  cat\n  dog\n}");
    n.add_attr("shape", "box");
    REQUIRE(d.to_string() == "digraph {\n\n  cat\n  dog  [shape=\"box\"]\n}");
  }

  LIBSEMIGROUPS_TEST_CASE("Dot", "004", "add_edge", "[dot][quick]") {
    Dot d;
    d.add_node("cat");
    d.add_node("dog");
    REQUIRE(d.edges().size() == 0);
    auto& e = d.add_edge("cat", "dog");
    REQUIRE(d.edges().size() == 1);
    REQUIRE(e.attrs == std::map<std::string, std::string>{});
    e.add_attr("color", "#00FF00");
    REQUIRE(e.attrs
            == std::map<std::string, std::string>{{"color", "#00FF00"}});
  }

  LIBSEMIGROUPS_TEST_CASE("Dot", "005", "add_subgraph", "[dot][quick]") {
    Dot d;
    d.name("pets");
    REQUIRE(d.name() == "pets");
    d.add_node("cat");
    d.add_node("dog");
    d.add_edge("cat", "dog");

    REQUIRE(d.subgraphs().size() == 0);
    d.add_subgraph(d);
    REQUIRE(d.subgraphs().size() == 1);
    REQUIRE(d.subgraphs()[0].kind() == Dot::Kind::subgraph);
    REQUIRE(d.to_string()
            == "digraph pets {\n\nsubgraph cluster_pets {\n  label=\"pets\"\n  "
               "cluster_pets_cat  [label=\"cat\"]\n  cluster_pets_dog  "
               "[label=\"dog\"]\n  cluster_pets_cat -> cluster_pets_dog  "
               "[constraint=\"false\"]\n}\n  cat\n  dog\n  cat -> dog\n}");
  }

  LIBSEMIGROUPS_TEST_CASE("Dot", "006", "kind", "[dot][quick]") {
    Dot d;
    REQUIRE(d.kind() == Dot::Kind::digraph);
    d.kind(Dot::Kind::graph);
    REQUIRE(d.kind() == Dot::Kind::graph);
    d.kind(Dot::Kind::subgraph);
    REQUIRE(d.kind() == Dot::Kind::subgraph);
  }
}  // namespace libsemigroups
