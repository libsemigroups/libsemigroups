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

// This file contains the implementations of the functionality declared in
// paths.hpp for iterating through paths in an ActionDigraph.

namespace libsemigroups {
  namespace detail {
    static inline double magic_number(size_t N) {
      return 0.0015 * N + 2.43;
    }
  }  // namespace detail

  template <typename Node>
  const_pilo_iterator<Node>::~const_pilo_iterator() = default;

  template <typename Node>
  const_pilo_iterator<Node>::const_pilo_iterator(const_pilo_iterator const&)
      = default;

  template <typename Node>
  const_pilo_iterator<Node>::const_pilo_iterator() = default;

  template <typename Node>
  const_pilo_iterator<Node>&
  const_pilo_iterator<Node>::operator=(const_pilo_iterator const&)
      = default;

  template <typename Node>
  const_pilo_iterator<Node>&
  const_pilo_iterator<Node>::operator=(const_pilo_iterator&&)
      = default;

  template <typename Node>
  const_pilo_iterator<Node>::const_pilo_iterator(const_pilo_iterator&&)
      = default;

  template <typename Node>
  const_pilo_iterator<Node>::const_pilo_iterator(ActionDigraph<Node> const* ptr,
                                                 Node      source,
                                                 size_type min,
                                                 size_type max)
      : _edges(),
        _digraph(ptr),
        _edge(UNDEFINED),
        _min(min),
        _max(max),
        _nodes() {
    if (_min < _max) {
      _nodes.push_back(source);
      if (_min != 0) {
        ++(*this);
      }
    }
  }

  template <typename Node>
  const_pilo_iterator<Node> const& const_pilo_iterator<Node>::operator++() {
    if (_nodes.empty()) {
      return *this;
    } else if (_edge == UNDEFINED) {
      // first call
      _edge = 0;
    }

    do {
      node_type next;
      std::tie(next, _edge)
          = _digraph->unsafe_next_neighbor(_nodes.back(), _edge);
      if (next != UNDEFINED && _edges.size() < _max - 1) {
        _nodes.push_back(next);
        _edges.push_back(_edge);
        _edge = 0;
        if (_edges.size() >= _min) {
          return *this;
        }
      } else {
        _nodes.pop_back();
        if (!_edges.empty()) {
          _edge = _edges.back() + 1;
          _edges.pop_back();
        }
      }
    } while (!_nodes.empty());

    return *this;
  }

  template <typename Node>
  void const_pilo_iterator<Node>::swap(const_pilo_iterator& that) noexcept {
    std::swap(_edges, that._edges);
    std::swap(_digraph, that._digraph);
    std::swap(_edge, that._edge);
    std::swap(_min, that._min);
    std::swap(_max, that._max);
    std::swap(_nodes, that._nodes);
  }

  // Assert that the forward iterator requirements are met
  static_assert(
      std::is_default_constructible<const_pilo_iterator<size_t>>::value,
      "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_pilo_iterator<size_t>>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_pilo_iterator<size_t>>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_pilo_iterator<size_t>>::value,
                "forward iterator requires destructible");

  template <typename Node>
  const_pislo_iterator<Node>::~const_pislo_iterator() = default;

  template <typename Node>
  const_pislo_iterator<Node>::const_pislo_iterator(const_pislo_iterator const&)
      = default;

  template <typename Node>
  const_pislo_iterator<Node>::const_pislo_iterator() = default;

  template <typename Node>
  const_pislo_iterator<Node>&
  const_pislo_iterator<Node>::operator=(const_pislo_iterator const&)
      = default;

  template <typename Node>
  const_pislo_iterator<Node>&
  const_pislo_iterator<Node>::operator=(const_pislo_iterator&&)
      = default;

  template <typename Node>
  const_pislo_iterator<Node>::const_pislo_iterator(const_pislo_iterator&&)
      = default;

  template <typename Node>
  const_pislo_iterator<Node>::const_pislo_iterator(
      ActionDigraph<Node> const*                     ptr,
      Node                                           source,
      typename const_pislo_iterator<Node>::size_type min,
      typename const_pislo_iterator<Node>::size_type max)
      : _length(min >= max ? UNDEFINED : min),
        _it(),
        _max(max),
        _source(source) {
    if (_length != UNDEFINED) {
      _it = cbegin_pilo(*ptr, source, _length, _length + 1);
    } else {
      _it = cend_pilo(*ptr);
    }
  }

  template <typename Node>
  const_pislo_iterator<Node> const& const_pislo_iterator<Node>::operator++() {
    ++_it;
    if (_it == cend_pilo(_it.digraph())) {
      if (_length < _max - 1) {
        ++_length;
        _it = cbegin_pilo(_it.digraph(), _source, _length, _length + 1);
        if (_it == cend_pilo(_it.digraph())) {
          _length = UNDEFINED;
        }
      } else {
        _length = UNDEFINED;
      }
    }
    return *this;
  }

  template <typename Node>
  void const_pislo_iterator<Node>::swap(const_pislo_iterator& that) noexcept {
    std::swap(_length, that._length);
    std::swap(_it, that._it);
    std::swap(_max, that._max);
    std::swap(_source, that._source);
  }

  static_assert(
      std::is_default_constructible<const_pislo_iterator<size_t>>::value,
      "forward iterator requires default-constructible");
  static_assert(std::is_copy_constructible<const_pislo_iterator<size_t>>::value,
                "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_pislo_iterator<size_t>>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_pislo_iterator<size_t>>::value,
                "forward iterator requires destructible");

  template <typename Node>
  const_pstilo_iterator<Node>::const_pstilo_iterator() = default;

  template <typename Node>
  const_pstilo_iterator<Node>::const_pstilo_iterator(
      const_pstilo_iterator const&)
      = default;

  template <typename Node>
  const_pstilo_iterator<Node>::const_pstilo_iterator(const_pstilo_iterator&&)
      = default;

  template <typename Node>
  const_pstilo_iterator<Node>&
  const_pstilo_iterator<Node>::operator=(const_pstilo_iterator const&)
      = default;

  template <typename Node>
  const_pstilo_iterator<Node>&
  const_pstilo_iterator<Node>::operator=(const_pstilo_iterator&&)
      = default;

  template <typename Node>
  const_pstilo_iterator<Node>::~const_pstilo_iterator() = default;

  template <typename Node>
  const_pstilo_iterator<Node>::const_pstilo_iterator(
      ActionDigraph<Node> const* ptr,
      node_type                  source,
      node_type                  target,
      size_type                  min,
      size_type                  max)
      : _edges({}),
        _digraph(ptr),
        _edge(UNDEFINED),
        _min(min),
        _max(max),
        _nodes(),
        _target(target) {
    if (_min < _max) {
      _nodes.push_back(source);
      ++(*this);
    }
  }

  template <typename Node>
  const_pstilo_iterator<Node> const& const_pstilo_iterator<Node>::operator++() {
    if (_nodes.empty()) {
      return *this;
    } else if (_edge == UNDEFINED) {
      // first call
      _edge = 0;
      init_can_reach_target();
      if (_min == 0 && _nodes.front() == _target) {
        // special case if the source == target, and we allow words of
        // length 0, then we return the empty word here.
        return *this;
      }
    }

    do {
      node_type next;
      std::tie(next, _edge)
          = _digraph->unsafe_next_neighbor(_nodes.back(), _edge);
      if (next != UNDEFINED && _edges.size() < _max - 1) {
        // Avoid infinite loops when we can never reach _target
        if (_can_reach_target[next]) {
          _nodes.push_back(next);
          _edges.push_back(_edge);
          _edge = 0;
          if (_edges.size() >= _min && next == _target) {
            return *this;
          }
        } else {
          _edge++;
        }
      } else {
        _nodes.pop_back();
        if (!_edges.empty()) {
          _edge = _edges.back() + 1;
          _edges.pop_back();
        }
      }
    } while (!_nodes.empty());
    return *this;
  }

  template <typename Node>
  void const_pstilo_iterator<Node>::swap(const_pstilo_iterator& that) noexcept {
    std::swap(_edges, that._edges);
    std::swap(_digraph, that._digraph);
    std::swap(_edge, that._edge);
    std::swap(_min, that._min);
    std::swap(_max, that._max);
    std::swap(_nodes, that._nodes);
    std::swap(_target, that._target);
  }

  template <typename Node>
  void const_pstilo_iterator<Node>::init_can_reach_target() {
    if (_can_reach_target.empty()) {
      std::vector<std::vector<node_type>> in_neighbours(
          _digraph->number_of_nodes(), std::vector<node_type>({}));
      for (auto n = _digraph->cbegin_nodes(); n != _digraph->cend_nodes();
           ++n) {
        for (auto e = _digraph->cbegin_edges(*n); e != _digraph->cend_edges(*n);
             ++e) {
          if (*e != UNDEFINED) {
            in_neighbours[*e].push_back(*n);
          }
        }
      }

      _can_reach_target.resize(_digraph->number_of_nodes(), false);
      _can_reach_target[_target]   = true;
      std::vector<node_type>& todo = in_neighbours[_target];
      std::vector<node_type>  next;

      while (!todo.empty()) {
        for (auto& m : todo) {
          if (_can_reach_target[m] == 0) {
            _can_reach_target[m] = true;
            next.insert(
                next.end(), in_neighbours[m].cbegin(), in_neighbours[m].cend());
          }
        }
        std::swap(next, todo);
        next.clear();
      }
    }
  }

  static_assert(
      std::is_default_constructible<const_pstilo_iterator<size_t>>::value,
      "forward iterator requires default-constructible");
  static_assert(
      std::is_copy_constructible<const_pstilo_iterator<size_t>>::value,
      "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_pstilo_iterator<size_t>>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_pstilo_iterator<size_t>>::value,
                "forward iterator requires destructible");

  template <typename Node>
  const_pstislo_iterator<Node>::~const_pstislo_iterator() = default;

  template <typename Node>
  const_pstislo_iterator<Node>::const_pstislo_iterator(
      const_pstislo_iterator const&)
      = default;

  template <typename Node>
  const_pstislo_iterator<Node>::const_pstislo_iterator() = default;

  template <typename Node>
  const_pstislo_iterator<Node>&
  const_pstislo_iterator<Node>::operator=(const_pstislo_iterator const&)
      = default;

  template <typename Node>
  const_pstislo_iterator<Node>&
  const_pstislo_iterator<Node>::operator=(const_pstislo_iterator&&)
      = default;

  template <typename Node>
  const_pstislo_iterator<Node>::const_pstislo_iterator(const_pstislo_iterator&&)
      = default;

  template <typename Node>
  const_pstislo_iterator<Node> const&
  const_pstislo_iterator<Node>::operator++() {
    if (_it.target() != UNDEFINED) {
      ++_it;
      while (_it.target() != _target && _it != _end) {
        ++_it;
      }
      if (_it == _end) {
        _target = UNDEFINED;
      }
    }
    return *this;
  }
  static_assert(
      std::is_default_constructible<const_pstislo_iterator<size_t>>::value,
      "forward iterator requires default-constructible");
  static_assert(
      std::is_copy_constructible<const_pstislo_iterator<size_t>>::value,
      "forward iterator requires copy-constructible");
  static_assert(std::is_copy_assignable<const_pstislo_iterator<size_t>>::value,
                "forward iterator requires copy-assignable");
  static_assert(std::is_destructible<const_pstislo_iterator<size_t>>::value,
                "forward iterator requires destructible");

  namespace detail {
    template <typename Node1, typename Node2>
    uint64_t number_of_paths_trivial(ActionDigraph<Node1> const& d,
                                     Node2                       source,
                                     size_t                      min,
                                     size_t                      max) {
      if (min >= max) {
        return 0;
      } else if (d.validate()) {
        // every edge is defined, and so the graph is not acyclic, and so the
        // number of words labelling paths is just the number of words
        if (max == POSITIVE_INFINITY) {
          return POSITIVE_INFINITY;
        } else {
          // TODO(later) it's easy for number_of_words to exceed 2 ^ 64, so
          // better do something more intelligent here to avoid this case.
          return number_of_words(d.out_degree(), min, max);
        }
      }
      // Some edges are not defined ...
      if (!action_digraph_helper::is_acyclic(d, source)
          && max == POSITIVE_INFINITY) {
        // Not acyclic
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_trivial(ActionDigraph<Node1> const& d,
                                     Node2                       source,
                                     Node2                       target,
                                     size_t                      min,
                                     size_t                      max) {
      if (min >= max
          || !action_digraph_helper::is_reachable(d, source, target)) {
        return 0;
      } else if (!action_digraph_helper::is_acyclic(d, source, target)
                 && max == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_matrix(ActionDigraph<Node1> const& d,
                                    Node2                       source,
                                    size_t                      min,
                                    size_t                      max) {
      auto am = action_digraph::adjacency_matrix(d);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto     acc   = action_digraph::pow(am, min);
      uint64_t total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc.row(source).sum();
        if (add == 0) {
          break;
        }
        total += add;
        acc *= am;
      }
#else
      auto           tmp   = am;
      uint64_t const N     = number_of_nodes();
      auto           acc   = matrix_helpers::pow(am, min);
      uint64_t       total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = std::accumulate(acc.cbegin() + source * N,
                                       acc.cbegin() + source * N + N,
                                       uint64_t(0));
        if (add == 0) {
          break;
        }
        total += add;
        tmp.product_inplace(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    // Used by the matrix(source, target) and the dfs(source, target)
    // algorithms
    template <typename Node1, typename Node2>
    bool number_of_paths_special(ActionDigraph<Node1> const& d,
                                 Node2                       source,
                                 Node2                       target,
                                 size_t,
                                 size_t max) {
      if (max == POSITIVE_INFINITY) {
        if (source == target
            && std::any_of(d.cbegin_edges(source),
                           d.cend_edges(source),
                           [&d, source](auto n) {
                             return n != UNDEFINED
                                    && action_digraph_helper::is_reachable(
                                        d, n, source);
                           })) {
          return true;
        } else if (source != target
                   && action_digraph_helper::is_reachable(d, source, target)
                   && action_digraph_helper::is_reachable(d, target, source)) {
          return true;
        }
      }
      return false;
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_matrix(ActionDigraph<Node1> const& d,
                                    Node2                       source,
                                    Node2                       target,
                                    size_t                      min,
                                    size_t                      max) {
      if (!action_digraph_helper::is_reachable(d, source, target)) {
        // Complexity is O(number of nodes + number of edges).
        return 0;
      } else if (number_of_paths_special(d, source, target, min, max)) {
        return POSITIVE_INFINITY;
      }
      auto am = action_digraph::adjacency_matrix(d);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      auto     acc   = action_digraph::pow(am, min);
      uint64_t total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc(source, target);
        if (add == 0 && acc.row(source).isZero()) {
          break;
        }
        total += add;
        acc *= am;
      }
#else
      size_t const N     = number_of_nodes();
      auto         tmp   = am;
      auto         acc   = matrix_helpers::pow(am, min);
      size_t       total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = acc(source, target);

        if (add == 0
            && std::all_of(acc.cbegin() + source * N,
                           acc.cbegin() + source * N + N,
                           [](uint64_t j) { return j == 0; })) {
          break;
        }
        total += add;
        tmp.product_inplace(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_acyclic(ActionDigraph<Node1> const& d,
                                     Node2                       source,
                                     size_t                      min,
                                     size_t                      max) {
      auto topo = action_digraph_helper::topological_sort(d, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from %llu is not acyclic",
                                static_cast<uint64_t>(source));
      } else if (topo.size() <= min) {
        // There are fewer than `min` nodes reachable from source, and so there
        // are no paths of length `min` or greater
        return 0;
      }

      LIBSEMIGROUPS_ASSERT(static_cast<Node1>(source) == topo.back());
      // Digraph is acyclic...
      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(later) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(max, topo.size()),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);
      number_paths.set(topo[0], 0, 1);
      for (size_t m = 1; m < topo.size(); ++m) {
        number_paths.set(topo[m], 0, 1);
        for (auto n = d.cbegin_edges(topo[m]); n != d.cend_edges(topo[m]);
             ++n) {
          if (*n != UNDEFINED) {
            // there are no paths longer than m + 1 from the m-th entry in
            // the topological sort.
            for (size_t i = 1; i < std::min(max, m + 1); ++i) {
              number_paths.set(topo[m],
                               i,
                               number_paths.get(*n, i - 1)
                                   + number_paths.get(topo[m], i));
            }
          }
        }
      }
      return std::accumulate(number_paths.cbegin_row(source) + min,
                             number_paths.cbegin_row(source)
                                 + std::min(topo.size(), max),
                             uint64_t(0));
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_acyclic(ActionDigraph<Node1> const& d,
                                     Node2                       source,
                                     Node2                       target,
                                     size_t                      min,
                                     size_t                      max) {
      auto topo = action_digraph_helper::topological_sort(d, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from %llu is not acyclic",
                                static_cast<uint64_t>(source));
      } else if ((max == 1 && source != target)
                 || (min != 0 && source == target)) {
        return 0;
      } else if (source == target) {
        // the empty path
        return 1;
      }

      // Subdigraph induced by nodes reachable from `source` is acyclic...
      LIBSEMIGROUPS_ASSERT(static_cast<Node1>(source) == topo.back());
      auto it = std::find(topo.cbegin(), topo.cend(), target);
      if (it == topo.cend() || size_t(std::distance(it, topo.cend())) <= min) {
        // 1) `target` not reachable from `source`, or
        // 2) every path from `source` to `target` has length < min.
        return 0;
      }
      // Don't visit nodes that occur after target in "topo".
      std::vector<bool> lookup(d.number_of_nodes(), true);
      std::for_each(
          topo.cbegin(), it, [&lookup](auto n) { lookup[n] = false; });

      // Remove the entries in topo after target
      topo.erase(topo.cbegin(), it);

      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(later) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(topo.size(), max),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);

      for (size_t m = 1; m < topo.size(); ++m) {
        for (auto n = d.cbegin_edges(topo[m]); n != d.cend_edges(topo[m]);
             ++n) {
          if (*n == static_cast<Node1>(target)) {
            number_paths.set(topo[m], 1, number_paths.get(topo[m], 1) + 1);
          }
          if (*n != UNDEFINED && lookup[*n]) {
            // there are no paths longer than m + 1 from the m-th entry in
            // the topological sort.
            for (size_t i = 1; i < std::min(max, m + 1); ++i) {
              number_paths.set(topo[m],
                               i,
                               number_paths.get(*n, i - 1)
                                   + number_paths.get(topo[m], i));
            }
          }
        }
      }
      return std::accumulate(number_paths.cbegin_row(source) + min,
                             number_paths.cbegin_row(source)
                                 + std::min(topo.size(), max),
                             uint64_t(0));
    }

  }  // namespace detail

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(ActionDigraph<Node1> const& d, Node2 source) {
    // Don't allow selecting the algorithm because we check
    // acyclicity anyway.
    // TODO(later): could use algorithm::dfs in some cases.
    action_digraph_helper::validate_node(d, source);
    auto topo = action_digraph_helper::topological_sort(d, source);
    if (topo.empty()) {
      // Can't topologically sort, so the subdigraph induced by the nodes
      // reachable from source, contains cycles, and so there are infinitely
      // many words labelling paths.
      return POSITIVE_INFINITY;
    } else {
      // Digraph is acyclic...
      LIBSEMIGROUPS_ASSERT(topo.back() == static_cast<Node1>(source));
      if (static_cast<Node1>(source) == topo[0]) {
        // source is the "sink" of the digraph, and so there's only 1 path
        // (the empty one).
        return 1;
      } else {
        std::vector<uint64_t> number_paths(d.number_of_nodes(), 0);
        for (auto m = topo.cbegin() + 1; m < topo.cend(); ++m) {
          for (auto n = d.cbegin_edges(*m); n != d.cend_edges(*m); ++n) {
            if (*n != UNDEFINED) {
              number_paths[*m] += (number_paths[*n] + 1);
            }
          }
        }
        return number_paths[source] + 1;
      }
    }
  }

  template <typename Node1, typename Node2>
  paths::algorithm number_of_paths_algorithm(ActionDigraph<Node1> const& d,
                                             Node2                       source,
                                             size_t                      min,
                                             size_t                      max) {
    if (min >= max || d.validate()) {
      return paths::algorithm::trivial;
    }

    auto topo = action_digraph_helper::topological_sort(d, source);
    if (topo.empty()) {
      // Can't topologically sort, so the digraph contains cycles, and so
      // there are infinitely many words labelling paths.
      if (max == POSITIVE_INFINITY) {
        return paths::algorithm::trivial;
      } else if (d.number_of_edges() < detail::magic_number(d.number_of_nodes())
                                           * d.number_of_nodes()) {
        return paths::algorithm::dfs;
      } else {
        return paths::algorithm::matrix;
      }
    } else {
      // TODO(later) figure out threshold for using paths::algorithm::dfs
      return paths::algorithm::acyclic;
    }
  }

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(ActionDigraph<Node1> const& d,
                           Node2                       source,
                           size_t                      min,
                           size_t                      max,
                           paths::algorithm            lgrthm) {
    action_digraph_helper::validate_node(d, source);

    switch (lgrthm) {
      case paths::algorithm::dfs:
        return std::distance(cbegin_pilo(d, source, min, max), cend_pilo(d));
      case paths::algorithm::matrix:
        return detail::number_of_paths_matrix(d, source, min, max);
      case paths::algorithm::acyclic:
        return detail::number_of_paths_acyclic(d, source, min, max);
      case paths::algorithm::trivial:
        return detail::number_of_paths_trivial(d, source, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return number_of_paths(d,
                               source,
                               min,
                               max,
                               number_of_paths_algorithm(d, source, min, max));
    }
  }

  template <typename Node1, typename Node2>
  paths::algorithm number_of_paths_algorithm(ActionDigraph<Node1> const& d,
                                             Node2                       source,
                                             Node2                       target,
                                             size_t                      min,
                                             size_t                      max) {
    bool acyclic = action_digraph_helper::is_acyclic(d, source, target);
    if (min >= max || !action_digraph_helper::is_reachable(d, source, target)
        || (!acyclic && max == POSITIVE_INFINITY)) {
      return paths::algorithm::trivial;
    } else if (acyclic && action_digraph_helper::is_acyclic(d, source)) {
      return paths::algorithm::acyclic;
    } else if (d.number_of_edges() < detail::magic_number(d.number_of_nodes())
                                         * d.number_of_nodes()) {
      return paths::algorithm::dfs;
    } else {
      return paths::algorithm::matrix;
    }
  }

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(ActionDigraph<Node1> const& d,
                           Node2                       source,
                           Node2                       target,
                           size_t                      min,
                           size_t                      max,
                           paths::algorithm            lgrthm) {
    action_digraph_helper::validate_node(d, source);
    action_digraph_helper::validate_node(d, target);

    switch (lgrthm) {
      case paths::algorithm::dfs:
        if (detail::number_of_paths_special(d, source, target, min, max)) {
          return POSITIVE_INFINITY;
        }
        return std::distance(cbegin_pstilo(d, source, target, min, max),
                             cend_pstilo(d));
      case paths::algorithm::matrix:
        return detail::number_of_paths_matrix(d, source, target, min, max);
      case paths::algorithm::acyclic:
        return detail::number_of_paths_acyclic(d, source, target, min, max);
      case paths::algorithm::trivial:
        return detail::number_of_paths_trivial(d, source, target, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return number_of_paths(
            d,
            source,
            target,
            min,
            max,
            number_of_paths_algorithm(d, source, target, min, max));
    }
  }

  template <typename Node>
  bool Paths<Node>::set_iterator() const {
    size_t const N = _digraph->number_of_nodes();

    if (!_current_valid && N != 0) {
      _current_valid = true;
      if (_order == order::shortlex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstislo(*_digraph, _source, _target, _min, _max);
          _end     = cend_pstislo(*_digraph);
        } else {
          _current = cbegin_pislo(*_digraph, _source, _min, _max);
          _end     = cend_pislo(*_digraph);
        }
      } else if (_order == order::lex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstilo(*_digraph, _source, _target, _min, _max);
          _end     = cend_pstilo(*_digraph);
        } else {
          _current = cbegin_pilo(*_digraph, _source, _min, _max);
          _end     = cend_pilo(*_digraph);
        }
      }
      return true;
    }
    return N != 0;
  }

  template <typename Node>
  uint64_t Paths<Node>::size_hint() const {
    if (_digraph->number_of_nodes() == 0) {
      return 0;
    } else if (_target != UNDEFINED) {
      return number_of_paths(*_digraph, _source, _target, _min, _max);
    } else {
      return number_of_paths(*_digraph, _source, _min, _max);
    }
  }

  template <typename Node>
  Paths<Node>& Paths<Node>::init() {
    _current_valid = false;
    _digraph       = nullptr;
    _order         = order::shortlex;
    _max           = POSITIVE_INFINITY;
    _min           = 0;
    _source        = UNDEFINED;
    _target        = UNDEFINED;
    return *this;
  }

  template <typename Node>
  Node Paths<Node>::to() const {
    if (_target != UNDEFINED) {
      return _target;
    } else {
      set_iterator();
      // We are enumerating all paths with a given source, and so we return
      // the current target node using the iterator.
      return std::visit([](auto& it) { return it.target(); }, _current);
    }
  }

  template <typename Node>
  template <typename Subclass>
  Subclass& Paths<Node>::order(Subclass* obj, enum order val) {
    if (val != order::shortlex && val != order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be order::shortlex or order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return obj;
  }

}  // namespace libsemigroups

template <typename Node>
struct rx::is_input_range<typename libsemigroups::Paths<Node>>
    : std::true_type {};
