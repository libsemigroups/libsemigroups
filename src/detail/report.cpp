//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2023 James D. Mitchell
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

#include "libsemigroups/detail/report.hpp"

#include <unordered_set>  // for unordered_set

#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {
  namespace {
    class Reporter {
     public:
      explicit Reporter(bool report = true);

      Reporter(Reporter const&)            = delete;
      Reporter(Reporter&&)                 = delete;
      Reporter& operator=(Reporter const&) = delete;
      Reporter& operator=(Reporter&&)      = delete;

      void report(bool val) {
        _report = val;
      }

      bool report() const noexcept {
        return _report;
      }

      std::atomic<bool> _report;
    };

    Reporter::Reporter(bool report) : _report(report) {}
  }  // namespace

  Reporter                REPORTER;
  detail::ThreadIdManager THREAD_ID_MANAGER;

  ReportGuard::ReportGuard(bool val) {
    REPORTER.report(val);
  }

  ReportGuard::~ReportGuard() {
    REPORTER.report(false);
  }

  namespace detail {

    ThreadIdManager::ThreadIdManager() : _mtx(), _next_tid(0), _thread_map() {
      tid(std::this_thread::get_id());
    }

    void ThreadIdManager::reset() {
      // Only do this from the main thread
      LIBSEMIGROUPS_ASSERT(tid(std::this_thread::get_id()) == 0);
      // Delete all thread_ids
      _thread_map.clear();
      _next_tid = 0;
      // Reinsert the main thread's id
      tid(std::this_thread::get_id());
    }

    size_t ThreadIdManager::tid(std::thread::id t) {
      std::lock_guard<std::mutex> lg(_mtx);
      auto                        it = _thread_map.find(t);
      if (it != _thread_map.end()) {
        return (*it).second;
      } else {
        // Don't check the assert below because on a single thread machine
        // (such as those used by appveyor), for an fp-semigroup more than 1
        // thread will be used, and this assertion will fail.
        // LIBSEMIGROUPS_ASSERT(_next_tid <=
        // std::thread::hardware_concurrency());
        _thread_map.emplace(t, _next_tid++);
        return _next_tid - 1;
      }
    }

    bool string_time_incremental(std::string&              result,
                                 std::chrono::nanoseconds& elapsed,
                                 bool                      use_float) {
      using seconds = std::chrono::seconds;
      seconds x     = std::chrono::duration_cast<seconds>(elapsed);
      if (x.count() > 0) {
        if (use_float) {
          double x_float = static_cast<double>(elapsed.count()) / 1'000'000'000;
          result += fmt::format("{:.3f}s", x_float);
        } else {
          result += fmt::format("{}", x);
        }
        elapsed -= std::chrono::nanoseconds(x);
        return true;
      }
      return false;
    }
  }  // namespace detail

  namespace report {
    namespace {
      std::unordered_set<std::string_view>& suppressions() {
        static std::unordered_set<std::string_view> _suppressions;
        return _suppressions;
      }
    }  // namespace

    bool should_report() noexcept {
      return REPORTER.report();
    }

    bool suppress(std::string_view const& prefix) {
      // TODO throw exception if prefix is empty
      return suppressions().insert(prefix).second;
    }

    bool stop_suppressing(std::string_view const& prefix) {
      // TODO throw exception if prefix is empty
      return suppressions().erase(prefix);
    }

    bool is_suppressed(std::string_view const& prefix) {
      return suppressions().find(prefix) != suppressions().cend();
    }

  }  // namespace report

}  // namespace libsemigroups
