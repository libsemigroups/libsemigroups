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
#include "rwse.h"

namespace libsemigroups {

  Reporter glob_reporter;

  // Static data members
  Semigroup::pos_t const Semigroup::UNDEFINED =
      std::numeric_limits<size_t>::max();
  Semigroup::pos_t const Semigroup::LIMIT_MAX =
      std::numeric_limits<size_t>::max();

  Semigroup::Semigroup(std::vector<Element*> const* gens)
      : _batch_size(8192),
        _degree(UNDEFINED),
        _duplicate_gens(),
        _elements(new std::vector<Element*>()),
        _final(),
        _first(),
        _found_one(false),
        _gens(new std::vector<Element*>()),
        _id(),
        _idempotents(),
        _idempotents_found(false),
        _idempotents_start_pos(0),
        _is_idempotent(),
        _index(),
        _left(new cayley_graph_t(gens->size())),
        _length(),
        _lenindex(),
        _letter_to_pos(),
        _map(),
        _max_threads(std::thread::hardware_concurrency()),
        _multiplied(),
        _nr(0),
        _nrgens(gens->size()),
        _nr_idempotents(0),
        _nrrules(0),
        _pos(0),
        _pos_one(0),
        _pos_sorted(nullptr),
        _prefix(),
        _reduced(gens->size()),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(new cayley_graph_t(gens->size())),
        _sorted(nullptr),
        _suffix(),
        _wordlen(0) {  // (length of the current word) - 1
    assert(_nrgens != 0);

    reserve(_nrgens);

    _degree = (*gens)[0]->degree();

    for (Element* x : *gens) {
      assert(x->degree() == _degree);
      _gens->push_back(x->really_copy());
    }

    _tmp_product = (*_gens)[0]->identity();
    _lenindex.push_back(0);
    _id = (*_gens)[0]->identity();

    // add the generators
    for (size_t i = 0; i < _nrgens; i++) {
      auto it = _map.find((*_gens)[i]);
      if (it != _map.end()) {  // duplicate generator
        _letter_to_pos.push_back(it->second);
        _nrrules++;
        _duplicate_gens.push_back(std::make_pair(i, _first[it->second]));
        // i.e. _gens[i] = _gens[_first[it->second]]
        // _first maps from pos_t -> letter_t :)
      } else {
        is_one((*_gens)[i], _nr);
        _elements->push_back((*_gens)[i]);
        // Note that every non-duplicate generator is *really* stored in
        // _elements, and so must be *really_delete*d from _elements but not
        // _gens
        _first.push_back(i);
        _final.push_back(i);
        _index.push_back(_nr);
        _letter_to_pos.push_back(_nr);
        _length.push_back(1);
        _map.insert(std::make_pair(_elements->back(), _nr));
        _prefix.push_back(UNDEFINED);
        _suffix.push_back(UNDEFINED);
        _nr++;
      }
    }
    expand(_nr);
    _lenindex.push_back(_index.size());
  }

  Semigroup::Semigroup(std::vector<Element*> const& gens) : Semigroup(&gens) {}

  // Copy constructor

  Semigroup::Semigroup(const Semigroup& copy)
      : _batch_size(copy._batch_size),
        _degree(copy._degree),
        _duplicate_gens(copy._duplicate_gens),
        _elements(new std::vector<Element*>()),
        _final(copy._final),
        _first(copy._first),
        _found_one(copy._found_one),
        _gens(new std::vector<Element*>()),
        _id(copy._id->really_copy()),
        _idempotents(copy._idempotents),
        _idempotents_found(copy._idempotents_found),
        _idempotents_start_pos(copy._idempotents_start_pos),
        _is_idempotent(copy._is_idempotent),
        _index(copy._index),
        _left(new cayley_graph_t(*copy._left)),
        _length(copy._length),
        _lenindex(copy._lenindex),
        _letter_to_pos(copy._letter_to_pos),
        _max_threads(copy._max_threads),
        _multiplied(copy._multiplied),
        _nr(copy._nr),
        _nrgens(copy._nrgens),
        _nr_idempotents(copy._nr_idempotents),
        _nrrules(copy._nrrules),
        _pos(copy._pos),
        _pos_one(copy._pos_one),
        _pos_sorted(nullptr),  // TODO(JDM) copy this if set
        _prefix(copy._prefix),
        _reduced(copy._reduced),
        _relation_gen(copy._relation_gen),
        _relation_pos(copy._relation_pos),
        _right(new cayley_graph_t(*copy._right)),
        _sorted(nullptr),  // TODO(JDM) copy this if set
        _suffix(copy._suffix),
        _wordlen(copy._wordlen) {
    _elements->reserve(_nr);
    _map.reserve(_nr);
    _tmp_product = copy._id->really_copy();

    size_t i = 0;
    for (Element const* x : *(copy._elements)) {
      Element* y = x->really_copy();
      _elements->push_back(y);
      _map.insert(std::make_pair(y, i++));
    }
    copy_gens();
  }

  // Private - partial copy
  //
  // <add_generators> or <closure> should usually be called after this.
  Semigroup::Semigroup(Semigroup const& copy, std::vector<Element*> const* coll)
      // TODO(JDM) Element const*
      : _batch_size(copy._batch_size),
        _degree(copy._degree),  // copy for comparison in add_generators
        _duplicate_gens(copy._duplicate_gens),
        _elements(new std::vector<Element*>()),
        _found_one(copy._found_one),  // copy in case degree doesn't change in
                                      // add_generators
        _gens(new std::vector<Element*>()),
        _idempotents(copy._idempotents),
        _idempotents_found(copy._idempotents_found),
        _idempotents_start_pos(copy._idempotents_start_pos),
        _is_idempotent(copy._is_idempotent),
        _left(new cayley_graph_t(*copy._left)),
        _letter_to_pos(copy._letter_to_pos),
        _max_threads(copy._max_threads),
        _multiplied(copy._multiplied),
        _nr(copy._nr),
        _nrgens(copy._nrgens),
        _nr_idempotents(copy._nr_idempotents),
        _nrrules(0),
        _pos(copy._pos),
        _pos_one(copy._pos_one),  // copy in case degree doesn't change in
                                  // add_generators
        _pos_sorted(nullptr),
        _reduced(copy._reduced),
        _relation_gen(0),
        _relation_pos(UNDEFINED),
        _right(new cayley_graph_t(*copy._right)),
        _sorted(nullptr),
        _wordlen(0) {
    assert(!coll->empty());
    assert(coll->at(0)->degree() >= copy.degree());

#ifdef DEBUG
    for (Element* x : *coll) {
      assert(x->degree() == (*coll)[0]->degree());
    }
#endif

    _elements->reserve(copy._nr);
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
    _index.reserve(copy._nr);

    // add the distinct old generators to new _index
    for (size_t i = 0; i < copy._lenindex[1]; i++) {
      _index.push_back(copy._index[i]);
      _final[_index[i]]  = copy._final[copy._index[i]];
      _first[_index[i]]  = copy._first[copy._index[i]];
      _prefix[_index[i]] = UNDEFINED;
      _suffix[_index[i]] = UNDEFINED;
      _length[_index[i]] = 1;
    }

    _id          = coll->at(0)->identity();
    _tmp_product = copy._id->really_copy(deg_plus);

    size_t i = 0;
    for (Element const* x : *(copy._elements)) {
      Element* y = x->really_copy(deg_plus);
      _elements->push_back(y);
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

    _id->really_delete();
    delete _id;

    delete _left;
    delete _right;
    delete _sorted;
    delete _pos_sorted;

    // delete those generators not in _elements, i.e. the duplicate ones
    for (auto& x : _duplicate_gens) {
      (*_gens)[x.first]->really_delete();
      delete (*_gens)[x.first];
    }
    delete _gens;

    for (Element* x : *_elements) {
      x->really_delete();
      delete x;
    }
    delete _elements;
  }

  void Semigroup::reserve(size_t n) {
    _elements->reserve(n);
    _final.reserve(n);
    _first.reserve(n);
    _gens->reserve(n);
    _index.reserve(n);
    _length.reserve(n);
    _letter_to_pos.reserve(n);
    _map.reserve(n);
    _prefix.reserve(n);
    _suffix.reserve(n);

    // TODO the analogue of the below using RecVec::reserve, which currently
    // does not exist
    // if (n > _right->nr_rows()) {
    //  expand(n - _right->nr_rows());
    // }
  }

  // w is a word in the generators (i.e. a vector of letter_t's)
  Semigroup::pos_t Semigroup::word_to_pos(word_t const& w) const {
    assert(w.size() > 0);
    if (w.size() == 1) {
      return letter_to_pos(w[0]);
    }
    pos_t out = letter_to_pos(w[0]);
    for (auto it = w.begin() + 1; it < w.end(); it++) {
      assert(*it < nrgens());
      out = fast_product(out, letter_to_pos(*it));
    }
    return out;
  }

  Element* Semigroup::word_to_element(word_t const& w) const {
    assert(w.size() > 0);
    if (is_done() || w.size() == 1) {
      return (*_elements)[word_to_pos(w)]->really_copy();
    }
    Element* out = _tmp_product->really_copy();
    out->redefine((*_gens)[w[0]], (*_gens)[w[1]]);
    for (auto it = w.begin() + 2; it < w.end(); it++) {
      assert(*it < nrgens());
      _tmp_product->copy(out);
      out->redefine(_tmp_product, (*_gens)[*it]);
    }
    return out;
  }

  // Product by tracing in the left or right Cayley graph
  Semigroup::pos_t Semigroup::product_by_reduction(pos_t i, pos_t j) const {
    assert(i < _nr && j < _nr);
    if (length_const(i) <= length_const(j)) {
      while (i != UNDEFINED) {
        j = _left->get(j, _final[i]);
        i = _prefix[i];
      }
      return j;
    } else {
      while (j != UNDEFINED) {
        i = _right->get(i, _first[j]);
        j = _suffix[j];
      }
      return i;
    }
  }

  // Product by multiplying or by tracing in the Cayley graph whichever is
  // faster

  Semigroup::pos_t Semigroup::fast_product(pos_t i, pos_t j) const {
    assert(i < _nr && j < _nr);
    if (length_const(i) < 2 * _tmp_product->complexity()
        || length_const(j) < 2 * _tmp_product->complexity()) {
      return product_by_reduction(i, j);
    } else {
      _tmp_product->redefine((*_elements)[i], (*_elements)[j]);
      return _map.find(_tmp_product)->second;
    }
  }

  // Get the number of idempotents

  size_t Semigroup::nr_idempotents() {
    if (!_idempotents_found) {
      find_idempotents();
    }
    return _nr_idempotents;
  }

  bool Semigroup::is_idempotent(pos_t pos) {
    if (!_idempotents_found) {
      find_idempotents();
    }
    assert(pos < size());
    return _is_idempotent[pos];
  }

  // Const iterator to the first position of an idempotent

  typename std::vector<Semigroup::pos_t>::const_iterator
  Semigroup::idempotents_cbegin() {
    if (!_idempotents_found) {
      find_idempotents();
    }
    return _idempotents.cbegin();
  }

  typename std::vector<Semigroup::pos_t>::const_iterator
  Semigroup::idempotents_cend() {
    if (!_idempotents_found) {
      find_idempotents();
    }
    return _idempotents.cend();
  }

  // Get the position of an element in the semigroup

  Semigroup::pos_t Semigroup::position(Element* x) {
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

  size_t Semigroup::sorted_position(Element* x) {
    pos_t pos = position(x);

    if (pos == UNDEFINED) {
      return UNDEFINED;
    } else if (_pos_sorted == nullptr) {
      sort_elements();
      _pos_sorted = new std::vector<size_t>();
      _pos_sorted->resize(_sorted->size());
      for (size_t i = 0; i < _sorted->size(); i++) {
        (*_pos_sorted)[(*_sorted)[i].second] = i;
      }
    }
    return (*_pos_sorted)[pos];
  }

  Element* Semigroup::at(pos_t pos) {
    enumerate(pos + 1);

    if (pos < _elements->size()) {
      return (*_elements)[pos];
    } else {
      return nullptr;
    }
  }

  Element* Semigroup::sorted_at(pos_t pos) {
    sort_elements();
    if (pos < _sorted->size()) {
      return (*_sorted)[pos].first;
    } else {
      return nullptr;
    }
  }

  word_t* Semigroup::minimal_factorisation(Element* x) {
    pos_t pos = this->position(x);
    if (pos == Semigroup::UNDEFINED) {
      return nullptr;
    }
    return factorisation(pos);
  }

  word_t* Semigroup::factorisation(Element* x) {
    if (x->get_type() == Element::elm_t::RWSE) {
      return RWS::rws_word_to_word(
          (reinterpret_cast<RWSE*>(x))->get_rws_word());
    }
    assert(x->get_type() == Element::elm_t::NOT_RWSE);
    return minimal_factorisation(x);
  }

  word_t* Semigroup::minimal_factorisation(pos_t pos) {
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

  void Semigroup::minimal_factorisation(word_t& word, pos_t pos) {
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

  void Semigroup::next_relation(std::vector<size_t>& relation) {
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
          if (!_reduced.get(_index[_relation_pos], _relation_gen)
              && (_relation_pos < _lenindex[1]
                  || _reduced.get(_suffix[_index[_relation_pos]],
                                  _relation_gen))) {
            relation.push_back(_index[_relation_pos]);
            relation.push_back(_relation_gen);
            relation.push_back(
                _right->get(_index[_relation_pos], _relation_gen));
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

  void Semigroup::enumerate(std::atomic<bool>& killed, size_t limit) {
    _mtx.lock();
    if (_pos >= _nr || limit <= _nr || killed) {
      _mtx.unlock();
      return;
    }
    limit = std::max(limit, _nr + _batch_size);

    REPORT("limit = " << limit);
    Timer timer;
    timer.start();
    size_t tid = glob_reporter.thread_id(std::this_thread::get_id());

    // multiply the generators by every generator
    if (_pos < _lenindex[1]) {
      size_t nr_shorter_elements = _nr;
      while (_pos < _lenindex[1]) {
        size_t i       = _index[_pos];
        _multiplied[i] = true;
        for (size_t j = 0; j != _nrgens; ++j) {
          _tmp_product->redefine((*_elements)[i], (*_gens)[j], tid);
          auto it = _map.find(_tmp_product);

          if (it != _map.end()) {
            _right->set(i, j, it->second);
            _nrrules++;
          } else {
            is_one(_tmp_product, _nr);
            _elements->push_back(_tmp_product->really_copy());
            _first.push_back(_first[i]);
            _final.push_back(j);
            _index.push_back(_nr);
            _length.push_back(2);
            _map.insert(std::make_pair(_elements->back(), _nr));
            _prefix.push_back(i);
            _reduced.set(i, j, true);
            _right->set(i, j, _nr);
            _suffix.push_back(_letter_to_pos[j]);
            _nr++;
          }
        }
        _pos++;
      }
      for (size_t i = 0; i != _pos; ++i) {
        size_t b = _final[_index[i]];
        for (size_t j = 0; j != _nrgens; ++j) {
          _left->set(_index[i], j, _right->get(_letter_to_pos[j], b));
        }
      }
      _wordlen++;
      expand(_nr - nr_shorter_elements);
      _lenindex.push_back(_index.size());
    }

    // multiply the words of length > 1 by every generator
    bool stop = (_nr >= limit || killed);

    while (_pos != _nr && !stop) {
      size_t nr_shorter_elements = _nr;
      while (_pos != _lenindex[_wordlen + 1] && !stop) {
        size_t i       = _index[_pos];
        size_t b       = _first[i];
        size_t s       = _suffix[i];
        _multiplied[i] = true;
        for (size_t j = 0; j != _nrgens; ++j) {
          if (!_reduced.get(s, j)) {
            size_t r = _right->get(s, j);
            if (_found_one && r == _pos_one) {
              _right->set(i, j, _letter_to_pos[b]);
            } else if (_prefix[r] != UNDEFINED) {  // r is not a generator
              _right->set(
                  i, j, _right->get(_left->get(_prefix[r], b), _final[r]));
            } else {
              _right->set(i, j, _right->get(_letter_to_pos[b], _final[r]));
            }
          } else {
            _tmp_product->redefine((*_elements)[i], (*_gens)[j], tid);
            auto it = _map.find(_tmp_product);

            if (it != _map.end()) {
              _right->set(i, j, it->second);
              _nrrules++;
            } else {
              is_one(_tmp_product, _nr);
              _elements->push_back(_tmp_product->really_copy());
              _first.push_back(b);
              _final.push_back(j);
              _length.push_back(_wordlen + 2);
              _map.insert(std::make_pair(_elements->back(), _nr));
              _prefix.push_back(i);
              _reduced.set(i, j, true);
              _right->set(i, j, _nr);
              _suffix.push_back(_right->get(s, j));
              _index.push_back(_nr);
              _nr++;
              stop = (_nr >= limit || killed);
            }
          }
        }  // finished applying gens to <_elements->at(_pos)>
        _pos++;
      }  // finished words of length <wordlen> + 1
      expand(_nr - nr_shorter_elements);

      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        for (size_t i = _lenindex[_wordlen]; i != _pos; ++i) {
          size_t p = _prefix[_index[i]];
          size_t b = _final[_index[i]];
          for (size_t j = 0; j != _nrgens; ++j) {
            _left->set(_index[i], j, _right->get(_left->get(p, j), b));
          }
        }
        _wordlen++;
        _lenindex.push_back(_index.size());
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
    _mtx.unlock();
  }

  Semigroup* Semigroup::copy_closure(std::vector<Element*> const* coll) {
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

  void Semigroup::closure(std::vector<Element*> const& coll) {
    closure(&coll);
  }

  void Semigroup::closure(std::vector<Element*> const* coll) {
    if (coll->empty()) {
      return;
    } else {
      std::vector<Element*> singleton(1, nullptr);

      for (auto const& x : *coll) {
        if (!test_membership(x)) {
          singleton[0] = x;
          add_generators(singleton);
        }
      }
    }
  }

  Semigroup*
  Semigroup::copy_add_generators(std::vector<Element*> const* coll) const {
    if (coll->empty()) {
      return new Semigroup(*this);
    } else {
      // Partially copy
      Semigroup* out = new Semigroup(*this, coll);
      out->add_generators(coll);
      return out;
    }
  }

  void Semigroup::add_generators(std::vector<Element*> const& coll) {
    add_generators(&coll);
  }

  void Semigroup::add_generators(const std::vector<Element*>* coll) {
    if (coll->empty()) {
      return;
    }
    Timer timer;
    timer.start();
    size_t tid = glob_reporter.thread_id(std::this_thread::get_id());

    assert(degree() == (*coll->begin())->degree());

    // get some parameters from the old semigroup
    size_t old_nrgens  = _nrgens;
    size_t old_nr      = _nr;
    size_t nr_old_left = _pos;

    std::vector<bool> old_new;  // have we seen _elements->at(i) yet in new?

    // erase the old index
    _index.erase(_index.begin() + _lenindex[1], _index.end());

    // set up old_new
    old_new.resize(old_nr, false);
    for (size_t i = 0; i < _letter_to_pos.size(); i++) {
      old_new[_letter_to_pos[i]] = true;
    }

    // add the new generators to new _gens, _elements, and _index
    for (Element const* x : *coll) {
      assert(x->degree() == degree());
      auto it = _map.find(x);
      if (it == _map.end()) {  // new generator
        _gens->push_back(x->really_copy());
        _elements->push_back(_gens->back());
        _map.insert(std::make_pair(_gens->back(), _nr));

        _first.push_back(_gens->size() - 1);
        _final.push_back(_gens->size() - 1);

        _letter_to_pos.push_back(_nr);
        _index.push_back(_nr);

        is_one(x, _nr);
        _multiplied.push_back(false);
        _prefix.push_back(UNDEFINED);
        _suffix.push_back(UNDEFINED);
        _length.push_back(1);
        _nr++;
      } else if (_letter_to_pos[_first[it->second]] == it->second) {
        _gens->push_back(x->really_copy());
        // x is one of the existing generators
        _duplicate_gens.push_back(
            std::make_pair(_gens->size() - 1, _first[it->second]));
        // _gens[_gens.size() - 1] = _gens[_first[it->second])]
        // since _first maps pos_t -> letter_t
        _letter_to_pos.push_back(it->second);
      } else {
        // x is an old element that will now be a generator
        _gens->push_back((*_elements)[it->second]);
        _letter_to_pos.push_back(it->second);
        _index.push_back(it->second);

        _first[it->second]  = _gens->size() - 1;
        _final[it->second]  = _gens->size() - 1;
        _prefix[it->second] = UNDEFINED;
        _suffix[it->second] = UNDEFINED;
        _length[it->second] = UNDEFINED;

        old_new[it->second] = true;
      }
    }

    // reset the data structure
    _idempotents_found = false;
    _nrrules           = _duplicate_gens.size();
    _pos               = 0;
    _wordlen           = 0;
    _nrgens            = _gens->size();
    _lenindex.clear();
    _lenindex.push_back(0);
    _lenindex.push_back(_nrgens - _duplicate_gens.size());

    // Add columns for new generators
    // FIXME isn't this a bit wasteful, we could recycle the old _reduced, to
    // avoid reallocation
    _reduced = flags_t(_nrgens, _reduced.nr_rows() + _nrgens - old_nrgens);
    _left->add_cols(_nrgens - _left->nr_cols());
    _right->add_cols(_nrgens - _right->nr_cols());

    // Add rows in for newly added generators
    _left->add_rows(_nrgens - old_nrgens);
    _right->add_rows(_nrgens - old_nrgens);

    size_t nr_shorter_elements;

    // Repeat until we have multiplied all of the elements of <old> up to the
    // old value of _pos by all of the (new and old) generators

    while (nr_old_left > 0) {
      nr_shorter_elements = _nr;
      while (_pos < _lenindex[_wordlen + 1] && nr_old_left > 0) {
        pos_t    i = _index[_pos];  // position in _elements
        letter_t b = _first[i];
        pos_t    s = _suffix[i];
        if (_multiplied[i]) {
          nr_old_left--;
          // _elements[i] is in old semigroup, and its descendants are
          // known
          for (size_t j = 0; j < old_nrgens; j++) {
            size_t k = _right->get(i, j);
            if (!old_new[k]) {  // it's new!
              is_one((*_elements)[k], k);
              _first[k]  = _first[i];
              _final[k]  = j;
              _length[k] = _wordlen + 2;
              _prefix[k] = i;
              _reduced.set(i, j, true);
              if (_wordlen == 0) {
                _suffix[k] = _letter_to_pos[j];
              } else {
                _suffix[k] = _right->get(s, j);
              }
              _index.push_back(k);
              old_new[k] = true;
            } else if (s == UNDEFINED || _reduced.get(s, j)) {
              // this clause could be removed if _nrrules wasn't necessary
              _nrrules++;
            }
          }
          for (size_t j = old_nrgens; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_new, old_nr, tid);
          }
        } else {
          // _elements[i] is either not in old, or it is in old but its
          // descendants are not known
          _multiplied[i] = true;
          for (size_t j = 0; j < _nrgens; j++) {
            closure_update(i, j, b, s, old_new, old_nr, tid);
          }
        }
        _pos++;
      }  // finished words of length <wordlen> + 1

      expand(_nr - nr_shorter_elements);
      if (_pos > _nr || _pos == _lenindex[_wordlen + 1]) {
        if (_wordlen == 0) {
          for (size_t i = 0; i < _pos; i++) {
            size_t b = _final[_index[i]];
            for (size_t j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left->set(_index[i], j, _right->get(_letter_to_pos[j], b));
            }
          }
        } else {
          for (size_t i = _lenindex[_wordlen]; i < _pos; i++) {
            size_t p = _prefix[_index[i]];
            size_t b = _final[_index[i]];
            for (size_t j = 0; j < _nrgens; j++) {
              // TODO(JDM) reuse old info here!
              _left->set(_index[i], j, _right->get(_left->get(p, j), b));
            }
          }
        }
        _lenindex.push_back(_index.size());
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

  void Semigroup::sort_elements() {
    if (_sorted != nullptr) {
      return;
    }
    enumerate();
    _sorted = new std::vector<std::pair<Element*, size_t>>();
    _sorted->reserve(_elements->size());
    for (size_t i = 0; i < _elements->size(); i++) {
      _sorted->push_back(std::make_pair((*_elements)[i], i));
    }
    std::sort(_sorted->begin(), _sorted->end(), myless(*this));
  }

  // FIXME(JDM) improve this to either multiply or product_by_reduction
  // depending on which will be quickest. It is actually slow because of not
  // doing this

  void Semigroup::idempotents_thread(size_t&             nr,
                                     std::vector<pos_t>& idempotents,
                                     std::vector<bool>&  is_idempotent,
                                     pos_t               begin,
                                     pos_t               end) {
    Timer timer;
    timer.start();

    for (pos_t k = begin; k < end; k++) {
      // this is product_by_reduction, don't have to consider lengths because
      // they are equal!!
      pos_t i = k, j = k;
      while (j != UNDEFINED) {
        i = _right->get(i, _first[j]);
        j = _suffix[j];
      }
      if (i == k) {
        idempotents.push_back(k);
        is_idempotent.push_back(true);
        nr++;
      } else {
        is_idempotent.push_back(false);
      }
    }

    REPORT(timer.string("elapsed time = "));
  }

  void inline Semigroup::closure_update(pos_t              i,
                                        letter_t           j,
                                        letter_t           b,
                                        pos_t              s,
                                        std::vector<bool>& old_new,
                                        pos_t              old_nr,
                                        size_t const&      tid) {
    if (_wordlen != 0 && !_reduced.get(s, j)) {
      size_t r = _right->get(s, j);
      if (_found_one && r == _pos_one) {
        _right->set(i, j, _letter_to_pos[b]);
      } else if (_prefix[r] != UNDEFINED) {
        _right->set(i, j, _right->get(_left->get(_prefix[r], b), _final[r]));
      } else {
        _right->set(i, j, _right->get(_letter_to_pos[b], _final[r]));
      }
    } else {
      _tmp_product->redefine((*_elements)[i], (*_gens)[j], tid);
      auto it = _map.find(_tmp_product);
      if (it == _map.end()) {  // it's new!
        is_one(_tmp_product, _nr);
        _elements->push_back(_tmp_product->really_copy());
        _first.push_back(b);
        _final.push_back(j);
        _length.push_back(_wordlen + 2);
        _map.insert(std::make_pair(_elements->back(), _nr));
        _prefix.push_back(i);
        _reduced.set(i, j, true);
        _right->set(i, j, _nr);
        if (_wordlen == 0) {
          _suffix.push_back(_letter_to_pos[j]);
        } else {
          _suffix.push_back(_right->get(s, j));
        }
        _index.push_back(_nr);
        _nr++;
      } else if (it->second < old_nr && !old_new[it->second]) {
        // we didn't process it yet!
        is_one(_tmp_product, it->second);
        _first[it->second]  = b;
        _final[it->second]  = j;
        _length[it->second] = _wordlen + 2;
        _prefix[it->second] = i;
        _reduced.set(i, j, true);
        _right->set(i, j, it->second);
        if (_wordlen == 0) {
          _suffix[it->second] = _letter_to_pos[j];
        } else {
          _suffix[it->second] = _right->get(s, j);
        }
        _index.push_back(it->second);
        old_new[it->second] = true;
      } else {  // it->second >= old->_nr || old_new[it->second]
        // it's old
        _right->set(i, j, it->second);
        _nrrules++;
      }
    }
  }

  // TOOD(JDM) improve this if R/L-classes are known to stop performing the
  // product if we fall out of the R-class of the initial element.

  void Semigroup::find_idempotents() {
    _idempotents_found = true;
    enumerate();

    Timer timer;
    timer.start();

    size_t sum_word_lengths = 0;
    for (size_t i = length_non_const(_idempotents_start_pos);
         i < _lenindex.size();
         i++) {
      sum_word_lengths += i * (_lenindex[i] - _lenindex[i - 1]);
    }

    if (_max_threads == 1 || size() < 823543) {
      if ((_nr - _idempotents_start_pos) * _tmp_product->complexity()
          < sum_word_lengths) {
        for (size_t i = _idempotents_start_pos; i < _nr; i++) {
          _tmp_product->redefine((*_elements)[i], (*_elements)[i]);
          if (*_tmp_product == *(*_elements)[i]) {
            _nr_idempotents++;
            _idempotents.push_back(i);
            _is_idempotent.push_back(true);
          } else {
            _is_idempotent.push_back(false);
          }
        }
      } else {
        for (size_t i = _idempotents_start_pos; i < _nr; i++) {
          // TODO(JDM) redo this to not use product_by_reduction
          if (product_by_reduction(i, i) == i) {
            _nr_idempotents++;
            _idempotents.push_back(i);
            _is_idempotent.push_back(true);
          } else {
            _is_idempotent.push_back(false);
          }
        }
      }
    } else {
      size_t av_load    = sum_word_lengths / _max_threads;
      pos_t  begin      = _idempotents_start_pos;
      pos_t  end        = _idempotents_start_pos;
      size_t total_load = 0;

      std::vector<size_t>             nr(_max_threads, 0);
      std::vector<std::vector<pos_t>> idempotents(_max_threads,
                                                  std::vector<pos_t>());
      std::vector<std::vector<bool>> is_idempotent(_max_threads,
                                                   std::vector<bool>());
      std::vector<std::thread> threads;
      glob_reporter.reset_thread_ids();

      // TODO(JDM) use less threads if the av_load is too low
      for (size_t i = 0; i < _max_threads; i++) {
        size_t thread_load = 0;
        if (i != _max_threads - 1) {
          while (thread_load < av_load) {
            thread_load += length_const(end);
            end++;
          }
          total_load += thread_load;
        } else {
          end         = size();
          thread_load = sum_word_lengths - total_load;
        }

        REPORT("thread " << i + 1 << " has load " << thread_load)

        threads.push_back(std::thread(&Semigroup::idempotents_thread,
                                      this,
                                      std::ref(nr[i]),
                                      std::ref(idempotents[i]),
                                      std::ref(is_idempotent[i]),
                                      begin,
                                      end));
        begin = end;
      }

      for (size_t i = 0; i < _max_threads; i++) {
        threads[i].join();
        _nr_idempotents += nr[i];
      }
      _idempotents.reserve(_nr_idempotents);
      _is_idempotent.reserve(size());
      for (size_t i = 0; i < _max_threads; i++) {
        _idempotents.insert(
            _idempotents.end(), idempotents[i].begin(), idempotents[i].end());
        _is_idempotent.insert(_is_idempotent.end(),
                              is_idempotent[i].begin(),
                              is_idempotent[i].end());
      }
    }
    _idempotents_start_pos = _nr;
    REPORT(timer.string("elapsed time = "));
  }

  // _nrgens, _duplicates_gens, _letter_to_pos, and _elements must all be
  // initialised for this to work, and _gens must point to an empty vector.
  void Semigroup::copy_gens() {
    assert(_gens->empty());
    _gens->resize(_nrgens, nullptr);
    // really copy duplicate gens from _elements
    for (auto const& x : _duplicate_gens) {
      // The degree of everything in _elements has already been increased (if
      // it needs to be at all), and so we do not need to increase the degree
      // in the copy below.
      (*_gens)[x.first] = (*_elements)[_letter_to_pos[x.second]]->really_copy();
    }
    // the non-duplicate gens are already in _elements, so don't really copy
    for (size_t i = 0; i < _nrgens; i++) {
      if ((*_gens)[i] == nullptr) {
        (*_gens)[i] = (*_elements)[_letter_to_pos[i]];
      }
    }
  }

}  // namespace libsemigroups
