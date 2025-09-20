//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

// This file contains the implementation of the function templates defined in
// detail/report.hpp

namespace libsemigroups {

  namespace detail {

    ////////////////////////////////////////////////////////////////////////
    // TickerImpl
    ////////////////////////////////////////////////////////////////////////

    class Ticker::TickerImpl {
      using nanoseconds = std::chrono::nanoseconds;

      std::function<void(void)> _func;
      std::mutex                _mtx;
      bool                      _stop;
      nanoseconds               _report_time_interval;

     public:
      template <typename Func, typename Time = std::chrono::seconds>
      explicit TickerImpl(Func&& func, Time time = std::chrono::seconds(1))
          : _func(std::forward<Func>(func)),
            _stop(false),
            _report_time_interval(nanoseconds(time)) {
        auto thread_func = [this](TickerImpl* dtg) {
          std::unique_ptr<TickerImpl> ptr;
          ptr.reset(dtg);
          std::this_thread::sleep_for(_report_time_interval);
          while (true) {
            {
              std::lock_guard<std::mutex> lck(_mtx);
              // This stops _func() from being called, when the object it
              // relates to is destroyed, two scenarios arise:
              // 1. the Ticker goes out of scope, the mutex is locked, then
              //    _stop is set to false, Ticker is destroyed, maybe the
              //    object where Ticker is created is also destroyed (the
              //    object containing _func and/or its related data), then
              //    we acquire the mutex here, and _stop is false, so we
              //    don't call _func (which is actually destroyed).
              // 2. we acquire the lock on the mutex here first, and then
              //    Ticker goes out of scope, the destructor of Ticker is
              //    then blocked (and so too is the possible destruction of
              //    the object containing the data for _func) until we've
              //    finished calling _func again.
              if (!_stop) {
                _func();
              } else {
                break;
              }
            }
            std::this_thread::sleep_for(_report_time_interval);
          }
        };
        auto t = std::thread(thread_func, this);
        t.detach();
      }

      void stop() {
        _stop = true;
      }

      std::mutex& mtx() noexcept {
        return _mtx;
      }
    };  // TickerImpl

    ////////////////////////////////////////////////////////////////////////
    // Ticker
    ////////////////////////////////////////////////////////////////////////

    template <typename Func, typename Time>
    Ticker::Ticker(Func&& func, Time time)
        : _ticker_impl(new TickerImpl(std::forward<Func>(func), time)) {}

    template <typename Func, typename Time>
    void Ticker::operator()(Func&& func, Time time) {
      if (_ticker_impl != nullptr) {
        LIBSEMIGROUPS_EXCEPTION("the Ticker is already in use!");
      }
      _ticker_impl = new TickerImpl(std::forward<Func>(func), time);
    }

    ////////////////////////////////////////////////////////////////////////
    // ReportCell
    ////////////////////////////////////////////////////////////////////////

    template <size_t C>
    template <typename... Args>
    void ReportCell<C>::operator()(std::string_view fmt_str, Args&&... args) {
      static_assert(sizeof...(args) <= C);
      _rows.push_back(Row({std::string(fmt_str), std::forward<Args>(args)...}));
      for (size_t i = 0; i < _rows.back().size(); ++i) {
        _col_widths[i]
            = std::max(_col_widths[i], visible_length(_rows.back()[i]));
      }
    }

    template <size_t C>
    void ReportCell<C>::emit() {
      for (size_t i = 0; i < _rows.size(); ++i) {
        for (size_t j = 1; j < C + 1; ++j) {
          auto pad
              = std::string(_col_widths[j] - visible_length(_rows[i][j]), ' ');
          if (_align[j] == Align::right) {
            _rows[i][j] = pad + _rows[i][j];
          } else {
            _rows[i][j] += pad;
          }
        }
      }
      auto fmt = [](auto&&... args) {
        report_no_lock_no_prefix(
            fmt_default(std::forward<decltype(args)>(args)...));
      };
      // lock the report mutex so the whole table is printed before anything
      // else (from another thread)
      std::lock_guard lock(report_mutex());
      for (size_t i = 0; i < _rows.size(); ++i) {
        std::apply(fmt, _rows[i]);
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
