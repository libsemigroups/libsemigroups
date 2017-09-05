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

// TODO: A configurable memory limit

#include "p.h"

#include <utility>

namespace libsemigroups {

  Congruence::P::P(Congruence& cong)
      : DATA(cong, 2000, 40000),
        _class_lookup(),
        _done(false),
        _found_pairs(new std::unordered_set<p_pair_const_t, PHash, PEqual>()),
        _lookup(0),
        _map(),
        _map_next(0),
        _next_class(0),
        _pairs_to_mult(new std::queue<p_pair_const_t>()),
        _reverse_map(),
        _tmp1(nullptr),
        _tmp2(nullptr) {
    LIBSEMIGROUPS_ASSERT(cong._semigroup != nullptr);

    _tmp1 = cong._semigroup->gens(0)->really_copy();
    _tmp2 = _tmp1->really_copy();

    // Set up _pairs_to_mult
    for (relation_t const& rel : cong._extra) {
      Element* x = cong._semigroup->word_to_element(rel.first);
      Element* y = cong._semigroup->word_to_element(rel.second);
      add_pair(x, y);
      x->really_delete();
      y->really_delete();
      delete x;
      delete y;
    }
  }

  Congruence::P::~P() {
    delete_tmp_storage();
    for (auto& x : _map) {
      const_cast<Element*>(x.first)->really_delete();
      delete x.first;
    }
  }

  void Congruence::P::run() {
    run(_killed);
  }

  void Congruence::P::run(std::atomic<bool>& killed) {
    while (!killed && !is_done()) {
      run(Congruence::LIMIT_MAX, killed);
    }
  }

  // This cannot currently be tested
  void Congruence::P::run(size_t steps) {
    run(steps, _killed);
  }

  void Congruence::P::run(size_t steps, std::atomic<bool>& killed) {
    REPORT("number of steps = " << steps);
    size_t tid = glob_reporter.thread_id(std::this_thread::get_id());
    while (!_pairs_to_mult->empty()) {
      // Get the next pair
      p_pair_const_t current_pair = _pairs_to_mult->front();

      _pairs_to_mult->pop();

      // Add its left and/or right multiples
      for (size_t i = 0; i < _cong._nrgens; i++) {
        Element const* gen = _cong._semigroup->gens(i);
        if (_cong._type == LEFT || _cong._type == TWOSIDED) {
          _tmp1->redefine(gen, current_pair.first, tid);
          _tmp2->redefine(gen, current_pair.second, tid);
          add_pair(_tmp1, _tmp2);
        }
        if (_cong._type == RIGHT || _cong._type == TWOSIDED) {
          _tmp1->redefine(current_pair.first, gen, tid);
          _tmp2->redefine(current_pair.second, gen, tid);
          add_pair(_tmp1, _tmp2);
        }
      }
      if (_report_next++ > _report_interval) {
        REPORT("found " << _found_pairs->size() << " pairs: " << _map_next
                        << " elements in "
                        << _lookup.nr_blocks()
                        << " classes, "
                        << _pairs_to_mult->size()
                        << " pairs on the stack");
        _report_next = 0;
        if (tid != 0 && _cong._semigroup->is_done()
            && _found_pairs->size() > _cong._semigroup->size()) {
          // If the congruence is only using 1 thread, then this will never
          // happen, if the congruence uses > 1 threads, then it is ok for P to
          // kill itself, because another thread will complete and return the
          // required DATA*.
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
      for (p_index_t i = 1; i < _lookup.get_size(); i++) {
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
      REPORT("finished with " << _found_pairs->size() << " pairs: " << _map_next
                              << " elements in "
                              << _lookup.nr_blocks()
                              << " classes");
      _done = true;
      delete_tmp_storage();
    } else {
      REPORT("killed");
    }
  }

  void Congruence::P::delete_tmp_storage() {
    delete _found_pairs;
    _found_pairs = nullptr;

    delete _pairs_to_mult;
    _pairs_to_mult = nullptr;

    if (_tmp1 != nullptr) {
      _tmp1->really_delete();
      delete _tmp1;
      _tmp1 = nullptr;
    }
    if (_tmp2 != nullptr) {
      _tmp2->really_delete();
      delete _tmp2;
      _tmp2 = nullptr;
    }
  }

  void Congruence::P::add_pair(Element const* x, Element const* y) {
    if (!(*x == *y)) {
      Element const *xx = nullptr, *yy = nullptr;
      p_index_t      i, j;

      auto it_x = _map.find(x);
      if (it_x == _map.end()) {
        xx = x->really_copy();
        i  = add_index(xx);
      } else {
        i = it_x->second;
      }

      auto it_y = _map.find(y);
      if (it_y == _map.end()) {
        yy = y->really_copy();
        j  = add_index(yy);
      } else {
        j = it_y->second;
      }

      LIBSEMIGROUPS_ASSERT(i != j);
      p_pair_const_t pair;
      if (xx != nullptr || yy != nullptr) {  // it's a new pair
        xx   = (xx == nullptr ? it_x->first : xx);
        yy   = (yy == nullptr ? it_y->first : yy);
        pair = (i < j ? p_pair_const_t(xx, yy) : p_pair_const_t(yy, xx));
      } else {
        pair = (i < j ? p_pair_const_t(it_x->first, it_y->first)
                      : p_pair_const_t(it_y->first, it_x->first));
        if (_found_pairs->find(pair) != _found_pairs->end()) {
          return;
        }
      }
      _found_pairs->insert(pair);
      _pairs_to_mult->push(pair);
      _lookup.unite(i, j);
    }
  }

  Congruence::P::p_index_t Congruence::P::get_index(Element const* x) {
    auto it = _map.find(x);
    if (it == _map.end()) {
      return add_index(x->really_copy());
    }
    return it->second;
  }

  Congruence::P::p_index_t Congruence::P::add_index(Element const* x) {
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

  size_t Congruence::P::nr_classes() {
    LIBSEMIGROUPS_ASSERT(is_done());
    return _cong._semigroup->size() - _class_lookup.size() + _next_class;
  }

  Congruence::class_index_t
  Congruence::P::word_to_class_index(word_t const& w) {
    LIBSEMIGROUPS_ASSERT(is_done());

    Element*  x     = _cong._semigroup->word_to_element(w);
    p_index_t ind_x = get_index(x);
    x->really_delete();
    delete x;
    LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
    LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
    return _class_lookup[ind_x];
  }

  Congruence::DATA::result_t Congruence::P::current_equals(word_t const& w1,
                                                           word_t const& w2) {
    if (is_done()) {
      return word_to_class_index(w1) == word_to_class_index(w2)
                 ? result_t::TRUE
                 : result_t::FALSE;
    }
    Element*  x     = _cong._semigroup->word_to_element(w1);
    Element*  y     = _cong._semigroup->word_to_element(w2);
    p_index_t ind_x = get_index(x);
    p_index_t ind_y = get_index(y);
    x->really_delete();
    y->really_delete();
    delete x;
    delete y;
    return _lookup.find(ind_x) == _lookup.find(ind_y) ? result_t::TRUE
                                                      : result_t::UNKNOWN;
  }

  Partition<word_t>* Congruence::P::nontrivial_classes() {
    LIBSEMIGROUPS_ASSERT(is_done());
    LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_nontrivial_elms);
    LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_nontrivial_elms);

    Partition<word_t>* classes = new Partition<word_t>(_nr_nontrivial_classes);

    for (p_index_t ind = 0; ind < _nr_nontrivial_elms; ind++) {
      Element* elm  = const_cast<Element*>(_reverse_map[ind]);
      word_t*  word = _cong._semigroup->factorisation(elm);
      (*classes)[_class_lookup[ind]]->push_back(word);
    }
    return classes;
  }
}  // namespace libsemigroups
