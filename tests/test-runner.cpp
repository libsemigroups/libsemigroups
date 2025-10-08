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

// The purpose of this file is to test the Runner class.

#include <chrono>   // for operator==, seconds, mill...
#include <cstddef>  // for size_t
#include <string>   // for operator==, basic_string
#include <thread>   // for sleep_for
#include <utility>  // for move, forward

#include "Catch2-3.8.0/catch_amalgamated.hpp"  // for SourceLineInfo, operator"...
#include "test-main.hpp"                       // for LIBSEMIGROUPS_TEST_CASE

#include "libsemigroups/exception.hpp"  // for LibsemigroupsException
#include "libsemigroups/runner.hpp"     // for Reporter, Runner, delta

#include "libsemigroups/detail/report.hpp"  // for ReportGuard

namespace libsemigroups {
  struct LibsemigroupsException;

  namespace detail {

    class TestRunner1 : public Runner {
     private:
      void run_impl() override {
        while (!stopped()) {
        }
      }

      bool finished_impl() const override {
        return stopped();
      }
    };

    class TestRunner2 : public Runner {
     private:
      void run_impl() override {
        while (!stopped()) {
          std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
      }
      bool finished_impl() const override {
        return false;
      }
    };

    class TestRunner3 : public Runner {
     private:
      void run_impl() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }

      bool finished_impl() const override {
        return started();
      }
    };

    class TestRunner4 : public Runner {
     private:
      void run_impl() override {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        LIBSEMIGROUPS_EXCEPTION("testing");
      }

      bool finished_impl() const override {
        return started();
      }
    };

    LIBSEMIGROUPS_TEST_CASE("Reporter", "000", "Code coverage", "[quick]") {
      Reporter r;
      REQUIRE(!r.report());
      REQUIRE_NOTHROW(r.last_report());
      r.reset_last_report();
      REQUIRE(delta(r.last_report()) < std::chrono::seconds(1));
      r.report_prefix("Banana");
      REQUIRE(r.report_prefix() == "Banana");
      r.init();
      REQUIRE(r.report_prefix() == "");
      r.report_prefix("Banana");

      Reporter s;
      s = r;
      REQUIRE(s.report_prefix() == "Banana");
      REQUIRE(s.last_report() == r.last_report());
      s.init();
      REQUIRE(s.report_prefix() == "");

      Reporter t(std::move(r));
      REQUIRE(t.report_prefix() == "Banana");

      t = std::move(s);
      REQUIRE(t.report_prefix() == "");

      s.report_divider("666");
      REQUIRE(s.report_divider() == "666");
      s.emit_divider();
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "001", "run_for", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      tr.run_for(std::chrono::milliseconds(10));
      REQUIRE(tr.running_for_how_long() == std::chrono::milliseconds(10));
      REQUIRE(tr.finished());
      REQUIRE(tr.success());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());

      tr.run_for(std::chrono::nanoseconds(1000000));
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "002", "run_for", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      tr.run_for(std::chrono::nanoseconds(1000000));
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "003", "run_for", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner2 tr;
      tr.run_for(std::chrono::milliseconds(50));
      REQUIRE(!tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
      REQUIRE(tr.timed_out());
      REQUIRE(!tr.stopped_by_predicate());
      tr.run_for(std::chrono::milliseconds(50));
      REQUIRE(!tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
      REQUIRE(tr.timed_out());
      REQUIRE(!tr.stopped_by_predicate());
      REQUIRE_NOTHROW(tr.report_why_we_stopped());

      size_t i = 0;
      tr.run_until([&i]() -> bool {
        ++i;
        return i > 10;
      });

      REQUIRE(!tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
      REQUIRE(!tr.timed_out());
      REQUIRE(tr.stopped_by_predicate());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "004", "run_for", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner3 tr;
      tr.run_for(FOREVER);
      REQUIRE(tr.started());
      REQUIRE(!tr.running());
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
      REQUIRE(!tr.timed_out());
      REQUIRE_NOTHROW(tr.report_why_we_stopped());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "005", "started", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      REQUIRE(!tr.started());
      tr.run_for(std::chrono::nanoseconds(1000000));
      REQUIRE(tr.finished());
      REQUIRE(tr.started());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner",
                            "006",
                            "run_until",
                            "[quick][no-valgrind]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      size_t      i = 0;
      tr.run_until([&i]() -> bool {
        ++i;
        return i == 1000000;
      });
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "007", "kill", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      tr.kill();
      REQUIRE(!tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(tr.dead());
      REQUIRE_NOTHROW(tr.report_why_we_stopped());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "008", "copy constructor", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr1;
      tr1.run_for(std::chrono::milliseconds(10));
      REQUIRE(tr1.finished());
      REQUIRE(tr1.stopped());
      REQUIRE(!tr1.dead());
      REQUIRE_NOTHROW(tr1.report_why_we_stopped());

      TestRunner1 tr2(tr1);
      REQUIRE(tr2.finished());
      REQUIRE(tr2.stopped());
      REQUIRE(!tr2.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "009", "report", "[quick]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      REQUIRE(!tr.report());
      // We include this deprecated function so that we can still test tr.report
      // without increasing the runtime of this test to one second.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
      tr.report_every(std::chrono::milliseconds(10));
#pragma GCC diagnostic pop
      tr.run_for(std::chrono::milliseconds(20));
      REQUIRE(tr.report());
    }

    bool first_time = true;
    namespace {
      bool fn_ptr() {
        if (first_time) {
          first_time = false;
          return false;
        }
        // if we return true the first time, then run is not called at all
        return true;
      }
    }  // namespace

    LIBSEMIGROUPS_TEST_CASE("Runner",
                            "010",
                            "run_until with function pointer",
                            "[quick][no-valgrind]") {
      auto        rg = ReportGuard(false);
      TestRunner1 tr;
      tr.run_until(fn_ptr);
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());

      tr.init();
      REQUIRE(!tr.finished());
      REQUIRE(!tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner",
                            "011",
                            "run throws an exception",
                            "[quick][no-valgrind]") {
      auto        rg = ReportGuard(false);
      TestRunner4 tr;
      REQUIRE_THROWS_AS(tr.run(), LibsemigroupsException);
      REQUIRE(tr.current_state() == Runner::state::not_running);
      REQUIRE(tr.finished());
      REQUIRE_NOTHROW(tr.run_for(std::chrono::seconds(1)));
      REQUIRE(tr.current_state() == Runner::state::not_running);
      tr.init();
      REQUIRE(!tr.finished());
      REQUIRE(!tr.dead());
      REQUIRE(tr.current_state() == Runner::state::never_run);
      REQUIRE_THROWS_AS(tr.run_for(std::chrono::seconds(1)),
                        LibsemigroupsException);
      REQUIRE_THROWS_AS(tr.run(), LibsemigroupsException);
      REQUIRE(tr.finished());
      REQUIRE_NOTHROW(tr.run_for(std::chrono::seconds(1)));
    }

    LIBSEMIGROUPS_TEST_CASE("Runner",
                            "012",
                            "run throws an exception",
                            "[quick][no-valgrind]") {
      TestRunner3 tr;
      REQUIRE(!tr.started());
      tr.run();
      REQUIRE(tr.finished());

      TestRunner3 copy;
      copy = tr;  // test copy assignment
      REQUIRE(copy.finished());
      REQUIRE(copy.current_state() == tr.current_state());

      TestRunner3 other(std::move(copy));
      REQUIRE(other.finished());
      REQUIRE(other.current_state() == tr.current_state());

      other = std::move(tr);
      REQUIRE(other.finished());
    }

  }  // namespace detail
}  // namespace libsemigroups
