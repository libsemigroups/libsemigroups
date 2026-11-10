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

#ifndef LIBSEMIGROUPS_HS_ERROR_C_H_
#define LIBSEMIGROUPS_HS_ERROR_C_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  LSEMI_OK                  = 0,
  LSEMI_ERR_LIBSEMIGROUPS   = 1,
  LSEMI_ERR_STD_EXCEPTION   = 2,
  LSEMI_ERR_UNKNOWN         = 3
} lsemi_error_code;

const char* lsemi_last_error_message(void);

void lsemi_clear_last_error(void);

#ifdef __cplusplus
}
#endif

#endif
