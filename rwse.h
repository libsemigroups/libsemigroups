//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#ifndef LIBSEMIGROUPS_RWSE_H_
#define LIBSEMIGROUPS_RWSE_H_

#include <algorithm>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "elements.h"
#include "rws.h"

namespace libsemigroups {

  // This class is just a wrapper for an rws_word_t.
  class RWSE : public Element {
   private:
    RWSE(RWS* rws, rws_word_t* w, bool reduce, size_t hv)
        : Element(hv), _rws(rws), _rws_word(w) {
      if (reduce) {
        rws_word_t buf;
        _rws->rewrite(_rws_word, buf);
      }
    }

   public:
    RWSE(RWS* rws, rws_word_t* w) : RWSE(rws, w, true, Element::UNDEFINED) {}

    RWSE(RWS& rws, rws_word_t const& w) : RWSE(&rws, new rws_word_t(w)) {}

    RWSE(RWS& rws, letter_t a)
        : RWSE(&rws, new rws_word_t(RWS::letter_to_rws_word(a))) {}

    RWSE(RWS& rws, word_t w)
        : RWSE(&rws, new rws_word_t(RWS::word_to_rws_word(w))) {}

    bool operator==(Element const& that) const override {
      return *(static_cast<RWSE const&>(that)._rws_word) == *(this->_rws_word);
    }

    bool operator<(const Element& that) const override;

    Element* really_copy(size_t increase_deg_by) const override;
    void copy(Element const* x) override;

    void really_delete() override {
      delete _rws_word;
    }

    size_t complexity() const override {
      return Semigroup::LIMIT_MAX;
    }

    size_t degree() const override {
      return 0;  // TODO(JDM): Ok?
    }

    Element* identity() const override {
      return new RWSE(_rws, new rws_word_t());
    }

    void cache_hash_value() const override {
      this->_hash_value = std::hash<rws_word_t>()(*_rws_word);
    }

    void redefine(Element const* x,
                  Element const* y,
                  size_t const&  thread_id) override;

   private:
    // TODO const!
    RWS*              _rws;
    rws_word_t*       _rws_word;
    static std::vector<rws_word_t> _buf;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_RWSE_H_
