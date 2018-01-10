//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2017 Michael Torpey
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

// This file contains implementations for the private inner class of Congruence
// called P, which is a subclass of Congruence::DATA.  This class is for
// performing an exhaustive enumeration of pairs of elements belonging to a
// congruence. It is intended that this runs before the underlying semigroup is
// fully enumerated, and when the congruence contains a very small number of
// related pairs.

#ifndef LIBSEMIGROUPS_SRC_CONG_P_H_
#define LIBSEMIGROUPS_SRC_CONG_P_H_

#include <queue>
#include <utility>
#include <vector>

#include "../cong.h"
#include "../uf.h"

namespace libsemigroups {
  class Congruence::P : public DATA {
    // Index of an element in the UF table
    typedef size_t p_index_t;
    // A generating pair of the congruence
    typedef std::pair<Element const*, Element const*> p_pair_const_t;
    typedef std::pair<Element*, Element*>             p_pair_t;

   public:
    explicit P(Congruence& cong);
    ~P();

    bool is_done() const final {
      return _done;
    }

    size_t nr_classes() final;

    class_index_t word_to_class_index(word_t const& word) final;
    result_t current_equals(word_t const& w1, word_t const& w2) final;

    Partition<word_t>* nontrivial_classes() final;

    void run() final;
    void run(size_t steps) final;
    void run(std::atomic<bool>& killed);
    void run(size_t steps, std::atomic<bool>& killed);

   private:
    struct PHash {
     public:
      size_t operator()(p_pair_const_t const& pair) const {
        return pair.first->hash_value() + 17 * pair.second->hash_value();
      }
    };

    struct PEqual {
      size_t operator()(p_pair_const_t pair1, p_pair_const_t pair2) const {
        return *pair1.first == *pair2.first && *pair1.second == *pair2.second;
      }
    };

    void add_pair(Element const* x, Element const* y);

    void delete_tmp_storage();

    p_index_t get_index(Element const* x);
    p_index_t add_index(Element const* x);

    std::vector<class_index_t> _class_lookup;
    bool                       _done;
    std::unordered_set<p_pair_const_t, PHash, PEqual>* _found_pairs;
    UF _lookup;
    std::unordered_map<Element const*, p_index_t>
                                _map;
    p_index_t                   _map_next;
    class_index_t               _next_class;
    p_index_t                   _nr_nontrivial_classes;
    p_index_t                   _nr_nontrivial_elms;
    std::queue<p_pair_const_t>* _pairs_to_mult;
    std::vector<Element const*> _reverse_map;
    Element*                    _tmp1;
    Element*                    _tmp2;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_CONG_P_H_
