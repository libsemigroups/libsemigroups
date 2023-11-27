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

    class ThreadIdManager {
     public:
      ThreadIdManager();
      ThreadIdManager(ThreadIdManager const&)            = delete;
      ThreadIdManager(ThreadIdManager&&)                 = delete;
      ThreadIdManager& operator=(ThreadIdManager const&) = delete;
      ThreadIdManager& operator=(ThreadIdManager&&)      = delete;

      void   reset();
      size_t tid(std::thread::id t);

     private:
      std::mutex                                  _mtx;
      size_t                                      _next_tid;
      std::unordered_map<std::thread::id, size_t> _thread_map;
    };

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

  }  // namespace

  ThreadIdManager THREAD_ID_MANAGER;

  t_id thread_id(std::thread::id t) {
    return THREAD_ID_MANAGER.tid(t);
  }

  t_id this_threads_id() {
    return thread_id(std::this_thread::get_id());
  }

  void reset_thread_ids() {
    THREAD_ID_MANAGER.reset();
  }

  Reporter REPORTER;

  ReportGuard::ReportGuard(bool val) {
    REPORTER.report(val);
  }

  ReportGuard::~ReportGuard() {
    REPORTER.report(false);
  }

  bool reporting_enabled() noexcept {
    return REPORTER.report();
  }

  namespace report {
    namespace {
      std::unordered_set<std::string_view>& suppressions() {
        static std::unordered_set<std::string_view> _suppressions;
        return _suppressions;
      }
    }  // namespace

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
