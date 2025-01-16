//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024 James D. Mitchell
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

namespace libsemigroups {
  ////////////////////////////////////////////////////////////////////////
  // Congruence - out of line implementations
  ////////////////////////////////////////////////////////////////////////

  template <typename Word>
  Congruence<Word>& Congruence<Word>::init() {
    CongruenceInterface::init();
    _race.init();
    _runners_initted = false;
    _runner_kinds.clear();
    return *this;
  }

  template <typename Word>
  Congruence<Word>& Congruence<Word>::init(congruence_kind           type,
                                           Presentation<Word> const& p) {
    CongruenceInterface::init(type);
    init();
    _race.max_threads(POSITIVE_INFINITY);
    if (type == congruence_kind::twosided) {
      add_runner(std::make_shared<Kambites<Word>>(type, p));
    }
    add_runner(std::make_shared<KnuthBendix<Word>>(type, p));

    add_runner(std::make_shared<ToddCoxeter<Word>>(type, p));
    auto tc = std::make_shared<ToddCoxeter<Word>>(type, p);
    tc->strategy(ToddCoxeter<Word>::options::strategy::felsch);
    add_runner(std::move(tc));
    return *this;
  }

  template <typename Word>
  template <typename Node>
  Congruence<Word>& Congruence<Word>::init(congruence_kind        knd,
                                           FroidurePinBase&       S,
                                           WordGraph<Node> const& wg) {
    if (S.is_finite() != tril::FALSE) {
      S.run();
    } else {
      LIBSEMIGROUPS_EXCEPTION(
          "the 2nd argument does not represent a finite semigroup!");
    }
    CongruenceInterface::init(knd);

    // TODO(later) if necessary make a runner that tries to S.run(), then get
    // the Cayley graph and use that in the ToddCoxeter, at present
    // that'll happen here in the constructor, same for the creation of the
    // presentation this could take place in the Runner so that they are done
    // in parallel
    add_runner(
        std::make_shared<ToddCoxeter<Word>>(to_todd_coxeter(knd, S, wg)));

    // FIXME uncommenting the following lines causes multiple issues in the
    // extreme Congruence test
    // auto tc = to_todd_coxeter(knd, S, wg);
    // tc.strategy(ToddCoxeter<Word>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    // We can no longer add the following runner if wg is the left_cayley_graph
    // of S, then the below will compute the corresponding right congruence,
    // which is unavoidable. TODO(1) reconsider this?
    // auto tc = ToddCoxeter<Word>(knd, to_presentation<Word>(S));
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    // tc = ToddCoxeter<Word>(knd, to_presentation<Word>(S));
    // tc.strategy(ToddCoxeter<Word>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<Word>>(std::move(tc)));

    return *this;
  }

  template <typename Word>
  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  tril Congruence<Word>::currently_contains_no_checks(Iterator1 first1,
                                                      Iterator2 last1,
                                                      Iterator3 first2,
                                                      Iterator4 last2) const {
    if (std::equal(first1, last1, first2, last2)) {
      return tril::TRUE;
    } else if (finished()) {
      auto winner_kind = _runner_kinds[_race.winner_index()];
      if (winner_kind == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<Word>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      } else if (winner_kind == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendix<Word>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      } else {
        LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
        return std::static_pointer_cast<Kambites<Word>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      }
    }
    init_runners();
    tril result = tril::unknown;
    for (auto const& [i, runner] : rx::enumerate(_race)) {
      if (_runner_kinds[i] == RunnerKind::TC) {
        result
            = std::static_pointer_cast<ToddCoxeter<Word>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      } else if (_runner_kinds[i] == RunnerKind::KB) {
        result
            = std::static_pointer_cast<KnuthBendix<Word>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      } else {
        LIBSEMIGROUPS_ASSERT(_runner_kinds[i] == RunnerKind::K);
        result
            = std::static_pointer_cast<Kambites<Word>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      }
      if (result != tril::unknown) {
        break;
      }
    }
    return result;
  }

  template <typename Word>
  template <typename OutputIterator, typename Iterator1, typename Iterator2>
  OutputIterator
  Congruence<Word>::reduce_no_run_no_checks(OutputIterator d_first,
                                            Iterator1      first,
                                            Iterator2      last) const {
    // If we're finished then there's no longer a 1-1 correspondence between
    // entries in _runner_kinds and the Runners in _race, and so we need this
    // special case.
    if (finished()) {
      auto winner_kind = _runner_kinds[_race.winner_index()];
      if (winner_kind == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<Word>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      } else if (winner_kind == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendix<Word>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      } else {
        LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
        return std::static_pointer_cast<Kambites<Word>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      }
    }
    init_runners();
    // TODO(1) the following could be more nuanced.
    if (_runner_kinds[0] == RunnerKind::TC) {
      return std::static_pointer_cast<ToddCoxeter<Word>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    } else if (_runner_kinds[0] == RunnerKind::KB) {
      return std::static_pointer_cast<KnuthBendix<Word>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    } else {
      LIBSEMIGROUPS_ASSERT(_runner_kinds[0] == RunnerKind::K);
      return std::static_pointer_cast<Kambites<Word>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    }
  }

  template <typename Word>
  template <typename Iterator1, typename Iterator2>
  void Congruence<Word>::throw_if_letter_out_of_bounds(Iterator1 first,
                                                       Iterator2 last) const {
    if (!_race.empty()) {
      size_t index = (finished() ? _race.winner_index() : 0);

      if (_runner_kinds[index] == RunnerKind::TC) {
        std::static_pointer_cast<ToddCoxeter<Word>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      } else if (_runner_kinds[index] == RunnerKind::KB) {
        std::static_pointer_cast<KnuthBendix<Word>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      } else {
        LIBSEMIGROUPS_ASSERT(_runner_kinds[index] == RunnerKind::K);
        std::static_pointer_cast<Kambites<Word>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      }
      return;
    }
    LIBSEMIGROUPS_EXCEPTION(
        "No presentation has been set, so cannot validate the word!");
  }

  template <typename Word>
  template <typename Thing>
  std::shared_ptr<Thing> Congruence<Word>::get() const {
    init_runners();
    RunnerKind  val;
    std::string name;
    if constexpr (std::is_same_v<Thing, Kambites<Word>>) {
      val  = RunnerKind::K;
      name = "Kambites";
    } else if constexpr (std::is_same_v<Thing, KnuthBendix<Word>>) {
      val  = RunnerKind::KB;
      name = "KnuthBendix";
    } else if constexpr (std::is_same_v<Thing, ToddCoxeter<Word>>) {
      val  = RunnerKind::TC;
      name = "ToddCoxeter";
    } else {
      // TODO(0) throw
      LIBSEMIGROUPS_ASSERT(false);
      val = RunnerKind::TC;  // To prevent compiler warnings
    }

    if (finished()) {
      if (_runner_kinds[_race.winner_index()] == val) {
        return std::static_pointer_cast<Thing>(_race.winner());
      } else {
        LIBSEMIGROUPS_EXCEPTION("There's no {} in the congruence", name)
      }
    }
    auto it = std::find(_runner_kinds.begin(), _runner_kinds.end(), val);
    if (it == _runner_kinds.end()) {
      LIBSEMIGROUPS_EXCEPTION("There's no {} in the congruence", name)
    }
    return std::static_pointer_cast<Thing>(
        *(_race.begin() + std::distance(_runner_kinds.begin(), it)));
  }

  template <typename Word>
  template <typename Thing>
  bool Congruence<Word>::has() const {
    init_runners();
    RunnerKind val;
    if constexpr (std::is_same_v<Thing, Kambites<Word>>) {
      val = RunnerKind::K;
    } else if constexpr (std::is_same_v<Thing, KnuthBendix<Word>>) {
      val = RunnerKind::KB;
    } else if constexpr (std::is_same_v<Thing, ToddCoxeter<Word>>) {
      val = RunnerKind::TC;
    } else {
      LIBSEMIGROUPS_ASSERT(false);
      val = RunnerKind::TC;  // To prevent compiler warnings
    }

    if (finished()) {
      return _runner_kinds[_race.winner_index()] == val;
    }

    return std::find(_runner_kinds.begin(), _runner_kinds.end(), val)
           != _runner_kinds.end();
    // For some reason the following line works in C++ but not in the
    // python bindings
    // return _race.find_runner<Thing>() != nullptr;
  }

  template <typename Word>
  uint64_t Congruence<Word>::number_of_classes() {
    run();
    auto winner_kind = _runner_kinds[_race.winner_index()];
    if (winner_kind == RunnerKind::TC) {
      return std::static_pointer_cast<ToddCoxeter<Word>>(_race.winner())
          ->number_of_classes();
    } else if (winner_kind == RunnerKind::KB) {
      return std::static_pointer_cast<KnuthBendix<Word>>(_race.winner())
          ->number_of_classes();
    } else {
      LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
      return std::static_pointer_cast<Kambites<Word>>(_race.winner())
          ->number_of_classes();
    }
  }

  template <typename Word>
  void Congruence<Word>::init_runners() const {
    if (!_runners_initted) {
      _runners_initted = true;
      for (auto const& [i, runner] : rx::enumerate(_race)) {
        auto first = internal_generating_pairs().cbegin();
        auto last  = internal_generating_pairs().cend();
        // We have to call the specific add_generating_pair_no_checks for
        // each type of runner so that the generating pairs are correctly
        // modified to match the native letters in the alphabet
        if (_runner_kinds[i] == RunnerKind::TC) {
          auto         tc = std::static_pointer_cast<ToddCoxeter<Word>>(runner);
          size_t const n  = tc->number_of_generating_pairs();
          // Only add the generating pairs not already in the runners
          for (auto it = first + n; it != last; it += 2) {
            tc->add_generating_pair_no_checks(std::begin(*it),
                                              std::end(*it),
                                              std::begin(*(it + 1)),
                                              std::end(*(it + 1)));
          }
        } else if (_runner_kinds[i] == RunnerKind::KB) {
          auto         kb = std::static_pointer_cast<KnuthBendix<Word>>(runner);
          size_t const n  = kb->number_of_generating_pairs();
          // Only add the generating pairs not already in the runners
          for (auto it = first + n; it != last; it += 2) {
            kb->add_generating_pair_no_checks(std::begin(*it),
                                              std::end(*it),
                                              std::begin(*(it + 1)),
                                              std::end(*(it + 1)));
          }
        } else {
          LIBSEMIGROUPS_ASSERT(_runner_kinds[i] == RunnerKind::K);
          auto         k = std::static_pointer_cast<Kambites<Word>>(runner);
          size_t const n = k->number_of_generating_pairs();
          for (auto it = first + n; it != last; it += 2) {
            k->add_generating_pair_no_checks(std::begin(*it),
                                             std::end(*it),
                                             std::begin(*(it + 1)),
                                             std::end(*(it + 1)));
          }
        }
      }
    }
  }

  template <typename Word>
  void Congruence<Word>::run_impl() {
    init_runners();
    _race.run_until([this]() { return this->stopped(); });
  }

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type"
#endif
  template <typename Word>
  Presentation<Word> const& Congruence<Word>::presentation() const {
    if (_race.empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "No presentation has been set, and it cannot be returned!");
    }
    if (finished()) {
      size_t index = _race.winner_index();
      if (_runner_kinds[index] == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<Word>>(_race.winner())
            ->presentation();
      } else if (_runner_kinds[index] == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendix<Word>>(_race.winner())
            ->presentation();
      } else if (_runner_kinds[index] == RunnerKind::K) {
        return std::static_pointer_cast<Kambites<Word>>(_race.winner())
            ->presentation();
      }
    } else {
      if (_runner_kinds[0] == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<Word>>(*_race.begin())
            ->presentation();
      } else if (_runner_kinds[0] == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendix<Word>>(*_race.begin())
            ->presentation();
      } else if (_runner_kinds[0] == RunnerKind::K) {
        return std::static_pointer_cast<Kambites<Word>>(*_race.begin())
            ->presentation();
      }
    }
  }

  template <typename Word>
  std::vector<Word> const& Congruence<Word>::generating_pairs() const {
    if (_race.empty()) {
      LIBSEMIGROUPS_EXCEPTION("No generating pairs have been defined, and they "
                              "cannot be returned!");
    }
    init_runners();
    if (finished()) {
      size_t index = _race.winner_index();
      if (_runner_kinds[index] == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<Word>>(_race.winner())
            ->generating_pairs();
      } else if (_runner_kinds[index] == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendix<Word>>(_race.winner())
            ->generating_pairs();
      } else if (_runner_kinds[index] == RunnerKind::K) {
        return std::static_pointer_cast<Kambites<Word>>(_race.winner())
            ->generating_pairs();
      }
    }
    if (_runner_kinds[0] == RunnerKind::TC) {
      return std::static_pointer_cast<ToddCoxeter<Word>>(*_race.begin())
          ->generating_pairs();
    } else if (_runner_kinds[0] == RunnerKind::KB) {
      return std::static_pointer_cast<KnuthBendix<Word>>(*_race.begin())
          ->generating_pairs();
    } else if (_runner_kinds[0] == RunnerKind::K) {
      return std::static_pointer_cast<Kambites<Word>>(*_race.begin())
          ->generating_pairs();
    }
  }
#if defined(__clang__)
#pragma clang diagnostic pop
#endif

  template <typename Word>
  std::string to_human_readable_repr(Congruence<Word> const& c) {
    std::string p;
    try {
      // TODO(0) (do this better)
      p = " over " + to_human_readable_repr(c.presentation());
    } catch (...) {
    }

    return fmt::format("<Congruence{} with {} runner{}>",
                       p,
                       c.number_of_runners(),
                       c.number_of_runners() > 1 ? "s" : "");
  }

  // This function is declared in obv-inf.hpp
  template <typename Word>
  bool is_obviously_infinite(Congruence<Word>& c) {
    if (c.template has<ToddCoxeter<Word>>()
        && is_obviously_infinite(*c.template get<ToddCoxeter<Word>>())) {
      return true;
    } else if (c.template has<KnuthBendix<Word>>()
               && is_obviously_infinite(*c.template get<KnuthBendix<Word>>())) {
      return true;
    } else if (c.template has<Kambites<Word>>()
               && is_obviously_infinite(*c.template get<Kambites<Word>>())) {
      return true;
    }
    return false;
  }

}  // namespace libsemigroups
