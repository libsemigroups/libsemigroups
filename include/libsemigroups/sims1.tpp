//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for semigroups and monoid.

namespace libsemigroups {

#ifdef LIBSEMIGROUPS_ENABLE_STATS
  template <typename T>
  void Sims1<T>::const_iterator::stats_update(size_type current_num_edges) {
    if (_felsch_graph.number_of_edges() > current_num_edges) {
      if (_stats.depth > 0) {
        _stats.depth--;
      }
    } else {
      _stats.depth++;
    }
    _stats.max_depth = std::max(_stats.max_depth, _stats.depth);
    _stats.max_pending
        = std::max(_stats.max_pending, static_cast<uint64_t>(_pending.size()));
    _stats.num_nodes++;
    if (_stats.depth > 0) {
      _stats.mean_depth
          += (_stats.depth - _stats.mean_depth) / _stats.num_nodes;
    }
  }

  template <typename T>
  sims1::Stats const &Sims1<T>::const_iterator::stats() const noexcept {
    return _stats;
  }

  std::ostream &operator<<(std::ostream &               os,
                           typename sims1::Stats const &stats) {
    detail::PrintTable pt;
    pt.header("Summary of statistics (Sims low-index algorithm)");
    pt("mean depth ", "%'14lf", stats.mean_depth);
    pt("max depth ", "%'14llu", stats.max_depth);
    pt("max pending ", "%'14llu", stats.max_pending);
    pt("number of nodes visited ", "%'14llu", stats.num_nodes);
    pt("approx. number of good nodes ", "%'14llu*", stats.num_good_nodes);
    pt("ratio of good / bad nodes ",
       "%'14lf",
       static_cast<double>(stats.num_good_nodes) / stats.num_nodes);
    pt("");
    pt("*the number of complete digraphs found times their number of nodes, "
       "this value overcounts the number of good nodes!");
    pt.footer("End of summary (Sims low-index algorithm)");
    os << pt.emit();
    return os;
  }

#endif

  template <typename T>
  Sims1<T>::~Sims1() = default;

  template <typename T>
  Sims1<T>::Sims1(congruence_kind ck, Presentation<word_type> const &p)
      : Sims1(ck, p, Presentation<word_type>()) {}

  template <typename T>
  Sims1<T>::Sims1(congruence_kind                ck,
                  Presentation<word_type> const &p,
                  Presentation<word_type> const &e)
      : _extra(), _final(), _presentation() {
    if (ck == congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected congruence_kind::right or congruence_kind::left");
    } else if (p.alphabet() != e.alphabet() && !e.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd and 3rd arguments (presentations) are not defined over "
          "the same alphabets, expected alphabet %s for 3rd argument got %s",
          detail::to_string(p.alphabet()).c_str(),
          detail::to_string(e.alphabet()).c_str());
    }
    p.validate();  // TODO Test for this

    // We call make in the next two lines to ensure that the generators of the
    // presentation are {0, ..., n - 1} where n is the size of the alphabet.
    _presentation = make<Presentation<word_type>>(p);
    _extra        = make<Presentation<word_type>>(e);
    if (ck == congruence_kind::left) {
      presentation::reverse(_presentation);
      presentation::reverse(_extra);
    }
  }

  template <typename T>
  uint64_t Sims1<T>::number_of_congruences(size_type n) const {
    uint64_t                                       result = 0;
    std::chrono::high_resolution_clock::time_point last_report
        = std::chrono::high_resolution_clock::now();
    auto const last = cend(n);
    auto       it   = cbegin(n);
    for (; it != last; ++it) {
      ++result;
      auto now     = std::chrono::high_resolution_clock::now();
      auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(
          now - last_report);
      if (elapsed > std::chrono::duration_cast<std::chrono::nanoseconds>(
              std::chrono::seconds(1))) {
        std::swap(now, last_report);
        REPORT_DEFAULT("found %llu congruences so far!\n", uint64_t(result));
      }
    }
    return result;
  }

  template <typename T>
  Sims1<T>::const_iterator::const_iterator(
      Presentation<word_type> const &p,
      Presentation<word_type> const &extra,
      Presentation<word_type> const &final_,
      size_type                      n)

      : _extra(extra),
        _felsch_graph(p, n),
        _final(final_),
        _max_num_classes(p.contains_empty_word() ? n : n + 1),
        _min_target_node(p.contains_empty_word() ? 0 : 1),
        _num_active_nodes(n == 0 ? 0 : 1),  // = 0 indicates iterator is done
        // TODO(later) could sink _num_active_nodes into DigraphWithSources or
        // FelschDigraph
        _num_gens(p.alphabet().size()),
        _pending() {
    if (_num_active_nodes == 0) {
      return;
    }
    if (n > 1) {
      _pending.emplace_back(0, 0, 1, 0, 2);
    }
    if (_min_target_node == 0) {
      _pending.emplace_back(0, 0, 0, 0, 1);
    }
    ++(*this);
    // The increment above is required so that when dereferencing any
    // instance of this type we obtain a valid word graph (o/w the value
    // pointed to here is empty).
  }

  template <typename T>
  typename Sims1<T>::const_iterator const &
  Sims1<T>::const_iterator::operator++() {
    while (true) {
    dive:
      if (_pending.empty()) {
        break;
      }
      auto const current = std::move(_pending.back());
#if LIBSEMIGROUPS_ENABLE_STATS
      stats_update(current.num_edges);
#endif
      _pending.pop_back();
      LIBSEMIGROUPS_ASSERT(current.target < current.num_nodes);
      LIBSEMIGROUPS_ASSERT(current.num_nodes <= _max_num_classes);

      // Backtrack if necessary
      _felsch_graph.reduce_number_of_edges_to(current.num_edges);

      // It might be that current.target is a new node, in which case
      // _num_active_nodes includes this new node even before the edge
      // current.source -> current.target is defined.
      _num_active_nodes = current.num_nodes;

      LIBSEMIGROUPS_ASSERT(
          _felsch_graph.unsafe_neighbor(current.source, current.generator)
          == UNDEFINED);
      {
        size_type start = _felsch_graph.number_of_edges();

        _felsch_graph.def_edge(
            current.source, current.generator, current.target);

        for (auto it = _extra.rules.cbegin(); it != _extra.rules.cend();
             it += 2) {
          if (!_felsch_graph.compatible(0, *it, *(it + 1))) {
            goto dive;
          }
        }

        if (_felsch_graph.process_definitions(start)) {
          letter_type a = current.generator + 1;
          for (node_type next = current.source; next < _num_active_nodes;
               ++next) {
            for (; a < _num_gens; ++a) {
              if (_felsch_graph.unsafe_neighbor(next, a) == UNDEFINED) {
                for (node_type b = _min_target_node; b < _num_active_nodes;
                     ++b) {
                  _pending.emplace_back(next,
                                        a,
                                        b,
                                        _felsch_graph.number_of_edges(),
                                        _num_active_nodes);
                }
                if (_num_active_nodes < _max_num_classes) {
                  _pending.emplace_back(next,
                                        a,
                                        _num_active_nodes,
                                        _felsch_graph.number_of_edges(),
                                        _num_active_nodes + 1);
                }
                goto dive;
              }
            }
            a = 0;
          }
          // No undefined edges, word graph is complete
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.num_good_nodes += _num_active_nodes;
#endif
          for (auto it = _final.rules.cbegin(); it != _final.rules.cend();
               it += 2) {
            for (node_type n = 0; n < _num_active_nodes; ++n) {
              if (!_felsch_graph.compatible(n, *it, *(it + 1))) {
                goto dive;
              }
            }
          }
          return *this;
        }
      }
    }
    _num_active_nodes = 0;  // indicates that the iterator is done
    _felsch_graph.restrict(0);
    return *this;
  }
}  // namespace libsemigroups
