//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 James D. Mitchell
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

#include <atomic>
#include <unordered_map>  // for unordered_map
#include <unordered_set>  // for unordered_set

#include "libsemigroups/debug.hpp"  // for LIBSEMIGROUPS_ASSERT

namespace libsemigroups {
  namespace {
    class ReportingData {
     public:
      explicit ReportingData(bool report = true) : _report(report) {}

      ReportingData(ReportingData const&)            = delete;
      ReportingData(ReportingData&&)                 = delete;
      ReportingData& operator=(ReportingData const&) = delete;
      ReportingData& operator=(ReportingData&&)      = delete;

      void report(bool val) {
        _report = val;
      }

      bool report() const noexcept {
        return _report;
      }

      bool suppress(std::string_view prefix) {
        return _suppressions.insert(prefix).second;
      }

      bool stop_suppressing(std::string_view prefix) {
        return _suppressions.erase(prefix);
      }

      bool is_report_suppressed_for(std::string_view prefix) {
        return _suppressions.find(prefix) != _suppressions.end();
      }

      std::atomic<bool>                    _report;
      std::unordered_set<std::string_view> _suppressions;
    };

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
      {
        std::lock_guard<std::mutex> lg(_mtx);

        // Delete all thread_ids
        _thread_map.clear();
        _next_tid = 0;
      }
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
    ThreadIdManager thread_id_manager;
    ReportingData   report_data;

  }  // namespace

  ////////////////////////////////////////////////////////////////////////
  // Threads
  ////////////////////////////////////////////////////////////////////////

  namespace detail {

    t_id thread_id(std::thread::id t) {
      return thread_id_manager.tid(t);
    }

    t_id this_threads_id() {
      return thread_id(std::this_thread::get_id());
    }

    void reset_thread_ids() {
      thread_id_manager.reset();
    }

    Ticker::~Ticker() {
      if (_ticker_impl != nullptr) {
        // See TickerImpl for an explanation of why we lock the mtx here.
        std::lock_guard<std::mutex> lck(_ticker_impl->mtx());
        _ticker_impl->stop();
      }
    }

  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // Reporting
  ////////////////////////////////////////////////////////////////////////

  std::mutex& report_mutex() {
    static std::mutex mtx;
    return mtx;
  }

  bool reporting_enabled() noexcept {
    return report_data.report();
  }

  namespace detail {
    bool is_report_suppressed_for(std::string_view prefix) {
      return report_data.is_report_suppressed_for(prefix);
    }
  }  // namespace detail

  ////////////////////////////////////////////////////////////////////////
  // ReportGuard
  ////////////////////////////////////////////////////////////////////////

  ReportGuard::ReportGuard(bool val) {
    report_data.report(val);
  }

  ReportGuard::~ReportGuard() {
    report_data.report(false);
  }

  ////////////////////////////////////////////////////////////////////////
  // SuppressReportFor
  ////////////////////////////////////////////////////////////////////////

  SuppressReportFor::SuppressReportFor(std::string_view name) : _prefix(name) {
    report_data.suppress(_prefix);
  }

  SuppressReportFor::~SuppressReportFor() {
    report_data.stop_suppressing(_prefix);
  }

}  // namespace libsemigroups
