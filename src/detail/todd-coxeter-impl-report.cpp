//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2026 James D. Mitchell
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

// This file contains the implementations of the private member functions of
// ToddCoxeterImpl that related to reporting.

#include "libsemigroups/detail/todd-coxeter-impl.hpp"

#include <algorithm>    // for max, for_each
#include <atomic>       // for atomic
#include <cctype>       // for toupper
#include <chrono>       // for operator+, dur...
#include <cmath>        // for isnan
#include <cstddef>      // for size_t
#include <cstdint>      // for uint64_t, int64_t
#include <ratio>        // for ratio
#include <set>          // for set
#include <string>       // for basic_string
#include <string_view>  // for basic_string_view
#include <utility>      // for forward

#include "libsemigroups/debug.hpp"         // for LIBSEMIGROUPS_...
#include "libsemigroups/presentation.hpp"  // for length, longes...
#include "libsemigroups/runner.hpp"        // for delta

#include "libsemigroups/detail/fmt.hpp"                // for format
#include "libsemigroups/detail/formatters.hpp"         // for magic_enum
#include "libsemigroups/detail/report.hpp"             // for ReportCell
#include "libsemigroups/detail/string.hpp"             // for group_digits
#include "libsemigroups/detail/timer.hpp"              // for string_time
#include "libsemigroups/detail/todd-coxeter-impl.hpp"  // for ToddCoxeterImpl

namespace libsemigroups::detail {
  using ReportCell_ = ReportCell<6>;

  namespace {
    constexpr bool no_print_divider = false;

    ReportCell_ report_cell() {
      ReportCell_ rc;
      rc.min_width(12).min_width(0, 0).min_width(1, 16).align(1, Align::left);
      return rc;
    }

    constexpr auto const run_color = fmt::bg(fmt::terminal_color::white)
                                     | fmt::fg(fmt::terminal_color::black);
    auto const phase_color
        = fmt::bg(fmt::rgb(96, 96, 96)) | fmt::fg(fmt::terminal_color::white);

    std::string italic(char const* var) {
      return fmt::format(fmt::emphasis::italic, "{}", var);
    }

    template <typename Thing>
    std::string toupper(Thing const& thing) {
      auto result = fmt::format("{}", thing);
      std::for_each(result.begin(), result.end(), [](auto& val) {
        val = std::toupper(val);
      });
      return result;
    }

    void report_keys(std::set<std::string> const& keys) {
      if (!keys.empty()) {
        report_default("ToddCoxeter: where:  ");
      }
      bool first = true;
      for (auto const& key : keys) {
        if (!first) {
          report_default("ToddCoxeter:         {}", key);
        } else {
          report_no_prefix("{}", key);
          first = false;
        }
      }
    }

    std::string to_percent(uint64_t num, uint64_t denom) {
      double val = static_cast<double>(num) * 100 / denom;
      return std::isnan(val) ? "-" : fmt::format("{:.0f}%", val);
    }

    std::string underline(char const* var) {
      return fmt::format(fmt::emphasis::underline, "{}", var);
    }

    std::string underline(std::string const& var) {
      return fmt::format(fmt::emphasis::underline, "{}", var);
    }

    // Simple struct that allows the "receivers" value to be set to "val" but
    // only when the object goes out of scope. Useful in reporting when we
    // want to do something with an old value, then update the data member of
    // Stats.
    class DeferSet {
      uint64_t& _receiver;
      uint64_t  _val;

     public:
      DeferSet(uint64_t& receiver, uint64_t val)
          : _receiver(receiver), _val(val) {}

      operator uint64_t() {
        return _val;
      }

      ~DeferSet() {
        _receiver = _val;
      }
    };

  }  // namespace

  // The 2nd and 3rd arguments for the next function are required
  // because we need the values at a fixed point in time (due to
  // multi-threaded reporting).
  void ToddCoxeterImpl::add_edges_rows(ReportCell_& rc,
                                       uint64_t     active_nodes,
                                       uint64_t     active_edges) const {
    auto const percent_complete = complete(active_nodes, active_edges);
    auto const X = _stats.run_index, Y = _stats.phase_index,
               Z = _stats.report_index;
    auto const missing_edges
        = active_nodes * _word_graph.out_degree() - active_edges;

    rc("{}: {} | {} | {} | {}\n",
       report_prefix(),
       "",
       underline("active"),
       underline("missing"),
       underline("% complete"));
    rc("{}: {} | {} | {} | {}\n",
       report_prefix(),
       "edges",
       group_digits(active_edges),
       group_digits(missing_edges),
       fmt::format("{:.1f}%", 100 * percent_complete));
    if (Z > 0) {
      auto const  active_diff1 = active_edges - _stats.report_edges_active_prev;
      float const complete_diff1
          = 100
            * (percent_complete
               - static_cast<float>(_stats.report_complete_prev));
      auto const missing_diff1
          = missing_edges
            - (_stats.report_nodes_active_prev * _word_graph.out_degree()
               - _stats.report_edges_active_prev);

      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         fmt::format("diff {}.{}.{}", X, Y, Z - 1),
         signed_group_digits(active_diff1),
         signed_group_digits(missing_diff1),
         fmt::format(
             "{}{:.1f}%", complete_diff1 >= 0 ? "+" : "", complete_diff1));
      if (Z > 1) {
        auto const active_diff2
            = active_edges - _stats.phase_edges_active_at_start;
        float const complete_diff2
            = 100
              * (percent_complete
                 - static_cast<float>(_stats.phase_complete_at_start));
        auto const missing_diff2
            = missing_edges
              - (_stats.phase_nodes_active_at_start * _word_graph.out_degree()
                 - _stats.phase_edges_active_at_start);
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           fmt::format("diff {}.{}.0", X, Y),
           signed_group_digits(active_diff2),
           signed_group_digits(missing_diff2),
           fmt::format(
               "{}{:.1f}%", complete_diff2 >= 0 ? "+" : "", complete_diff2));
      }
    }
    _stats.report_complete_prev = percent_complete;
  }

  void ToddCoxeterImpl::add_lookahead_or_behind_row(ReportCell_& rc) const {
    if ((_state == state::lookahead || _state == state::lookbehind)
        && _stats.report_index != 0 && this_threads_id() != 0) {
      // Don't call this in the main thread, because that's where we write
      // after a lookahead, where this percentage is often wrong and
      // superfluous.

      // It is difficult to get the exact value of the % complete due to
      // multi-threading issues, hence we don't try, we just assume that
      // nodes are uniformly randomly killed, leading to the following
      // approximate progress . . .
      auto const N = _stats.phase_nodes_active_at_start;
      auto const p = _stats.lookahead_or_behind_position.load();
      auto const r = _stats.lookahead_or_behind_nodes_killed.load();
      rc("{}: {} | {} \n",
         report_prefix(),
         fmt::format("{} progress", _state.load()),
         fmt::format("~{:.1f}%",
                     (p - static_cast<double>(p * r) / N) * 100 / (N - r)));
    }
  }

  // The 2nd argument for the next function is required
  // because we need the value at a fixed point in time (due to
  // multi-threaded reporting).
  void ToddCoxeterImpl::add_nodes_rows(ReportCell_& rc,
                                       uint64_t     active_nodes) const {
    auto const X = _stats.run_index, Y = _stats.phase_index,
               Z = _stats.report_index;

    DeferSet defined(_stats.report_nodes_defined_prev,
                     current_word_graph().number_of_nodes_defined());
    DeferSet killed(_stats.report_nodes_killed_prev,
                    current_word_graph().number_of_nodes_killed());

    auto const active_diff1
        = signed_group_digits(active_nodes - _stats.report_nodes_active_prev);
    auto const killed_diff1
        = signed_group_digits(killed - _stats.report_nodes_killed_prev);
    auto const defined_diff1
        = signed_group_digits(defined - _stats.report_nodes_defined_prev);

    auto const active_diff2 = signed_group_digits(
        active_nodes - _stats.phase_nodes_active_at_start);
    auto const killed_diff2
        = signed_group_digits(killed - _stats.phase_nodes_killed_at_start);
    auto const defined_diff2
        = signed_group_digits(defined - _stats.phase_nodes_defined_at_start);

    rc("{}: {} | {} | {} | {}\n",
       report_prefix(),
       fmt::format(fmt::emphasis::underline,
                   "{} {}.{}.{}",
                   toupper(_state.load()),
                   X,
                   Y,
                   Z),
       underline("active"),
       underline("killed"),
       underline("defined"));
    rc("{}: {} | {} | {} | {}\n",
       report_prefix(),
       "nodes",
       group_digits(active_nodes),
       group_digits(killed),
       group_digits(defined));
    if (Z > 0) {
      rc("{}: {} | {} | {} | {}\n",
         report_prefix(),
         fmt::format("diff {}.{}.{}", X, Y, Z - 1),
         active_diff1,
         killed_diff1,
         defined_diff1);
      if (Z > 1) {
        rc("{}: {} | {} | {} | {}\n",
           report_prefix(),
           fmt::format("diff {}.{}.0", X, Y),
           active_diff2,
           killed_diff2,
           defined_diff2);
      }
    }
    // TODO(1) could add rows with max. overall/run/phase./min. values.
    // I (JDM) think this might be quite useful, but there's already a lot in
    // the reported info, so I'm skipping it for now.
  }

  void ToddCoxeterImpl::add_timing_row(ReportCell_& rc) const {
    auto this_run_time   = delta(_stats.run_start_time);
    auto this_phase_time = delta(_stats.phase_start_time);
    // We don't use start_time() in the next line because this gets reset in
    // Runner::run_for.
    auto elapsed = delta(_stats.create_or_init_time);

    LIBSEMIGROUPS_ASSERT(elapsed >= _stats.all_runs_time + this_run_time);
    std::string c1;
    // TODO(1) if we add an additional column, then we should split the timing
    // row into two, to make them narrower.
    if (_stats.report_index == 0 || _state == state::none) {
      c1 = underline("time");
    } else {
      c1 = fmt::format("phase {}.{} = {}",
                       _stats.run_index,
                       _stats.phase_index,
                       string_time(this_phase_time));
    }

    rc("{}: {} | {} | {} | {}\n",
       report_prefix(),
       c1,
       fmt::format("run {} = {}", _stats.run_index, string_time(this_run_time)),
       fmt::format("all runs = {}",
                   string_time(_stats.all_runs_time + this_run_time)),
       fmt::format("elapsed = {}", string_time(elapsed)));
  }

  void ToddCoxeterImpl::report_after_phase() const {
    if (reporting_enabled()) {
      report_no_prefix(report_divider());
      report_default("ToddCoxeter: {}\n",
                     fmt::format(phase_color,
                                 "{} {}.{} STOP",
                                 toupper(_state.load()),
                                 _stats.run_index,
                                 _stats.phase_index));
      report_progress_from_thread(no_print_divider);
    }
  }

  void ToddCoxeterImpl::report_after_run() const {
    if (reporting_enabled()) {
      report_no_prefix(report_divider());

      std::string reason = finished() ? "finished" : string_why_we_stopped();

      if (reason.empty()
          && (strategy() == options::strategy::lookahead
              || strategy() == options::strategy::lookbehind)) {
        reason = fmt::format("{} complete", strategy());
      }
      // Often the end of a run coincides with the end of a lookahead, which
      // already prints out this info, so avoid duplication in case nothing
      // has changed.

      report_default("{}: {} ({})\n",
                     report_prefix(),
                     fmt::format(run_color, "RUN {} STOP", _stats.run_index),
                     reason);
      auto rc = report_cell();
      rc("{}: {} | {} | {} | {} | {}\n",
         report_prefix(),
         underline(fmt::format("run {}", _stats.run_index)),
         underline("lookahead"),
         underline("lookbehind"),
         underline("hlt"),
         underline("felsch"));
      rc("{}: {} | {} | {} | {} | {}\n",
         report_prefix(),
         "num. phases",
         group_digits(_stats.run_num_lookahead_phases),
         group_digits(_stats.run_num_lookbehind_phases),
         group_digits(_stats.run_num_hlt_phases),
         group_digits(_stats.run_num_felsch_phases));

      auto this_run_time = delta(_stats.run_start_time);

      auto percent_run_time_lookahead = to_percent(
          _stats.run_lookahead_phases_time.count(), this_run_time.count());
      auto percent_run_time_lookbehind = to_percent(
          _stats.run_lookbehind_phases_time.count(), this_run_time.count());
      auto percent_run_time_hlt = to_percent(_stats.run_hlt_phases_time.count(),
                                             this_run_time.count());
      auto percent_run_time_felsch = to_percent(
          _stats.run_felsch_phases_time.count(), this_run_time.count());

      // When the times are very short (microseconds) the percentage spent in
      // each phase type won't add up to 100% (it will be less) because the
      // calling of the functions before hlt/felsch (before_run etc) take a
      // non-trivial % of the run time. Be good to fix this, but not sure how
      // exactly.
      rc("{}: {} | {} | {} | {} | {}\n",
         report_prefix(),
         "time spent in phases",
         fmt::format("{} ({})",
                     string_time(_stats.run_lookahead_phases_time),
                     percent_run_time_lookahead),
         fmt::format("{} ({})",
                     string_time(_stats.run_lookbehind_phases_time),
                     percent_run_time_lookbehind),
         fmt::format("{} ({})",
                     string_time(_stats.run_hlt_phases_time),
                     percent_run_time_hlt),
         fmt::format("{} ({})",
                     string_time(_stats.run_felsch_phases_time),
                     percent_run_time_felsch));
      if (_stats.run_index > 0) {
        rc("{}: {} | {} | {} | {} | {}\n",
           report_prefix(),
           underline("all runs"),
           underline("lookahead"),
           underline("lookbehind"),
           underline("hlt"),
           underline("felsch"));
        rc("{}: {} | {} | {} | {} | {}\n",
           report_prefix(),
           "num. phases ",
           group_digits(_stats.all_num_lookahead_phases
                        + _stats.run_num_lookahead_phases),
           group_digits(_stats.all_num_lookbehind_phases
                        + _stats.run_num_lookbehind_phases),
           group_digits(_stats.all_num_hlt_phases + _stats.run_num_hlt_phases),
           group_digits(_stats.all_num_felsch_phases
                        + _stats.run_num_felsch_phases));

        auto total_lookahead = _stats.all_lookahead_phases_time
                               + _stats.run_lookahead_phases_time;
        auto total_lookbehind = _stats.all_lookbehind_phases_time
                                + _stats.run_lookbehind_phases_time;
        auto total_hlt
            = _stats.all_hlt_phases_time + _stats.run_hlt_phases_time;
        auto total_felsch
            = _stats.all_felsch_phases_time + _stats.run_felsch_phases_time;
        auto total = (_stats.all_runs_time + this_run_time).count();

        auto percent_total_lookahead
            = to_percent(total_lookahead.count(), total);
        auto percent_total_lookbehind
            = to_percent(total_lookbehind.count(), total);
        auto percent_total_hlt    = to_percent(total_hlt.count(), total);
        auto percent_total_felsch = to_percent(total_felsch.count(), total);

        rc("{}: {} | {} | {} | {} | {}\n",
           report_prefix(),
           "time spent in",
           fmt::format("{} ({})",
                       string_time(total_lookahead),
                       percent_total_lookahead),
           fmt::format("{} ({})",
                       string_time(total_lookbehind),
                       percent_total_lookbehind),
           fmt::format("{} ({})", string_time(total_hlt), percent_total_hlt),
           fmt::format(
               "{} ({})", string_time(total_felsch), percent_total_felsch));
      }
      add_timing_row(rc);
      // TODO(1) time spent process_definitions, process_coincidences?
    }
  }

  void ToddCoxeterImpl::report_before_lookahead() const {
    if (!reporting_enabled()) {
      return;
    }
    report_before_phase(fmt::format("lookahead_extent() = {}, "
                                    "lookahead_style() = {}",
                                    lookahead_extent(),
                                    lookahead_style()));
    if (strategy() == options::strategy::lookahead) {
      return;
    }
    // If the strategy() is lookahead, then lookahead was called via the
    // mem fns perform_lookahead(_for/until) and so this information is
    // not relevant.
    if (current_word_graph().definitions().any_skipped()) {
      report_default(
          "ToddCoxeter: triggered because there are skipped "
          "definitions ({} active nodes)!\n",
          group_digits(current_word_graph().number_of_nodes_active()));
    } else if (current_word_graph().number_of_nodes_active()
               > lookahead_next()) {
      auto ln      = lookahead_next();
      auto ln_name = italic("n");
      auto ln_key  = fmt::format(
          "{} = lookahead_next()         = {}\n", ln_name, group_digits(ln));

      auto a      = current_word_graph().number_of_nodes_active();
      auto a_name = italic("a");
      auto a_key  = fmt::format(
          "{} = number_of_nodes_active() = {}\n", a_name, group_digits(a));

      std::set<std::string> keys;
      report_default("ToddCoxeter: because {} >= {}\n", a_name, ln_name);
      keys.insert(a_key);
      keys.insert(ln_key);
      report_keys(keys);
    }
  }

  void ToddCoxeterImpl::report_before_phase(std::string_view info) const {
    if (reporting_enabled()) {
      report_no_prefix(report_divider());
      report_default("ToddCoxeter: {}{}\n",
                     fmt::format(phase_color,
                                 "{} {}.{} START",
                                 toupper(_state.load()),
                                 _stats.run_index,
                                 _stats.phase_index),
                     info.empty() ? "" : fmt::format(" ({})", info));
      report_progress_from_thread(no_print_divider);
    }
  }

  void ToddCoxeterImpl::report_before_run() const {
    if (!reporting_enabled()) {
      return;
    }
    report_no_prefix(report_divider());
    report_default("ToddCoxeter: {} (strategy() = {})\n",
                   fmt::format(run_color, "RUN {} START", _stats.run_index),
                   strategy());
    if (_stats.run_index > 0) {
      report_times();
    }

    report_presentation();
  }

  void
  ToddCoxeterImpl::report_lookahead_settings(size_t old_lookahead_next) const {
    auto gd  = group_digits;
    auto sgd = signed_group_digits;

    if (!reporting_enabled() || finished()) {
      return;
    }

    auto lgf      = lookahead_growth_factor();
    auto lgf_name = italic("f");
    auto lgf_key  = fmt::format(
        "{} = lookahead_growth_factor()    = {}\n", lgf_name, lgf);

    auto lgt      = lookahead_growth_threshold();
    auto lgt_name = italic("t");
    auto lgt_key  = fmt::format(
        "{} = lookahead_growth_threshold() = {}\n", lgt_name, lgt);

    auto oln      = old_lookahead_next;
    auto oln_name = italic("n");
    auto oln_key  = fmt::format(
        "{} = lookahead_next()             = {}\n", oln_name, gd(oln));

    auto ln = lookahead_next();

    auto a      = _word_graph.number_of_nodes_active();
    auto a_name = italic("a");
    auto a_key  = fmt::format(
        "{} = number_of_nodes_active()     = {}\n", a_name, gd(a));

    auto l      = _stats.lookahead_or_behind_nodes_killed.load();
    auto l_name = italic("l");
    auto l_key  = fmt::format(
        "{} = nodes killed in lookahead    = {}\n", l_name, gd(l));

    auto m      = lookahead_min();
    auto m_name = italic("m");
    auto m_key  = fmt::format(
        "{} = lookahead_min()              = {}\n", m_name, gd(m));

    std::set<std::string> keys;

    int64_t const diff   = static_cast<int64_t>(ln) - oln;
    std::string   reason = fmt_default("ToddCoxeter: lookahead_next() is now ");

    if (a * lgf < oln || a > oln) {
      reason += fmt::format("max({} x {} = {}, {} = {}) ({})\n",
                            lgf_name,
                            a_name,
                            gd(lgf * a),
                            m_name,
                            gd(m),
                            sgd(diff));
      if (a * lgf < oln) {
        // TODO(1) add different levels of reporting, and only print the
        // "because" stuff if the level is > 0 (where 0 would be the
        // default).
        reason += fmt_default(
            "ToddCoxeter: because {} x {} < {}\n", lgf_name, a_name, oln_name);
      } else {
        reason
            += fmt_default("ToddCoxeter: because {} > {}\n", a_name, oln_name);
      }
      keys.insert(a_key);
      keys.insert(lgf_key);
      keys.insert(oln_key);
      keys.insert(m_key);
    } else if (l < (l + a) / lgt) {
      reason += fmt::format(
          "{} x {} = {} ({})\n", oln_name, lgf_name, gd(oln * lgf), sgd(diff));

      reason += fmt_default("ToddCoxeter: because: {} < ({} + {}) / {} = {}\n",
                            l_name,
                            l_name,
                            a_name,
                            lgt_name,
                            gd((l + a) / lgt));
      keys.insert(a_key);
      keys.insert(l_key);
      keys.insert(lgf_key);
      keys.insert(lgt_key);
      keys.insert(oln_key);
    } else {
      reason += fmt::format("{} ({})\n", gd(ln), sgd(diff));
      reason += fmt_default("ToddCoxeter: because:\n");
      reason += fmt_default("ToddCoxeter: 1. {} <= {} x {} = {}\n",
                            oln_name,
                            lgf_name,
                            a_name,
                            gd(lgf * a));
      reason += fmt_default("ToddCoxeter: 2. {} <= {}\n", a_name, oln_name);
      reason += fmt_default("ToddCoxeter: 3. {} >= ({} + {}) / {} = {}\n",
                            l_name,
                            l_name,
                            a_name,
                            lgt_name,
                            gd((l + a) / lgt));
      keys.insert(a_key);
      keys.insert(l_key);
      keys.insert(lgf_key);
      keys.insert(lgt_key);
      keys.insert(oln_key);
    }

    if (!finished()) {
      report_no_prefix(reason);
      report_keys(keys);
    }
  }

  void ToddCoxeterImpl::report_progress_from_thread(bool divider) const {
    if (!reporting_enabled() || _state == state::none) {
      return;
    }
    // Sometimes this gets called concurrently but slightly after the end
    // of a phase, which results in a weird NONE block with messed up
    // numbers being printed.
    auto rc = report_cell();

    // Set the value of _stats.report_nodes_active_prev to the current
    // number of active nodes when active_nodes is destructed,
    // active_nodes has the 2nd argument as its value
    DeferSet active_nodes(_stats.report_nodes_active_prev,
                          current_word_graph().number_of_nodes_active());
    DeferSet active_edges(_stats.report_edges_active_prev,
                          current_word_graph().number_of_edges_active());

    if (divider) {
      report_no_prefix(report_divider());
    }
    add_nodes_rows(rc, active_nodes);
    add_edges_rows(rc, active_nodes, active_edges);
    add_timing_row(rc);
    add_lookahead_or_behind_row(rc);

    stats_report_stop();
  }

  void
  ToddCoxeterImpl::report_lookahead_stop_early(size_t expected,
                                               size_t killed_last_interval) {
    if (!reporting_enabled()) {
      return;
    }
    auto gd       = group_digits;
    auto interval = string_time(lookahead_stop_early_interval());
    report_no_prefix(report_divider());
    report_default("ToddCoxeter: too few nodes killed in last {} = "
                   "{}, stopping lookahead early!\n",
                   italic("i"),
                   interval);
    report_default("ToddCoxeter: expected at least {} x {} = {} but "
                   "found {}\n",
                   italic("r"),
                   italic("a"),
                   gd(expected),
                   gd(killed_last_interval));
    report_keys({fmt::format("{} = lookahead_stop_early_ratio()    = {}\n",
                             italic("r"),
                             lookahead_stop_early_ratio()),
                 fmt::format("{} = lookahead_stop_early_interval() = {}\n",
                             italic("i"),
                             interval),
                 fmt::format("{} = number_of_nodes_active()        = {}\n",
                             italic("a"),
                             gd(number_of_nodes_active()))});
  }

  void ToddCoxeterImpl::report_presentation() const {
    report_default("ToddCoxeter: {}",
                   presentation::to_report_string(internal_presentation()));
  }

  void ToddCoxeterImpl::report_times() const {
    auto rc = report_cell();
    add_timing_row(rc);
  }

}  // namespace libsemigroups::detail
