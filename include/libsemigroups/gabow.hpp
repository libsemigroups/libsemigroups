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

// This file contains an implementation of Gabow's algorithm for WordGraphs.

#ifndef LIBSEMIGROUPS_GABOW_HPP_
#define LIBSEMIGROUPS_GABOW_HPP_

#include <stack>  // for stack

#include "digraph.hpp"  // for ActionDigraph
#include "forest.hpp"   // for Forest
#include "runner.hpp"   // for Runner

#include "rx/ranges.hpp"  // for transform

namespace libsemigroups {

  template <typename Node>
  class Gabow : public Runner {
   public:
    using node_type  = Node;
    using label_type = typename ActionDigraph<node_type>::label_type;
    using size_type  = size_t;

   private:
    ActionDigraph<node_type> const&     _graph;
    std::vector<std::vector<node_type>> _comps;
    bool                                _finished;
    std::vector<size_type>              _id;
    Forest                              _bckwd_forest;
    bool                                _bckwd_forest_defined;
    Forest                              _forwd_forest;
    bool                                _forwd_forest_defined;

   public:
    Gabow(ActionDigraph<node_type> const& wg)
        : _graph(wg),
          _comps(),
          _finished(false),
          _id(),
          _bckwd_forest(),
          _bckwd_forest_defined(false),
          _forwd_forest(),
          _forwd_forest_defined(false) {}

    [[nodiscard]] size_type id_no_checks(node_type v) {
      run();
      return _id[v];
    }

    //! Returns the id-number of the strongly connected component of a node.
    //!
    //! \param nd the node.
    //!
    //! \returns
    //! The index of the node \p nd, a value of type scc_index_type.
    //!
    //! \throws LibsemigroupsException if \p nd is not valid.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because validate_node isn't
    [[nodiscard]] size_type id(node_type v) {
      run();
      validate_node(v);
      return _id[v];
    }

    //! Returns an iterator pointing to the vector of nodes in the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_sccs.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] auto components() {
      run();
      return _comps;
    }

    [[nodiscard]] auto component(size_t i) {
      run();
      if (i >= number()) {
        LIBSEMIGROUPS_EXCEPTION_V3("TODO");
      }
      return _comps[i];
    }

    [[nodiscard]] auto component_no_checks(size_t i) {
      run();
      return _comps[i];
    }

    // TODO better name
    //! Returns the number of strongly connected components.
    //!
    //! \returns
    //! A `size_t`.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] auto number() {
      run();
      return _comps.size();
    }

    //! Returns an iterator pointing to the root of the first scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc_roots.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    [[nodiscard]] auto roots() {
      return (rx::iterator_range(_comps.cbegin(), _comps.cend())
              | rx::transform([](auto const& comp) { return comp[0]; }));
    }

    //! Returns the root of a strongly connected components containing a given
    //! node.
    //!
    //! \param nd a node.
    //!
    //! \returns
    //! The root of the scc containing the node \p nd, a value of
    //! \ref node_type.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //! \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    // Not noexcept because scc_id isn't
    [[nodiscard]] node_type root_of(node_type n) {
      run();
      validate_node(n);
      return root_of_no_checks(n);
    }

    [[nodiscard]] node_type root_of_no_checks(node_type n) {
      run();
      return component_of_no_checks(n)[0];
    }

    //! Returns an iterator pointing to the first node in the scc with
    //! the specified id-number.
    //!
    //! \param i the id-number of the scc.
    //!
    //! \returns
    //! A \ref const_iterator_scc.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl.
    //!
    //! \throws LibsemigroupsException if \p i is not in the range \c 0 to \c
    //! number_of_scc() - 1.
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \note
    //! \basic_guarantee
    //!
    [[nodiscard]] auto component_of(node_type n) {
      run();
      validate_node(n);
      return _comps[_id[n]];
    }

    [[nodiscard]] auto component_of_no_checks(node_type n) {
      run();
      return _comps[_id[n]];
    }

    Gabow& reset() noexcept {
      _finished             = false;
      _bckwd_forest_defined = false;
      _forwd_forest_defined = false;
      return *this;
    }

    //! Returns a spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented away from the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& spanning_forest() {
      if (_forwd_forest_defined) {
        return _forwd_forest;
      }

      run();

      std::vector<bool>     seen(_graph.number_of_nodes(), false);
      std::queue<node_type> queue;

      _forwd_forest.clear();
      _forwd_forest.add_nodes(_graph.number_of_nodes());

      for (size_t i = 0; i < number(); ++i) {
        queue.push(_comps[i][0]);
        seen[_comps[i][0]] = true;
        do {
          size_t x = queue.front();
          for (auto e : _graph.labels()) {
            size_t y = _graph.unsafe_neighbor(x, e);
            if (y != UNDEFINED && !seen[y] && _id[y] == _id[x]) {
              _forwd_forest.set(y, x, e);
              queue.push(y);
              seen[y] = true;
            }
          }
          queue.pop();
        } while (!queue.empty());
      }
      _forwd_forest_defined = true;
      return _forwd_forest;
    }

    //! Returns a reverse spanning forest of the strongly connected components.
    //!
    //! Returns a Forest comprised of spanning trees for each
    //! scc of \c this, rooted on the minimum node of that component, with
    //! edges oriented towards the root.
    //!
    //! \returns
    //! A const reference to a Forest.
    //!
    //! \throws LibsemigroupsException if it is not the case that every node
    //! has exactly out_degree() out-neighbors. In other words, if
    //! neighbor() is libsemigroups::UNDEFINED for any node \c nd and
    //! any label \c lbl. \basic_guarantee
    //!
    //! \complexity
    //! At most \f$O(mn)\f$ where \c m is number_of_nodes() and \c n is
    //! out_degree().
    //!
    //! \par Parameters
    //! (None)
    Forest const& reverse_spanning_forest() {
      if (_bckwd_forest_defined) {
        return _bckwd_forest;
      }
      run();

      _bckwd_forest.clear();
      _bckwd_forest.add_nodes(_graph.number_of_nodes());

      // TODO remove this
      std::vector<std::vector<node_type>> reverse_edges(
          _graph.number_of_nodes(), std::vector<node_type>());
      std::vector<std::vector<node_type>> reverse_labels(
          _graph.number_of_nodes(), std::vector<node_type>());

      for (auto n : _graph.nodes()) {
        size_t const scc_id_n = id(n);
        for (auto e : _graph.labels()) {
          size_t const k = _graph.unsafe_neighbor(n, e);
          if (id(k) == scc_id_n) {
            reverse_edges[k].push_back(n);
            reverse_labels[k].push_back(e);
          }
        }
      }

      std::queue<size_t> queue;
      std::vector<bool>  seen(_graph.number_of_nodes(), false);

      for (size_t i = 0; i < number(); ++i) {
        LIBSEMIGROUPS_ASSERT(queue.empty());
        queue.push(_comps[i][0]);
        seen[_comps[i][0]] = true;
        while (!queue.empty()) {
          size_t x = queue.front();
          for (size_t j = 0; j < reverse_edges[x].size(); ++j) {
            size_t y = reverse_edges[x][j];
            if (!seen[y]) {
              queue.push(y);
              seen[y] = true;
              _bckwd_forest.set(y, x, reverse_labels[x][j]);
            }
          }
          queue.pop();
        }
      }
      _bckwd_forest_defined = true;
      return _bckwd_forest;
    }

   private:
    bool finished_impl() const override {
      return _finished;
    }

    void run_impl() override {
      if (finished()) {
        return;
      }

      std::stack<node_type>                        stack1;
      std::stack<node_type>                        stack2;
      std::stack<std::pair<node_type, label_type>> frame;
      std::vector<node_type>                       preorder;

      preorder.assign(_graph.number_of_nodes(), UNDEFINED);
      LIBSEMIGROUPS_ASSERT(stack1.empty());
      LIBSEMIGROUPS_ASSERT(stack2.empty());
      LIBSEMIGROUPS_ASSERT(frame.empty());

      _comps.clear();
      _id.assign(_graph.number_of_nodes(), UNDEFINED);

      node_type C     = 0;
      node_type index = 0;

      for (auto w : _graph.nodes()) {
        if (_id[w] == UNDEFINED) {
          frame.emplace(w, 0);
        dfs_start:
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          node_type  v = frame.top().first;
          label_type i = frame.top().second;

          preorder[v] = C++;
          stack1.push(v);
          stack2.push(v);
          for (; i < _graph.out_degree(); ++i) {
          dfs_end:
            LIBSEMIGROUPS_ASSERT(v < number_of_nodes() && i < _degree);
            node_type u = _graph.unsafe_neighbor(v, i);
            if (u != UNDEFINED) {
              if (preorder[u] == UNDEFINED) {
                frame.top().second = i;
                frame.emplace(u, 0);
                goto dfs_start;
              } else if (_id[u] == UNDEFINED) {
                LIBSEMIGROUPS_ASSERT(!stack2.empty());
                while (preorder[stack2.top()] > preorder[u]) {
                  stack2.pop();
                }
              }
            }
          }
          if (v == stack2.top()) {
            _comps.emplace_back();
            node_type x;
            do {
              LIBSEMIGROUPS_ASSERT(!stack1.empty());
              x      = stack1.top();
              _id[x] = index;
              _comps[index].push_back(x);
              stack1.pop();
            } while (x != v);
            ++index;
            LIBSEMIGROUPS_ASSERT(!stack2.empty());
            stack2.pop();
          }
          LIBSEMIGROUPS_ASSERT(!frame.empty());
          frame.pop();
          if (!frame.empty()) {
            v = frame.top().first;
            i = frame.top().second;
            goto dfs_end;
          }
        }
      }
      _finished = true;
    }

    void validate_node(node_type n) const {
      if (n >= _id.size()) {
        LIBSEMIGROUPS_EXCEPTION_V3("TODO");
      }
    }

    // TODO use this or delete it
    void validate_scc_index(size_t i) const {
      if (i >= number()) {
        LIBSEMIGROUPS_EXCEPTION("strong component index out of bounds, "
                                "expected value in the range [0, %d), got %d",
                                number(),
                                i);
      }
    }
  };

  template <typename Node>
  Gabow(ActionDigraph<Node> const&) -> Gabow<Node>;

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_GABOW_HPP_
