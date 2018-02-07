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
// called TC, which is a subclass of Congruence::DATA.  This class is for
// performing the Todd-Coxeter algorithm.

#ifndef LIBSEMIGROUPS_SRC_CONG_TC_H_
#define LIBSEMIGROUPS_SRC_CONG_TC_H_

#include <stack>
#include <vector>

#include "../cong.h"

namespace libsemigroups {

  class Congruence::TC : public Congruence::DATA {
    typedef int64_t signed_class_index_t;

   public:
    explicit TC(Congruence& cong);
    ~TC() {}

    void run() final;
    void run(size_t steps) final;

    bool is_done() const final {
      return _tc_done;
    }

    size_t nr_classes() final {
      LIBSEMIGROUPS_ASSERT(is_done());
      return _active - 1;
    }

    class_index_t word_to_class_index(word_t const& word) final;
    result_t current_equals(word_t const& w1, word_t const& w2) final;

    // This method compresses the coset table used by <todd_coxeter>.
    void compress();

    void prefill();  // no args means use the semigroup used to define this
    void prefill(RecVec<class_index_t>& table);

    void set_pack(size_t val) override {
      _pack = val;
    }

   private:
    void init();
    void init_after_prefill();
    void init_tc_relations();

    void        new_coset(class_index_t const&, letter_t const&);
    void        identify_cosets(class_index_t, class_index_t);
    inline void trace(class_index_t const&, relation_t const&, bool add = true);

    size_t                            _active;  // Number of active cosets
    std::vector<signed_class_index_t> _bckwd;
    size_t                            _cosets_killed;
    class_index_t                     _current;
    class_index_t                     _current_no_add;
    size_t                            _defined;
    std::vector<relation_t>           _extra;
    std::vector<class_index_t>        _forwd;
    class_index_t                     _id_coset;   // TODO(JDM) Remove?
    bool                              _init_done;  // Has init() been run yet?
    class_index_t                     _last;
    std::stack<class_index_t> _lhs_stack;  // Stack for identifying cosets
    class_index_t             _next;
    size_t                    _pack;  // Nr of active cosets allowed before a
                                      // packing phase starts
    bool                      _prefilled;
    RecVec<class_index_t>     _preim_init;
    RecVec<class_index_t>     _preim_next;
    std::vector<relation_t>   _relations;
    std::stack<class_index_t> _rhs_stack;  // Stack for identifying cosets
    size_t                    _steps;
    size_t                    _stop_packing;  // TODO(JDM): make this a bool?
    RecVec<class_index_t>     _table;
    bool                      _tc_done;  // Has Todd-Coxeter been completed?
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_CONG_TC_H_
