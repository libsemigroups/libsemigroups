// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025-2026 Joshua James-lee
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

#ifndef LIBSEMIGROUPS_HS_ERROR_INTERNAL_HPP_
#define LIBSEMIGROUPS_HS_ERROR_INTERNAL_HPP_

#include "error_c.h"

#include <exception>
#include <string>

#include <libsemigroups/exception.hpp>

namespace lsemi_hs {
  extern thread_local std::string g_last_error_message;
}

#define LSEMI_TRY                                                          \
  try {                                                                    \
    ::lsemi_hs::g_last_error_message.clear();

#define LSEMI_CATCH                                                        \
    return LSEMI_OK;                                                       \
  } catch (::libsemigroups::LibsemigroupsException const& e) {             \
    ::lsemi_hs::g_last_error_message = e.what();                           \
    return LSEMI_ERR_LIBSEMIGROUPS;                                        \
  } catch (std::exception const& e) {                                      \
    ::lsemi_hs::g_last_error_message = e.what();                           \
    return LSEMI_ERR_STD_EXCEPTION;                                        \
  } catch (...) {                                                          \
    ::lsemi_hs::g_last_error_message = "unknown C++ exception";            \
    return LSEMI_ERR_UNKNOWN;                                              \
  }

#endif
