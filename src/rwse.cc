//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2016 James D. Mitchell
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

#include "rwse.h"

#include <algorithm>
#include <string>

namespace libsemigroups {
  bool RWSE::operator<(Element const& that) const {
    LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
    LIBSEMIGROUPS_ASSERT(static_cast<RWSE const&>(that)._rws_word != nullptr);
    rws_word_t u = *(this->_rws_word);
    rws_word_t v = *(static_cast<RWSE const&>(that)._rws_word);
    if (u != v && (u.size() < v.size() || (u.size() == v.size() && u < v))) {
      // TODO allow other reduction orders here
      return true;
    } else {
      return false;
    }
  }

  Element* RWSE::really_copy(size_t increase_deg_by) const {
    LIBSEMIGROUPS_ASSERT(increase_deg_by == 0);
    LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
    (void) increase_deg_by;  // to keep the compiler happy
    rws_word_t* rws_word(new rws_word_t(*(this->_rws_word)));
    return new RWSE(_rws, rws_word, false);
  }

  void RWSE::copy(Element const* x) {
    LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
    RWSE const* xx(static_cast<RWSE const*>(x));
    LIBSEMIGROUPS_ASSERT(xx->_rws_word != nullptr);
    _rws_word->assign(xx->_rws_word->cbegin(), xx->_rws_word->cend());
    reset_hash_value();
  }

  void RWSE::swap(Element* x) {
    LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
    RWSE* xx = static_cast<RWSE*>(x);
    LIBSEMIGROUPS_ASSERT(xx->_rws_word != nullptr);
    _rws_word->swap(*(xx->_rws_word));
    std::swap(this->_hash_value, xx->_hash_value);
  }

  void RWSE::redefine(Element const* x, Element const* y) {
    RWSE const* xx = static_cast<RWSE const*>(x);
    RWSE const* yy = static_cast<RWSE const*>(y);
    LIBSEMIGROUPS_ASSERT(xx->_rws_word != nullptr);
    LIBSEMIGROUPS_ASSERT(yy->_rws_word != nullptr);
    LIBSEMIGROUPS_ASSERT(_rws_word != nullptr);
    LIBSEMIGROUPS_ASSERT(xx->_rws == yy->_rws);
    _rws_word->clear();
    _rws_word->append(*(xx->_rws_word));
    _rws_word->append(*(yy->_rws_word));
    _rws->rewrite(_rws_word);
    this->reset_hash_value();
  }
}  // namespace libsemigroups
