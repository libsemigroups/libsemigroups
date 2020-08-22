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

#include "libsemigroups/report.hpp"

namespace libsemigroups {
  detail::Reporter        REPORTER;
  detail::ThreadIdManager THREAD_ID_MANAGER;

  namespace detail {
    Reporter& Reporter::prefix() {
      if (_report) {
        std::lock_guard<std::mutex> lg(_mtx);
        size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        resize(tid + 1);
        _options[tid].prefix = "";
      }
      return *this;
    }

    Reporter& Reporter::color(fmt::color c) {
      if (_report) {
        size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        resize(tid + 1);
        _options[tid].color = c;
      }
      return *this;
    }

    Reporter& Reporter::thread_color() {
      if (_report) {
        std::lock_guard<std::mutex> lg(_mtx);
        size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        resize(tid + 1);
        _options[tid].color = thread_colors[tid % thread_colors.size()];
      }
      return *this;
    }

    Reporter& Reporter::flush_right() {
      if (_report) {
        std::lock_guard<std::mutex> lg(_mtx);
        size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        resize(tid + 1);
        _options[tid].flush_right = true;
      }
      return *this;
    }

    void Reporter::flush() {
      if (_report) {
        std::lock_guard<std::mutex> lg(_mtx);
        size_t tid = THREAD_ID_MANAGER.tid(std::this_thread::get_id());
        size_t pad = 0;
        _msg[tid]  = _options[tid].prefix + _msg[tid];
        if (_options[tid].flush_right
            && _last_msg[tid].size() + unicode_string_length(_msg[tid]) < 80) {
          pad = (80 - _last_msg[tid].size()) - unicode_string_length(_msg[tid]);
          _msg[tid] = std::string(pad, ' ') + _msg[tid];
        }
#ifdef LIBSEMIGROUPS_VERBOSE
        if (_msg[tid].back() != '\n') {
          _msg[tid] += "\n";
        }
#endif
        _msg[tid] = wrap(_options[tid].prefix.length(), _msg[tid]);
        fmt::print(fg(_options[tid].color), _msg[tid]);
        _options[tid] = Options();
      }
    }

    void Reporter::resize(size_t n) {
      if (n > _msg.size()) {
        _last_msg.resize(n);
        _msg.resize(n);
        _options.resize(n);
      }
    }
  }  // namespace detail
}  // namespace libsemigroups
