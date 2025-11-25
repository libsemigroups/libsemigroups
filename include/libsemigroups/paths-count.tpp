//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains implementations related to counting paths in
// WordGraphs.

namespace libsemigroups::v4::paths {

  // TODO(v4) remove the prefix ::libsemigroups in this file

  namespace detail {
    static inline double magic_number(size_t N) {
      return 0.0015 * N + 2.43;
    }

    template <typename Node1, typename Node2>
    uint64_t count_trivial(WordGraph<Node1> const& wg,
                           Node2                   source,
                           size_t                  min,
                           size_t                  max) {
      if (min > max) {
        return 0;
      } else if (word_graph::is_complete(wg)) {
        // every edge is defined, and so the graph is not acyclic, and so the
        // number of words labelling paths is just the number of words
        if (max == POSITIVE_INFINITY) {
          return POSITIVE_INFINITY;
        } else {
          // TODO(2) it's easy for number_of_words to exceed 2 ^ 64, so
          // better do something more intelligent here to avoid this case.
          return number_of_words(wg.out_degree(), min, max + 1);
        }
      }
      // Some edges are not defined ...
      if (!word_graph::is_acyclic(wg, source) && max == POSITIVE_INFINITY) {
        // Not acyclic
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t count_trivial(WordGraph<Node1> const& wg,
                           Node2                   source,
                           Node2                   target,
                           size_t                  min,
                           size_t                  max) {
      if (min > max || !word_graph::is_reachable(wg, source, target)) {
        return 0;
      } else if (!word_graph::is_acyclic(wg, source, target)
                 && max == POSITIVE_INFINITY) {
        return POSITIVE_INFINITY;
      }
      LIBSEMIGROUPS_EXCEPTION("number of paths cannot be trivially determined");
    }

    template <typename Node1, typename Node2>
    uint64_t count_matrix(WordGraph<Node1> const& wg,
                          Node2                   source,
                          size_t                  min,
                          size_t                  max) {
      auto am = word_graph::adjacency_matrix(wg);
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
      for (size_t i = min; i <= max; ++i) {
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
    bool count_special(WordGraph<Node1> const& wg,
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
                                    && word_graph::is_reachable(
                                        wg, n, static_cast<Node1>(source));
                           })) {
          return true;
        } else if (source != target
                   && word_graph::is_reachable(wg, source, target)
                   && word_graph::is_reachable(wg, target, source)) {
          return true;
        }
      }
      return false;
    }

    template <typename Node1, typename Node2>
    uint64_t count_matrix(WordGraph<Node1> const& wg,
                          Node2                   source,
                          Node2                   target,
                          size_t                  min,
                          size_t                  max) {
      if (!word_graph::is_reachable(wg, source, target)) {
        // Complexity is O(number of nodes + number of edges).
        return 0;
      } else if (count_special(wg, source, target, min, max)) {
        return POSITIVE_INFINITY;
      }
      auto am = word_graph::adjacency_matrix(wg);
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
      for (size_t i = min; i <= max; ++i) {
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
      for (size_t i = min; i <= max; ++i) {
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
    uint64_t count_acyclic(WordGraph<Node1> const& wg,
                           Node2                   source,
                           size_t                  min,
                           size_t                  max) {
      auto topo = word_graph::topological_sort(wg, source);
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
      // TODO(v4) remove prefix ::libsemigroups::detail::
      auto number_paths = ::libsemigroups::detail::DynamicArray2<uint64_t>(
          std::min(max + 1, topo.size()),
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
            for (size_t i = 1; i <= std::min(max, m); ++i) {
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
                                 + std::min(topo.size(), max + 1),
                             static_cast<uint64_t>(0));
    }

    template <typename Node1, typename Node2>
    uint64_t count_acyclic(WordGraph<Node1> const& wg,
                           Node2                   source,
                           Node2                   target,
                           size_t                  min,
                           size_t                  max) {
      auto topo = word_graph::topological_sort(wg, source);
      if (topo.empty()) {
        // Can't topologically sort, so the digraph contains cycles.
        LIBSEMIGROUPS_EXCEPTION("the subdigraph induced by the nodes reachable "
                                "from {} is not acyclic",
                                source);
      } else if ((max == 0 && source != target)
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
      // TODO(v4) remove prefix ::libsemigroups::detail::
      auto number_paths = ::libsemigroups::detail::DynamicArray2<uint64_t>(
          std::min(max + 1, topo.size()),
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
            for (size_t i = 1; i <= std::min(max, m); ++i) {
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
                                 + std::min(topo.size(), max + 1),
                             static_cast<uint64_t>(0));
    }

  }  // namespace detail

  template <typename Node1, typename Node2>
  uint64_t count(WordGraph<Node1> const& wg, Node2 source) {
    // Don't allow selecting the algorithm because we check
    // acyclicity anyway.
    // TODO(2): could use algorithm::dfs in some cases.
    // TODO(v4) remove prefix ::libsemigroups::
    ::libsemigroups::word_graph::throw_if_node_out_of_bounds(
        wg, static_cast<Node1>(source));
    auto topo = word_graph::topological_sort(wg, source);
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
  paths::algorithm count_algorithm(WordGraph<Node1> const& wg,
                                   Node2                   source,
                                   size_t                  min,
                                   size_t                  max) {
    if (min > max || word_graph::is_complete(wg)) {
      return paths::algorithm::trivial;
    }

    auto topo = word_graph::topological_sort(wg, source);
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
  uint64_t count(WordGraph<Node1> const& wg,
                 Node2                   source,
                 size_t                  min,
                 size_t                  max,
                 paths::algorithm        lgrthm) {
    ::libsemigroups::word_graph::throw_if_node_out_of_bounds(
        wg, static_cast<Node1>(source));

    switch (lgrthm) {
      case paths::algorithm::dfs:
        return std::distance(cbegin_pilo(wg, source, min, max), cend_pilo(wg));
      case paths::algorithm::matrix:
        return detail::count_matrix(wg, source, min, max);
      case paths::algorithm::acyclic:
        return detail::count_acyclic(wg, source, min, max);
      case paths::algorithm::trivial:
        return detail::count_trivial(wg, source, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return v4::paths::count(
            wg,
            source,
            min,
            max,
            v4::paths::count_algorithm(wg, source, min, max));
    }
  }

  template <typename Node1, typename Node2>
  paths::algorithm count_algorithm(WordGraph<Node1> const& wg,
                                   Node2                   source,
                                   Node2                   target,
                                   size_t                  min,
                                   size_t                  max) {
    bool acyclic = word_graph::is_acyclic(wg, source, target);
    if (min > max || !word_graph::is_reachable(wg, source, target)
        || (!acyclic && max == POSITIVE_INFINITY)) {
      return paths::algorithm::trivial;
    } else if (acyclic && word_graph::is_acyclic(wg, source)) {
      return paths::algorithm::acyclic;
    } else if (wg.number_of_edges() < detail::magic_number(wg.number_of_nodes())
                                          * wg.number_of_nodes()) {
      return paths::algorithm::dfs;
    } else {
      return paths::algorithm::matrix;
    }
  }

  template <typename Node1, typename Node2>
  uint64_t count(WordGraph<Node1> const& wg,
                 Node2                   source,
                 Node2                   target,
                 size_t                  min,
                 size_t                  max,
                 paths::algorithm        lgrthm) {
    ::libsemigroups::word_graph::throw_if_node_out_of_bounds(
        wg, static_cast<Node1>(source));
    ::libsemigroups::word_graph::throw_if_node_out_of_bounds(
        wg, static_cast<Node1>(target));

    switch (lgrthm) {
      case paths::algorithm::dfs:
        if (detail::count_special(wg, source, target, min, max)) {
          return POSITIVE_INFINITY;
        }
        return std::distance(cbegin_pstilo(wg, source, target, min, max),
                             cend_pstilo(wg));
      case paths::algorithm::matrix:
        return detail::count_matrix(wg, source, target, min, max);
      case paths::algorithm::acyclic:
        return detail::count_acyclic(wg, source, target, min, max);
      case paths::algorithm::trivial:
        return detail::count_trivial(wg, source, target, min, max);
      case paths::algorithm::automatic:
        // intentional fall through
      default:
        return v4::paths::count(
            wg,
            source,
            target,
            min,
            max,
            v4::paths::count_algorithm(wg, source, target, min, max));
    }
  }

}  // namespace libsemigroups::v4::paths
