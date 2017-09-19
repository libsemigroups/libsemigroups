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

#include "semigroups.h"

#include <numeric>

#include "rwse.h"

namespace libsemigroups {

  Reporter glob_reporter;

  // Static data members
  Semigroup::index_t const Semigroup::UNDEFINED
      = std::numeric_limits<index_t>::max();
  Semigroup::index_t const Semigroup::LIMIT_MAX
      = std::numeric_limits<index_t>::max();

  std::vector<Semigroup::element_index_t> Semigroup::_tmp_inverter;
  std::vector<bool>                       Semigroup::_old_new;

  Semigroup::Semigroup(std::vector<Element const*> const* gens)
      : _batch_size(8192),
        _degree(UNDEFINED),
        _duplicate_gens(),
        _elements(),
        _enumerate_order(),
        _final(),
        _first(),
        _found_one(false),
        _gens(),
        _id(),
        _idempotents(),
        _idempotents_found(false),
        _is_idempotent(),
        _left(gens->size()),
        _length(),
        _lenindex(),
        _letter_to_pos(),
        _map(),
        _max_threads(std::thread::hardware_concurrency()),
        _nr(0),
        _nrgens(gens->size()),
        _nrrules(0),
        _pos(0),
        _pos_one(0),
        _prefix(),
        _reduced(gens->size()),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(gens->size()),
        _sorted(),
        _suffix(),
        _wordlen(0) {  // (length of the current word) - 1
    LIBSEMIGROUPS_ASSERT(_nrgens != 0);
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif
    _right.set_default_value(UNDEFINED);
    reserve(_nrgens);

    _degree = (*gens)[0]->degree();

    for (Element const* x : *gens) {
      LIBSEMIGROUPS_ASSERT(x->degree() == _degree);
      _gens.push_back(x->really_copy());
    }

    _tmp_product = _gens[0]->identity();
    _lenindex.push_back(0);
    _id = _gens[0]->identity();

    // add the generators
    for (letter_t i = 0; i < _nrgens; i++) {
      auto it = _map.find(_gens[i]);
      if (it != _map.end()) {  // duplicate generator
        _letter_to_pos.push_back(it->second);
        _nrrules++;
        _duplicate_gens.push_back(std::make_pair(i, _first[it->second]));
        // i.e. _gens[i] = _gens[_first[it->second]]
        // _first maps from element_index_t -> letter_t :)
      } else {
        is_one(_gens[i], _nr);
        _elements.push_back(_gens[i]);
        // Note that every non-duplicate generator is *really* stored in
        // _elements, and so must be *really_delete*d from _elements but not
        // _gens
        _first.push_back(i);
        _final.push_back(i);
        _enumerate_order.push_back(_nr);
        _letter_to_pos.push_back(_nr);
        _length.push_back(1);
        _map.insert(std::make_pair(_elements.back(), _nr));
        _prefix.push_back(UNDEFINED);
        // _prefix.push_back(_nr) and get rid of _letter_to_pos, and
        // the extra clause in the enumerate method!
        _suffix.push_back(UNDEFINED);
        _nr++;
      }
    }
    expand(_nr);
    _lenindex.push_back(_enumerate_order.size());
  }

  Semigroup::Semigroup(std::vector<Element const*> const& gens)
      : Semigroup(&gens) {}

  // Copy constructor
  Semigroup::Semigroup(Semigroup const& copy)
      : _batch_size(copy._batch_size),
        _degree(copy._degree),
        _duplicate_gens(copy._duplicate_gens),
        _elements(),
        _enumerate_order(copy._enumerate_order),
        _final(copy._final),
        _first(copy._first),
        _found_one(copy._found_one),
        _gens(),
        _id(copy._id->really_copy()),
        _idempotents(copy._idempotents),
        _idempotents_found(copy._idempotents_found),
        _is_idempotent(copy._is_idempotent),
        _left(copy._left),
        _length(copy._length),
        _lenindex(copy._lenindex),
        _letter_to_pos(copy._letter_to_pos),
        _max_threads(copy._max_threads),
        _nr(copy._nr),
        _nrgens(copy._nrgens),
        _nrrules(copy._nrrules),
        _pos(copy._pos),
        _pos_one(copy._pos_one),
        _prefix(copy._prefix),
        _reduced(copy._reduced),
        _relation_gen(copy._relation_gen),
        _relation_pos(copy._relation_pos),
        _right(copy._right),
        _sorted(),  // TODO(JDM) copy this if set
        _suffix(copy._suffix),
        _wordlen(copy._wordlen) {
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif
    _elements.reserve(_nr);
    _map.reserve(_nr);
    _tmp_product = copy._id->really_copy();

    element_index_t i = 0;
    for (Element const* x : copy._elements) {
      Element* y = x->really_copy();
      _elements.push_back(y);
      _map.insert(std::make_pair(y, i++));
    }
    copy_gens();
  }

  // Private - partial copy
  //
  // <add_generators> or <closure> should usually be called after this.
  Semigroup::Semigroup(Semigroup const&                   copy,
                       std::vector<Element const*> const* coll)
      : _batch_size(copy._batch_size),
        _degree(copy._degree),  // copy for comparison in add_generators
        _duplicate_gens(copy._duplicate_gens),
        _elements(),
        _found_one(copy._found_one),  // copy in case degree doesn't change in
                                      // add_generators
        _gens(),
        _idempotents(copy._idempotents),
        _idempotents_found(copy._idempotents_found),
        _is_idempotent(copy._is_idempotent),
        _left(copy._left),
        _letter_to_pos(copy._letter_to_pos),
        _max_threads(copy._max_threads),
        _nr(copy._nr),
        _nrgens(copy._nrgens),
        _nrrules(0),
        _pos(copy._pos),
        _pos_one(copy._pos_one),  // copy in case degree doesn't change in
                                  // add_generators
        _reduced(copy._reduced),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(copy._right),
        _sorted(),
        _wordlen(0) {
    LIBSEMIGROUPS_ASSERT(!coll->empty());
    LIBSEMIGROUPS_ASSERT(coll->at(0)->degree() >= copy.degree());

#ifdef LIBSEMIGROUPS_DEBUG
    for (Element const* x : *coll) {
      LIBSEMIGROUPS_ASSERT(x->degree() == (*coll)[0]->degree());
    }
#endif
#ifdef LIBSEMIGROUPS_STATS
    _nr_products = 0;
#endif

    _elements.reserve(copy._nr);
    _map.reserve(copy._nr);

    // the following are required for assignment to specific positions in
    // add_generators
    _final.resize(copy._nr, 0);
    _first.resize(copy._nr, 0);
    _length.resize(copy._nr, 0);
    _prefix.resize(copy._nr, 0);
    _suffix.resize(copy._nr, 0);

    size_t deg_plus = coll->at(0)->degree() - copy.degree();

    if (deg_plus != 0) {
      _degree += deg_plus;
      _found_one = false;
      _pos_one   = 0;
    }

    _lenindex.push_back(0);
    _lenindex.push_back(copy._lenindex[1]);
    _enumerate_order.reserve(copy._nr);

    // add the distinct old generators to new _enumerate_order
    for (enumerate_index_t i = 0; i < copy._lenindex[1]; i++) {
      _enumerate_order.push_back(copy._enumerate_order[i]);
      _final[_enumerate_order[i]]  = copy._final[copy._enumerate_order[i]];
      _first[_enumerate_order[i]]  = copy._first[copy._enumerate_order[i]];
      _prefix[_enumerate_order[i]] = UNDEFINED;
      _suffix[_enumerate_order[i]] = UNDEFINED;
      _length[_enumerate_order[i]] = 1;
    }

    _id          = coll->at(0)->identity();
    _tmp_product = copy._id->really_copy(deg_plus);

    element_index_t i = 0;
    for (Element const* x : copy._elements) {
      Element* y = x->really_copy(deg_plus);
      _elements.push_back(y);
      _map.insert(std::make_pair(y, i));
      is_one(y, i++);
    }
    copy_gens();  // copy the old generators
    // Now this is ready to have add_generators or closure called on it
  }

  // Destructor

  Semigroup::~Semigroup() {
    _tmp_product->really_delete();
    delete _tmp_product;

    const_cast<Element*>(_id)->really_delete();
    delete _id;

    // delete those generators not in _elements, i.e. the duplicate ones
    for (auto& x : _duplicate_gens) {
      const_cast<Element*>(_gens[x.first])->really_delete();
      delete _gens[x.first];
    }
    really_delete_cont(_elements);
  }

  void Semigroup::reserve(size_t n) {
    // Since the Semigroup we are enumerating is bounded in size by the maximum
    // value of an element_index_t, we cast the argument here to this integer
    // type.
    element_index_t nn = static_cast<element_index_t>(n);
    _elements.reserve(nn);
    _final.reserve(nn);
    _first.reserve(nn);
    _enumerate_order.reserve(nn);
    _length.reserve(nn);
    _map.reserve(nn);
    _prefix.reserve(nn);
    _suffix.reserve(nn);

    // TODO 1: the analogue of the below using RecVec::reserve, which currently
    // does not exist
    // if (n > _right.nr_rows()) {
    //  expand(n - _right.nr_rows());
    // }
    // TODO 2: could reserve memory for the Elements themselves too
  }

  // w is a word in the generators (i.e. a vector of letter_t's)
  Semigroup::element_index_t Semigroup::word_to_pos(word_t const& w) const {
    LIBSEMIGROUPS_ASSERT(w.size() > 0);
    if (w.size() == 1) {
      return letter_to_pos(w[0]);
    }
    element_index_t out = letter_to_pos(w[0]);
    for (auto it = w.begin() + 1; it < w.end(); it++) {
      LIBSEMIGROUPS_ASSERT(*it < nrgens());
      out = fast_product(out, letter_to_pos(*it));
    }
    return out;
  }

  Element* Semigroup::word_to_element(word_t const& w) const {
    LIBSEMIGROUPS_ASSERT(w.size() > 0);
    if (is_done() || w.size() == 1) {
      return _elements[word_to_pos(w)]->really_copy();
    }
    Element* out = _tmp_product->really_copy();
    out->redefine(_gens[w[0]], _gens[w[1]]);
    for (auto it = w.begin() + 2; it < w.end(); it++) {
      LIBSEMIGROUPS_ASSERT(*it < nrgens());
      _tmp_product->swap(out);
      out->redefine(_tmp_product, _gens[*it]);
    }
    return out;
  }

  // Product by tracing in the left or right Cayley graph
  Semigroup::element_index_t
  Semigroup::product_by_reduction(element_index_t i, element_index_t j) const {
    LIBSEMIGROUPS_ASSERT(i < _nr && j < _nr);
    if (length_const(i) <= length_const(j)) {
      while (i != UNDEFINED) {
        j = _left.get(j, _final[i]);
        i = _prefix[i];
      }
      return j;
    } else {
      while (j != UNDEFINED) {
        i = _right.get(i, _first[j]);
        j = _suffix[j];
      }
      return i;
    }
  }

  // Product by multiplying or by tracing in the Cayley graph whichever is
  // faster

  Semigroup::element_index_t Semigroup::fast_product(element_index_t i,
                                                     element_index_t j) const {
    LIBSEMIGROUPS_ASSERT(i < _nr && j < _nr);
    if (length_const(i) < 2 * _tmp_product->complexity()
        || length_const(j) < 2 * _tmp_product->complexity()) {
      return product_by_reduction(i, j);
    } else {
      _tmp_product->redefine(_elements[i], _elements[j]);
      return _map.find(_tmp_product)->second;
    }
  }

  // Get the number of idempotents
  size_t Semigroup::nridempotents() {
    init_idempotents();
    return _idempotents.size();
  }

  bool Semigroup::is_idempotent(element_index_t pos) {
    LIBSEMIGROUPS_ASSERT(pos < size());
    init_idempotents();
    return _is_idempotent[pos];
  }

  // Get the position of an element in the semigroup

  Semigroup::element_index_t Semigroup::position(Element const* x) {
    if (x->degree() != _degree) {
      return UNDEFINED;
    }

    while (true) {
      auto it = _map.find(x);
      if (it != _map.end()) {
        return it->second;
      }
      if (is_done()) {
        return UNDEFINED;
      }
      enumerate(_nr + 1);
      // _nr + 1 means we enumerate _batch_size more elements
    }
  }

  Semigroup::element_index_t
  Semigroup::position_to_sorted_position(element_index_t pos) {
    enumerate(LIMIT_MAX);
    if (pos >= _nr) {
      return UNDEFINED;
    }
    init_sorted();
    return _sorted[pos].second;
  }

  Semigroup::element_index_t Semigroup::sorted_position(Element const* x) {
    return position_to_sorted_position(position(x));
  }

  Element const* Semigroup::at(element_index_t pos) {
    enumerate(pos + 1);
    if (pos < _elements.size()) {
      return _elements[pos];
    } else {
      return nullptr;
    }
  }

  Element const* Semigroup::sorted_at(element_index_t pos) {
    init_sorted();
    if (pos < _sorted.size()) {
      return _sorted[pos].first;
    } else {
      return nullptr;
    }
  }

  word_t* Semigroup::minimal_factorisation(Element const* x) {
    element_index_t pos = this->position(x);
    if (pos == Semigroup::UNDEFINED) {
      return nullptr;
    }
    return factorisation(pos);
  }

  word_t* Semigroup::factorisation(Element const* x) {
    if (x->get_type() == Element::elm_t::RWSE) {
      return const_cast<word_t*>(RWS::rws_word_to_word(
          (reinterpret_cast<RWSE const*>(x))->get_rws_word()));
    }
    LIBSEMIGROUPS_ASSERT(x->get_type() == Element::elm_t::NOT_RWSE);
    return minimal_factorisation(x);
  }

  word_t* Semigroup::minimal_factorisation(element_index_t pos) {
    if (pos >= _nr && !is_done()) {
      enumerate(pos + 1);
    }

    if (pos >= _nr) {
      return nullptr;
    }
    word_t* word = new word_t();
    while (pos != UNDEFINED) {
      word->push_back(_first[pos]);
      pos = _suffix[pos];
    }
    return word;
  }

  void Semigroup::minimal_factorisation(word_t& word, element_index_t pos) {
    if (pos >= _nr && !is_done()) {
      enumerate(pos + 1);
    }

    if (pos < _nr) {
      word.clear();
      while (pos != UNDEFINED) {
        word.push_back(_first[pos]);
        pos = _suffix[pos];
      }
    }
  }

  void Semigroup::next_relation(word_t& relation) {
    if (!is_done()) {
      enumerate();
    }

    relation.clear();

    if (_relation_pos == _nr) {  // no more relations
      return;
    }

    if (_relation_pos != UNDEFINED) {
      while (_relation_pos < _nr) {
        while (_relation_gen < _nrgens) {
          if (!_reduced.get(_enumerate_order[_relation_pos], _relation_gen)
              && (_relation_pos < _lenindex[1]
                  || _reduced.get(_suffix[_enumerate_order[_relation_pos]],
                                  _relation_gen))) {
            relation.push_back(_enumerate_order[_relation_pos]);
            relation.push_back(_relation_gen);
            relation.push_back(
                _right.get(_enumerate_order[_relation_pos], _relation_gen));
            break;
          }
          _relation_gen++;
        }
        if (_relation_gen == _nrgens) {  // then relation is empty
          _relation_gen = 0;
          _relation_pos++;
        } else {
          break;
        }
      }
      _relation_gen++;
    } else {
      // duplicate generators
      if (_relation_gen < _duplicate_gens.size()) {
        relation.push_back(_duplicate_gens[_relation_gen].first);
        relation.push_back(_duplicate_gens[_relation_gen].second);
        _relation_gen++;
      } else {
        _relation_gen = 0;
        _relation_pos++;
        next_relation(relation);
      }
    }
  }

  void Semigroup::enumerate(std::atomic<bool>& killed, size_t limit_size_t) {
    _mtx.lock();
    if (_pos >= _nr || limit_size_t <= _nr || killed) {
      _mtx.unlock();
      return;
    }
    // Ensure that limit isn't too big
    index_t limit = static_cast<index_t>(limit_size_t);

    if (Semigroup::LIMIT_MAX - _batch_size > _nr) {
      limit = std::max(limit, _nr + _batch_size);
    } else {  // _batch_size is very big for some reason
      limit = _batch_size;
    }

    REPORT("limit = " << limit);
    Timer timer;
    timer.start();
    size_t tid = glob_reporter.thread_id(std::this_thread::get_id());

    // multiply the generators by every generator
    if (_pos < _lenindex[1]) {
      index_t nr_shorter_elements = _nr;
      while (_pos < _lenindex[1]) {
        element_index_t i = _enumerate_order[_pos];
        for (letter_t j = 0; j != _nrgens; ++j) {
          _tmp_product->redefine(_elements[i], _gens[j], tid);
#ifdef LIBSEMIGROUPS_STATS
          _nr_products++;
#endif
          auto it = _map.find(_tmp_product);

          if (it != _map.end()) {
            _right.set(i, j, it->second);
            _nrrules++;
          } else {
            is_one(_tmp_product, _nr);
            _elements.push_back(_tmp_product->really_copy());
            _first.push_back(_first[i]);
            _final.push_back(j);
            _enumerate_order.push_back(_nr);
            _length.push_back(2);
            _map.insert(std::make_pair(_elements.back(), _nr));
            _prefix.push_back(i);
            _reduced.set(i, j, true);
            _right.set(i, j, _nr);
            _suffix.push_back(_letter_to_pos[j]);
            _nr++;
          }
        }
        _pos++;
      }
      for (enumerate_index_t i = 0; i != _pos; ++i) {
        letter_t b = _final[_enumerate_order[i]];
        for (letter_t j = 0; j != _nrgens; ++j) {
          _left.set(_enumerate_order[i], j, _right.get(_letter_to_pos[j], b));
        }
      }
      _wordlen++;
      expand(_nr - nr_shorter_elements);
      _lenindex.push_back(_enumerate_order.size());
    }

    // multiply the words of length > 1 by every generator
    bool stop = (_nr >= limit || killed);

    while (_pos != _nr && !stop) {
      index_t nr_shorter_elements = _nr;
      while (_pos != _lenindex[_wordlen + 1] && !stop) {
        element_index_t i = _enumerate_order[_pos];
        letter_t        b = _first[i];
        element_index_t s = _suffix[i];
        for (letter_t j = 0; j != _nrgens; ++j) {
          if (!_reduced.get(s, j)) {
            element_index_t r = _right.get(s, j);
            if (_found_one && r == _pos_one) {
              _right.set(i, j, _letter_to_pos[b]);
            } else if (_prefix[r] != UNDEFINED) {  // r is not a generator
              _right.set(i, j, _right.get(_left.get(_prefix[r], b), _final[r]));
            } else {
              _right.set(i, j, _right.get(_letter_to_pos[b], _final[r]));
            }
          } else {
            _tmp_product->redefine(_elements[i], _gens[j], tid);
#ifdef LIBSEMIGROUPS_STATS
            _nr_products++;
#endif
            auto it = _map.find(_tmp_product);

            if (it != _map.end()) {
              _right.set(i, j, it->second);
              _nrrules++;
            } else {
              is_one(_tmp_product, _nr);
              _elements.push_back(_tmp_product->really_copy());
              _first.push_back(b);
              _final.push_back(j);
              _length.push_back(_wordlen + 2);
              _map.insert(std::make_pair(_elements.back(), _nr));
              _prefix.push_back(i);
              _reduced.set(i, j, true);
              _right.set(i, j, _nr);
              _suffix.push_back(_right.get(s, j));
              _enumerate_order.push_back(_nr);
              _nr++;
              stop = (_nr >= limit || killed);
            }
          }
        }  // finished applying gens to <_elements.at(_pos)>
        _pos++;
      }  // finished words of length <wordlen> + 1
      expand(_nr - nr_shorter_elements);

      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        for (enumerate_index_t i = _lenindex[_wordlen]; i != _pos; ++i) {
          element_index_t p = _prefix[_enumerate_order[i]];
          letter_t        b = _final[_enumerate_order[i]];
          for (letter_t j = 0; j != _nrgens; ++j) {
            _left.set(_enumerate_order[i], j, _right.get(_left.get(p, j), b));
          }
        }
        _wordlen++;
        _lenindex.push_back(_enumerate_order.size());
      }

      if (!is_done()) {
        REPORT("found " << _nr << " elements, " << _nrrules
                        << " rules, max word length "
                        << current_max_word_length()
                        << ", so far")
      } else {
        REPORT("found " << _nr << " elements, " << _nrrules
                        << " rules, max word length "
                        << current_max_word_length()
                        << ", finished")
      }
    }
    REPORT(timer.string("elapsed time = "));
    if (killed) {
      REPORT("killed");
    }
#ifdef LIBSEMIGROUPS_STATS
    REPORT("number of products = " << _nr_products);
#endif
    _mtx.unlock();
  }

  Semigroup* Semigroup::copy_closure(std::vector<Element const*> const* coll) {
    if (coll->empty()) {
      return new Semigroup(*this);
    } else {
      // The next line is required so that when we call the closure method on
      // out, the partial copy contains enough information to all membership
      // testing without a call to enumerate (which will fail because the
      // partial copy does not contain enough data to run enumerate).
      this->enumerate(LIMIT_MAX);
      // Partially copy
      Semigroup* out = new Semigroup(*this, coll);
      out->closure(coll);
      return out;
    }
  }

  void Semigroup::closure(std::vector<Element const*> const& coll) {
    closure(&coll);
  }

  void Semigroup::closure(std::vector<Element const*> const* coll) {
    if (coll->empty()) {
      return;
    } else {
      std::vector<Element const*> singleton(1, nullptr);

      for (auto const& x : *coll) {
        if (!test_membership(x)) {
          singleton[0] = x;
          add_generators(singleton);
        }
      }
    }
  }

  Semigroup* Semigroup::copy_add_generators(
      std::vector<Element const*> const* coll) const {
    if (coll->empty()) {
      return new Semigroup(*this);
    } else {
      // Partially copy
      Semigroup* out = new Semigroup(*this, coll);
      out->add_generators(coll);
      return out;
    }
  }

  void Semigroup::add_generators(std::vector<Element const*> const& coll) {
    add_generators(&coll);
  }

  void Semigroup::add_generators(std::vector<Element const*> const* coll) {
    if (coll->empty()) {
      return;
    }
    Timer timer;
    timer.start();
    size_t tid = glob_reporter.thread_id(std::this_thread::get_id());

    LIBSEMIGROUPS_ASSERT(degree() == (*coll->begin())->degree());

    // get some parameters from the old semigroup
    letter_t old_nrgens  = _nrgens;
    index_t  old_nr      = _nr;
    index_t  nr_old_left = _pos;

    // erase the old index
    _enumerate_order.erase(_enumerate_order.begin() + _lenindex[1],
                           _enumerate_order.end());

    // _old_new[i] indicates if we have seen _elements.at(i) yet in new.
    _old_new.clear();
    _old_new.resize(old_nr, false);
    for (letter_t i = 0; i < _letter_to_pos.size(); i++) {
      _old_new[_letter_to_pos[i]] = true;
    }

    // add the new generators to new _gens, _elements, and _enumerate_order
    for (Element const* x : *coll) {
      LIBSEMIGROUPS_ASSERT(x->degree() == degree());
      auto it = _map.find(x);
      if (it == _map.end()) {  // new generator
        _gens.push_back(x->really_copy());
        _elements.push_back(_gens.back());
        _map.insert(std::make_pair(_gens.back(), _nr));

        _first.push_back(_gens.size() - 1);
        _final.push_back(_gens.size() - 1);

        _letter_to_pos.push_back(_nr);
        _enumerate_order.push_back(_nr);

        is_one(x, _nr);
        _prefix.push_back(UNDEFINED);
        _suffix.push_back(UNDEFINED);
        _length.push_back(1);
        _nr++;
      } else if (_letter_to_pos[_first[it->second]] == it->second) {
        _gens.push_back(x->really_copy());
        // x is one of the existing generators
        _duplicate_gens.push_back(
            std::make_pair(_gens.size() - 1, _first[it->second]));
        // _gens[_gens.size() - 1] = _gens[_first[it->second])]
        // since _first maps element_index_t -> letter_t
        _letter_to_pos.push_back(it->second);
      } else {
        // x is an old element that will now be a generator
        _gens.push_back(_elements[it->second]);
        _letter_to_pos.push_back(it->second);
        _enumerate_order.push_back(it->second);

        _first[it->second]  = _gens.size() - 1;
        _final[it->second]  = _gens.size() - 1;
        _prefix[it->second] = UNDEFINED;
        _suffix[it->second] = UNDEFINED;
        _length[it->second] = UNDEFINED;

        _old_new[it->second] = true;
      }
    }

    // reset the data structure
    _idempotents_found = false;
    _nrrules           = _duplicate_gens.size();
    _pos               = 0;
    _wordlen           = 0;
    _nrgens            = _gens.size();
    _lenindex.clear();
    _lenindex.push_back(0);
    _lenindex.push_back(_nrgens - _duplicate_gens.size());

    // Add columns for new generators
    // FIXME isn't this a bit wasteful, we could recycle the old _reduced, to
    // avoid reallocation
    _reduced = flags_t(_nrgens, _reduced.nr_rows() + _nrgens - old_nrgens);
    _left.add_cols(_nrgens - _left.nr_cols());
    _right.add_cols(_nrgens - _right.nr_cols());

    // Add rows in for newly added generators
    _left.add_rows(_nrgens - old_nrgens);
    _right.add_rows(_nrgens - old_nrgens);

    index_t nr_shorter_elements;

    // Repeat until we have multiplied all of the elements of <old> up to the
    // old value of _pos by all of the (new and old) generators

    while (nr_old_left > 0) {
      nr_shorter_elements = _nr;
      while (_pos < _lenindex[_wordlen + 1] && nr_old_left > 0) {
        element_index_t i = _enumerate_order[_pos];  // position in _elements
        letter_t        b = _first[i];
        element_index_t s = _suffix[i];
        if (_right.get(i, 0) != UNDEFINED) {
          nr_old_left--;
          // _elements[i] is in old semigroup, and its descendants are
          // known
          for (letter_t j = 0; j < old_nrgens; j++) {
            element_index_t k = _right.get(i, j);
            if (!_old_new[k]) {  // it's new!
              is_one(_elements[k], k);
              _first[k]  = _first[i];
              _final[k]  = j;
              _length[k] = _wordlen + 2;
              _prefix[k] = i;
              _reduced.set(i, j, true);
              if (_wordlen == 0) {
                _suffix[k] = _letter_to_pos[j];
              } else {
                _suffix[k] = _right.get(s, j);
              }
              _enumerate_order.push_back(k);
              _old_new[k] = true;
            } else if (s == UNDEFINED || _reduced.get(s, j)) {
              // this clause could be removed if _nrrules wasn't necessary
              _nrrules++;
            }
          }
          for (letter_t j = old_nrgens; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_nr, tid);
          }
        } else {
          // _elements[i] is either not in old, or it is in old but its
          // descendants are not known
          for (letter_t j = 0; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_nr, tid);
          }
        }
        _pos++;
      }  // finished words of length <wordlen> + 1

      expand(_nr - nr_shorter_elements);
      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        if (_wordlen == 0) {
          for (enumerate_index_t i = 0; i < _pos; i++) {
            size_t b = _final[_enumerate_order[i]];
            for (letter_t j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left.set(
                  _enumerate_order[i], j, _right.get(_letter_to_pos[j], b));
            }
          }
        } else {
          for (enumerate_index_t i = _lenindex[_wordlen]; i < _pos; i++) {
            element_index_t p = _prefix[_enumerate_order[i]];
            letter_t        b = _final[_enumerate_order[i]];
            for (letter_t j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left.set(_enumerate_order[i], j, _right.get(_left.get(p, j), b));
            }
          }
        }
        _lenindex.push_back(_enumerate_order.size());
        _wordlen++;
      }

      if (!is_done()) {
        REPORT("found " << _nr << " elements, " << _nrrules
                        << " rules, max word length "
                        << current_max_word_length()
                        << ", so far")
      } else {
        REPORT("found " << _nr << " elements, " << _nrrules
                        << " rules, max word length "
                        << current_max_word_length()
                        << ", finished")
      }
    }
    REPORT(timer.string("elapsed time = "));
  }

  // Private methods

  // _nrgens, _duplicates_gens, _letter_to_pos, and _elements must all be
  // initialised for this to work, and _gens must point to an empty vector.
  void Semigroup::copy_gens() {
    LIBSEMIGROUPS_ASSERT(_gens.empty());
    _gens.resize(_nrgens, nullptr);
    // really copy duplicate gens from _elements
    for (auto const& x : _duplicate_gens) {
      // The degree of everything in _elements has already been increased (if
      // it needs to be at all), and so we do not need to increase the degree
      // in the copy below.
      _gens[x.first] = _elements[_letter_to_pos[x.second]]->really_copy();
    }
    // the non-duplicate gens are already in _elements, so don't really copy
    for (letter_t i = 0; i < _nrgens; i++) {
      if (_gens[i] == nullptr) {
        _gens[i] = _elements[_letter_to_pos[i]];
      }
    }
  }

  void inline Semigroup::closure_update(element_index_t i,
                                        letter_t        j,
                                        letter_t        b,
                                        element_index_t s,
                                        element_index_t old_nr,
                                        size_t const&   tid) {
    if (_wordlen != 0 && !_reduced.get(s, j)) {
      element_index_t r = _right.get(s, j);
      if (_found_one && r == _pos_one) {
        _right.set(i, j, _letter_to_pos[b]);
      } else if (_prefix[r] != UNDEFINED) {
        _right.set(i, j, _right.get(_left.get(_prefix[r], b), _final[r]));
      } else {
        _right.set(i, j, _right.get(_letter_to_pos[b], _final[r]));
      }
    } else {
      _tmp_product->redefine(_elements[i], _gens[j], tid);
      auto it = _map.find(_tmp_product);
      if (it == _map.end()) {  // it's new!
        is_one(_tmp_product, _nr);
        _elements.push_back(_tmp_product->really_copy());
        _first.push_back(b);
        _final.push_back(j);
        _length.push_back(_wordlen + 2);
        _map.insert(std::make_pair(_elements.back(), _nr));
        _prefix.push_back(i);
        _reduced.set(i, j, true);
        _right.set(i, j, _nr);
        if (_wordlen == 0) {
          _suffix.push_back(_letter_to_pos[j]);
        } else {
          _suffix.push_back(_right.get(s, j));
        }
        _enumerate_order.push_back(_nr);
        _nr++;
      } else if (it->second < old_nr && !_old_new[it->second]) {
        // we didn't process it yet!
        is_one(_tmp_product, it->second);
        _first[it->second]  = b;
        _final[it->second]  = j;
        _length[it->second] = _wordlen + 2;
        _prefix[it->second] = i;
        _reduced.set(i, j, true);
        _right.set(i, j, it->second);
        if (_wordlen == 0) {
          _suffix[it->second] = _letter_to_pos[j];
        } else {
          _suffix[it->second] = _right.get(s, j);
        }
        _enumerate_order.push_back(it->second);
        _old_new[it->second] = true;
      } else {  // it->second >= old->_nr || _old_new[it->second]
        // it's old
        _right.set(i, j, it->second);
        _nrrules++;
      }
    }
  }

  void Semigroup::init_sorted() {
    if (_sorted.size() == size()) {
      return;
    }
    size_t n = size();
    _sorted.reserve(n);
    for (element_index_t i = 0; i < n; i++) {
      _sorted.push_back(std::make_pair(_elements[i], i));
    }
    std::sort(_sorted.begin(),
              _sorted.end(),
              [](std::pair<Element const*, element_index_t> const& x,
                 std::pair<Element const*, element_index_t> const& y) -> bool {
                return *(x.first) < *(y.first);
              });

    // Invert the permutation in _sorted[*].second
    _tmp_inverter.resize(n);
    for (element_index_t i = 0; i < n; i++) {
      _tmp_inverter[_sorted[i].second] = i;
    }
    for (element_index_t i = 0; i < n; i++) {
      _sorted[i].second = _tmp_inverter[i];
    }
  }

  void Semigroup::init_idempotents() {
    if (_idempotents_found) {
      return;
    }
    _idempotents_found = true;
    enumerate();
    _is_idempotent.resize(_nr, false);

    Timer timer;
    timer.start();

    // Find the threshold beyond which it is quicker to simply multiply
    // elements rather than follow a path in the Cayley graph. This is the
    // enumerate_index_t i for which length(i) >= 2 * complexity.
    size_t complexity       = _tmp_product->complexity();
    size_t threshold_length = std::min(_lenindex.size() - 2, complexity - 1);
    enumerate_index_t threshold_index = _lenindex[threshold_length];

    size_t total_load = 0;
    for (size_t i = 1; i <= threshold_length; ++i) {
      total_load += i * (_lenindex[i] - _lenindex[i - 1]);
    }

#ifdef LIBSEMIGROUPS_STATS
    REPORT("complexity of multiplication = " << complexity);
    REPORT("multiple words longer than " << threshold_length + 1);
    REPORT("number of paths traced in Cayley graph = " << threshold_index);
    REPORT("mean path length = " << total_load / threshold_index);
    REPORT("number of products = " << _nr - threshold_index);
#endif

    total_load += complexity * (_nr - _lenindex[threshold_length - 1]);

    size_t concurrency_threshold = 823543;

    if (_max_threads == 1 || size() < concurrency_threshold) {
      // Use only 1 thread
      idempotents(0, _nr, threshold_index, _idempotents);
    } else {
      // Use > 1 threads
      size_t                         mean_load = total_load / _max_threads;
      size_t                         len       = 1;
      std::vector<enumerate_index_t> first(_max_threads, 0);
      std::vector<enumerate_index_t> last(_max_threads, _nr);
      std::vector<std::vector<idempotent_value_t>> tmp(
          _max_threads, std::vector<idempotent_value_t>());
      std::vector<std::thread> threads;
      glob_reporter.reset_thread_ids();

      for (size_t i = 0; i < _max_threads - 1; i++) {
        size_t thread_load = 0;
        last[i]            = first[i];
        while (thread_load < mean_load && last[i] < threshold_index) {
          if (last[i] >= _lenindex[len]) {
            ++len;
          }
          thread_load += len;
          ++last[i];
        }
        while (thread_load < mean_load) {
          thread_load += complexity;
          ++last[i];
        }
        total_load -= thread_load;
        REPORT("thread " << i + 1 << " has load " << thread_load)
        first[i + 1] = last[i];

        threads.push_back(std::thread(&Semigroup::idempotents,
                                      this,
                                      first[i],
                                      last[i],
                                      threshold_index,
                                      std::ref(tmp[i])));
      }
      // TODO use less threads if the av_load is too low

      REPORT("thread " << _max_threads << " has load " << total_load)
      threads.push_back(std::thread(&Semigroup::idempotents,
                                    this,
                                    first[_max_threads - 1],
                                    last[_max_threads - 1],
                                    threshold_index,
                                    std::ref(tmp[_max_threads - 1])));

      size_t nridempotents = 0;
      for (size_t i = 0; i < _max_threads; i++) {
        threads[i].join();
        nridempotents += tmp[i].size();
      }
      _idempotents.reserve(nridempotents);
      for (size_t i = 0; i < _max_threads; i++) {
        _idempotents.insert(_idempotents.end(), tmp[i].begin(), tmp[i].end());
      }
    }
    REPORT(timer.string("elapsed time = "));
  }

  void Semigroup::idempotents(enumerate_index_t const          first,
                              enumerate_index_t const          last,
                              enumerate_index_t const          threshold,
                              std::vector<idempotent_value_t>& idempotents) {
    REPORT("first = " << first << ", last = " << last << ", diff = "
                      << last - first);
    Timer timer;
    timer.start();

    enumerate_index_t pos = first;

    for (; pos < std::min(threshold, last); pos++) {
      element_index_t k = _enumerate_order[pos];
      if (!_is_idempotent[k]) {
        // The following is product_by_reduction, don't have to consider lengths
        // because they are equal!!
        element_index_t i = k, j = k;
        while (j != UNDEFINED) {
          i = _right.get(i, _first[j]);
          // TODO improve this if R/L-classes are known to stop performing the
          // product if we fall out of the R/L-class of the initial element.
          j = _suffix[j];
        }
        if (i == k) {
          idempotents.push_back(std::make_pair(_elements[k], k));
          _is_idempotent[k] = true;
        }
      }
    }

    if (pos >= last) {
      REPORT(timer.string("elapsed time = "));
      return;
    }

    // Cannot use _tmp_product itself since there are multiple threads here!
    Element* tmp_product = _tmp_product->really_copy();
    size_t   tid         = glob_reporter.thread_id(std::this_thread::get_id());

    for (; pos < last; pos++) {
      element_index_t k = _enumerate_order[pos];
      if (!_is_idempotent[k]) {
        tmp_product->redefine(_elements[k], _elements[k], tid);
        if (*tmp_product == *_elements[k]) {
          idempotents.push_back(std::make_pair(_elements[k], k));
          _is_idempotent[k] = true;
        }
      }
    }
    tmp_product->really_delete();
    delete tmp_product;
    REPORT(timer.string("elapsed time = "));
  }
}  // namespace libsemigroups
