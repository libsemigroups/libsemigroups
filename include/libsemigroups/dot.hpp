//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 James D. Mitchell
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
#include "libsemigroups/bipart.hpp"
#include "ranges.hpp"  // for LIBSEMIGROUPS_EXCEPTION

#include "detail/fmt.hpp"     // for format
#include "detail/report.hpp"  // for LIBSEMIGROUPS_EXCEPTION

namespace libsemigroups {

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
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

#endif  // LIBSEMIGROUPS_PARSED_BY_DOXYGEN

  //! \defgroup dot_group Visualisation
  //!
  //! This page contains links to the various classes and functions in
  //! `libsemigroups` for visualising instances of some of the other objects
  //! implemented in `libsemigroups`.
  //!
  //! The class Dot facilitates the creation and rendering of graph
  //! descriptions in the [DOT][] language of the [Graphviz][] graph drawing
  //! software.
  //!
  //! The function \c dot can be used to produce initial Dot objects for
  //! various of the data structures and algorithms in `libsemigroups`:
  //!
  //! * \ref aho_corasick::dot(AhoCorasick&)
  //! * \ref forest::dot(Forest const&)
  //! * \ref stephen::dot(Stephen<PresentationType>&)
  //! * \ref ukkonen::dot(Ukkonen const&)
  //! * \ref v4::word_graph::dot(WordGraph<Node> const&)
  //!
  //! [DOT]: https://www.graphviz.org/doc/info/lang.html
  //! [Graphviz]: https://www.graphviz.org

  //! \ingroup dot_group
  //!
  //! \brief A representation of a graph in the [DOT][] language of
  //! [Graphviz][].
  //!
  //! Defined in `dot.hpp`.
  //!
  //! This class facilitates the creation and rendering of graph
  //! descriptions in the [DOT][] language of the [Graphviz][] graph drawing
  //! software. This class is fairly rudimentary, and is not intended to
  //! implement every feature of the [DOT][] language.
  //!
  //! You can create a Dot object, assemble the graph by adding nodes and
  //! edges, attributes, labels, colours, subgraphs, and clusters, and
  //! retrieve its [DOT][] source code string (Dot::to_string). Write the source
  //! code to a file and render it with the [Graphviz] installation on your
  //! system.
  //!
  //! \warning No checks of any kind for the correctness of the output
  //! (Dot::to_string) std::string are performed.
  //!
  //! [DOT]: https://www.graphviz.org/doc/info/lang.html
  //! [Graphviz]: https://www.graphviz.org
  class Dot {
   public:
    //! \brief This nested struct represents a node in the represented graph.
    //!
    //! Defined in `dot.hpp`.
    //!
    //! This nested struct represents a node in the represented graph.
    struct Node {
      //! Map containing the attributes of the Node.
      std::map<std::string, std::string> attrs;

      //! The name of the node.
      std::string name;

      //! \brief Construct from anything.
      //!
      //! This constructor creates a new Node object from the parameter
      //! \p thing, which is converted to a string (using std::to_string if
      //! \p thing is not already a std::string) by the constructor.
      //!
      //! The newly constructed Node has no attributes set.
      //!
      //! \tparam Thing the type of the argument.
      //!
      //! \param thing the object whose string representation will be the name
      //! of the node.
      template <typename Thing>
      explicit Node(Thing&& thing)
          : attrs(), name(detail::dot_to_string(std::forward<Thing>(thing))) {}

      //! \brief Default constructor.
      Node() = default;

      //! \brief Default copy constructor.
      Node(Node const&) = default;

      //! \brief Default move constructor.
      Node(Node&&) = default;

      //! \brief Default copy assignment.
      Node& operator=(Node const&) = default;

      //! \brief Default move assignment.
      Node& operator=(Node&&) = default;

      ~Node();

      //! \brief Add an attribute to a node.
      //!
      //! This function adds a new attribute, or replaces the value of an
      //! existing attribute of a Node. The arguments \p key and \p val are
      //! converted to std::string (unless they are std::string,
      //! std::string_view, or `char const*` already).
      //!
      //! \tparam Thing1 the type of the first argument.
      //! \tparam Thing2 the type of the second argument.
      //!
      //! \param key the name of the attribute.
      //! \param val the value of the attribute.
      //!
      //! \returns A reference to \c this.
      //!
      //! \warning No checks of any kind are performed that the string
      //! representations of \p key or \p val are valid in [DOT][]; see the
      //! [Graphviz documentation][] for more details.
      //!
      //! [Graphviz documentation]: https://www.graphviz.org/docs/nodes/
      //! [DOT]: https://www.graphviz.org/doc/info/lang.html
      template <typename Thing1, typename Thing2>
      Node& add_attr(Thing1&& key, Thing2&& val) {
        auto key_str = detail::dot_to_string(std::forward<Thing1>(key));
        auto val_str = detail::dot_to_string(std::forward<Thing2>(val));

        add_or_replace_attr(attrs, key_str, val_str);
        return *this;
      }
    };

    //! \brief This nested struct represents an edge in the represented graph.
    //!
    //! Defined in `dot.hpp`.
    //!
    //! This nested struct represents an edge in the represented graph.
    struct Edge {
      //! \brief Construct from head and tail.
      //!
      //! This constructor creates a new Edge object with head \p h (converted
      //! to a std::string using std::to_string if \p thing is not already a
      //! std::string, std::string_view, or `char const*`) and tail \p t
      //! (similarly converted to a std::string).
      //!
      //! The newly constructed Edge has no attributes set.
      //!
      //! \tparam Thing the type of the argument.
      //!
      //! \param h the head of the edge.
      //! \param t the tail of the edge.
      template <typename Thing1, typename Thing2>
      Edge(Thing1&& h, Thing2&& t)
          : attrs(),
            head(detail::dot_to_string(std::forward<Thing1>(h))),
            tail(detail::dot_to_string(std::forward<Thing2>(t))) {}

      //! \brief Add an attribute to an edge.
      //!
      //! This function adds a new attribute, or replaces the value of an
      //! existing attribute of an Edge. The arguments \p key and \p val are
      //! converted to std::string (unless they are std::string,
      //! std::string_view, or `char const*` already).
      //!
      //! \tparam Thing1 the type of the first argument.
      //! \tparam Thing2 the type of the second argument.
      //!
      //! \param key the name of the attribute.
      //! \param val the value of the attribute.
      //!
      //! \returns A reference to \c this.
      //!
      //! \warning No checks of any kind are performed that the string
      //! representations of \p key or \p val are valid in [DOT][]; see the
      //! [Graphviz documentation][] for more details.
      //!
      //! [Graphviz documentation]: https://www.graphviz.org/docs/nodes/
      //! [DOT]: https://www.graphviz.org/doc/info/lang.html
      template <typename Thing1, typename Thing2>
      Edge& add_attr(Thing1&& key, Thing2&& val) {
        auto key_str = detail::dot_to_string(std::forward<Thing1>(key));
        auto val_str = detail::dot_to_string(std::forward<Thing2>(val));

        add_or_replace_attr(attrs, key_str, val_str);
        return *this;
      }

      //! \brief Default constructor.
      Edge() = default;

      //! \brief Default copy constructor.
      Edge(Edge const&) = default;

      //! \brief Default move constructor.
      Edge(Edge&&) = default;

      //! \brief Default copy assignment.
      Edge& operator=(Edge const&) = default;

      //! \brief Default move assignment.
      Edge& operator=(Edge&&) = default;

      ~Edge();

      //! \brief Map of attributes.
      std::map<std::string, std::string> attrs;

      //! \brief Name of the head of the edge.
      std::string head;

      //! \brief Name of the tail of the edge.
      std::string tail;
    };

    //! The kind of object represented.
    enum class Kind {
      //! Value indicating that the represented graph has directed edges `->`.
      digraph,
      //! Value indicating that the represented graph has undirected edges `--`.
      graph,
      //! Value indicating that the represented graph is a subgraph of another
      //! Dot object.
      subgraph
    };

    //! \brief An array of default HTML/hex colours.
    //!
    //! An array of default HTML/hex colours.
    static constexpr std::array<std::string_view, 24> colors
        = {"#00ff00", "#ff00ff", "#007fff", "#ff7f00", "#7fbf7f", "#4604ac",
           "#de0328", "#19801d", "#d881f5", "#00ffff", "#ffff00", "#00ff7f",
           "#ad5867", "#85f610", "#84e9f5", "#f5c778", "#207090", "#764ef3",
           "#7b4c00", "#0000ff", "#b80c9a", "#601045", "#29b7c0", "#839f12"};

   private:
    std::map<std::string, std::string> _attrs;
    std::vector<Edge>                  _edges;
    Kind                               _kind{Kind::digraph};
    std::string                        _name;
    std::map<std::string, Node>        _nodes;
    std::vector<Dot>                   _subgraphs;

   public:
    //! Default constructor
    Dot();

    //! Default copy constructor
    Dot(Dot const&);

    //! Default move constructor
    Dot(Dot&&);

    //! Default copy assignment operator
    Dot& operator=(Dot const&);

    //! Default move assignment operator
    Dot& operator=(Dot&&);

    ~Dot();

    //! \brief Set the kind of the represented graph.
    //!
    //! \param val the kind.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \noexcept
    Dot& kind(Kind val) noexcept {
      _kind = val;
      return *this;
    }

    //! \brief Get the kind of the represented graph.
    //!
    //! \returns A value of type Dot::kind.
    //!
    //! \exceptions
    //! \noexcept
    Kind kind() const noexcept {
      return _kind;
    }

    //! \brief Set the name of the represented graph.
    //!
    //! \param val the name.
    //!
    //! \returns A reference to \c this.
    Dot& name(std::string const& val) {
      _name = val;
      return *this;
    }

    //! \brief Get the current name of the represented graph.
    //!
    //! Get the current name of the represented graph.
    //!
    //! \returns The current name of the represented graph.
    std::string const& name() const noexcept {
      return _name;
    }

    //! \brief Returns a range object of references to the current nodes.
    //!
    //! Returns a range object of references for the current nodes in the
    //! represented graph; see \ref ranges_group for more details.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    auto nodes() noexcept {
      return rx::iterator_range(_nodes.begin(), _nodes.end())
             | rx::transform([](auto& pair) -> Node& { return pair.second; });
    }

    //! \brief Returns a range object of const references to the current nodes.
    //!
    //! Returns a range object of const references for the current nodes in the
    //! represented graph; see \ref ranges_group for more details.
    //!
    //! \returns A range object.
    //!
    //! \exceptions
    //! \noexcept
    auto nodes() const noexcept {
      return rx::iterator_range(_nodes.begin(), _nodes.end())
             | rx::transform(
                 [](auto& pair) -> Node const& { return pair.second; });
    }

    //! \brief Returns a reference to the vector of edges.
    //!
    //! Returns a reference to a std::vector of the current edges (Edge
    //! objects) in the represented graph.
    //!
    //! \returns A reference to the std::vector of current Edge objects.
    //!
    //! \exceptions
    //! \noexcept
    std::vector<Edge>& edges() noexcept {
      return _edges;
    }

    //! \brief Returns a const reference to the vector of edges.
    //!
    //! Returns a const reference to a std::vector of the current edges (Edge
    //! objects) in the represented graph.
    //!
    //! \returns A const reference to the std::vector of current Edge objects.
    //!
    //! \exceptions
    //! \noexcept
    std::vector<Edge> const& edges() const noexcept {
      return _edges;
    }

    //! \brief Returns a reference to the vector of subgraphs.
    //!
    //! Returns a reference to a std::vector of the current subgraphs (Dot
    //! objects) in the represented graph.
    //!
    //! \returns A reference to the std::vector of current subgraph
    //! objects.
    //!
    //! \exceptions
    //! \noexcept
    std::vector<Dot>& subgraphs() noexcept {
      return _subgraphs;
    }

    //! \brief Returns a const reference to the vector of subgraphs.
    //!
    //! Returns a const reference to a std::vector of the current subgraphs (Dot
    //! objects) in the represented graph.
    //!
    //! \returns A const reference to the std::vector of current subgraph
    //! objects.
    //!
    //! \exceptions
    //! \noexcept
    std::vector<Dot> const& subgraphs() const noexcept {
      return _subgraphs;
    }

    //! \brief Returns a const reference to the map of attributes.
    //!
    //! Returns a const reference to a std::map of the current attributes of the
    //! represented graph.
    //!
    //! \returns A const reference to the std::vector of current subgraph
    //! objects.
    //!
    //! \exceptions
    //! \noexcept
    std::map<std::string, std::string> const& attrs() const noexcept {
      return _attrs;
    }

    //! \brief Add a Dot object as a subgraph.
    //!
    //! This function adds the Dot object \p subgraph as a subgraph of \c this.
    //! The following transformations are performed:
    //!
    //! * the \c label attribute of the added subgraph is the \ref name of
    //! \p subgraph;
    //! * the \ref name of the added subgraph has the prefix `"cluster_"`
    //! prepended;
    //! * every node in the added subgraph has the prefix `"name_"` prepended
    //! (where `"name"` is the return value of \ref name);
    //! * the \c label attribute of every node in the added subgraph is its
    //! original name in \p subgraph;
    //! * every edge is modified so that its head and tail use the new node
    //! names;
    //! * every edge has the attribute `constraint` set to `false`.
    //!
    //! \param subgraph the Dot object to use as a subgraph.
    //!
    //! \returns A reference to \c this.
    Dot& add_subgraph(Dot&& subgraph);

    //! \copydoc add_subgraph(Dot&&)
    Dot& add_subgraph(Dot const& subgraph);

    //! \brief Add an attribute to the graph.
    //!
    //! This function adds a new attribute, or replaces the value of an
    //! existing attribute, of a Dot object. The arguments \p key and \p val are
    //! converted to std::string (unless they are std::string,
    //! std::string_view, or `char const*` already).
    //!
    //! \tparam Thing1 the type of the first argument.
    //! \tparam Thing2 the type of the second argument.
    //!
    //! \param key the name of the attribute.
    //! \param val the value of the attribute.
    //!
    //! \returns A reference to \c this.
    //!
    //! \warning No checks of any kind are performed that the string
    //! representations of \p key or \p val are valid in [DOT][]; see the
    //! [Graphviz documentation][] for more details.
    //!
    //! [Graphviz documentation]: https://www.graphviz.org/docs/nodes/
    //! [DOT]: https://www.graphviz.org/doc/info/lang.html
    template <typename Thing1, typename Thing2>
    Dot& add_attr(Thing1&& key, Thing2&& val) {
      auto key_str = detail::dot_to_string(std::forward<Thing1>(key));
      auto val_str = detail::dot_to_string(std::forward<Thing2>(val));

      add_or_replace_attr(_attrs, key_str, val_str);
      return *this;
    }

    //! \brief Add an attribute to the graph.
    //!
    //! This function adds a new attribute, or replaces the value of an
    //! existing attribute, of a Dot object. The argument \p key
    //! is converted to std::string (unless they are std::string,
    //! std::string_view, or `char const*` already).
    //!
    //! \tparam Thing the type of the  argument.
    //!
    //! \param key the name of the attribute.
    //!
    //! \returns A reference to \c this.
    //!
    //! \warning No checks of any kind are performed that the string
    //! representations of \p key or \p val are valid in [DOT][]; see the
    //! [Graphviz documentation][] for more details.
    //!
    //! [Graphviz documentation]: https://www.graphviz.org/docs/nodes/
    //! [DOT]: https://www.graphviz.org/doc/info/lang.html
    template <typename Thing>
    Dot& add_attr(Thing&& key) {
      add_or_replace_attr(_attrs, key, "");
      return *this;
    }

    //! \brief Check if there is a node with a given name.
    //!
    //! This function returns \c true if \p name is currently the name of a node
    //! in the represented graph and \c false otherwise.
    //!
    //! \param name the name.
    //!
    //! \returns A \c bool.
    [[nodiscard]] bool is_node(std::string const& name) const {
      return _nodes.count(name);
    }

    //! \brief Check if there is a node with name obtained from an object.
    //!
    //! This function returns \c true if the value returned by std::to_string
    //! applied to \p name is currently the name of a node
    //! in the represented graph and \c false otherwise.
    //!
    //! \tparam Thing the type of the argument.
    //! \param thing the object that whose string representation might be a
    //! node.
    //!
    //! \returns A \c bool.
    template <typename Thing,
              typename std::enable_if_t<
                  !std::is_same_v<std::decay_t<Thing>, std::string>>>
    [[nodiscard]] bool is_node(Thing&& thing) const {
      return is_node(detail::dot_to_string(std::forward<Thing>(thing)));
    }

    //! \brief Add a node to the represented graph.
    //!
    //! This function adds a node with name obtained from \p thing by
    //! converting it to a std::string (unless they are std::string,
    //! std::string_view, or `char const*` already) using std::to_string.
    //!
    //! \tparam Thing the type of the argument.
    //!
    //! \param thing the object to use as the name of a node.
    //!
    //! \returns A reference to the Node object with \ref name obtained from
    //! \p thing.
    //!
    //! \throws LibsemigroupsException if there is already a node with
    //! name `std::to_string(thing)`.
    template <typename Thing>
    Node& add_node(Thing&& thing) {
      auto name_str       = detail::dot_to_string(std::forward<Thing>(thing));
      auto [it, inserted] = _nodes.emplace(name_str, Node(name_str));
      if (!inserted) {
        LIBSEMIGROUPS_EXCEPTION("there is already a node named {}!", name_str);
      }
      return it->second;
    }

    //! \brief Return a node from the represented graph.
    //!
    //! This function returns a reference to a node with name obtained from
    //! \p thing by converting it to a std::string (unless they are std::string,
    //! std::string_view, or `char const*` already) using std::to_string.
    //!
    //! \tparam Thing the type of the argument.
    //!
    //! \param thing the object to use as the name of a node.
    //!
    //! \returns A reference to the Node object with \ref name obtained from
    //! \p thing.
    //!
    //! \throws LibsemigroupsException if there is no node with name
    //! `std::to_string(thing)`.
    template <typename Thing>
    Node& node(Thing&& thing) {
      auto name_str = detail::dot_to_string(std::forward<Thing>(thing));
      auto it       = _nodes.find(name_str);
      if (it == _nodes.cend()) {
        LIBSEMIGROUPS_EXCEPTION("there is no node named {}!", name_str);
      }
      return it->second;
    }

    //! \brief Add an edge with given head and tail.
    //!
    //! This function adds an edge with head and tail obtained from \p head and
    //! \p tail by converting \p head and \p tail to std::string objects (unless
    //! they are std::string, std::string_view, or `char const*` already) using
    //! std::to_string.
    //!
    //! \tparam Thing1 the type of the 1st argument.
    //! \tparam Thing2 the type of the 2nd argument.
    //!
    //! \param head the head of the edge.
    //! \param tail the tail of the edge.
    //!
    //! \returns A reference to the Edge object from \p head to \p tail.
    //!
    //! \throws LibsemigroupsException if \p head or \p tail does not represent
    //! a node in the graph.
    template <typename Thing1, typename Thing2>
    Edge& add_edge(Thing1&& head, Thing2&& tail) {
      auto head_str = detail::dot_to_string(std::forward<Thing1>(head));
      auto tail_str = detail::dot_to_string(std::forward<Thing2>(tail));
      throw_if_not_node(head_str);
      throw_if_not_node(tail_str);
      _edges.emplace_back(head_str, tail_str);
      return _edges.back();
    }

    //! \brief Returns the first edge with given head and tail.
    //!
    //! This function returns the first edge with head and tail obtained from
    //! \p head and \p tail by converting \p head and \p tail to std::string
    //! objects (unless they are std::string, std::string_view, or `char const*`
    //! already) using std::to_string.
    //!
    //! \tparam Thing1 the type of the 1st argument.
    //! \tparam Thing2 the type of the 2nd argument.
    //!
    //! \param head the head of the edge.
    //! \param tail the tail of the edge.
    //!
    //! \returns A reference to the Edge object from \p head to \p tail.
    //!
    //! \throws LibsemigroupsException if \p head or \p tail does not represent
    //! a node in the graph.
    //!
    //! \throws LibsemigroupsException if there's no edge from \p head to
    //! \p tail.
    template <typename Thing1, typename Thing2>
    Edge& edge(Thing1&& head, Thing2&& tail) {
      auto head_str = detail::dot_to_string(std::forward<Thing1>(head));
      auto tail_str = detail::dot_to_string(std::forward<Thing2>(tail));
      throw_if_not_node(head_str);
      throw_if_not_node(tail_str);
      auto it = std::find_if(_edges.begin(), _edges.end(), [&](Edge const& e) {
        return e.head == head_str && e.tail == tail_str;
      });
      if (it == _edges.cend()) {
        LIBSEMIGROUPS_EXCEPTION(
            "there is no edges from {} to {}!", head_str, tail_str);
      }
      return *it;
    }

    //! \brief Convert a Dot object to a string.
    //!
    //! This function returns the string representation of the Dot object. This
    //! string contains a representation of the graph in the [DOT][] language
    //! for [Graphviz][].
    //!
    //! \returns A std::string by value.
    //!
    //! [DOT]: https://www.graphviz.org/doc/info/lang.html
    //! [Graphviz]: https://www.graphviz.org
    std::string to_string() const;

   private:
    void throw_if_not_node(std::string const& s);

    std::string_view edge_string() const noexcept;

    static void add_or_replace_attr(std::map<std::string, std::string>& attrs,
                                    std::string const&                  key,
                                    std::string const&                  val);
  };  // class Dot

  //! \relates Dot
  //!
  //! \brief Return a human readable representation of a Dot object.
  //!
  //! Return a human readable representation of a Dot object.
  //!
  //! \param d the Dot object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  std::string to_human_readable_repr(Dot const& d);

  //! \relates Dot::Node
  //!
  //! \brief Return a human readable representation of a Dot::Node object.
  //!
  //! Return a human readable representation of a Dot::Node object.
  //!
  //! \param n the Dot::Node object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  std::string to_human_readable_repr(Dot::Node const& n);

  //! \relates Dot::Edge
  //!
  //! \brief Return a human readable representation of a Dot::Edge object.
  //!
  //! Return a human readable representation of a Dot::Edge object.
  //!
  //! \param e the Dot::Edge object.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  std::string to_human_readable_repr(Dot::Edge const& e);

  //! \relates Dot
  //!
  //! \brief Return a human readable representation of a Dot::Kind object.
  //!
  //! Return a human readable representation of a Dot::Kind object.
  //!
  //! \param k the Dot::Kind object.
  //! \param sep separator to use between "Dot" and "Kind" (defaults to "::").
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  std::string to_human_readable_repr(Dot::Kind const&   k,
                                     std::string const& sep = "::");

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DOT_HPP_
