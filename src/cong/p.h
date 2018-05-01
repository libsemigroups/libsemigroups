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

  template <typename TElementType,
            typename TElementHash,
            typename TElementEqual>
    class Congruence::P : public DATA, ElementContainer<TElementType> {
     public:
      explicit P(Congruence& cong)
          : DATA(cong, 2000, 40000),
            _class_lookup(),
            _done(false),
            _found_pairs(),
            _lookup(0),
            _map(),
            _map_next(0),
            _next_class(0),
            _pairs_to_mult(),
            _reverse_map(),
            _tmp1(),
            _tmp2() {
        LIBSEMIGROUPS_ASSERT(cong._semigroup != nullptr);

        auto semigroup = static_cast<Semigroup<TElementType>*>(cong._semigroup);
        _tmp1 = this->copy(semigroup->gens(0));
        _tmp2 = this->copy(_tmp1);

        // Set up _pairs_to_mult
        for (relation_t const& rel : cong._extra) {
          TElementType x = semigroup->word_to_element(rel.first);
          TElementType y = semigroup->word_to_element(rel.second);
          add_pair(x, y);
          this->free(x);
          this->free(y);
        }
      }

      void delete_tmp_storage() {
        std::unordered_set<std::pair<TElementType, TElementType>,
                           PHash,
                           PEqual>()
            .swap(_found_pairs);
        std::queue<std::pair<TElementType, TElementType>>().swap(
            _pairs_to_mult);
      }

      ~P() {
        delete_tmp_storage();
        this->free(_tmp1);
        this->free(_tmp2);
        for (auto& x : _map) {
          this->free(x.first);
        }
      }

      bool is_done() const final {
        return _done;
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return this->_cong._semigroup->size() - _class_lookup.size()
               + _next_class;
      }

      class_index_t word_to_class_index(word_t const& w) final {
        LIBSEMIGROUPS_ASSERT(is_done());
        auto semigroup
            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        TElementType x     = semigroup->word_to_element(w);
        size_t       ind_x = get_index(x);
        this->free(x);
        LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[ind_x];
      }

      result_t current_equals(word_t const& w1, word_t const& w2) final {
        if (is_done()) {
          return word_to_class_index(w1) == word_to_class_index(w2) ? TRUE
                                                                    : FALSE;
        }
        auto semigroup
            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        TElementType x     = semigroup->word_to_element(w1);
        TElementType y     = semigroup->word_to_element(w2);
        size_t       ind_x = get_index(x);
        size_t       ind_y = get_index(y);
        this->free(x);
        this->free(y);
        return _lookup.find(ind_x) == _lookup.find(ind_y) ? TRUE : UNKNOWN;
      }

      Partition<word_t>* nontrivial_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_nontrivial_elms);
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_nontrivial_elms);

        Partition<word_t>* classes
            = new Partition<word_t>(_nr_nontrivial_classes);
        auto semigroup = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        for (size_t ind = 0; ind < _nr_nontrivial_elms; ind++) {
          word_t* word
              = semigroup->factorisation(_reverse_map[ind]);
          (*classes)[_class_lookup[ind]]->push_back(word);
        }
        return classes;
      }

      void run() final {
        run(this->_killed);
      }

      void run(size_t steps) final {
        run(steps, this->_killed);
      }

      void run(std::atomic<bool>& killed) {
        while (!killed && !is_done()) {
          run(Congruence::LIMIT_MAX, killed);
        }
      }

      void run(size_t steps, std::atomic<bool>& killed) {
        REPORT("number of steps = " << steps);
        size_t tid = glob_reporter.thread_id(std::this_thread::get_id());
        while (!_pairs_to_mult.empty()) {
          // Get the next pair
          std::pair<TElementType, TElementType> current_pair
              = _pairs_to_mult.front();

          _pairs_to_mult.pop();

          // Add its left and/or right multiples
          auto semigroup
              = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
          for (size_t i = 0; i < this->_cong._nrgens; i++) {
            TElementType gen = semigroup->gens(i);
            if (this->_cong._type == LEFT || this->_cong._type == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, gen, current_pair.first, tid);
              _tmp2 = this->multiply(_tmp2, gen, current_pair.second, tid);
              add_pair(_tmp1, _tmp2);
            }
            if (this->_cong._type == RIGHT || this->_cong._type == TWOSIDED) {
              _tmp1 = this->multiply(_tmp1, current_pair.first, gen, tid);
              _tmp2 = this->multiply(_tmp2, current_pair.second, gen, tid);
              add_pair(_tmp1, _tmp2);
            }
          }
          if (this->_report_next++ > this->_report_interval) {
            REPORT("found " << _found_pairs.size() << " pairs: " << _map_next
                            << " elements in "
                            << _lookup.nr_blocks()
                            << " classes, "
                            << _pairs_to_mult.size()
                            << " pairs on the stack");
            this->_report_next = 0;
            if (tid != 0 && this->_cong._semigroup->is_done()
                && _found_pairs.size() > this->_cong._semigroup->size()) {
              // If the congruence is only using 1 thread, then this will never
              // happen, if the congruence uses > 1 threads, then it is ok for
              // P to kill itself, because another thread will complete and
              // return the required DATA*.
              REPORT("too many pairs found, stopping");
              killed = true;
              return;
            }
          }
          if (killed) {
            REPORT("killed");
            return;
          }
          if (--steps == 0) {
            return;
          }
        }
        // Make a normalised class lookup (class numbers {0, .., n-1}, in order)
        if (_lookup.get_size() > 0) {
          _class_lookup.reserve(_lookup.get_size());
          _next_class = 1;
          size_t nr;
          size_t max = 0;
          LIBSEMIGROUPS_ASSERT(_lookup.find(0) == 0);
          _class_lookup.push_back(0);
          for (size_t i = 1; i < _lookup.get_size(); i++) {
            nr = _lookup.find(i);
            if (nr > max) {
              _class_lookup.push_back(_next_class++);
              max = nr;
            } else {
              _class_lookup.push_back(_class_lookup[nr]);
            }
          }
        }

        // Record information about non-trivial classes
        _nr_nontrivial_classes = _next_class;
        _nr_nontrivial_elms    = _map_next;

        if (!killed) {
          REPORT("finished with " << _found_pairs.size() << " pairs: "
                                  << _map_next
                                  << " elements in "
                                  << _lookup.nr_blocks()
                                  << " classes");
          _done = true;
          delete_tmp_storage();
        } else {
          REPORT("killed");
        }
      }

     private:
      struct PHash {
       public:
        size_t
        operator()(std::pair<TElementType, TElementType> const& pair) const {
          return TElementHash()(pair.first) + 17 * TElementHash()(pair.second);
        }
      };

      struct PEqual {
        size_t operator()(std::pair<TElementType, TElementType> pair1,
                          std::pair<TElementType, TElementType> pair2) const {
          return TElementEqual()(pair1.first, pair2.first)
                 && TElementEqual()(pair1.second, pair2.second);
        }
      };

      void add_pair(TElementType x, TElementType y) {
        if (!TElementEqual()(x, y)) {
          TElementType xx, yy;
          bool         xx_new = false, yy_new = false;
          size_t       i, j;

          auto it_x = _map.find(x);
          if (it_x == _map.end()) {
            xx_new = true;
            xx     = this->copy(x);
            i      = add_index(xx);
          } else {
            i = it_x->second;
          }

          auto it_y = _map.find(y);
          if (it_y == _map.end()) {
            yy_new = true;
            yy     = this->copy(y);
            j      = add_index(yy);
          } else {
            j = it_y->second;
          }

          LIBSEMIGROUPS_ASSERT(i != j);
          std::pair<TElementType, TElementType> pair;
          if (xx_new || yy_new) {  // it's a new pair
            xx   = (xx_new ? xx : it_x->first);
            yy   = (yy_new ? yy : it_y->first);
            pair = (i < j ? std::pair<TElementType, TElementType>(xx, yy)
                          : std::pair<TElementType, TElementType>(yy, xx));
          } else {
            pair = (i < j ? std::pair<TElementType, TElementType>(it_x->first,
                                                                  it_y->first)
                          : std::pair<TElementType, TElementType>(it_y->first,
                                                                  it_x->first));
            if (_found_pairs.find(pair) != _found_pairs.end()) {
              return;
            }
          }
          _found_pairs.insert(pair);
          _pairs_to_mult.push(pair);
          _lookup.unite(i, j);
        }
      }

      size_t get_index(TElementType x) {
        auto it = _map.find(x);
        if (it == _map.end()) {
          return add_index(this->copy(x));
        }
        return it->second;
      }

      size_t add_index(TElementType x) {
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() == _map_next);
        LIBSEMIGROUPS_ASSERT(_map.size() == _map_next);
        _map.emplace(x, _map_next);
        _reverse_map.push_back(x);
        _lookup.add_entry();
        if (_done) {
          _class_lookup.push_back(_next_class++);
        }
        return _map_next++;
      }

      std::vector<class_index_t> _class_lookup;
      bool                       _done;
      std::unordered_set<std::pair<TElementType, TElementType>, PHash, PEqual>
          _found_pairs;
      UF  _lookup;
      std::unordered_map<TElementType, size_t> _map;
      size_t        _map_next;
      class_index_t _next_class;
      size_t        _nr_nontrivial_classes;
      size_t        _nr_nontrivial_elms;
      std::queue<std::pair<TElementType, TElementType>> _pairs_to_mult;
      std::vector<TElementType> _reverse_map;
      TElementType              _tmp1;
      TElementType              _tmp2;
    };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_CONG_P_H_
