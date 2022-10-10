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
  std::ostream& operator<<(std::ostream& os, Sims1Stats const& stats) {
    os << "#0: Sims1: total number of nodes in search tree was "
       << detail::group_digits(stats.total_pending) << std::endl;
    os << "#0: Sims1: max. number of pending definitions was "
       << detail::group_digits(stats.max_pending) << std::endl;
    return os;
  }
#endif

  ////////////////////////////////////////////////////////////////////////
  // Sims1Settings
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  Sims1Settings<T>::Sims1Settings()
      : _extra(),
        _longs(),
        _num_threads(),
        _report_interval(),
        _shorts()
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        ,
        _stats()
#endif
  {
    number_of_threads(1);
    report_interval(999);
  }

  template <typename T>
  template <typename S>
  Sims1Settings<T>::Sims1Settings(Sims1Settings<S> const& that)
      : _extra(that.extra()),
        _longs(that.long_rules()),
        _num_threads(that.number_of_threads()),
        _report_interval(that.report_interval()),
        _shorts(that.short_rules())
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        ,
        _stats(that.stats())
#endif
  {
  }

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::short_rules(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    // This normalises the rules in the case they are of the right type but
    // not normalised
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (Presentation) must not have 0 generators");
    }
    auto normal_p = make<Presentation<word_type>>(p);
    validate_presentation(normal_p, long_rules());
    validate_presentation(normal_p, extra());
    _shorts = normal_p;
    return static_cast<T&>(*this);
  }

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::long_rules(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    // We call make in the next two lines to ensure that the generators of
    // the presentation are {0, ..., n - 1} where n is the size of the
    // alphabet.
    auto normal_p = make<Presentation<word_type>>(p);
    validate_presentation(normal_p, short_rules());
    validate_presentation(normal_p, extra());
    _longs = normal_p;
    return static_cast<T&>(*this);
  }

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::extra(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    auto normal_p = make<Presentation<word_type>>(p);
    validate_presentation(normal_p, short_rules());
    validate_presentation(normal_p, long_rules());
    _extra = normal_p;
    return static_cast<T&>(*this);
  }

  template <typename T>
  T& Sims1Settings<T>::number_of_threads(size_t val) {
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_t) must be non-zero");
    }
    _num_threads = val;
    return static_cast<T&>(*this);
  }

  template <typename T>
  T& Sims1Settings<T>::long_rule_length(size_t val) {
    auto partition = [&val](auto first, auto last) {
      for (; first != last; first += 2) {
        if (first->size() + (first + 1)->size() >= val) {
          break;
        }
      }
      if (first == last) {
        return first;
      }

      for (auto lhs = first + 2; lhs < last; lhs += 2) {
        auto rhs = lhs + 1;
        if (lhs->size() + rhs->size() < val) {
          std::iter_swap(lhs, first++);
          std::iter_swap(rhs, first++);
        }
      }
      return first;
    };

    // points at the lhs of the first rule of length at least val
    auto its = partition(_shorts.rules.begin(), _shorts.rules.end());
    _longs.rules.insert(_longs.rules.end(),
                        std::make_move_iterator(its),
                        std::make_move_iterator(_shorts.rules.end()));
    auto lastl = _longs.rules.end() - std::distance(its, _shorts.rules.end());
    _shorts.rules.erase(its, _shorts.rules.end());

    // points at the lhs of the first rule of length at least val
    auto itl = partition(_longs.rules.begin(), lastl);
    _shorts.rules.insert(_shorts.rules.end(),
                         std::make_move_iterator(_longs.rules.begin()),
                         std::make_move_iterator(itl));
    _longs.rules.erase(_longs.rules.begin(), itl);
    return static_cast<T&>(*this);
  }

  template <typename T>
  void Sims1Settings<T>::split_at(size_t val) {
    if (val > _shorts.rules.size() / 2 + _longs.rules.size() / 2) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected a value in the range [0, %llu), found %llu",
          uint64_t(_shorts.rules.size() / 2 + _longs.rules.size() / 2),
          uint64_t(val));
    }

    val *= 2;
    if (val < _shorts.rules.size()) {
      _longs.rules.insert(_longs.rules.begin(),
                          _shorts.rules.begin() + val,
                          _shorts.rules.end());
      _shorts.rules.erase(_shorts.rules.begin() + val, _shorts.rules.end());
    } else {
      val -= _shorts.rules.size();
      _shorts.rules.insert(_shorts.rules.end(),
                           _longs.rules.begin(),
                           _longs.rules.begin() + val);
      _longs.rules.erase(_longs.rules.begin(), _longs.rules.begin() + val);
    }
  }

  template <typename T>
  void Sims1Settings<T>::validate_presentation(
      Presentation<word_type> const& arg,
      Presentation<word_type> const& existing) {
    if (!arg.alphabet().empty() && !existing.alphabet().empty()
        && arg.alphabet() != existing.alphabet()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) is not defined over "
          "the correct alphabet, expected alphabet %s got %s",
          detail::to_string(existing.alphabet()).c_str(),
          detail::to_string(arg.alphabet()).c_str());
    }
    arg.validate();
  }

  ////////////////////////////////////////////////////////////////////////
  // Sims1
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  Sims1<T>::Sims1(congruence_kind ck) : Sims1Settings<Sims1<T>>(), _kind(ck) {
    if (ck == congruence_kind::twosided) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected congruence_kind::right or congruence_kind::left");
    }
  }

  template <typename T>
  Sims1<T>::~Sims1() = default;

  template <typename T>
  uint64_t Sims1<T>::number_of_congruences(size_type n) const {
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
  template <typename T>
  void Sims1<T>::for_each(size_type                                n,
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
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        // Copy the iterator stats into this so that we can retrieve it
        // after den is destroyed.
        stats(it.stats());
        report_stats();
#endif
      }
    } else {
      thread_runner den(short_rules(),
                        extra(),
                        long_rules(),
                        n,
                        number_of_threads(),
                        report_interval());
      auto          pred_wrapper = [&pred](digraph_type const& ad) {
        pred(ad);
        return false;
      };
      den.run(pred_wrapper);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      // Copy the thread_runner stats into this so that we can retrieve it
      // after den is destroyed.
      stats(den.stats());
      report_stats();
#endif
    }
  }

  template <typename T>
  typename Sims1<T>::digraph_type
  Sims1<T>::find_if(size_type                                n,
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
        auto       start_time  = std::chrono::high_resolution_clock::now();
        auto       last_report = start_time;
        uint64_t   last_count  = 0;
        uint64_t   count       = 0;
        std::mutex mtx;  // does nothing
        auto       it   = cbegin(n);
        auto const last = cend(n);

        for (; it != last; ++it) {
          if (pred(*it)) {
            final_report_number_of_congruences(start_time, ++count);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
            stats(it.stats());
            report_stats();
#endif
            return *it;
          }
          report_number_of_congruences(report_interval(),
                                       start_time,
                                       last_report,
                                       last_count,
                                       ++count,
                                       mtx);
        }
        final_report_number_of_congruences(start_time, ++count);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
        // Copy the iterator stats into this so that we can retrieve it
        // after it is destroyed.
        stats(it.stats());
        report_stats();
#endif
        return *last;  // the empty digraph
      }
    } else {
      thread_runner den(short_rules(),
                        extra(),
                        long_rules(),
                        n,
                        number_of_threads(),
                        report_interval());
      den.run(pred);
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      // Copy the thread_runner stats into this so that we can retrieve it
      // after den is destroyed.
      stats(den.stats());
      report_stats();
#endif
      return den.digraph();
    }
  }

  template <typename T>
  void Sims1<T>::report_at_start(Presentation<word_type> const& shorts,
                                 Presentation<word_type> const& longs,
                                 size_t                         num_classes,
                                 size_t                         num_threads) {
    if (num_threads == 1) {
      REPORT_DEFAULT_V3("Sims1: using 0 additional threads\n");
    } else {
      REPORT_DEFAULT_V3("Sims1: using %d / %d additional threads\n",
                        num_threads,
                        std::thread::hardware_concurrency());
    }
    REPORT_DEFAULT_V3("Sims1: finding congruences with at most %llu classes\n",
                      uint64_t(num_classes));
    REPORT_DEFAULT_V3(
        "Sims1: using %llu generators, and %llu short relations u = v"
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
    REPORT_DEFAULT_V3(
        "Sims1: |u| + |v| \u2208 [%llu, %llu] and \u2211(|u| + |v|) = %llu\n",
        shortest_short,
        longest_short,
        presentation::length(shorts));
    if (!longs.rules.empty()) {
      REPORT_DEFAULT_V3("Sims1: %llu long relations u = v with:\n",
                        longs.rules.size() / 2);
      REPORT_DEFAULT_V3(
          "Sims1: |u| + |v| \u2208 [%llu, %llu] and \u2211(|u| + |v|) = %llu\n",
          presentation::shortest_rule_length(longs),
          presentation::longest_rule_length(longs),
          presentation::length(longs));
    }
  }

  template <typename T>
  template <typename S>
  void Sims1<T>::report_number_of_congruences(uint64_t    report_interval,
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
        REPORT_DEFAULT_V3(
            "Sims1: found %s congruences in %llus (%s/s)!\n",
            detail::group_digits(count_now).c_str(),
            total_time.count(),
            detail::group_digits((count_now - last_count) / diff_time.count())
                .c_str());
        std::swap(now, last_report);
        last_count = count_now;
      }
    }
  }

  template <typename T>
  void Sims1<T>::final_report_number_of_congruences(time_point& start_time,
                                                    uint64_t    count) {
    using std::chrono::duration_cast;
    using std::chrono::nanoseconds;

    auto elapsed = duration_cast<nanoseconds>(
        std::chrono::high_resolution_clock::now() - start_time);
    if (count != 0) {
      REPORT_DEFAULT_V3(
          "Sims1: found %s congruences in %s (%s per congruence)!\n",
          detail::group_digits(count).c_str(),
          detail::Timer::string(elapsed).c_str(),
          detail::Timer::string(elapsed / count).c_str());
    } else {
      REPORT_DEFAULT_V3("Sims1: found %s congruences in %s!\n",
                        detail::group_digits(count).c_str(),
                        detail::Timer::string(elapsed).c_str());
    }
  }

#ifdef LIBSEMIGROUPS_ENABLE_STATS
  template <typename T>
  void Sims1<T>::report_stats() const {
    REPORT_DEFAULT("total number of nodes in search tree was %s\n",
                   detail::group_digits(stats().total_pending).c_str());
    REPORT_DEFAULT("max. number of pending definitions was %s\n",
                   detail::group_digits(stats().max_pending).c_str());
  }
#endif

  ///////////////////////////////////////////////////////////////////////////////
  // iterator_base nested class
  ///////////////////////////////////////////////////////////////////////////////

  template <typename T>
  Sims1<T>::iterator_base::iterator_base(Presentation<word_type> const& p,
                                         Presentation<word_type> const& extra,
                                         Presentation<word_type> const& final_,
                                         size_type                      n)
      :  // private
        _extra(extra),
        _longs(final_),
        _max_num_classes(p.contains_empty_word() ? n : n + 1),
        _min_target_node(p.contains_empty_word() ? 0 : 1),
        // protected
        _felsch_graph(p, n),
        _mtx(),
        _pending() {
    // n == 0 only when the iterator is cend
    _felsch_graph.number_of_active_nodes(n == 0 ? 0 : 1);
    // = 0 indicates iterator is done
  }

  // The following function is separated from the constructor so that it isn't
  // called in the constructor of every thread_iterator
  template <typename T>
  void Sims1<T>::iterator_base::init(size_type n) {
    if (n != 0) {
      if (n > 1 || _min_target_node == 1) {
        _pending.emplace_back(0, 0, 1, 0, 2);
      }
      if (_min_target_node == 0) {
        _pending.emplace_back(0, 0, 0, 0, 1);
      }
    }
  }

  template <typename T>
  bool Sims1<T>::iterator_base::try_pop(PendingDef& pd) {
    std::lock_guard<std::mutex> lock(_mtx);
    if (_pending.empty()) {
      return false;
    }
    pd = std::move(_pending.back());
    _pending.pop_back();
    return true;
  }

  template <typename T>
  bool Sims1<T>::iterator_base::try_define(PendingDef const& current) {
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
          _felsch_graph.unsafe_neighbor(current.source, current.generator)
          == UNDEFINED);

      size_type const start = _felsch_graph.number_of_edges();

      _felsch_graph.def_edge(current.source, current.generator, current.target);

      auto first = _extra.rules.cbegin();
      auto last  = _extra.rules.cend();
      if (!felsch_digraph::compatible(_felsch_graph, 0, first, last)
          || !_felsch_graph.process_definitions(start)) {
        // Seems to be important to check _extra first then
        // process_definitions
        return false;
      }
    }

    letter_type     a        = current.generator + 1;
    size_type const M        = _felsch_graph.number_of_active_nodes();
    size_type const N        = _felsch_graph.number_of_edges();
    size_type const num_gens = _felsch_graph.out_degree();

    for (node_type next = current.source; next < M; ++next) {
      for (; a < num_gens; ++a) {
        if (_felsch_graph.unsafe_neighbor(next, a) == UNDEFINED) {
          std::lock_guard<std::mutex> lock(_mtx);
          if (M < _max_num_classes) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
            ++_stats.total_pending;
#endif
            _pending.emplace_back(next, a, M, N, M + 1);
          }
          for (node_type b = M; b-- > _min_target_node;) {
            _pending.emplace_back(next, a, b, N, M);
          }
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          _stats.total_pending += M - _min_target_node;
          _stats.max_pending = std::max(static_cast<uint64_t>(_pending.size()),
                                        _stats.max_pending);
#endif
          return false;
        }
      }
      a = 0;
    }
    // No undefined edges, word graph is complete
    LIBSEMIGROUPS_ASSERT(N == M * num_gens);

    auto first = _longs.rules.cbegin();
    auto last  = _longs.rules.cend();
    return felsch_digraph::compatible(_felsch_graph, 0, M, first, last);
  }

  template <typename T>
  Sims1<T>::iterator_base::iterator_base(Sims1<T>::iterator_base const& that)
      : _extra(that._extra),
        _longs(that._longs),
        _max_num_classes(that._max_num_classes),
        _min_target_node(that._min_target_node),
        _felsch_graph(that._felsch_graph),
        _pending(that._pending) {}

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.
  template <typename T>
  Sims1<T>::iterator_base::iterator_base(Sims1<T>::iterator_base&& that)
      : _extra(std::move(that._extra)),
        _longs(std::move(that._longs)),
        _max_num_classes(std::move(that._max_num_classes)),
        _min_target_node(std::move(that._min_target_node)),
        _felsch_graph(std::move(that._felsch_graph)),
        _pending(std::move(that._pending)) {}

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.
  template <typename T>
  typename Sims1<T>::iterator_base&
  Sims1<T>::iterator_base::operator=(Sims1<T>::iterator_base const& that) {
    _extra           = that._extra;
    _longs           = that._longs;
    _max_num_classes = that._max_num_classes;
    _min_target_node = that._min_target_node;
    _felsch_graph    = that._felsch_graph;
    _pending         = that._pending;
    return *this;
  }

  // Intentionally don't copy the mutex, it doesn't compile, wouldn't make
  // sense if the mutex was used here.
  template <typename T>
  typename Sims1<T>::iterator_base&
  Sims1<T>::iterator_base::operator=(Sims1<T>::iterator_base&& that) {
    _extra           = std::move(that._extra);
    _longs           = std::move(that.long_rules());
    _max_num_classes = std::move(that._max_num_classes);
    _min_target_node = std::move(that._min_target_node);
    _felsch_graph    = std::move(that._felsch_graph);
    _pending         = std::move(that._pending);
    return *this;
  }

  template <typename T>
  void Sims1<T>::iterator_base::swap(Sims1<T>::iterator_base& that) noexcept {
    std::swap(_extra, that._extra);
    std::swap(_felsch_graph, that._felsch_graph);
    std::swap(_max_num_classes, that._max_num_classes);
    std::swap(_min_target_node, that._min_target_node);
    std::swap(_pending, that._pending);
  }

  ///////////////////////////////////////////////////////////////////////////////
  // iterator nested class
  ///////////////////////////////////////////////////////////////////////////////

  template <typename T>
  Sims1<T>::iterator::iterator(Presentation<word_type> const& p,
                               Presentation<word_type> const& e,
                               Presentation<word_type> const& f,
                               size_type                      n)
      : iterator_base(p, e, f, n) {
    if (this->_felsch_graph.number_of_active_nodes() == 0) {
      return;
    }
    init(n);
    ++(*this);
    // The increment above is required so that when dereferencing any
    // instance of this type we obtain a valid word graph (o/w the value
    // pointed to here is empty).
  }

  template <typename T>
  typename Sims1<T>::iterator const& Sims1<T>::iterator::operator++() {
    PendingDef current;
    while (try_pop(current)) {
      if (try_define(current)) {
        return *this;
      }
    }
    this->_felsch_graph.number_of_active_nodes(0);
    // indicates that the iterator is done
    this->_felsch_graph.restrict(0);
    return *this;
  }

  ///////////////////////////////////////////////////////////////////////////////
  // thread_iterator
  ///////////////////////////////////////////////////////////////////////////////

  // Note that this class is private, and not really an iterator in the usual
  // sense. It is designed solely to work with thread_runner.
  template <typename T>
  class Sims1<T>::thread_iterator : public iterator_base {
    friend class Sims1<T>::thread_runner;

    using iterator_base::copy_felsch_graph;

   public:
    //! No doc
    thread_iterator(Presentation<word_type> const& p,
                    Presentation<word_type> const& e,
                    Presentation<word_type> const& f,
                    size_type                      n)
        : iterator_base(p, e, f, n) {}

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

#ifdef LIBSEMIGROUPS_ENABLE_STATS
    using iterator_base::stats;
#endif

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
      // Copy the FelschDigraph from that into *this
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
      // Copy the FelschDigraph and half pending from *this into q
      q.steal_from(*this);  // Must call steal_from on q, so that q is locked
      return true;
    }
  };

  ////////////////////////////////////////////////////////////////////////
  // thread_runner
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  class Sims1<T>::thread_runner {
   private:
    std::atomic_bool                              _done;
    std::vector<std::unique_ptr<thread_iterator>> _theives;
    std::vector<std::thread>                      _threads;
    std::mutex                                    _mtx;
    size_type                                     _num_threads;
    uint64_t                                      _report_interval;
    digraph_type                                  _result;
#ifdef LIBSEMIGROUPS_ENABLE_STATS
    Sims1Stats _stats;
#endif

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
      REPORT_DEFAULT(
          "this thread created %s nodes\n",
          detail::group_digits(_theives[my_index]->stats().total_pending)
              .c_str());
#endif
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      std::lock_guard<std::mutex> lock(_mtx);
      _stats += _theives[my_index]->stats();
#endif
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
    thread_runner(Presentation<word_type> const& p,
                  Presentation<word_type> const& e,
                  Presentation<word_type> const& f,
                  size_type                      n,
                  size_type                      num_threads,
                  uint64_t                       report_interval)
        : _done(false),
          _theives(),
          _threads(),
          _mtx(),
          _num_threads(num_threads),
          _report_interval(report_interval),
          _result()
#ifdef LIBSEMIGROUPS_ENABLE_STATS
          ,
          _stats()
#endif

    {
      for (size_t i = 0; i < _num_threads; ++i) {
        _theives.push_back(std::make_unique<thread_iterator>(p, e, f, n));
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

#ifdef LIBSEMIGROUPS_ENABLE_STATS
    Sims1Stats const& stats() {
      return _stats;
    }
#endif
  };

  ////////////////////////////////////////////////////////////////////////
  // RepOrc helper class
  ////////////////////////////////////////////////////////////////////////

  template <typename T>
  ActionDigraph<T> RepOrc::digraph() const {
    using digraph_type = typename Sims1<T>::digraph_type;
    using node_type    = typename digraph_type::node_type;
    REPORT_DEFAULT(
        "searching for a faithful rep. o.r.c. on [%llu, %llu) points\n",
        _min,
        _max + 1);
    if (_min > _max || _max == 0) {
      REPORT_DEFAULT(
          "no faithful rep. o.r.c. exists in [%llu, %llu) = \u2205\n",
          _min,
          _max + 1);
      return ActionDigraph<T>(0, 0);
    }

    report::suppress("FroidurePin");

    std::atomic_uint64_t count(0);

    auto hook = [&](digraph_type const& x) {
      ++count;
      if (x.number_of_active_nodes() >= _min) {
        auto first = (short_rules().contains_empty_word() ? 0 : 1);
        auto S     = make<FroidurePin<Transf<0, node_type>>>(
            x, first, x.number_of_active_nodes());
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
        = Sims1<T>(congruence_kind::right).settings(*this).find_if(_max, hook);

    if (result.number_of_active_nodes() == 0) {
      REPORT_DEFAULT("no faithful rep. o.r.c. on [%llu, %llu) points found\n",
                     _min,
                     _max + 1);
      result.restrict(0);
    } else {
      REPORT_DEFAULT("found a faithful rep. o.r.c. on %llu points\n",
                     result.number_of_active_nodes());
      if (short_rules().contains_empty_word()) {
        result.restrict(result.number_of_active_nodes());
      } else {
        result.induced_subdigraph(1, result.number_of_active_nodes());
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
  template <typename T>
  ActionDigraph<T> MinimalRepOrc::digraph() const {
    auto cr = RepOrc(*this);

    size_t hi   = (short_rules().contains_empty_word() ? _size : _size + 1);
    auto   best = cr.min_nodes(1).max_nodes(hi).target_size(_size).digraph();

    if (best.number_of_nodes() < 1) {
#ifdef LIBSEMIGROUPS_ENABLE_STATS
      stats(cr.stats());
#endif
      return best;
    }

    hi                    = best.number_of_nodes();
    ActionDigraph<T> next = std::move(cr.max_nodes(hi - 1).digraph());
    while (next.number_of_nodes() != 0) {
      hi   = next.number_of_nodes();
      best = std::move(next);
      next = std::move(cr.max_nodes(hi - 1).digraph());
    }
#ifdef LIBSEMIGROUPS_ENABLE_STATS
    stats(cr.stats());
#endif
    return best;
  }

}  // namespace libsemigroups
