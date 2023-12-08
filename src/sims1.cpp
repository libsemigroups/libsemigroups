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

#include "libsemigroups/sims1.hpp"

#include <algorithm>      // for max, find_if, fill
#include <chrono>         // for duration, durat...
#include <functional>     // for ref
#include <memory>         // for unique_ptr, mak...
#include <ratio>          // for ratio
#include <string>         // for string
#include <string>         // for operator+, basi...
#include <thread>         // for thread, yield
#include <unordered_map>  // for operator!=, ope...
#include <utility>        // for swap

#include "libsemigroups/constants.hpp"     // for operator!=, ope...
#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_A...
#include "libsemigroups/exception.hpp"     // for LIBSEMIGROUPS_E...
#include "libsemigroups/felsch-graph.hpp"  // for FelschGraph
#include "libsemigroups/froidure-pin.hpp"  // for FroidurePin
#include "libsemigroups/presentation.hpp"  // for Presentation
#include "libsemigroups/sims1.hpp"
#include "libsemigroups/to-froidure-pin.hpp"  // for to_froidure_pin
#include "libsemigroups/transf.hpp"           // for Transf, validate
#include "libsemigroups/types.hpp"            // for congruence_kind
#include "libsemigroups/word-graph.hpp"       // for follow_path_no_...

#include "libsemigroups/detail/fmt.hpp"     // for buffer::append
#include "libsemigroups/detail/report.hpp"  // for report_default
#include "libsemigroups/detail/stl.hpp"     // for JoinThreads
#include "libsemigroups/detail/string.hpp"  // for group_digits

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Sims1Stats
  ////////////////////////////////////////////////////////////////////////

  Sims1Stats::Sims1Stats()
      : count_last(),
        count_now(),
        max_pending(),
        total_pending_last(),
        total_pending_now() {
    stats_zero();
  }

  Sims1Stats& Sims1Stats::stats_zero() {
    count_last         = 0;
    count_now          = 0;
    max_pending        = 0;
    total_pending_last = 0;
    total_pending_now  = 0;
    return *this;
  }

  Sims1Stats& Sims1Stats::init_from(Sims1Stats const& that) {
    count_last         = that.count_last;
    count_now          = that.count_now.load();
    max_pending        = that.max_pending;
    total_pending_last = that.total_pending_last;
    total_pending_now  = that.total_pending_now.load();
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // PendingDef
  ////////////////////////////////////////////////////////////////////////

  struct Sims1::PendingDef {
    PendingDef() = default;

    PendingDef(node_type   s,
               letter_type g,
               node_type   t,
               size_type   e,
               size_type   n) noexcept
        : source(s), generator(g), target(t), num_edges(e), num_nodes(n) {}
    node_type   source;
    letter_type generator;
    node_type   target;
    size_type   num_edges;  // Number of edges in the graph when
                            // *this was added to the stack
    size_type num_nodes;    // Number of nodes in the graph
                            // after the definition is made
  };

  ///////////////////////////////////////////////////////////////////////////////
  // iterator_base nested class
  ///////////////////////////////////////////////////////////////////////////////

  Sims1::iterator_base::iterator_base(Sims1 const* s,
                                      size_type    n)
      :  // private
        _max_num_classes(s->presentation().contains_empty_word() ? n : n + 1),
        _min_target_node(s->presentation().contains_empty_word() ? 0 : 1),
        // protected
        _felsch_graph(),
        _mtx(),
        _pending(),
        _sims1(s) {
    Presentation<word_type> p = s->presentation();
    size_t                  m = std::distance(s->presentation().rules.cbegin(),
                             s->cbegin_long_rules());
    p.rules.erase(p.rules.begin() + m, p.rules.end());
    _felsch_graph.init(std::move(p));
    // n == 0 only when the iterator is cend
    _felsch_graph.number_of_active_nodes(n == 0 ? 0 : 1);
    // = 0 indicates iterator is done
    _felsch_graph.add_nodes(n);
  }

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.
  Sims1::iterator_base::iterator_base(Sims1::iterator_base const& that)
      :  // private
        _max_num_classes(that._max_num_classes),
        _min_target_node(that._min_target_node),
        // protected
        _felsch_graph(that._felsch_graph),
        _mtx(),
        _pending(that._pending),
        _sims1(that._sims1) {}

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.

  Sims1::iterator_base::iterator_base(Sims1::iterator_base&& that)
      :  // private
        _max_num_classes(std::move(that._max_num_classes)),
        _min_target_node(std::move(that._min_target_node)),
        // protected
        _felsch_graph(std::move(that._felsch_graph)),
        _mtx(),
        _pending(std::move(that._pending)),
        _sims1(that._sims1) {}

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.

  typename Sims1::iterator_base&
  Sims1::iterator_base::operator=(Sims1::iterator_base const& that) {
    // private
    _max_num_classes = that._max_num_classes;
    _min_target_node = that._min_target_node;
    // protected
    _felsch_graph = that._felsch_graph;
    // keep our own _mtx
    _pending = that._pending;
    _sims1   = that._sims1;
    return *this;
  }

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.

  typename Sims1::iterator_base&
  Sims1::iterator_base::operator=(Sims1::iterator_base&& that) {
    // private
    _max_num_classes = std::move(that._max_num_classes);
    _min_target_node = std::move(that._min_target_node);

    // protected
    _felsch_graph = std::move(that._felsch_graph);
    _pending      = std::move(that._pending);
    // keep our own _mtx
    _sims1 = that._sims1;
    return *this;
  }

  void Sims1::iterator_base::swap(Sims1::iterator_base& that) noexcept {
    // private
    std::swap(_max_num_classes, that._max_num_classes);
    std::swap(_min_target_node, that._min_target_node);
    // protected
    std::swap(_felsch_graph, that._felsch_graph);
    std::swap(_pending, that._pending);
    std::swap(_sims1, that._sims1);
  }

  Sims1::iterator_base::~iterator_base() = default;

  // The following function is separated from the constructor so that it isn't
  // called in the constructor of every thread_iterator

  void Sims1::iterator_base::init(size_type n) {
    if (n != 0) {
      if (n > 1 || _min_target_node == 1) {
        _pending.emplace_back(0, 0, 1, 0, 2);
      }
      if (_min_target_node == 0) {
        _pending.emplace_back(0, 0, 0, 0, 1);
      }
    }
  }

  bool Sims1::iterator_base::try_pop(PendingDef& pd) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_pending.empty()) {
      return false;
    }
    pd = std::move(_pending.back());
    _pending.pop_back();
    return true;
  }

  bool Sims1::iterator_base::try_define(PendingDef const& current) {
    LIBSEMIGROUPS_ASSERT(current.target < current.num_nodes);
    LIBSEMIGROUPS_ASSERT(current.num_nodes <= _max_num_classes);
    {
      std::lock_guard<std::mutex> lock(_mtx);
      // Backtrack if necessary
      _felsch_graph.reduce_number_of_edges_to(current.num_edges);

      // It might be that current.target is a new node, in which case
      // _felsch_graph.number_of_active_nodes() includes this new node even
      // before the edge current.source -> current.target is defined.
      _felsch_graph.number_of_active_nodes(current.num_nodes);

      LIBSEMIGROUPS_ASSERT(
          _felsch_graph.target_no_checks(current.source, current.generator)
          == UNDEFINED);

      // Don't call number_of_edges because this calls the function in
      // WordGraph
      size_type const start = _felsch_graph.definitions().size();

      _felsch_graph.set_target_no_checks(
          current.source, current.generator, current.target);

      auto first = _sims1->include().cbegin();
      auto last  = _sims1->include().cend();
      if (!felsch_graph::make_compatible<RegisterDefs>(
              _felsch_graph, 0, 1, first, last)
          || !_felsch_graph.process_definitions(start)) {
        // Seems to be important to check include() first then
        // process_definitions
        return false;
      }

      first          = _sims1->exclude().cbegin();
      last           = _sims1->exclude().cend();
      node_type root = 0;

      for (auto it = first; it != last; it += 2) {
        auto l = word_graph::follow_path_no_checks(_felsch_graph, root, *it);
        if (l != UNDEFINED) {
          auto r = word_graph::follow_path_no_checks(
              _felsch_graph, root, *(it + 1));
          if (l == r) {
            return false;
          }
        }
      }
    }

    letter_type     a        = current.generator + 1;
    size_type const M        = _felsch_graph.number_of_active_nodes();
    size_type const N        = _felsch_graph.number_of_edges();
    size_type const num_gens = _felsch_graph.out_degree();
    auto&           stats    = _sims1->stats();

    for (node_type next = current.source; next < M; ++next) {
      for (; a < num_gens; ++a) {
        if (_felsch_graph.target_no_checks(next, a) == UNDEFINED) {
          {
            std::lock_guard<std::mutex> lock(_mtx);
            if (M < _max_num_classes) {
              _pending.emplace_back(next, a, M, N, M + 1);
            }
            for (node_type b = M; b-- > _min_target_node;) {
              _pending.emplace_back(next, a, b, N, M);
            }
          }
          stats.total_pending_now
              += M - _min_target_node + (M < _max_num_classes);
          stats.max_pending = std::max(static_cast<uint64_t>(_pending.size()),
                                       stats.max_pending);
          return false;
        }
      }
      a = 0;
    }
    // No undefined edges, word graph is complete
    LIBSEMIGROUPS_ASSERT(N == M * num_gens);

    auto first = _sims1->cbegin_long_rules();
    auto last  = _sims1->presentation().rules.cend();

    bool result = word_graph::is_compatible(_felsch_graph,
                                            _felsch_graph.cbegin_nodes(),
                                            _felsch_graph.cbegin_nodes() + M,
                                            first,
                                            last);
    if (result) {
      // stats.count_now is atomic so this is ok
      ++stats.count_now;
    }
    return result;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // iterator nested class
  ///////////////////////////////////////////////////////////////////////////////

  Sims1::iterator::iterator(Sims1 const* s, size_type n) : iterator_base(s, n) {
    if (this->_felsch_graph.number_of_active_nodes() == 0) {
      return;
    }
    init(n);
    ++(*this);
    // The increment above is required so that when dereferencing any
    // instance of this type we obtain a valid word graph (o/w the value
    // pointed to here is empty).
  }

  typename Sims1::iterator const& Sims1::iterator::operator++() {
    PendingDef current;
    while (try_pop(current)) {
      if (try_define(current)) {
        return *this;
      }
    }
    this->_felsch_graph.number_of_active_nodes(0);
    // indicates that the iterator is done
    this->_felsch_graph.induced_subgraph_no_checks(0, 0);
    return *this;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // thread_iterator
  ///////////////////////////////////////////////////////////////////////////////

  // Note that this class is private, and not really an iterator in the usual
  // sense. It is designed solely to work with thread_runner.

  class Sims1::thread_iterator : public iterator_base {
    friend class Sims1::thread_runner;

    using iterator_base::copy_felsch_graph;

   public:
    //! No doc
    thread_iterator(Sims1 const* s, size_type n) : iterator_base(s, n) {}

    // None of the constructors are noexcept because the corresponding
    // constructors for std::vector aren't (until C++17).
    //! No doc
    thread_iterator() = delete;
    //! No doc
    thread_iterator(thread_iterator const&) = delete;
    //! No doc
    thread_iterator(thread_iterator&&) = delete;
    //! No doc
    thread_iterator& operator=(thread_iterator const&) = delete;
    //! No doc
    thread_iterator& operator=(thread_iterator&&) = delete;

    //! No doc
    ~thread_iterator() = default;

    using iterator_base::stats;

   public:
    void push(PendingDef pd) {
      this->_pending.push_back(std::move(pd));
    }

    void steal_from(thread_iterator& that) {
      // WARNING <that> must be locked before calling this function
      std::lock_guard<std::mutex> lock(this->_mtx);
      LIBSEMIGROUPS_ASSERT(this->_pending.empty());
      size_t const n = that._pending.size();
      if (n == 1) {
        return;
      }
      // Copy the FelschGraph from that into *this
      copy_felsch_graph(that);

      // Unzip that._pending into _pending and that._pending, this seems to
      // give better performance in the search than splitting that._pending
      // into [begin, begin + size / 2) and [begin + size / 2, end)
      size_t i = 0;
      for (; i < n - 2; i += 2) {
        this->_pending.push_back(std::move(that._pending[i]));
        that._pending[i / 2] = std::move(that._pending[i + 1]);
      }
      this->_pending.push_back(std::move(that._pending[i]));
      if (i == n - 2) {
        that._pending[i / 2] = std::move(that._pending[i + 1]);
      }

      that._pending.erase(that._pending.cbegin() + that._pending.size() / 2,
                          that._pending.cend());
    }

    bool try_steal(thread_iterator& q) {
      std::lock_guard<std::mutex> lock(this->_mtx);
      if (this->_pending.empty()) {
        return false;
      }
      // Copy the FelschGraph and half pending from *this into q
      q.steal_from(*this);  // Must call steal_from on q, so that q is locked
      return true;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // thread_runner
  ////////////////////////////////////////////////////////////////////////

  class Sims1::thread_runner {
   private:
    std::atomic_bool                              _done;
    std::vector<std::unique_ptr<thread_iterator>> _theives;
    std::vector<std::thread>                      _threads;
    std::mutex                                    _mtx;
    size_type                                     _num_threads;
    word_graph_type                               _result;
    Sims1 const*                                  _sims1;

    void worker_thread(unsigned                                    my_index,
                       std::function<bool(word_graph_type const&)> hook) {
      PendingDef pd;
      auto const restarts = _sims1->idle_thread_restarts();
      for (size_t i = 0; i < restarts; ++i) {
        while ((pop_from_local_queue(pd, my_index)
                || pop_from_other_thread_queue(pd, my_index))
               && !_done) {
          if (_theives[my_index]->try_define(pd)) {
            if (hook(**_theives[my_index])) {
              // hook returns true to indicate that we should stop early
              std::lock_guard<std::mutex> lock(_mtx);
              if (!_done) {
                _done   = true;
                _result = **_theives[my_index];
              }
              return;
            }
          }
        }
        std::this_thread::yield();
        // It's possible to reach here before all of the work is done,
        // because by coincidence there's nothing in the local queue and
        // nothing in any other queue either, this sometimes leads to
        // threads shutting down earlier than desirable. On the other hand,
        // maybe this is a desirable.
      }
    }

    bool pop_from_local_queue(PendingDef& pd, unsigned my_index) {
      return _theives[my_index]->try_pop(pd);
    }

    bool pop_from_other_thread_queue(PendingDef& pd, unsigned my_index) {
      for (size_t i = 0; i < _theives.size() - 1; ++i) {
        unsigned const index = (my_index + i + 1) % _theives.size();
        // Could always do something different here, like find
        // the largest queue and steal from that? I tried this and it didn't
        // seem to be faster.
        if (_theives[index]->try_steal(*_theives[my_index])) {
          return pop_from_local_queue(pd, my_index);
        }
      }
      return false;
    }

   public:
    thread_runner(Sims1 const* s, size_type n, size_type num_threads)
        : _done(false),
          _theives(),
          _threads(),
          _mtx(),
          _num_threads(num_threads),
          _result(),
          _sims1(s) {
      for (size_t i = 0; i < _num_threads; ++i) {
        _theives.push_back(std::make_unique<thread_iterator>(s, n));
      }
      _theives.front()->init(n);
    }

    ~thread_runner() = default;

    word_graph_type const& word_graph() const {
      return _result;
    }

    void run(std::function<bool(word_graph_type const&)> hook) {
      try {
        detail::JoinThreads joiner(_threads);
        for (size_t i = 0; i < _num_threads; ++i) {
          _threads.push_back(std::thread(
              &thread_runner::worker_thread, this, i, std::ref(hook)));
        }
      } catch (...) {
        _done = true;
        throw;
      }
    }
  };  // class thread_runner

  ////////////////////////////////////////////////////////////////////////
  // Sims1
  ////////////////////////////////////////////////////////////////////////

  Sims1& Sims1::kind(congruence_kind ck) {
    if (ck == congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION("expected congruence_kind::right or "
                              "congruence_kind::left, found {}",
                              ck);
    } else if (ck != kind()) {
      presentation::reverse(_presentation);
      reverse(_include);
      reverse(_exclude);
      _kind = ck;
    }
    return *this;
  }

  uint64_t Sims1::number_of_congruences(size_type n) const {
    if (number_of_threads() == 1) {
      uint64_t result = 0;
      for_each(n, [&result](word_graph_type const&) { ++result; });
      return result;
    } else {
      std::atomic_int64_t result(0);
      for_each(n, [&result](word_graph_type const&) { ++result; });
      return result;
    }
  }

  // Apply the function pred to every one-sided congruence with at
  // most n classes
  void Sims1::for_each(size_type                                   n,
                       std::function<void(word_graph_type const&)> pred) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st argument (size_type) to be non-zero");
    } else if (presentation().rules.empty()
               && presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation() must be defined before "
                              "calling this function");
    }
    report_at_start(n);
    if (number_of_threads() == 1) {
      if (!reporting_enabled()) {
        // Don't care about stats in this case
        std::for_each(cbegin(n), cend(n), pred);
      } else {
        stats().stats_zero();
        detail::Ticker t([this]() { report_progress_from_thread(); });
        auto           it   = cbegin(n);
        auto const     last = cend(n);
        for (; it != last; ++it) {
          pred(*it);
        }
        report_final();
      }
    } else {
      thread_runner den(this, n, number_of_threads());
      auto          pred_wrapper = [&pred](word_graph_type const& ad) {
        pred(ad);
        return false;
      };
      if (!reporting_enabled()) {
        den.run(pred_wrapper);
      } else {
        stats().stats_zero();
        detail::Ticker t([this]() { report_progress_from_thread(); });
        den.run(pred_wrapper);
        report_final();
      }
    }
  }

  typename Sims1::word_graph_type
  Sims1::find_if(size_type                                   n,
                 std::function<bool(word_graph_type const&)> pred) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st argument (size_type) to be non-zero");
    } else if (presentation().rules.empty()
               && presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation() must be defined before "
                              "calling this function");
    }
    report_at_start(n);
    if (number_of_threads() == 1) {
      if (!reporting_enabled()) {
        return *std::find_if(cbegin(n), cend(n), pred);
      } else {
        stats().stats_zero();
        detail::Ticker t([this]() { report_progress_from_thread(); });

        auto       it   = cbegin(n);
        auto const last = cend(n);

        for (; it != last; ++it) {
          if (pred(*it)) {
            report_final();
            return *it;
          }
        }
        report_final();
        return *last;  // the empty digraph
      }
    } else {
      thread_runner den(this, n, number_of_threads());
      if (!reporting_enabled()) {
        den.run(pred);
        return den.word_graph();
      } else {
        stats().stats_zero();
        detail::Ticker t([this]() { report_progress_from_thread(); });
        den.run(pred);
        report_final();
        return den.word_graph();
      }
    }
  }

  Sims1::iterator Sims1::cbegin(size_type n) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_type) must be non-zero");
    } else if (presentation().rules.empty()
               && presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation() must be defined before "
                              "calling this function");
    }
    return iterator(this, n);
  }

  Sims1::iterator Sims1::cend(size_type n) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_type) must be non-zero");
    } else if (presentation().rules.empty()
               && presentation().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the presentation() must be defined before "
                              "calling this function");
    }
    return iterator(this, 0);
  }

  void Sims1::report_at_start(size_t num_classes) const {
    std::string num_threads = "0";
    if (number_of_threads() != 1) {
      num_threads = fmt::format(
          "{} / {}", number_of_threads(), std::thread::hardware_concurrency());
    }
    auto shortest_short = presentation::shortest_rule_length(presentation());
    auto longest_short  = presentation::longest_rule_length(presentation());

    std::string pairs;
    if (!include().empty() && !exclude().empty()) {
      pairs = fmt::format(", including {} + excluding {} pairs",
                          include().size() / 2,
                          exclude().size() / 2);
    } else if (!include().empty()) {
      pairs = fmt::format(", including {} pairs", include().size() / 2);
    } else if (!exclude().empty()) {
      pairs = fmt::format(", excluding {} pairs", exclude().size() / 2);
    }

    report_no_prefix("{:+<80}\n", "");
    report_default("Sims1: STARTING with {} additional threads . . . \n",
                   num_threads);
    report_no_prefix("{:+<80}\n", "");
    report_default("Sims1: \u2264 {} classes{} for \u27E8A|R\u27E9 with:\n",
                   num_classes,
                   pairs);
    report_default("Sims1: |A| = {}, |R| = {}, "
                   "|u| + |v| \u2208 [{}, {}], \u2211(|u| + |v|) = {}\n",
                   presentation().alphabet().size(),
                   presentation().rules.size() / 2,
                   shortest_short,
                   longest_short,
                   presentation::length(presentation()));

    if (cbegin_long_rules() != presentation().rules.cend()) {
      auto first = presentation().rules.cbegin(), last = cbegin_long_rules();

      report_default("Sims1: {} \"short\" relations with: ",
                     std::distance(first, last) / 2);
      report_no_prefix("|u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
                       presentation::shortest_rule_length(first, last),
                       presentation::longest_rule_length(first, last),
                       presentation::length(first, last));

      first = cbegin_long_rules(), last = presentation().rules.cend();
      report_default("Sims1: {} \"long\" relations with: ",
                     std::distance(first, last) / 2);
      report_no_prefix("|u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
                       presentation::shortest_rule_length(first, last),
                       presentation::longest_rule_length(first, last),
                       presentation::length(first, last));
    }
  }

  void Sims1::report_progress_from_thread() const {
    using namespace detail;       // NOLINT(build/namespaces)
    using namespace std::chrono;  // NOLINT(build/namespaces)

    // Reporter
    auto time_total_ns = delta(start_time());
    auto time_diff     = delta(last_report());

    // Stats
    auto count_now          = stats().count_now.load();
    auto count_diff         = count_now - stats().count_last;
    auto total_pending_now  = stats().total_pending_now.load();
    auto total_pending_diff = total_pending_now - stats().total_pending_last;

    constexpr uint64_t billion = 1'000'000'000;
    uint64_t congs_per_sec     = (billion * count_now) / time_total_ns.count();
    uint64_t nodes_per_sec
        = (billion * total_pending_now) / time_total_ns.count();

    nanoseconds time_per_cong_last_sec(0);
    if (count_diff != 0) {
      time_per_cong_last_sec = time_diff / count_diff;
    }

    nanoseconds time_per_node_last_sec(0);
    if (total_pending_diff != 0) {
      time_per_node_last_sec = time_diff / total_pending_diff;
    }

    nanoseconds time_per_cong(0);
    if (count_now != 0) {
      time_per_cong = time_total_ns / count_now;
    }

    nanoseconds time_per_node(0);
    if (total_pending_now != 0) {
      time_per_node = time_total_ns / total_pending_now;
    }

    ReportCell<2> rc;
    rc.min_width(0, 7).min_width(1, 11);
    rc(group_digits,
       "Sims1: total        {} (cong.)   | {} (nodes) \n",
       count_now,
       total_pending_now);

    rc(signed_group_digits,
       "Sims1: diff         {} (cong.)   | {} (nodes)\n",
       count_diff,
       total_pending_diff);

    rc(group_digits,
       "Sims1: mean         {} (cong./s) | {} (node/s)\n",
       congs_per_sec,
       nodes_per_sec);

    rc("Sims1: time last s. {} (/cong.)  | {} (/node)\n",
       string_time(time_per_cong_last_sec),
       string_time(time_per_node_last_sec));

    rc("Sims1: mean time    {} (/cong.)  | {} (/node)\n",
       string_time(time_per_cong),
       string_time(time_per_node));

    rc("Sims1: time         {} (total)   |\n", string_time(time_total_ns));

    reset_last_report();
    stats().stats_check_point();
  }

  void Sims1::report_final() const {
    report_progress_from_thread();
    report_no_prefix("{:+<80}\n", "");
    report_default("Sims1: FINISHED!\n");
    report_no_prefix("{:+<80}\n", "");
  }

  ////////////////////////////////////////////////////////////////////////
  // RepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  RepOrc& RepOrc::init() {
    _min  = 0;
    _max  = 0;
    _size = 0;
    return *this;
  }

  Sims1::word_graph_type RepOrc::word_graph() const {
    using word_graph_type = typename Sims1::word_graph_type;
    using node_type       = typename word_graph_type::node_type;

    report_no_prefix("{:+<80}\n", "");
    report_default(
        "RepOrc: Searching for a faithful rep. o.r.c. on [{}, {}) points\n",
        _min,
        _max + 1);
    if (_min > _max || _max == 0) {
      report_no_prefix("{:+<80}\n", "");
      report_default(
          "RepOrc: No faithful rep. o.r.c. exists in [{}, {}) = \u2205\n",
          _min,
          _max + 1);
      return word_graph_type(0, 0);
    }

    SuppressReportFor suppressor("FroidurePin");

    std::atomic_uint64_t count(0);

    auto hook = [&](word_graph_type const& x) {
      ++count;
      if (x.number_of_active_nodes() >= _min) {
        auto first = (presentation().contains_empty_word() ? 0 : 1);
        auto S     = to_froidure_pin<Transf<0, node_type>>(
            x, first, x.number_of_active_nodes());
        // It'd be nice to reuse S here, but this is tricky because hook
        // maybe called in multiple threads, and so we can't easily do this.
        if (presentation().contains_empty_word()) {
          auto one = S.generator(0).identity();
          if (!S.contains(one)) {
            S.add_generator(one);
          }
        }
        LIBSEMIGROUPS_ASSERT(S.size() <= _size);
        if (S.size() == _size) {
          return true;
        }
      }
      return false;
    };

    auto result = Sims1(congruence_kind::right)
                      .settings_copy_from(*this)
                      .find_if(_max, hook);

    if (result.number_of_active_nodes() == 0) {
      report_default(
          "RepOrc: No faithful rep. o.r.c. on [{}, {}) points found\n",
          _min,
          _max + 1);
      result.induced_subgraph_no_checks(0, 0);
    } else {
      report_default("RepOrc: Found a faithful rep. o.r.c. on {} points\n",
                     presentation().contains_empty_word()
                         ? result.number_of_active_nodes()
                         : result.number_of_active_nodes() - 1);
      if (presentation().contains_empty_word()) {
        result.induced_subgraph_no_checks(0, result.number_of_active_nodes());
      } else {
        result.induced_subgraph_no_checks(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
      }
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////
  // MinimalRepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  // An alternative to the approach used below would be to do a sort of
  // binary search for a minimal representation. It seems that in most
  // examples that I've tried, this actually finds the minimal rep close to
  // the start of the search. The binary search approach would only really
  // be useful if the rep found at the start of the search was much larger
  // than the minimal one, and that the minimal one would not be found until
  // late in the search through all the reps with [1, best rep so far). It
  // seems that in most examples, binary search will involve checking many
  // of the same graphs repeatedly, i.e. if we check [1, size = 1000) find a
  // rep on 57 points, then check [1, 57 / 2) find nothing, then check
  // [57/2, 57) and find nothing, then the search through [57/2, 57)
  // actually iterates through all the digraphs with [1, 57 / 2) again (just
  // doesn't construct a FroidurePin object for these). So, it seems to be
  // best to just search through the digraphs with [1, 57) nodes once.
  //
  // TODO(later) perhaps find minimal 2-sided congruences first (or try to)
  // and then run MinimalRepOrc for right congruences excluding all the
  // generating pairs from the minimal 2-sided congruences. Also with this
  // approach FroidurePin wouldn't be required in RepOrc. This might not work,
  // given that the minimal rc might contain some pairs from minimal 2-sided
  // congs, just not all of them.
  Sims1::word_graph_type MinimalRepOrc::word_graph() const {
    auto cr = RepOrc(*this);

    size_t hi   = (presentation().contains_empty_word() ? _size : _size + 1);
    auto   best = cr.min_nodes(1).max_nodes(hi).target_size(_size).word_graph();

    if (best.number_of_nodes() < 1) {
      stats_copy_from(cr.stats());
      return best;
    }

    hi        = best.number_of_nodes();
    auto next = cr.max_nodes(hi - 1).word_graph();
    while (next.number_of_nodes() != 0) {
      hi   = next.number_of_nodes();
      best = std::move(next);
      next = cr.max_nodes(hi - 1).word_graph();
    }
    stats_copy_from(cr.stats());
    return best;
  }

}  // namespace libsemigroups
