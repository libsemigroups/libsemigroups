// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2020 James D. Mitchell, Maria Tsalakou
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

#ifndef LIBSEMIGROUPS_DIGRAPH_HELPER_HPP_
#define LIBSEMIGROUPS_DIGRAPH_HELPER_HPP_

#include <stack>        // for stack
#include <type_traits>  // for decay
#include <utility>      // for pair
#include <vector>       // for vector

// TODO iwyu

#include "constants.hpp"
#include "libsemigroups-exception.hpp"
#include "types.hpp"

namespace libsemigroups {
  template <typename T>
  class ActionDigraph;

  namespace action_digraph_helper {
    //! Undoc
    template <typename T>
    using node_type = typename ActionDigraph<T>::node_type;

    //! Undoc
    template <typename T>
    using label_type = typename ActionDigraph<T>::label_type;

    namespace detail {
      template <typename T>
      void validate_node(ActionDigraph<T> const& ad, node_type<T> const v) {
        if (v >= ad.nr_nodes()) {
          LIBSEMIGROUPS_EXCEPTION("node value out of bounds, expected value in "
                                  "the range [0, %d), got %d",
                                  ad.nr_nodes(),
                                  v);
        }
      }
    }  // namespace detail

    //! Find the node that a path starting at a given node leads to.
    //!
    //! \tparam T the type used as the template parameter for the ActionDigraph.
    //!
    //! \param ad the ActionDigraph object to check.
    //! \param first the ActionDigraph object to check.
    //! \param path the ActionDigraph object to check.
    //!
    //! \returns
    //! A value of type ActionDigraph::node_type. If one or more edges in
    //! \p path are not defined, then libsemigroups::UNDEFINED is returned.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! Linear in the length of \p path.
    // TODO example
    template <typename T>
    node_type<T> follow_path(ActionDigraph<T> const& ad,
                             node_type<T> const      first,
                             word_type const&        path) {
      node_type<T> last = first;
      for (auto it = path.cbegin(); it < path.cend() && last != UNDEFINED;
           ++it) {
        last = ad.neighbor(last, *it);
      }
      return last;
    }

    namespace detail {
      template <typename T>
      using stack_type  = std::stack<std::pair<node_type<T>, label_type<T>>>;
      using lookup_type = std::vector<uint8_t>;

      // Helper function for the two versions of is_acyclic below.
      template <typename T>
      bool is_acyclic(ActionDigraph<T> const& ad,
                      stack_type<T>&          stck,
                      lookup_type&            seen) {
        size_t const M = ad.out_degree();
        do {
          auto& p = stck.top();
          if (seen[p.first] == 2) {
            return false;
          } else if (seen[p.first] == 1 || p.second >= M) {
            seen[p.first] = 1;
            stck.pop();
            seen[stck.top().first] = 0;
            stck.top().second++;
          } else {
            node_type<T> node;
            std::tie(node, p.second)
                = ad.unsafe_next_neighbor(p.first, p.second);
            if (node != UNDEFINED) {
              seen[p.first] = 2;
              stck.emplace(node, 0);
            }
          }
        } while (stck.size() > 1);
        return true;
      }
    }  // namespace detail

    //! Check if a digraph is acyclic.
    //!
    //! \tparam T the type used as the template parameter for the ActionDigraph.
    //!
    //! \param ad the ActionDigraph object to check.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the ActionDigraph
    //! \p ad and \f$n\f$ is the number of edges. Note that for ActionDigraph
    //! objects the number of edges is always \f$mk\f$ where \f$k\f$ is the
    //! ActionDigraph::out_degree.
    //!
    //! A digraph is acyclic if every directed cycle on the digraph is
    //! trivial.
    //!
    //! \par Example
    //! \code
    //! ActionDigraph<size_t> ad;
    //! ad.add_nodes(2);
    //! ad.add_to_out_degree(1);
    //! ad.add_edge(0, 1, 0);
    //! ad.add_edge(1, 0, 0);
    //! action_digraph_helper::is_acyclic(ad); // returns false
    //! \endcode
    template <typename T>
    bool is_acyclic(ActionDigraph<T> const& ad) {
      node_type<T> const                                 N = ad.nr_nodes();
      std::stack<std::pair<node_type<T>, label_type<T>>> stck;
      std::vector<uint8_t>                               seen(N, 0);
      for (node_type<T> m = 0; m < N; ++m) {
        if (seen[m] == 0) {
          stck.emplace(m, 0);
          if (!detail::is_acyclic(ad, stck, seen)) {
            return false;
          }
        }
      }
      return true;
    }

    //! Check if the subdigraph induced by the nodes reachable from a source
    //! node is acyclic.
    //!
    //! \tparam T the type used as the template parameter for the ActionDigraph.
    //!
    //! \param ad the ActionDigraph object to check.
    //! \param source the source node.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the ActionDigraph
    //! \p ad and \f$n\f$ is the number of edges. Note that for ActionDigraph
    //! objects the number of edges is always \f$mk\f$ where \f$k\f$ is the
    //! ActionDigraph::out_degree.
    //!
    //! A digraph is acyclic if every directed cycle on the digraph is
    //! trivial.
    //!
    //! \par Example
    //! \code
    //! ActionDigraph<size_t> ad;
    //! ad.add_nodes(4);
    //! ad.add_to_out_degree(1);
    //! ad.add_edge(0, 1, 0);
    //! ad.add_edge(1, 0, 0);
    //! ad.add_edge(2, 3, 0);
    //! action_digraph_helper::is_acyclic(ad); // returns false
    //! action_digraph_helper::is_acyclic(ad, 0); // returns false
    //! action_digraph_helper::is_acyclic(ad, 1); // returns false
    //! action_digraph_helper::is_acyclic(ad, 2); // returns true
    //! action_digraph_helper::is_acyclic(ad, 3); // returns true
    //! \endcode
    template <typename T>
    bool is_acyclic(ActionDigraph<T> const& ad, node_type<T> source) {
      detail::validate_node(ad, source);
      std::stack<std::pair<node_type<T>, label_type<T>>> stck;
      stck.emplace(source, 0);
      std::vector<uint8_t> seen(ad.nr_nodes(), 0);
      return detail::is_acyclic(ad, stck, seen);
    }

    namespace detail {
      template <typename T>
      bool is_reachable(ActionDigraph<T> const&    ad,
                        node_type<T> const         source,
                        node_type<T> const         target,
                        std::stack<node_type<T>>&  nodes,
                        std::stack<label_type<T>>& edges,
                        std::vector<bool>&         seen,
                        label_type<T>              edge) {
        nodes.push(source);
        seen[source] = true;
        do {
          node_type<T> node;
          std::tie(node, edge) = ad.unsafe_next_neighbor(nodes.top(), edge);
          if (node == target) {
            return true;
          } else if (node != UNDEFINED) {
            if (!seen[node]) {
              // dive, dive, dive!!
              seen[node] = true;
              nodes.push(node);
              edges.push(edge);
              edge = 0;
            } else {
              ++edge;
            }
          } else {
            // backtrack
            nodes.pop();
            if (!edges.empty()) {
              edge = edges.top();
              edges.pop();
            }
          }
        } while (!nodes.empty());
        return false;
      }
    }  // namespace detail

    //! Check if there is a path from one node to another.
    //!
    //! \tparam T the type used as the template parameter for the ActionDigraph.
    //!
    //! \param ad the ActionDigraph object to check.
    //! \param source the source node.
    //! \param target the source node.
    //!
    //! \returns
    //! A value of type `bool`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    //!
    //! \par Complexity
    //! \f$O(m + n)\f$ where \f$m\f$ is the number of nodes in the ActionDigraph
    //! \p ad and \f$n\f$ is the number of edges. Note that for ActionDigraph
    //! objects the number of edges is always \f$mk\f$ where \f$k\f$ is the
    //! ActionDigraph::out_degree.
    //!
    //! \note
    //! If \p source and \p target are equal, then, by convention, we consider
    //! \p target to be reachable from \p source, via the empty path.
    //!
    //! \par Example
    //! \code
    //! ActionDigraph<size_t> ad;
    //! ad.add_nodes(4);
    //! ad.add_to_out_degree(1);
    //! ad.add_edge(0, 1, 0);
    //! ad.add_edge(1, 0, 0);
    //! ad.add_edge(2, 3, 0);
    //! action_digraph_helper::is_reachable(ad, 0, 1); // returns true
    //! action_digraph_helper::is_reachable(ad, 1, 0); // returns true
    //! action_digraph_helper::is_reachable(ad, 1, 2); // returns false
    //! action_digraph_helper::is_reachable(ad, 2, 3); // returns true
    //! action_digraph_helper::is_reachable(ad, 3, 2); // returns false
    //! \endcode
    template <typename T>
    bool is_reachable(ActionDigraph<T> const& ad,
                      node_type<T> const      source,
                      node_type<T> const      target) {
      detail::validate_node(ad, source);
      detail::validate_node(ad, target);
      if (source == target) {
        return true;
      }
      std::stack<node_type<T>>  nodes;
      std::stack<label_type<T>> edges;
      std::vector<bool>         seen(ad.nr_nodes(), false);
      return detail::is_reachable(ad, source, target, nodes, edges, seen, 0);
    }
  }  // namespace action_digraph_helper
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_DIGRAPH_HELPER_HPP_
