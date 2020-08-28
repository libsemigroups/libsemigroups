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

// The purpose of this file is to test the Runner class.

#include <cstddef>  // for size_t

#include "catch.hpp"                 // for REQUIRE, REQUIRE_NOTHROW
#include "libsemigroups/report.hpp"  // for ReportGuard
#include "libsemigroups/runner.hpp"  // for Runner
#include "test-main.hpp"             // for LIBSEMIGROUPS_TEST_CASE

namespace libsemigroups {
  struct LibsemigroupsException;

  constexpr bool REPORT = false;

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

    LIBSEMIGROUPS_TEST_CASE("Runner", "000", "run_for", "[quick]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      tr.run_for(std::chrono::milliseconds(10));
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());

      tr.run_for(std::chrono::nanoseconds(1000000));
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "001", "run_for", "[quick]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      tr.run_for(std::chrono::nanoseconds(1000000));
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "002", "run_for", "[quick]") {
      auto        rg = ReportGuard(REPORT);
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

    LIBSEMIGROUPS_TEST_CASE("Runner", "003", "run_for", "[quick]") {
      auto        rg = ReportGuard(REPORT);
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

    LIBSEMIGROUPS_TEST_CASE("Runner", "004", "started", "[quick]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      REQUIRE(!tr.started());
      tr.run_for(std::chrono::nanoseconds(1000000));
      REQUIRE(tr.finished());
      REQUIRE(tr.started());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner",
                            "005",
                            "run_until",
                            "[quick][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
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

    LIBSEMIGROUPS_TEST_CASE("Runner", "006", "kill", "[quick]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      tr.kill();
      REQUIRE(!tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(tr.dead());
      REQUIRE_NOTHROW(tr.report_why_we_stopped());
    }

    LIBSEMIGROUPS_TEST_CASE("Runner", "007", "copy constructor", "[quick]") {
      auto        rg = ReportGuard(REPORT);
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

    LIBSEMIGROUPS_TEST_CASE("Runner", "008", "report", "[quick]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      REQUIRE(!tr.report());
      tr.report_every(std::chrono::milliseconds(10));
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
                            "009",
                            "run_until with function pointer",
                            "[quick][no-valgrind]") {
      auto        rg = ReportGuard(REPORT);
      TestRunner1 tr;
      tr.run_until(fn_ptr);
      REQUIRE(tr.finished());
      REQUIRE(tr.stopped());
      REQUIRE(!tr.dead());
    }

  }  // namespace detail
}  // namespace libsemigroups
