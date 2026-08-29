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

#ifndef LIBSEMIGROUPS_HS_FROIDURE_PIN_C_H_
#define LIBSEMIGROUPS_HS_FROIDURE_PIN_C_H_

#include <stddef.h>
#include <stdint.h>

#include "error_c.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle for FroidurePin<Transf<0, uint16_t>>. */
typedef struct lsemi_froidure_pin_transf16 lsemi_froidure_pin_transf16;

int lsemi_froidure_pin_transf16_new(lsemi_froidure_pin_transf16** out);

void lsemi_froidure_pin_transf16_free(lsemi_froidure_pin_transf16* fp);

/* Image array length must equal the FroidurePin's degree (set on first
   generator). Image values must be < degree. Throws on degree mismatch. */
int lsemi_froidure_pin_transf16_add_generator(
    lsemi_froidure_pin_transf16* fp,
    const uint16_t*              image,
    size_t                       degree);

int lsemi_froidure_pin_transf16_run(lsemi_froidure_pin_transf16* fp);

int lsemi_froidure_pin_transf16_size(
    lsemi_froidure_pin_transf16* fp,
    size_t*                      out);

int lsemi_froidure_pin_transf16_number_of_generators(
    lsemi_froidure_pin_transf16* fp,
    size_t*                      out);

#ifdef __cplusplus
}
#endif

#endif
