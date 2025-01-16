//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019 James D. Mitchell
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

// This file contains stuff for creating congruence over FroidurePinBase
// objects or over Presentation objects.

#include "libsemigroups/cong-class.hpp"

#include <algorithm>      // for max, find_if
#include <cctype>         // for isprint
#include <iterator>       // for begin, end, distance
#include <list>           // for operator!=
#include <memory>         // for static_pointer_cast
#include <tuple>          // for tie
#include <unordered_map>  // for operator==

#include "libsemigroups/cong-intf-class.hpp"     // for CongruenceInterface
#include "libsemigroups/constants.hpp"           // for operator==, operator!=
#include "libsemigroups/debug.hpp"               // for LIBSEMIGROUPS_ASSERT
#include "libsemigroups/exception.hpp"           // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/kambites-class.hpp"      // for Kambites
#include "libsemigroups/knuth-bendix-base.hpp"   // for KnuthBendixBase
#include "libsemigroups/paths.hpp"               // for number_of_paths
#include "libsemigroups/presentation.hpp"        // for add_rule, longest_ru...
#include "libsemigroups/ranges.hpp"              // for operator|, enumerate
#include "libsemigroups/to-presentation.hpp"     // for to_presentation
#include "libsemigroups/todd-coxeter-class.hpp"  // for ToddCoxeter<word_type>
#include "libsemigroups/types.hpp"               // for word_type, congruenc...
#include "libsemigroups/ukkonen.hpp"             // for number_of_pieces_no_...
#include "libsemigroups/word-graph.hpp"          // for WordGraph::cbegin_ta...

#include "libsemigroups/detail/fmt.hpp"       // for format, print
#include "libsemigroups/detail/iterator.hpp"  // for operator+
#include "libsemigroups/detail/race.hpp"      // for Race
#include "libsemigroups/detail/report.hpp"    // for report_default, repo...
#include "libsemigroups/detail/string.hpp"    // for group_digits, signed...
#include "libsemigroups/detail/timer.hpp"     // for string_time

namespace libsemigroups {

  //////////////////////////////////////////////////////////////////////////
  // Congruence - constructors - public
  //////////////////////////////////////////////////////////////////////////

  Congruence& Congruence::init() {
    CongruenceInterface::init();
    _race.init();
    _runners_initted = false;
    _runner_kinds.clear();
    return *this;
  }

  Congruence& Congruence::init(congruence_kind                type,
                               Presentation<word_type> const& p) {
    CongruenceInterface::init(type);
    init();
    _race.max_threads(POSITIVE_INFINITY);
    if (type == congruence_kind::twosided) {
      add_runner(std::make_shared<Kambites<word_type>>(type, p));
    }
    add_runner(std::make_shared<KnuthBendix<word_type>>(type, p));

    add_runner(std::make_shared<ToddCoxeter<word_type>>(type, p));
    auto tc = std::make_shared<ToddCoxeter<word_type>>(type, p);
    tc->strategy(ToddCoxeter<word_type>::options::strategy::felsch);
    add_runner(std::move(tc));
    return *this;
  }

  uint64_t Congruence::number_of_classes() {
    run();
    auto winner_kind = _runner_kinds[_race.winner_index()];
    if (winner_kind == RunnerKind::TC) {
      return std::static_pointer_cast<ToddCoxeter<word_type>>(_race.winner())
          ->number_of_classes();
    } else if (winner_kind == RunnerKind::KB) {
      return std::static_pointer_cast<KnuthBendix<word_type>>(_race.winner())
          ->number_of_classes();
    } else {
      LIBSEMIGROUPS_ASSERT(winner_kind == RunnerKind::K);
      return std::static_pointer_cast<Kambites<word_type>>(_race.winner())
          ->number_of_classes();
    }
  }

  void Congruence::init_runners() const {
    if (!_runners_initted) {
      _runners_initted = true;
      for (auto const& [i, runner] : rx::enumerate(_race)) {
        auto first = generating_pairs().cbegin();
        auto last  = generating_pairs().cend();
        // We have to call the specific add_generating_pair_no_checks for
        // each type of runner so that the generating pairs are correctly
        // modified to match the native letters in the alphabet
        if (_runner_kinds[i] == RunnerKind::TC) {
          auto tc = std::static_pointer_cast<ToddCoxeter<word_type>>(runner);
          size_t const n = tc->number_of_generating_pairs();
          // Only add the generating pairs not already in the runners
          for (auto it = first + n; it != last; it += 2) {
            tc->add_generating_pair_no_checks(std::begin(*it),
                                              std::end(*it),
                                              std::begin(*(it + 1)),
                                              std::end(*(it + 1)));
          }
        } else if (_runner_kinds[i] == RunnerKind::KB) {
          auto kb = std::static_pointer_cast<KnuthBendix<word_type>>(runner);
          size_t const n = kb->number_of_generating_pairs();
          // Only add the generating pairs not already in the runners
          for (auto it = first + n; it != last; it += 2) {
            kb->add_generating_pair_no_checks(std::begin(*it),
                                              std::end(*it),
                                              std::begin(*(it + 1)),
                                              std::end(*(it + 1)));
          }
        } else {
          LIBSEMIGROUPS_ASSERT(_runner_kinds[i] == RunnerKind::K);
          auto k = std::static_pointer_cast<Kambites<word_type>>(runner);
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

  void Congruence::run_impl() {
    init_runners();
    _race.run_until([this]() { return this->stopped(); });
  }

  Congruence::native_presentation_type const& Congruence::presentation() const {
    if (!_race.empty()) {
      if (finished()) {
        size_t index = _race.winner_index();

        if (_runner_kinds[index] == RunnerKind::TC) {
          return std::static_pointer_cast<ToddCoxeter<word_type>>(
                     _race.winner())
              ->presentation();
        } else if (_runner_kinds[index] == RunnerKind::K) {
          return std::static_pointer_cast<Kambites<word_type>>(_race.winner())
              ->presentation();
        }
        // KnuthBendixBase uses a std::string Presentation and this can't be
        // returned by reference here. There are probably better ways of
        // handling this:
        // 1. store a copy in the congruence object itself
        // 2. return by value
        // 3. Something else?
      } else {
        auto it = std::find_if(
            _runner_kinds.begin(), _runner_kinds.end(), [](auto const& val) {
              return val != RunnerKind::KB;
            });
        if (it != _runner_kinds.end()) {
          size_t index = std::distance(_runner_kinds.begin(), it);
          if (_runner_kinds[index] == RunnerKind::TC) {
            return std::static_pointer_cast<ToddCoxeter<word_type>>(
                       *(_race.begin() + index))
                ->presentation();
          } else if (_runner_kinds[index] == RunnerKind::K) {
            return std::static_pointer_cast<Kambites<word_type>>(
                       *(_race.begin() + index))
                ->presentation();
          }
        }
      }
    }
    LIBSEMIGROUPS_EXCEPTION(
        "No presentation has been set, or it cannot be returned!");
  }

  std::string to_human_readable_repr(Congruence const& c) {
    std::string p;
    try {
      p = " over " + to_human_readable_repr(c.presentation());
    } catch (...) {
    }

    return fmt::format("<Congruence{} with {} runner{}>",
                       p,
                       c.number_of_runners(),
                       c.number_of_runners() > 1 ? "s" : "");
  }
}  // namespace libsemigroups
