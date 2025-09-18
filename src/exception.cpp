//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2025 James D. Mitchell
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

// This file contains the implementation of LibsemigroupsException

#include "libsemigroups/exception.hpp"

#include <stdexcept>  // for std::runtime_error
#include <string>     // for char_traits, allocator, operator+

#include "libsemigroups/config.hpp"  // for LIBSEMIGROUPS_BACKWARD_ENABLED
                                     //
#ifdef LIBSEMIGROUPS_BACKWARD_ENABLED
#include <cstddef>  // for size_t
#if defined(__CYGWIN__) || defined(__CYGWIN32__)
#undef LIBSEMIGROUPS_BACKWARD_ENABLED
#else
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wundef"
#endif
#ifdef LIBSEMIGROUPS_WITH_INTERNAL_BACKWARD
#include "backward-cpp/backward.hpp"  // for StackTrace, TraceResolver, Reso...
#else
#include "backward.hpp"  // for StackTrace, TraceResolver, Reso...
#endif
#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#endif  // defined(__CYGWIN__) || defined(__CYGWIN32__)
#endif  // LIBSEMIGROUPS_BACKWARD_ENABLED

namespace libsemigroups {
  LibsemigroupsException::LibsemigroupsException(std::string const& fname,
                                                 int                linenum,
                                                 std::string const& funcname,
                                                 std::string const& msg)
      // Initialise runtime_error message in case something goes wrong below
      : std::runtime_error(fname + ":" + std::to_string(linenum) + ":"
                           + funcname + ": " + msg) {
#ifdef LIBSEMIGROUPS_BACKWARD_ENABLED
    std::string          full_msg;
    backward::StackTrace st;
    st.load_here();
    backward::TraceResolver tr;
    tr.load_stacktrace(st);
    if (st.size() > 0) {
      for (size_t i = st.size() - 1; i-- > 0;) {
        backward::ResolvedTrace trace = tr.resolve(st[i]);
        size_t                  pos   = trace.object_function.find("(");
        if (pos == std::string::npos) {
          continue;
        }
        std::string func(trace.object_function.begin(),
                         trace.object_function.begin() + pos);
        if (func.find("Catch") != std::string::npos
            || func.find("C_A_T_C_H") != std::string::npos
            || func.find("LibsemigroupsException") != std::string::npos
            || func.find("pybind11") != std::string::npos
            || func.find("libsemigroups") == std::string::npos) {
          continue;
        }
        full_msg += "#" + std::to_string(i - 1) + " " + func + "\n";
      }
    }
    full_msg = "Simplified stack trace (most recent call last):\n" + full_msg;
    full_msg
        += fname + ":" + std::to_string(linenum) + ":" + funcname + ": " + msg;
    static_cast<std::runtime_error&>(*this) = std::runtime_error(full_msg);
#endif
  }
}  // namespace libsemigroups

#ifdef LIBSEMIGROUPS_BACKWARD_ENABLED
namespace backward {
  backward::SignalHandling sh;
}  // namespace backward
#endif
