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

// This file contains the implementations of the functionality declared in
// paths.hpp for iterating through paths in an WordGraph.

namespace libsemigroups {
  namespace detail {
    static inline double magic_number(size_t N) {
      return 0.0015 * N + 2.43;
    }
  }  // namespace detail

  namespace detail {
    template <typename Node1, typename Node2>
    uint64_t number_of_paths_trivial(WordGraph<Node1> const& wg,
                                     Node2                   source,
                                     size_t                  min,
                                     size_t                  max) {
      if (min >= max) {
        return 0;
      } else if (v4::word_graph::is_complete(wg)) {
        // every edge is defined, and so the graph is not acyclic, and so the
        // number of words labelling paths is just the number of words
        if (max == POSITIVE_INFINITY) {
          return POSITIVE_INFINITY;
        } else {
          // TODO(2) it's easy for number_of_words to exceed 2 ^ 64, so
          // better do something more intelligent here to avoid this case.
          return number_of_words(wg.out_degree(), min, max);
        }
      }
      // Some edges are not defined ...
      if (!v4::word_graph::is_acyclic(wg, source) && max == POSITIVE_INFINITY) {
        // Not acyclic
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_trivial(WordGraph<Node1> const& wg,
                                     Node2                   source,
                                     Node2                   target,
                                     size_t                  min,
                                     size_t                  max) {
      if (min >= max || !v4::word_graph::is_reachable(wg, source, target)) {
        return 0;
      } else if (!v4::word_graph::is_acyclic(wg, source, target)
                 && max == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_matrix(WordGraph<Node1> const& wg,
                                    Node2                   source,
                                    size_t                  min,
                                    size_t                  max) {
      auto am = v4::word_graph::adjacency_matrix(wg);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      using Mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
#endif
      auto acc = Mat(Eigen::MatrixPower<Mat>(am)(min));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
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
      uint64_t const N     = wg.number_of_nodes();
      auto           acc   = matrix::pow(am, min);
      uint64_t       total = 0;
      for (size_t i = min; i < max; ++i) {
        uint64_t add = std::accumulate(acc.cbegin() + source * N,
                                       acc.cbegin() + source * N + N,
                                       static_cast<uint64_t>(0));
        if (add == 0) {
          break;
        }
        total += add;
        tmp.product_inplace_no_checks(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    // Used by the matrix(source, target) and the dfs(source, target)
    // algorithms
    template <typename Node1, typename Node2>
    bool number_of_paths_special(WordGraph<Node1> const& wg,
                                 Node2                   source,
                                 Node2                   target,
                                 size_t,
                                 size_t max) {
      if (max == POSITIVE_INFINITY) {
        if (source == target
            && std::any_of(wg.cbegin_targets(source),
                           wg.cend_targets(source),
                           [&wg, source](auto n) {
                             return n != UNDEFINED
                                    && v4::word_graph::is_reachable(
                                        wg, n, static_cast<Node1>(source));
                           })) {
          return true;
        } else if (source != target
                   && v4::word_graph::is_reachable(wg, source, target)
                   && v4::word_graph::is_reachable(wg, target, source)) {
          return true;
        }
      }
      return false;
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_matrix(WordGraph<Node1> const& wg,
                                    Node2                   source,
                                    Node2                   target,
                                    size_t                  min,
                                    size_t                  max) {
      if (!v4::word_graph::is_reachable(wg, source, target)) {
        // Complexity is O(number of nodes + number of edges).
        return 0;
      } else if (number_of_paths_special(wg, source, target, min, max)) {
        return POSITIVE_INFINITY;
      }
      auto am = v4::word_graph::adjacency_matrix(wg);
#ifdef LIBSEMIGROUPS_EIGEN_ENABLED
      using Mat = Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>;
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Winline"
#endif
      auto acc = Mat(Eigen::MatrixPower<Mat>(am)(min));
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif
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
      size_t const N     = wg.number_of_nodes();
      auto         tmp   = am;
      auto         acc   = matrix::pow(am, min);
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
        tmp.product_inplace_no_checks(acc, am);
        tmp.swap(acc);
      }
#endif
      return total;
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_acyclic(WordGraph<Node1> const& wg,
                                     Node2                   source,
                                     size_t                  min,
                                     size_t                  max) {
      auto topo = v4::word_graph::topological_sort(wg, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from {} is not acyclic",
                                source);
      } else if (topo.size() <= min) {
        // There are fewer than `min` nodes reachable from source, and so
        // there are no paths of length `min` or greater
        return 0;
      }

      LIBSEMIGROUPS_ASSERT(static_cast<Node1>(source) == topo.back());
      // Digraph is acyclic...
      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(2) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(max, topo.size()),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);
      number_paths.set(topo[0], 0, 1);
      for (size_t m = 1; m < topo.size(); ++m) {
        number_paths.set(topo[m], 0, 1);
        for (auto n = wg.cbegin_targets(topo[m]); n != wg.cend_targets(topo[m]);
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
                             static_cast<uint64_t>(0));
    }

    template <typename Node1, typename Node2>
    uint64_t number_of_paths_acyclic(WordGraph<Node1> const& wg,
                                     Node2                   source,
                                     Node2                   target,
                                     size_t                  min,
                                     size_t                  max) {
      auto topo = v4::word_graph::topological_sort(wg, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from {} is not acyclic",
                                source);
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
      std::vector<bool> lookup(wg.number_of_nodes(), true);
      std::for_each(
          topo.cbegin(), it, [&lookup](auto n) { lookup[n] = false; });

      // Remove the entries in topo after target
      topo.erase(topo.cbegin(), it);

      // Columns correspond to path lengths, rows to nodes in the graph
      // TODO(2) replace with DynamicTriangularArray2
      auto number_paths = detail::DynamicArray2<uint64_t>(
          std::min(topo.size(), max),
          *std::max_element(topo.cbegin(), topo.cend()) + 1,
          0);

      for (size_t m = 1; m < topo.size(); ++m) {
        for (auto n = wg.cbegin_targets(topo[m]); n != wg.cend_targets(topo[m]);
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
                             static_cast<uint64_t>(0));
    }

  }  // namespace detail

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(WordGraph<Node1> const& wg, Node2 source) {
    // Don't allow selecting the algorithm because we check
    // acyclicity anyway.
    // TODO(2): could use algorithm::dfs in some cases.
    word_graph::throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
    auto topo = v4::word_graph::topological_sort(wg, source);
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
        std::vector<uint64_t> number_paths(wg.number_of_nodes(), 0);
        for (auto m = topo.cbegin() + 1; m < topo.cend(); ++m) {
          for (auto n = wg.cbegin_targets(*m); n != wg.cend_targets(*m); ++n) {
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
  paths::algorithm number_of_paths_algorithm(WordGraph<Node1> const& wg,
                                             Node2                   source,
                                             size_t                  min,
                                             size_t                  max) {
    if (min >= max || v4::word_graph::is_complete(wg)) {
      return paths::algorithm::trivial;
    }

    auto topo = v4::word_graph::topological_sort(wg, source);
    if (topo.empty()) {
      // Can't topologically sort, so the digraph contains cycles, and so
      // there are infinitely many words labelling paths.
      if (max == POSITIVE_INFINITY) {
        return paths::algorithm::trivial;
      } else if (wg.number_of_edges()
                 < detail::magic_number(wg.number_of_nodes())
                       * wg.number_of_nodes()) {
        return paths::algorithm::dfs;
      } else {
        return paths::algorithm::matrix;
      }
    } else {
      // TODO(2) figure out threshold for using paths::algorithm::dfs
      return paths::algorithm::acyclic;
    }
  }

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(WordGraph<Node1> const& wg,
                           Node2                   source,
                           size_t                  min,
                           size_t                  max,
                           paths::algorithm        lgrthm) {
    word_graph::throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));

    switch (lgrthm) {
      case paths::algorithm::dfs:
        return std::distance(cbegin_pilo(wg, source, min, max), cend_pilo(wg));
      case paths::algorithm::matrix:
        return detail::number_of_paths_matrix(wg, source, min, max);
      case paths::algorithm::acyclic:
        return detail::number_of_paths_acyclic(wg, source, min, max);
      case paths::algorithm::trivial:
        return detail::number_of_paths_trivial(wg, source, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return number_of_paths(wg,
                               source,
                               min,
                               max,
                               number_of_paths_algorithm(wg, source, min, max));
    }
  }

  template <typename Node1, typename Node2>
  paths::algorithm number_of_paths_algorithm(WordGraph<Node1> const& wg,
                                             Node2                   source,
                                             Node2                   target,
                                             size_t                  min,
                                             size_t                  max) {
    bool acyclic = v4::word_graph::is_acyclic(wg, source, target);
    if (min >= max || !v4::word_graph::is_reachable(wg, source, target)
        || (!acyclic && max == POSITIVE_INFINITY)) {
      return paths::algorithm::trivial;
    } else if (acyclic && v4::word_graph::is_acyclic(wg, source)) {
      return paths::algorithm::acyclic;
    } else if (wg.number_of_edges() < detail::magic_number(wg.number_of_nodes())
                                          * wg.number_of_nodes()) {
      return paths::algorithm::dfs;
    } else {
      return paths::algorithm::matrix;
    }
  }

  template <typename Node1, typename Node2>
  uint64_t number_of_paths(WordGraph<Node1> const& wg,
                           Node2                   source,
                           Node2                   target,
                           size_t                  min,
                           size_t                  max,
                           paths::algorithm        lgrthm) {
    word_graph::throw_if_node_out_of_bounds(wg, static_cast<Node1>(source));
    word_graph::throw_if_node_out_of_bounds(wg, static_cast<Node1>(target));

    switch (lgrthm) {
      case paths::algorithm::dfs:
        if (detail::number_of_paths_special(wg, source, target, min, max)) {
          return POSITIVE_INFINITY;
        }
        return std::distance(cbegin_pstilo(wg, source, target, min, max),
                             cend_pstilo(wg));
      case paths::algorithm::matrix:
        return detail::number_of_paths_matrix(wg, source, target, min, max);
      case paths::algorithm::acyclic:
        return detail::number_of_paths_acyclic(wg, source, target, min, max);
      case paths::algorithm::trivial:
        return detail::number_of_paths_trivial(wg, source, target, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return number_of_paths(
            wg,
            source,
            target,
            min,
            max,
            number_of_paths_algorithm(wg, source, target, min, max));
    }
  }

  template <typename Node>
  Paths<Node>::Paths(Paths const&) = default;

  template <typename Node>
  Paths<Node>::Paths(Paths&&) = default;

  template <typename Node>
  Paths<Node>& Paths<Node>::operator=(Paths const&) = default;

  template <typename Node>
  Paths<Node>& Paths<Node>::operator=(Paths&&) = default;

  template <typename Node>
  Paths<Node>::~Paths() = default;

  template <typename Node>
  void Paths<Node>::throw_if_source_undefined() const {
    if (_source == UNDEFINED) {
      LIBSEMIGROUPS_EXCEPTION("no source node defined, use the member "
                              "function \"source\" to define the source node");
    }
  }

  template <typename Node>
  bool Paths<Node>::set_iterator_no_checks() const {
    size_t const N = _word_graph->number_of_nodes();

    if (!_current_valid && N != 0) {
      _current_valid = true;
      _position      = 0;
      if (_order == Order::shortlex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstislo(*_word_graph, _source, _target, _min, _max);
          _end     = cend_pstislo(*_word_graph);
        } else {
          _current = cbegin_pislo(*_word_graph, _source, _min, _max);
          _end     = cend_pislo(*_word_graph);
        }
      } else if (_order == Order::lex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstilo(*_word_graph, _source, _target, _min, _max);
          _end     = cend_pstilo(*_word_graph);
        } else {
          _current = cbegin_pilo(*_word_graph, _source, _min, _max);
          _end     = cend_pilo(*_word_graph);
        }
      }
      return true;
    }
    return N != 0;
  }

  template <typename Node>
  uint64_t Paths<Node>::size_hint() const {
    uint64_t num_paths = 0;
    if (_word_graph->number_of_nodes() == 0) {
      return num_paths;
    } else if (_target != UNDEFINED) {
      num_paths = number_of_paths(*_word_graph, _source, _target, _min, _max);
    } else {
      num_paths = number_of_paths(*_word_graph, _source, _min, _max);
    }

    if (_current_valid && num_paths != POSITIVE_INFINITY) {
      num_paths -= _position;
    }

    return num_paths;
  }

  template <typename Node>
  Paths<Node>& Paths<Node>::init() {
    _current_valid = false;
    _word_graph    = nullptr;
    _order         = Order::shortlex;
    _max           = static_cast<size_type>(POSITIVE_INFINITY);
    _min           = 0;
    _position      = 0;
    _source        = static_cast<Node>(UNDEFINED);
    _target        = static_cast<Node>(UNDEFINED);
    return *this;
  }

  template <typename Node>
  Node Paths<Node>::current_target() const {
    if (_target != UNDEFINED) {
      return _target;
    } else if (_source == UNDEFINED) {
      return UNDEFINED;
    }
    set_iterator_no_checks();
    // We are enumerating all paths with a given source, and so we return
    // the current target node using the iterator.
    return std::visit([](auto& it) { return it.target(); }, _current);
  }

  template <typename Node>
  template <typename Subclass>
  Subclass& Paths<Node>::order(Subclass* obj, Order val) {
    if (val != Order::shortlex && val != Order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be Order::shortlex or Order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return *obj;
  }

  template <typename Node>
  std::string to_human_readable_repr(Paths<Node> const& p) {
    using detail::group_digits;

    std::string source_target;
    std::string sep;
    if (p.source() != UNDEFINED) {
      source_target += fmt::format(" source {}", p.source());
      sep = ",";
    }
    if (p.target() != UNDEFINED) {
      source_target += fmt::format("{} target {}", sep, p.target());
      sep = ",";
    }

    return fmt::format(
        "<Paths in {} with{}{} length in [{}, {})>",
        to_human_readable_repr(p.word_graph()),
        source_target,
        sep,
        group_digits(p.min()),
        p.max() == POSITIVE_INFINITY ? "\u221e" : group_digits(p.max() + 1));
  }

}  // namespace libsemigroups

template <typename Node>
struct rx::is_input_range<typename libsemigroups::Paths<Node>>
    : std::true_type {};
