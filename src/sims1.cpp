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

namespace libsemigroups {
  // TODO use report_default
  std::ostream& operator<<(std::ostream& os, Sims1Stats const& stats) {
    os << "#0: Sims1: total number of nodes in search tree was "
       << detail::group_digits(stats.total_pending) << std::endl;
    os << "#0: Sims1: max. number of pending definitions was "
       << detail::group_digits(stats.max_pending) << std::endl;
    return os;
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

  Sims1::iterator_base::iterator_base(Sims1 const&                   s,
                                      Presentation<word_type> const& p,
                                      size_type                      n)
      :  // private
        _max_num_classes(p.contains_empty_word() ? n : n + 1),
        _min_target_node(p.contains_empty_word() ? 0 : 1),
        // protected
        _felsch_graph(p),
        _mtx(),
        _pending(),
        _sims1(&s) {
    // n == 0 only when the iterator is cend
    _felsch_graph.number_of_active_nodes(n == 0 ? 0 : 1);
    // = 0 indicates iterator is done
    _felsch_graph.add_nodes(n);
  }

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

      size_type const start = _felsch_graph.number_of_edges();

      _felsch_graph.set_target_no_checks(
          current.source, current.generator, current.target);

      auto first = _sims1->include().rules.cbegin();
      auto last  = _sims1->include().rules.cend();
      if (!felsch_graph::make_compatible<RegisterDefs>(
              _felsch_graph, 0, 1, first, last)
          || !_felsch_graph.process_definitions(start)) {
        // Seems to be important to check include() first then
        // process_definitions
        return false;
      }
      // TODO add checking that some pairs of words are not in the congruence
      // also
    }

    letter_type     a        = current.generator + 1;
    size_type const M        = _felsch_graph.number_of_active_nodes();
    size_type const N        = _felsch_graph.number_of_edges();
    size_type const num_gens = _felsch_graph.out_degree();
    auto&           stats    = _sims1->stats();

    for (node_type next = current.source; next < M; ++next) {
      for (; a < num_gens; ++a) {
        if (_felsch_graph.target_no_checks(next, a) == UNDEFINED) {
          std::lock_guard<std::mutex> lock(_mtx);
          if (M < _max_num_classes) {
            stats.total_pending++;
            _pending.emplace_back(next, a, M, N, M + 1);
          }
          for (node_type b = M; b-- > _min_target_node;) {
            _pending.emplace_back(next, a, b, N, M);
          }
          stats.total_pending += M - _min_target_node;
          stats.max_pending = std::max(static_cast<uint64_t>(_pending.size()),
                                       stats.max_pending);
          return false;
        }
      }
      a = 0;
    }
    // No undefined edges, word graph is complete
    LIBSEMIGROUPS_ASSERT(N == M * num_gens);

    auto first = _sims1->long_rules().rules.cbegin();
    auto last  = _sims1->long_rules().rules.cend();
    return felsch_graph::make_compatible<RegisterDefs>(
        _felsch_graph, 0, M, first, last);
  }

  ///////////////////////////////////////////////////////////////////////////////
  // iterator nested class
  ///////////////////////////////////////////////////////////////////////////////

  Sims1::iterator::iterator(Sims1 const&                   s,
                            Presentation<word_type> const& p,
                            size_type                      n)
      : iterator_base(s, p, n) {
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
    thread_iterator(Sims1 const&                   s,
                    Presentation<word_type> const& p,
                    size_type                      n)
        : iterator_base(s, p, n) {}

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
    uint64_t                                      _report_interval;
    digraph_type                                  _result;
    Sims1Stats                                    _stats;  // TODO remove

    void worker_thread(unsigned                                 my_index,
                       std::function<bool(digraph_type const&)> hook) {
      PendingDef pd;
      for (auto i = 0; i < 16; ++i) {
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
#ifdef LIBSEMIGROUPS_VERBOSE
      report_default(
          "this thread created {} nodes\n",
          detail::group_digits(_theives[my_index]->stats().total_pending));
#endif
      std::lock_guard<std::mutex> lock(_mtx);
      _stats += _theives[my_index]->stats();
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
    thread_runner(Sims1 const&                   s,
                  Presentation<word_type> const& p,
                  size_type                      n,
                  size_type                      num_threads,
                  uint64_t                       report_interval)
        : _done(false),
          _theives(),
          _threads(),
          _mtx(),
          _num_threads(num_threads),
          _report_interval(report_interval),
          _result(),
          _stats()

    {
      for (size_t i = 0; i < _num_threads; ++i) {
        _theives.push_back(std::make_unique<thread_iterator>(s, p, n));
      }
      _theives.front()->init(n);
    }

    ~thread_runner() = default;

    digraph_type const& digraph() const {
      return _result;
    }

    void run(std::function<bool(digraph_type const&)> hook) {
      detail::Timer t;
      auto          start_time  = std::chrono::high_resolution_clock::now();
      auto          last_report = start_time;
      std::atomic_uint64_t last_count(0);
      std::atomic_uint64_t count(0);

      auto actual_hook = hook;
      if (report::should_report()) {
        actual_hook = [&](digraph_type const& ad) {
          if (hook(ad)) {
            return true;
          }
          report_number_of_congruences(_report_interval,
                                       start_time,
                                       last_report,
                                       last_count,
                                       ++count,
                                       _mtx);
          return false;
        };
      }
      try {
        detail::JoinThreads joiner(_threads);
        for (size_t i = 0; i < _num_threads; ++i) {
          _threads.push_back(std::thread(
              &thread_runner::worker_thread, this, i, std::ref(actual_hook)));
        }
      } catch (...) {
        _done = true;
        throw;
      }
      final_report_number_of_congruences(start_time, count);
    }

    Sims1Stats& stats() {
      return _stats;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // Sims1
  ////////////////////////////////////////////////////////////////////////

  Sims1::Sims1(congruence_kind ck) : Sims1Settings<Sims1>(), _kind(ck) {
    if (ck == congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected congruence_kind::right or congruence_kind::left");
    }
  }

  Sims1::~Sims1() = default;

  uint64_t Sims1::number_of_congruences(size_type n) const {
    if (number_of_threads() == 1) {
      uint64_t result = 0;
      for_each(n, [&result](digraph_type const&) { ++result; });
      return result;
    } else {
      std::atomic_int64_t result(0);
      for_each(n, [&result](digraph_type const&) { ++result; });
      return result;
    }
  }

  // Apply the function pred to every one-sided congruence with at
  // most n classes

  void Sims1::for_each(size_type                                n,
                       std::function<void(digraph_type const&)> pred) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st argument (size_type) to be non-zero");
    } else if (short_rules().rules.empty()
               && short_rules().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the short_rules() must be defined before calling this function");
    }
    report_at_start(short_rules(), long_rules(), n, number_of_threads());
    if (number_of_threads() == 1) {
      if (!report::should_report()) {
        // No stats in this case
        std::for_each(cbegin(n), cend(n), pred);
      } else {
        auto       start_time  = std::chrono::high_resolution_clock::now();
        auto       last_report = start_time;
        uint64_t   last_count  = 0;
        uint64_t   count       = 0;
        std::mutex mtx;  // does nothing
        auto       it   = cbegin(n);
        auto const last = cend(n);
        for (; it != last; ++it) {
          report_number_of_congruences(report_interval(),
                                       start_time,
                                       last_report,
                                       last_count,
                                       ++count,
                                       mtx);
          pred(*it);
        }
        final_report_number_of_congruences(start_time, count);
        // Copy the iterator stats into this so that we can retrieve it
        // after den is destroyed.
        stats(it.stats());
        report_stats();
      }
    } else {
      thread_runner den(
          *this, short_rules(), n, number_of_threads(), report_interval());
      auto pred_wrapper = [&pred](digraph_type const& ad) {
        pred(ad);
        return false;
      };
      den.run(pred_wrapper);
      // Copy the thread_runner stats into this so that we can retrieve it
      // after den is destroyed.
      stats(den.stats());
      report_stats();
    }
  }

  typename Sims1::digraph_type
  Sims1::find_if(size_type                                n,
                 std::function<bool(digraph_type const&)> pred) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected the 1st argument (size_type) to be non-zero");
    } else if (short_rules().rules.empty()
               && short_rules().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the short_rules() must be defined before calling this function");
    }
    report_at_start(short_rules(), long_rules(), n, number_of_threads());
    if (number_of_threads() == 1) {
      if (!report::should_report()) {
        return *std::find_if(cbegin(n), cend(n), pred);
      } else {
        stats().zero_stats();
        std::thread               report_thread = launch_report_thread();
        detail::ReportThreadGuard tg(*this, report_thread);

        auto       it   = cbegin(n);
        auto const last = cend(n);

        for (; it != last; ++it) {
          if (pred(*it)) {
            // final_report_number_of_congruences(start_time, ++count);
            stats(it.stats());
            report_stats();
            return *it;
          }
        }
        // final_report_number_of_congruences(start_time, ++count);
        // Copy the iterator stats into this so that we can retrieve it
        // after it is destroyed.
        stats(it.stats());
        report_stats();
        return *last;  // the empty digraph
      }
    } else {
      thread_runner den(
          *this, short_rules(), n, number_of_threads(), report_interval());
      den.run(pred);
      // Copy the thread_runner stats into this so that we can retrieve it
      // after den is destroyed.
      stats(den.stats());
      report_stats();
      return den.digraph();
    }
  }

  Sims1::iterator Sims1::cbegin(size_type n) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_type) must be non-zero");
    } else if (short_rules().rules.empty()
               && short_rules().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the short_rules() must be defined before "
                              "calling this function");
    }
    return iterator(*this, short_rules(), n);
  }

  //! Returns a forward iterator pointing one beyond the
  //! last congruence.
  //!
  //! Returns a forward iterator pointing to the empty
  //! WordGraph. If incremented, the returned iterator
  //! remains valid and continues to point at the empty
  //! WordGraph.
  //!
  //! \param n the maximum number of classes in a
  //! congruence.
  //!
  //! \returns
  //! An iterator \c it of type \c iterator pointing to
  //! an WordGraph with at most \p 0 nodes.
  //!
  //! \throws LibsemigroupsException if \p n is \c 0.
  //! \throws LibsemigroupsException if `short_rules()`
  //! has 0-generators and 0-relations (i.e. it has not
  //! been initialised).
  //!
  //! \warning
  //! Copying iterators of this type is expensive.  As a
  //! consequence, prefix incrementing \c ++it the
  //! returned  iterator \c it significantly cheaper than
  //! postfix incrementing \c it++.
  //!
  //! \sa
  //! \ref cbegin
  Sims1::iterator Sims1::cend(size_type n) const {
    if (n == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_type) must be non-zero");
    } else if (short_rules().rules.empty()
               && short_rules().alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION("the short_rules() must be defined before "
                              "calling this function");
    }
    return iterator(*this, short_rules(), 0);
  }

  void Sims1::report_at_start(Presentation<word_type> const& shorts,
                              Presentation<word_type> const& longs,
                              size_t                         num_classes,
                              size_t                         num_threads) {
    if (num_threads == 1) {
      report_default("Sims1: using 0 additional threads\n");
    } else {
      report_default("Sims1: using {} / {} additional threads\n",
                     num_threads,
                     std::thread::hardware_concurrency());
    }
    report_default("Sims1: finding congruences with at most {} classes\n",
                   uint64_t(num_classes));
    report_default("Sims1: using {} generators, and {} short relations u = v"
                   " with:\n",
                   shorts.alphabet().size(),
                   shorts.rules.size() / 2);
    uint64_t shortest_short, longest_short;
    if (shorts.rules.empty()) {
      shortest_short = 0;
      longest_short  = 0;
    } else {
      shortest_short = presentation::shortest_rule_length(shorts);
      longest_short  = presentation::longest_rule_length(shorts);
    }
    report_default(
        "Sims1: |u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
        shortest_short,
        longest_short,
        presentation::length(shorts));
    if (!longs.rules.empty()) {
      report_default("Sims1: {} long relations u = v with:\n",
                     longs.rules.size() / 2);
      report_default(
          "Sims1: |u| + |v| \u2208 [{}, {}] and \u2211(|u| + |v|) = {}\n",
          presentation::shortest_rule_length(longs),
          presentation::longest_rule_length(longs),
          presentation::length(longs));
    }
  }

  template <typename S>
  void Sims1::report_number_of_congruences(uint64_t    report_interval,
                                           time_point& start_time,
                                           time_point& last_report,
                                           S&          last_count,
                                           uint64_t    count_now,
                                           std::mutex& mtx) {
    using std::chrono::duration_cast;
    using std::chrono::seconds;

    std::lock_guard<std::mutex> lock(mtx);
    if (count_now - last_count > report_interval) {
      auto now = std::chrono::high_resolution_clock::now();
      if (now - last_report > std::chrono::seconds(1)) {
        auto total_time = duration_cast<seconds>(now - start_time);
        auto diff_time  = duration_cast<seconds>(now - last_report);
        report_default(
            "Sims1: found {} congruences in {} ({}/s)!\n",
            detail::group_digits(count_now).c_str(),
            total_time.count(),
            detail::group_digits((count_now - last_count) / diff_time.count())
                .c_str());
        std::swap(now, last_report);
        last_count = count_now;
      }
    }
  }

  void Sims1::report_progress_from_thread() const {
    using std::chrono::duration_cast;
    using std::chrono::seconds;

    auto now        = std::chrono::high_resolution_clock::now();
    auto total_time = now - start_time();
    auto diff_time  = duration_cast<seconds>(now - last_report());

    auto& s = stats();

    report_default(
        "Sims1: found {} congruences in {} ({}/s)!\n",
        detail::group_digits(s.count_now),
        string_time(total_time),
        detail::group_digits((s.count_now - s.count_last) / diff_time.count()));
    last_report(now);
    s.count_last = s.count_now;
  }

  void Sims1::final_report_number_of_congruences(time_point& start_time,
                                                 uint64_t    count) {
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;

    auto elapsed = duration_cast<nanoseconds>(
        std::chrono::high_resolution_clock::now() - start_time);
    if (count != 0) {
      report_default("Sims1: found {} congruences in {} ({} per congruence)!\n",
                     detail::group_digits(count),
                     detail::Timer::string(elapsed),
                     detail::Timer::string(elapsed / count));
    } else {
      report_default("Sims1: found {} congruences in {}!\n",
                     detail::group_digits(count),
                     detail::Timer::string(elapsed));
    }
  }

  void Sims1::report_stats() const {
    report_default("total number of nodes in search tree was {}\n",
                   detail::group_digits(stats().total_pending));
    report_default("max. number of pending definitions was {}\n",
                   detail::group_digits(stats().max_pending));
  }

  ////////////////////////////////////////////////////////////////////////
  // RepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  Sims1::digraph_type RepOrc::digraph() const {
    using digraph_type = typename Sims1::digraph_type;
    using node_type    = typename digraph_type::node_type;
    report_default("searching for a faithful rep. o.r.c. on [{}, {}) points\n",
                   _min,
                   _max + 1);
    if (_min > _max || _max == 0) {
      report_default("no faithful rep. o.r.c. exists in [{}, {}) = \u2205\n",
                     _min,
                     _max + 1);
      return digraph_type(0, 0);
    }

    SuppressReportFor suppressor("FroidurePin");

    std::atomic_uint64_t count(0);

    auto hook = [&](digraph_type const& x) {
      ++count;
      if (x.number_of_active_nodes() >= _min) {
        auto first = (short_rules().contains_empty_word() ? 0 : 1);
        auto S     = to_froidure_pin<Transf<0, node_type>>(
            x, first, x.number_of_active_nodes());
        // TODO reuse S here, using init + whatever else.
        if (short_rules().contains_empty_word()) {
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

    auto result
        = Sims1(congruence_kind::right).settings(*this).find_if(_max, hook);

    if (result.number_of_active_nodes() == 0) {
      report_default(
          "no faithful rep. o.r.c. on [{}, {}) points found\n", _min, _max + 1);
      result.induced_subgraph_no_checks(0, 0);
    } else {
      // FIXME this seems to report the wrong number of points in i.e. [038]
      report_default("found a faithful rep. o.r.c. on {} points\n",
                     result.number_of_active_nodes());
      if (short_rules().contains_empty_word()) {
        result.induced_subgraph_no_checks(0, result.number_of_active_nodes());
      } else {
        result.induced_subgraph_no_checks(1, result.number_of_active_nodes());
        result.number_of_active_nodes(result.number_of_active_nodes() - 1);
      }
    }
    report::clear_suppressions();
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
  // TODO perhaps find minimal 2-sided congruences first (or try to) and then
  // run MinimalRepOrc for right congruences excluding all the generating pairs
  // from the minimal 2-sided congruences. Also with this approach FroidurePin
  // wouldn't be required in RepOrc.
  Sims1::digraph_type MinimalRepOrc::digraph() const {
    auto cr = RepOrc(*this);

    size_t hi   = (short_rules().contains_empty_word() ? _size : _size + 1);
    auto   best = cr.min_nodes(1).max_nodes(hi).target_size(_size).digraph();

    if (best.number_of_nodes() < 1) {
      stats(cr.stats());
      return best;
    }

    hi        = best.number_of_nodes();
    auto next = cr.max_nodes(hi - 1).digraph();
    while (next.number_of_nodes() != 0) {
      hi   = next.number_of_nodes();
      best = std::move(next);
      next = cr.max_nodes(hi - 1).digraph();
    }
    stats(cr.stats());
    return best;
  }

}  // namespace libsemigroups
