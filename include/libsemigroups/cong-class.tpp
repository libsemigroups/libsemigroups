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

  template <typename Node>
  Congruence& Congruence::init(congruence_kind        knd,
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
    // the Cayley graph and use that in the ToddCoxeter<word_type>, at present
    // that'll happen here in the constructor, same for the creation of the
    // presentation this could take place in the Runner so that they are done
    // in parallel
    add_runner(
        std::make_shared<ToddCoxeter<word_type>>(to_todd_coxeter(knd, S, wg)));

    // FIXME uncommenting the following lines causes multiple issues in the
    // extreme Congruence test
    // auto tc = to_todd_coxeter(knd, S, wg);
    // tc.strategy(ToddCoxeter<word_type>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<word_type>>(std::move(tc)));

    // We can no longer add the following runner if wg is the left_cayley_graph
    // of S, then the below will compute the corresponding right congruence,
    // which is unavoidable. TODO(1) reconsider this?
    // auto tc = ToddCoxeter<word_type>(knd, to_presentation<word_type>(S));
    // add_runner(std::make_shared<ToddCoxeter<word_type>>(std::move(tc)));

    // tc = ToddCoxeter<word_type>(knd, to_presentation<word_type>(S));
    // tc.strategy(ToddCoxeter<word_type>::options::strategy::felsch);
    // add_runner(std::make_shared<ToddCoxeter<word_type>>(std::move(tc)));

    return *this;
  }

  template <typename Iterator1,
            typename Iterator2,
            typename Iterator3,
            typename Iterator4>
  tril Congruence::currently_contains_no_checks(Iterator1 first1,
                                                Iterator2 last1,
                                                Iterator3 first2,
                                                Iterator4 last2) const {
    if (std::equal(first1, last1, first2, last2)) {
      return tril::TRUE;
    } else if (finished()) {
      auto winner_kind = _runner_kinds[_race.winner_index()];
      if (winner_kind == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<word_type>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      } else if (winner_kind == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendixBase<>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      } else {
        LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
        return std::static_pointer_cast<Kambites<word_type>>(_race.winner())
            ->currently_contains_no_checks(first1, last1, first2, last2);
      }
    }
    init_runners();
    tril result = tril::unknown;
    for (auto const& [i, runner] : rx::enumerate(_race)) {
      if (_runner_kinds[i] == RunnerKind::TC) {
        result
            = std::static_pointer_cast<ToddCoxeter<word_type>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      } else if (_runner_kinds[i] == RunnerKind::KB) {
        result
            = std::static_pointer_cast<KnuthBendixBase<>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      } else {
        LIBSEMIGROUPS_ASSERT(_runner_kinds[i] == RunnerKind::K);
        result
            = std::static_pointer_cast<Kambites<word_type>>(runner)
                  ->currently_contains_no_checks(first1, last1, first2, last2);
      }
      if (result != tril::unknown) {
        break;
      }
    }
    return result;
  }

  template <typename OutputIterator, typename Iterator1, typename Iterator2>
  OutputIterator Congruence::reduce_no_run_no_checks(OutputIterator d_first,
                                                     Iterator1      first,
                                                     Iterator2 last) const {
    // If we're finished then there's no longer a 1-1 correspondence between
    // entries in _runner_kinds and the Runners in _race, and so we need this
    // special case.
    if (finished()) {
      auto winner_kind = _runner_kinds[_race.winner_index()];
      if (winner_kind == RunnerKind::TC) {
        return std::static_pointer_cast<ToddCoxeter<word_type>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      } else if (winner_kind == RunnerKind::KB) {
        return std::static_pointer_cast<KnuthBendixBase<>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      } else {
        LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
        return std::static_pointer_cast<Kambites<word_type>>(_race.winner())
            ->reduce_no_run_no_checks(d_first, first, last);
      }
    }
    init_runners();
    // TODO(1) the following could be more nuanced.
    if (_runner_kinds[0] == RunnerKind::TC) {
      return std::static_pointer_cast<ToddCoxeter<word_type>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    } else if (_runner_kinds[0] == RunnerKind::KB) {
      return std::static_pointer_cast<KnuthBendixBase<>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    } else {
      LIBSEMIGROUPS_ASSERT(_runner_kinds[0] == RunnerKind::K);
      return std::static_pointer_cast<Kambites<word_type>>(*_race.begin())
          ->reduce_no_run_no_checks(d_first, first, last);
    }
  }

  template <typename Iterator1, typename Iterator2>
  void Congruence::throw_if_letter_out_of_bounds(Iterator1 first,
                                                 Iterator2 last) const {
    if (!_race.empty()) {
      size_t index = (finished() ? _race.winner_index() : 0);

      if (_runner_kinds[index] == RunnerKind::TC) {
        std::static_pointer_cast<ToddCoxeter<word_type>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      } else if (_runner_kinds[index] == RunnerKind::KB) {
        std::static_pointer_cast<KnuthBendixBase<>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      } else {
        LIBSEMIGROUPS_ASSERT(_runner_kinds[index] == RunnerKind::K);
        std::static_pointer_cast<Kambites<word_type>>(*_race.begin())
            ->throw_if_letter_out_of_bounds(first, last);
      }
      return;
    }
    LIBSEMIGROUPS_EXCEPTION(
        "No presentation has been set, so cannot validate the word!");
  }

  template <typename Thing>
  std::shared_ptr<Thing> Congruence::get() const {
    init_runners();
    RunnerKind  val;
    std::string name;
    if constexpr (std::is_same_v<Thing, Kambites<word_type>>) {
      val  = RunnerKind::K;
      name = "Kambites";
    } else if constexpr (std::is_same_v<Thing, KnuthBendixBase<>>) {
      val  = RunnerKind::KB;
      name = "KnuthBendixBase";
    } else if constexpr (std::is_same_v<Thing, ToddCoxeter<word_type>>) {
      val  = RunnerKind::TC;
      name = "ToddCoxeter<word_type>";
    } else {
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

  template <typename Thing>
  bool Congruence::has() const {
    init_runners();
    RunnerKind val;
    if constexpr (std::is_same_v<Thing, Kambites<word_type>>) {
      val = RunnerKind::K;
    } else if constexpr (std::is_same_v<Thing, KnuthBendixBase<>>) {
      val = RunnerKind::KB;
    } else if constexpr (std::is_same_v<Thing, ToddCoxeter<word_type>>) {
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

}  // namespace libsemigroups
