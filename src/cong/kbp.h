//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 James D. Mitchell
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

// This file contains the declaration for the private inner class of Congruence
// called KBP, which is a subclass of Congruence::DATA.  This class is for
// performing Knuth-Bendix followed by the P inner class of Congruence on the
// quotient.

#ifndef LIBSEMIGROUPS_SRC_CONG_KBP_H_
#define LIBSEMIGROUPS_SRC_CONG_KBP_H_

#include "../cong.h"
#include "../rws.h"
#include "../semigroups.h"
#include "p.h"

namespace libsemigroups {

  // Knuth-Bendix followed by the orbit on pairs algorithm
  class Congruence::KBP : public Congruence::DATA {
   public:
    explicit KBP(Congruence& cong)
        : DATA(cong, 200),
          _rws(new RWS()),
          _semigroup(nullptr),
          _P_cong(nullptr) {}

    ~KBP() {
      delete _rws;
      delete _semigroup;
      delete _P_cong;
    }

    void run() final;
    void run(size_t steps) final;

    bool is_done() const final {
      return (_semigroup != nullptr) && (_P_cong != nullptr)
             && (_P_cong->is_done());
    }

    size_t nr_classes() final {
      LIBSEMIGROUPS_ASSERT(is_done());
      return _P_cong->nr_classes();
    }

    class_index_t word_to_class_index(word_t const& word) final;
    result_t current_equals(word_t const& w1, word_t const& w2) final;
    Partition<word_t>* nontrivial_classes() final;

   private:
    void init();

    RWS*         _rws;
    Semigroup<>* _semigroup;
    Congruence*  _P_cong;
  };
}  // namespace libsemigroups

#endif  // LIBSEMIGROUPS_SRC_CONG_KBP_H_
