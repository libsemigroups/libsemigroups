//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 Joseph Edwards + James D. Mitchell
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

#include "libsemigroups/detail/rewriters.hpp"

#include <algorithm>
#include <atomic>
#include <chrono>

#include "libsemigroups/runner.hpp"  // for Ticker

#include "libsemigroups/detail/report.hpp"       // for report_default
#include "libsemigroups/detail/value-guard.hpp"  // for ValueGuard

namespace libsemigroups {
  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // RuleLookup
    ////////////////////////////////////////////////////////////////////////

    // Reverse lex order
    bool RuleLookup::operator<(RuleLookup const& that) const {
      auto it_this = _last - 1;
      auto it_that = that._last - 1;
      while (it_this > _first && it_that > that._first
             && *it_this == *it_that) {
        --it_that;
        --it_this;
      }
      return *it_this < *it_that;
    }

    ////////////////////////////////////////////////////////////////////////
    // RewritingSystemBase - constructors + initializers
    ////////////////////////////////////////////////////////////////////////

    RewritingSystemBase::RewritingSystemBase()
        : _cached_confluent(false),
          _confluence_known(false),
          _pending_rules_comparator(lhs_lex_cmp),
          _state(State::none),
          _ticker_running(false) {}

    RewritingSystemBase& RewritingSystemBase::init() {
      Rules::init();
      _cached_confluent         = false;
      _confluence_known         = false;
      _pending_rules_comparator = lhs_lex_cmp;
      _state                    = State::none;
      _ticker_running           = false;
      return *this;
    }

    RewritingSystemBase&
    RewritingSystemBase::operator=(RewritingSystemBase const& that) {
      Rules::operator=(that);
      _cached_confluent         = that._cached_confluent.load();
      _confluence_known         = that._confluence_known.load();
      _pending_rules_comparator = that._pending_rules_comparator;
      _state                    = that._state;
      _ticker_running           = that._ticker_running;

      return *this;
    }

    RewritingSystemBase&
    RewritingSystemBase::operator=(RewritingSystemBase&& that) {
      Rules::operator=(std::move(that));
      _cached_confluent         = that._cached_confluent.load();
      _confluence_known         = that._confluence_known.load();
      _pending_rules_comparator = std::move(that._pending_rules_comparator);
      _state                    = that._state;
      _ticker_running           = std::move(that._ticker_running);
      return *this;
    }

    RewritingSystemBase::~RewritingSystemBase() = default;

    ////////////////////////////////////////////////////////////////////////
    // RewritingSystemBase - public mem fns
    ////////////////////////////////////////////////////////////////////////

    void RewritingSystemBase::set_cached_confluent(tril val) const {
      if (val == tril::TRUE) {
        _confluence_known = true;
        _cached_confluent = true;
      } else if (val == tril::FALSE) {
        _confluence_known = true;
        _cached_confluent = false;
      } else {
        _confluence_known = false;
      }
    }

    bool RewritingSystemBase::confluent() {
      using std::chrono::high_resolution_clock;
      using std::chrono::time_point;

      if (confluent_known()) {
        return RewritingSystemBase::cached_confluent();
      }

      std::atomic_uint64_t seen = 0;
      if (reporting_enabled() && !_ticker_running) {
        ValueGuard sg(_state);
        _state = State::checking_confluence;
        ValueGuard tg(_ticker_running);
        _ticker_running           = true;
        time_point     start_time = high_resolution_clock::now();
        detail::Ticker ticker(
            [&]() { report_progress_from_thread(seen, start_time); });
        return confluent_impl(seen);
      } else {
        return confluent_impl(seen);
      }
    }

    void RewritingSystemBase::report_progress_from_thread(
        std::atomic_uint64_t const&                           seen,
        std::chrono::high_resolution_clock::time_point const& start_time) {
      if (_state == State::none) {
        using detail::string_time;
        auto gd       = detail::group_digits;
        auto active   = gd(Rules::active_rules().size());
        auto inactive = gd(Rules::number_of_inactive_rules());
        auto pending  = gd(Rules::pending_rules().size());
        auto defined  = gd(Rules::stats().total_rules);

        report_default("KnuthBendix: rules {} (active) | {} (inactive) | {} "
                       "(pending) | {} "
                       "(defined) | {}\n",
                       active,
                       inactive,
                       pending,
                       defined,
                       string_time(delta(start_time)));
      } else if (_state == State::checking_confluence) {
        report_checking_confluence(seen, start_time);
      } else {
        LIBSEMIGROUPS_ASSERT(_state == State::reducing_pending_rules);
        report_reducing_rules(seen, start_time);
      }
    }

  }  // namespace detail
}  // namespace libsemigroups
