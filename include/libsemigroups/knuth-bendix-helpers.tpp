//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2026 James D. Mitchell
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

// This file contains the implementations of helper function templates for the
// KnuthBendixImpl class template.

namespace libsemigroups {
  namespace congruence_common {
    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    std::vector<std::vector<Word>> non_trivial_classes(
        KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb1,
        KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb2) {
      using rx::operator|;

      // It is intended that kb2 is defined using the same presentation as kb1
      // and some additional rules. The output might still be meaningful if
      // this is not the case.
      if (kb1.number_of_classes() == POSITIVE_INFINITY
          && kb2.number_of_classes() != POSITIVE_INFINITY) {
        LIBSEMIGROUPS_EXCEPTION(
            "the 1st argument defines an infinite semigroup, and the 2nd "
            "argument defines a finite semigroup, so there is at least one "
            "infinite non-trivial class!");
      } else if (kb1.presentation().alphabet()
                 != kb2.presentation().alphabet()) {
        // It might be possible to handle this case too,
        // but doesn't seem worth it at present
        LIBSEMIGROUPS_EXCEPTION("the arguments must have presentations with "
                                "the same alphabets, found {} and {}",
                                kb1.presentation().alphabet(),
                                kb2.presentation().alphabet());
      }

      // We construct the WordGraph `wg` obtained by subtracting all of the
      // edges from the Gilman graph of kb2 from the Gilman graph of kb1. The
      // non-trivial classes are finite if and only if `wg` is acyclic. It
      // would be possible to do this without actually constructing `wg` but
      // constructing `wg` is simpler, and so we do that for now.

      auto const& g1 = kb1.gilman_graph();
      auto const& g2 = kb2.gilman_graph();

      LIBSEMIGROUPS_ASSERT(g1.number_of_nodes() > 0);
      LIBSEMIGROUPS_ASSERT(g2.number_of_nodes() > 0);

      if (g1.number_of_nodes() < g2.number_of_nodes()) {
        LIBSEMIGROUPS_EXCEPTION(
            "the Gilman graph of the 1st argument must have strictly fewer "
            "nodes than the Gilman graph of the 2nd argument, found {} nodes "
            "and {} nodes",
            g1.number_of_nodes(),
            g2.number_of_nodes());
      }

      // We need to obtain a mappings from the nodes of
      // g1 to g2 and vice versa.

      using node_type = typename std::decay_t<decltype(g1)>::node_type;

      std::vector<node_type> to_g2(g1.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g2[0] = 0;
      std::vector<node_type> to_g1(g2.number_of_nodes(),
                                   static_cast<node_type>(UNDEFINED));
      to_g1[0] = 0;
      for (auto v : g1.nodes()) {
        for (auto e : g1.labels()) {
          auto ve1 = g1.target_no_checks(v, e);
          if (to_g2[v] != UNDEFINED && ve1 != UNDEFINED) {
            auto ve2 = g2.target_no_checks(to_g2[v], e);
            if (ve2 != UNDEFINED && to_g2[ve1] == UNDEFINED) {
              to_g2[ve1] = ve2;
              to_g1[ve2] = ve1;
            }
          }
        }
      }

      // We do a depth first search simultaneously for cycles, and edges E in
      // g1 not in g2. Pre order for cycle detection, post order for "can we
      // reach a node incident to an edge in E" and "number of paths through a
      // node is infinite". Crucially here a node is incident to an edge if it
      // is the source or the target of an edge.
      size_t const N = g1.number_of_nodes();
      // can_reach[v] == true if there is a path from v to a node incident to
      // an edge in g1 that's not in g2.
      std::vector<bool> can_reach(N, false);
      std::vector<bool> inf_paths(N, false);
      std::vector<bool> seen(N, false);

      std::stack<node_type> stck;
      stck.push(0);

      while (!stck.empty()) {
        auto v = stck.top();
        stck.pop();
        if (v >= N) {
          // post order
          v -= N;
          for (auto e : g1.labels()) {
            auto ve1 = g1.target_no_checks(v, e);
            if (ve1 != UNDEFINED) {
              can_reach[v] = (can_reach[v] || can_reach[ve1]);
              if (can_reach[ve1]) {
                inf_paths[v] = inf_paths[ve1];
              }
              if (can_reach[v] && inf_paths[v]) {
                LIBSEMIGROUPS_EXCEPTION(
                    "there is an infinite non-trivial class!");
              }
            }
          }
        } else if (!seen[v]) {
          seen[v] = true;
          // so we can tell when all of the descendants of v have been
          // processed out of the stack
          stck.push(v + N);
          if (to_g2[v] == UNDEFINED) {
            can_reach[v] = true;
          }
          for (auto e : g1.labels()) {
            auto ve1 = g1.target_no_checks(v, e);
            if (ve1 != UNDEFINED) {
              // Check if (v, e, ve1) corresponds to an edge in g2
              if (to_g2[v] != UNDEFINED) {
                auto ve2 = g2.target_no_checks(to_g2[v], e);
                if (ve2 != UNDEFINED) {
                  // edges (v, e, ve1) and (to_g2[v], e, ve2) exist, so
                  // there's an edge in g1 not in g2 if the targets ve1 and ve2
                  // of these edges do not correspond to each other.
                  if (ve1 != to_g1[ve2]) {
                    can_reach[v]   = true;
                    can_reach[ve1] = true;
                  }
                }
              } else {
                // There's no edge labelled by e incident to the node
                // corresponding to v in g2, but there is such an edge in g1
                // and so (v, e, ve1) is in g1 but not g2.
                can_reach[v]   = true;
                can_reach[ve1] = true;
              }
              if (seen[ve1]) {
                // cycle detected
                inf_paths[v] = true;
              } else {
                stck.push(ve1);
              }
            }
          }
        }
      }

      // If we reach here, then the appropriate portion of g1 is acyclic, and
      // so all we do is enumerate the paths in that graph

      // Construct the "can_reach" subgraph of g1, could use a WordGraphView
      // here instead (but these don't yet exist) TODO(1)
      WordGraph<size_t> wg(g1.number_of_nodes(), g1.out_degree());

      for (auto v : wg.nodes()) {
        if (can_reach[v]) {
          for (auto e : wg.labels()) {
            auto ve = g1.target_no_checks(v, e);
            if (ve != UNDEFINED && can_reach[ve]) {
              wg.target_no_checks(v, e, ve);
            }
          }
        }
      }

      Paths paths(wg);
      // We only want those paths that pass through at least one of the edges
      // in g1 but not g2. Hence we require the `filter` in the next
      // expression.
      auto words = (paths.source(0) | rx::filter([&g2](word_type const& path) {
                      return v4::word_graph::last_node_on_path(
                                 g2, 0, path.cbegin(), path.cend())
                                 .second
                             != path.cend();
                    })
                    | rx::transform([&kb2](word_type const& path) {
                        Word result(path.size(), 0);
                        std::transform(
                            path.cbegin(),
                            path.cend(),
                            result.begin(),
                            [&kb2](auto index) {
                              return kb2.presentation().letter_no_checks(index);
                            });
                        return result;
                      }));
      // TODO(1) if Word == word_type, then we could avoid allocating Word
      // result above, just change path in-place.

      // Note that the normal forms in `kb2` never
      // contain an edge in g1 \ g2 and so we must add in every normal form.
      auto ntc = partition(kb2, words);
      for (auto& klass : ntc) {
        klass.push_back(reduce_no_checks(kb2, klass[0]));
      }
      return ntc;
    }
  }  // namespace congruence_common

  namespace knuth_bendix {

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - try_equal_to
    ////////////////////////////////////////////////////////////////////////

    // TODO(1) Doc
    // TODO(1) template std::string
    // TODO(1) re-include later
    // template <typename T>
    // inline tril try_equal_to(Presentation<std::string>& p,
    //                          std::string const&         lhs,
    //                          std::string const&         rhs,
    //                          T t = std::chrono::seconds(1));

    ////////////////////////////////////////////////////////////////////////
    // Possible future interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename Time>
    typename std::vector<Word>::const_iterator
    redundant_rule(Presentation<Word> const& p, Time t) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      p.throw_if_bad_alphabet_or_rules();
      Presentation<Word> q;
      q.alphabet(p.alphabet());
      q.contains_empty_word(p.contains_empty_word());
      KnuthBendix<Word> kb;

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        kb.init(twosided, q);
        kb.run_for(t);
        if (reduce_no_run_no_checks(kb, *omit)
            == reduce_no_run_no_checks(kb, *(omit + 1))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

    template <typename Word, typename RewritingSystem, typename ReductionOrder>
    void
    by_overlap_length(KnuthBendix<Word, RewritingSystem, ReductionOrder>& kb) {
      size_t prev_max_overlap = kb.max_overlap();
      kb.max_overlap(1);

      while (!kb.rewriting_system().confluent()) {
        kb.run();
        kb.max_overlap(kb.max_overlap() + 1);
      }
      kb.max_overlap(prev_max_overlap);
    }

    // TODO(1) deprecate and make this RewritingSystem mem fn
    template <typename RewritingSystem, typename ReductionOrder>
    bool
    is_reduced(detail::KnuthBendixImpl<RewritingSystem, ReductionOrder>& kb) {
      for (auto const& test_rule : kb.rewriting_system().rules()) {
        auto const lhs = test_rule.first;
        for (auto const& rule : kb.rewriting_system().rules()) {
          if (test_rule == rule) {
            continue;
          }

          if (rule.first.find(lhs) != std::string::npos
              || rule.second.find(lhs) != std::string::npos) {
            return false;
          }
        }
      }
      return true;
    }

    // template <typename T>
    // tril try_equal_to(Presentation<std::string>& p,
    //                          std::string const&         lhs,
    //                          std::string const&         rhs,
    //                          T                          t) {
    //   constexpr static congruence_kind twosided =
    //   congruence_kind::twosided;

    //   // TODO(1) throw_if_bad_alphabet_or_rules lhs and rhs
    //   KnuthBendixImpl         kb(twosided, p);
    //   std::string         lphbt = p.alphabet();
    //   std::vector<size_t> perm(lphbt.size(), 0);
    //   std::iota(perm.begin(), perm.end(), 0);

    //   do {
    //     detail::apply_permutation(lphbt, perm);

    //     p.alphabet(lphbt);
    //     p.throw_if_bad_alphabet_or_rules();

    //     kb.init(twosided, p);
    //     // TODO(1) no checks
    //     if (reduce_no_run(kb, lhs) == reduce_no_run(kb, rhs)) {
    //       return tril::TRUE;
    //     }
    //     kb.run_for(t);
    //     // TODO(1) no checks
    //     if (reduce_no_run(kb, lhs) == reduce_no_run(kb, rhs)) {
    //       return tril::TRUE;
    //     } else if (kb.finished()) {
    //       return tril::FALSE;
    //     }
    //   } while (std::next_permutation(perm.begin(), perm.end()));
    //   return tril::unknown;
    // }

    namespace detail {
      template <typename Iterator>
      auto subwords(Iterator first, Iterator last) {
        using Word = std::conditional_t<
            std::is_same_v<std::remove_const_t<typename Iterator::value_type>,
                           word_type>,
            word_type,
            std::string>;

        std::unordered_set<Word> mp;

        for (auto it = first; it < last; ++it) {
          auto const& w = *it;
          for (auto suffix = w.cbegin(); suffix < w.cend(); ++suffix) {
            for (auto prefix = suffix + 2; prefix < w.cend(); ++prefix) {
              mp.emplace(suffix, prefix);
            }
          }
        }
        std::vector<Word> words(mp.cbegin(), mp.cend());
        std::sort(words.begin(), words.end(), [](auto const& u, auto const& v) {
          return shortlex_compare(v, u);
        });
        return words;
      }

      static inline uint64_t factorial(uint16_t n) {
        LIBSEMIGROUPS_ASSERT(n <= 20);
        uint64_t result = 1;

        for (uint16_t i = 2; i <= n; ++i) {
          result *= i;
        }

        return result;
      }
    }  // namespace detail

    ////////////////////////////////////////////////////////////////////////
    // TietzeExplorer::TietzeRunner
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::TietzeRunner::run_impl() {
      std::vector<Word> subwords;
      auto              copy = _enclosing->_presentation;
      // Do try_pop_todo 2nd so that we don't pop and then stop
      while (!stopped() && _enclosing->try_pop_todo(subwords)) {
        if (run_one(copy, subwords)) {
          return;
        }
      }
    }

    template <typename Word, typename RewritingSystem>
    bool TietzeExplorer<Word, RewritingSystem>::TietzeRunner::run_one(
        Presentation<Word>       p,
        std::vector<Word> const& subwords) {
      using ::libsemigroups::detail::apply_permutation;

      for (auto const& word : subwords) {
        presentation::replace_word_with_new_generator(
            p, word.cbegin(), word.cend());
      }

      auto                lphbt      = p.alphabet();
      auto const          lphbt_orig = p.alphabet();
      std::vector<size_t> perm(lphbt.size(), 0);
      std::iota(perm.begin(), perm.end(), 0);

      do {
        ReportGuard rg(false);
        lphbt = lphbt_orig;
        apply_permutation(lphbt, perm);
        p.alphabet(lphbt);
        _kb.init(_kb.kind(), p);
        _kb.run_for(_enclosing->_run_each_for);
        if (_kb.rewriting_system().confluent()) {
          return true;
        }
        _enclosing->_counter++;
      } while (std::next_permutation(perm.begin(), perm.end()));
      return false;
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeExplorer - constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem>
    TietzeExplorer<Word, RewritingSystem>::TietzeExplorer(
        KnuthBendix<Word, RewritingSystem>& kb)
        : Runner(),
          // Mutable
          _counter(),
          _current_subwords_replaced_with_new_generators(),
          _kb(),
          _mtx(),
          _number_of_runs(),
          _presentation(),
          _todo(),
          _todo_populated(),

          // Non-mutable
          _depth_max(),
          _depth_min(),
          _finished(),
          _number_of_threads(),
          _race(),
          _run_each_for() {
      init(kb);
    }

    template <typename Word, typename RewritingSystem>
    TietzeExplorer<Word, RewritingSystem>&
    TietzeExplorer<Word, RewritingSystem>::init(
        KnuthBendix<Word, RewritingSystem>& kb) {
      Runner::init();
      // Mutable
      _counter = 0;
      // Do nothing to _current_subwords_replaced_with_new_generators, _mtx
      _kb             = kb;
      _presentation   = _kb.presentation();
      _number_of_runs = UNDEFINED;
      while (!_todo.empty()) {
        _todo.pop();
      }
      _todo_populated = false;

      // Non-mutable
      _depth_max         = 3;
      _depth_min         = 0;
      _finished          = false;
      _number_of_threads = 1;
      _race.init();
      _run_each_for = std::chrono::milliseconds(5);
      Runner::report_prefix("TietzeExplorer");
      _race.report_prefix("TietzeExplorer");
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeExplorer - settings
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem>
    TietzeExplorer<Word, RewritingSystem>&
    TietzeExplorer<Word, RewritingSystem>::number_of_threads(size_t val) {
      if (val == 0) {
        LIBSEMIGROUPS_EXCEPTION(
            "the argument (number of threads) must be at least 1, found {}",
            val);
      }
      _number_of_threads = val;
      return *this;
    }

    template <typename Word, typename RewritingSystem>
    size_t TietzeExplorer<Word, RewritingSystem>::number_of_runs() const {
      if (!_todo_populated) {
        LIBSEMIGROUPS_ASSERT(_number_of_runs == UNDEFINED);
        _number_of_runs = 0;
        populate_todo();
        LIBSEMIGROUPS_ASSERT(_number_of_runs != UNDEFINED);
      }
      return _number_of_runs;
    }

    template <typename Word, typename RewritingSystem>
    std::chrono::nanoseconds
    TietzeExplorer<Word, RewritingSystem>::estimated_run_time() const {
      return static_cast<std::chrono::nanoseconds::rep>(number_of_runs())
             * run_each_for() / number_of_threads();
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeExplorer - the main event
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::dfs(Presentation<Word>& p,
                                                    size_t depth) const {
      if (depth >= _depth_min) {
        _number_of_runs += detail::factorial(p.alphabet().size());
      }

      if (depth != _depth_max) {
        auto sbwrds = detail::subwords(p.rules.cbegin(), p.rules.cend());

        Presentation<Word> copy;
        for (auto const& w : sbwrds) {
          if (w.size() > 1) {
            copy = p;
            presentation::replace_word_with_new_generator(
                copy, w.cbegin(), w.cend());
            _current_subwords_replaced_with_new_generators.push_back(w);
            _todo.push(_current_subwords_replaced_with_new_generators);
            dfs(copy, depth + 1);
            _current_subwords_replaced_with_new_generators.pop_back();
          }
        }
      }
    }

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::run_impl() {
      using std::chrono::duration_cast;
      using std::chrono::seconds;

      // Note that if finished() is true, then this function is never called by
      // Runner::run.
      if (_presentation.alphabet().size() + depth_max() > 20) {
        LIBSEMIGROUPS_EXCEPTION("expected alphabet size + depth_max() to "
                                "be at most 20, found {}",
                                _kb.presentation().alphabet().size()
                                    + depth_max());
      }

      report_before_run();

      _counter = 0;
      populate_todo();

      while (_race.number_of_runners() < number_of_threads()) {
        _race.add_runner(std::make_shared<TietzeRunner>(this));
      }

      ::libsemigroups::detail::Ticker ticker;
      if ((!running_for()
           || duration_cast<seconds>(running_for_how_long()) >= seconds(1))) {
        ticker([this]() { report_progress_from_thread(); });
      }
      _race.run_until([this]() { return this->stopped(); });

      report_after_run();
      if (_race.finished() || !stopped()) {
        _finished = true;
      }
    }

    template <typename Word, typename RewritingSystem>
    std::optional<KnuthBendix<Word, RewritingSystem>>
    TietzeExplorer<Word, RewritingSystem>::result() {
      Runner::run();
      if (_race.winner() == nullptr) {
        return std::nullopt;
      }
      return std::make_optional(
          std::static_pointer_cast<TietzeRunner>(_race.winner())
              ->knuth_bendix());
    }

    ////////////////////////////////////////////////////////////////////////
    // TietzeExplorer - private
    ////////////////////////////////////////////////////////////////////////

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::populate_todo() const {
      if (!_todo_populated) {
        LIBSEMIGROUPS_ASSERT(_todo.empty());
        _todo_populated = true;
        if (depth_min() == 0) {
          _todo.emplace();  // no new generators
        }
        auto copy = _kb.presentation();
        dfs(copy);
      }
    }

    template <typename Word, typename RewritingSystem>
    bool TietzeExplorer<Word, RewritingSystem>::try_pop_todo(
        std::vector<Word>& result) {
      LIBSEMIGROUPS_ASSERT(_todo_populated);
      std::lock_guard<std::mutex> lg(_mtx);
      if (!_todo.empty()) {
        result = std::move(_todo.front());
        _todo.pop();
        return true;
      }
      return false;
    }

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::report_before_run() const {
      using ::libsemigroups::detail::group_digits;
      using ::libsemigroups::detail::string_time;
      reset_start_time();

      fmt::print("{:=<32}\n", "");
      fmt::print("#0: TietzeExplorer: DOING ≤ {} runs @ {} each / "
                 "{} thread(s) | ≤ ~{} (est. time)\n",
                 group_digits(number_of_runs()),
                 string_time(_run_each_for),
                 number_of_threads(),
                 string_time(estimated_run_time()));
      fmt::print("#0: TietzeExplorer: depth min = {}, max = {}\n",
                 _depth_min,
                 _depth_max);
    }

    template <typename Word, typename RewritingSystem>
    void
    TietzeExplorer<Word, RewritingSystem>::report_progress_from_thread() const {
      using ::libsemigroups::detail::group_digits;
      using ::libsemigroups::detail::string_time;
      if (delta(start_time()) >= std::chrono::milliseconds(500)) {
        size_t count         = _counter.load();
        auto   num_runs      = group_digits(number_of_runs());
        auto   elapsed       = delta(start_time());
        auto   mean_run_time = elapsed / count;
        auto   estimate      = number_of_runs() * mean_run_time;
        fmt::print("#0: TietzeExplorer: {:>{}} / {} ({:>4.1f}%) @ ~{} "
                   "per run | {:>7} / {:>7}\n",
                   group_digits(count),
                   num_runs.size(),
                   num_runs,
                   static_cast<float>(100 * count) / number_of_runs(),
                   string_time(mean_run_time),
                   string_time(elapsed),
                   fmt::format("~{}", string_time(estimate)));
      }
    }

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::report_after_run() const {
      using ::libsemigroups::detail::group_digits;
      using ::libsemigroups::detail::string_time;
      std::string_view const success_or_fail
          = (_race.winner_index() != UNDEFINED) ? "SUCCESS" : "FAILED";
      auto count = _counter.load();
      fmt::print("#0: TietzeExplorer: {} @ run {} of {} | "
                 "elapsed {}\n",
                 success_or_fail,
                 group_digits(count),
                 group_digits(number_of_runs()),
                 string_time(delta(start_time())));
    }

    template <typename Word, typename RewritingSystem>
    void TietzeExplorer<Word, RewritingSystem>::throw_if_todo_populated(
        std::string_view msg) const {
      if (_todo_populated) {
        LIBSEMIGROUPS_EXCEPTION("it is not possible to set `{}` at this point, "
                                "please use `init`, and try again",
                                msg);
      }
    }

  }  // namespace knuth_bendix
}  // namespace libsemigroups
