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

#include "froidure_pin_c.h"
#include "error_internal.hpp"

#include <cstdint>
#include <vector>

#include <libsemigroups/froidure-pin.hpp>
#include <libsemigroups/transf.hpp>

namespace {
  using TransfU16 = libsemigroups::Transf<0, uint16_t>;
  using FP        = libsemigroups::FroidurePin<TransfU16>;
}  // namespace

struct lsemi_froidure_pin_transf16 {
  FP impl;
};

extern "C" int
lsemi_froidure_pin_transf16_new(lsemi_froidure_pin_transf16** out) {
  LSEMI_TRY
  *out = new lsemi_froidure_pin_transf16{};
  LSEMI_CATCH
}

extern "C" void
lsemi_froidure_pin_transf16_free(lsemi_froidure_pin_transf16* fp) {
  delete fp;
}

extern "C" int
lsemi_froidure_pin_transf16_add_generator(lsemi_froidure_pin_transf16* fp,
                                          const uint16_t*              image,
                                          size_t                       degree) {
  LSEMI_TRY
  std::vector<uint16_t> img(image, image + degree);
  fp->impl.add_generator(TransfU16(std::move(img)));
  LSEMI_CATCH
}

extern "C" int
lsemi_froidure_pin_transf16_run(lsemi_froidure_pin_transf16* fp) {
  LSEMI_TRY
  fp->impl.run();
  LSEMI_CATCH
}

extern "C" int lsemi_froidure_pin_transf16_size(lsemi_froidure_pin_transf16* fp,
                                                size_t* out) {
  LSEMI_TRY
  *out = fp->impl.size();
  LSEMI_CATCH
}

extern "C" int lsemi_froidure_pin_transf16_number_of_generators(
    lsemi_froidure_pin_transf16* fp,
    size_t*                      out) {
  LSEMI_TRY
  *out = fp->impl.number_of_generators();
  LSEMI_CATCH
}
