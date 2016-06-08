/*******************************************************************************
 * Semigroups++
 *
 * This file contains ...
 *
 *******************************************************************************/

#include "tc.h"

size_t Congruence::INFTY = -1;
size_t Congruence::UNDEFINED = -1;

Congruence::Congruence (size_t                         nrgens,
                        std::vector<relation_t> const& relations,
                        std::vector<relation_t> const& extra) :
  _nrgens(nrgens),
  _relations(relations),
  _forwd(1, UNDEFINED),
  _bckwd(1, 0),
  _current(0),
  _current_no_add(UNDEFINED),
  _next(UNDEFINED),
  _last(0),
  _active(1),
  _pack(120000),
  _table(_nrgens, 1, UNDEFINED),
  _preim_init(_nrgens, 1, UNDEFINED),
  _preim_next(_nrgens, 1, UNDEFINED),
  _report(true),
  _defined(1),
  _killed(0),
  _stop_packing(false),
  _next_report(0) {
    // TODO: check that the entries in extra/relations are properly defined
    // i.e. that every entry is at most nrgens - 1
    // TODO: ok?
    for (relation_t const& rel: extra) {
      trace(0, rel);
    }
}

Congruence::Congruence (Semigroup* semigroup) :
  Congruence(semigroup, std::vector<relation_t>()) {}

Congruence::Congruence (Semigroup*                     semigroup,
                        std::vector<relation_t> const& extra) :
  Congruence(semigroup->nrgens(),  std::vector<relation_t>(), extra) {

  std::vector<size_t> relation;

  semigroup->reset_next_relation();
  semigroup->next_relation(relation, _report);

  if (relation.size() == 2) { // this is for the case when there are duplicate gens
    assert(false); // FIXME
  }

  while (! relation.empty()) {
    word_t lhs = *(semigroup->factorisation(relation[0]));
    lhs.push_back(relation[1]);
    word_t rhs = *(semigroup->factorisation(relation[2]));
    _relations.push_back(std::make_pair(lhs, rhs));
    semigroup->next_relation(relation, _report);
  }
}

void Congruence::new_coset (coset_t const& c, letter_t const& a) {

  _active++;
  _defined++;
  _next_report++;

  if (_next == UNDEFINED) { // there are no free cosets to recycle
    _next = _active - 1;
    _forwd[_last] = _next;
    _forwd.push_back(UNDEFINED);
    _bckwd.push_back(_last);
    _table.add_rows();
    _preim_init.add_rows();
    _preim_next.add_rows();
  } else {
    _bckwd[_next] = _last;
  }

  _last = _next;
  _next = _forwd[_last];

  for (letter_t i = 0; i < _nrgens; i++) {
    _table.set(_last, i, UNDEFINED);
    _preim_init.set(_last, i, UNDEFINED);
  }
  _table.set(c, a, _last);
  _preim_init.set(_last, a, c);
  _preim_next.set(c, a, UNDEFINED);

  // TODO something with pos?
}

void Congruence::identify_cosets (coset_t lhs, coset_t rhs) {
  assert(_lhs_stack.empty() && _rhs_stack.empty());

  if (lhs == rhs) {
    return;
  } else if (rhs < lhs) {
    coset_t tmp = lhs;
    lhs = rhs;
    rhs = tmp;
  }

  // TODO something with pos?

  while (true) {
    while (_bckwd[lhs] < 0) {
      lhs = -_bckwd[lhs];
    }
    while (_bckwd[rhs] < 0) {
      rhs = -_bckwd[rhs];
    }

    if (lhs != rhs) {
      _active--;
      if (rhs == _current) {
        // TODO why bckwd?
        _current = _bckwd[_current];
      }
      if (rhs == _current_no_add) {
        _current_no_add = _bckwd[_current_no_add];
      }
      if (rhs == _last) {
        _last = _bckwd[_last];
      } else {
         _bckwd[_forwd[rhs]] = _bckwd[rhs];  // drop |t| from queue.
         _forwd[_bckwd[rhs]] = _forwd[rhs];
         _forwd[rhs] = _next;            // link |t| to free list.
         _forwd[_last] = rhs;
      }
      _next = rhs;
      _bckwd[rhs] = -lhs;

      for (letter_t i = 0; i < _nrgens; i++) {
        coset_t v = _preim_init.get(rhs, i);
        while (v != UNDEFINED) {
          _table.set(v, i, lhs);
          coset_t u = _preim_next.get(v, i);
          _preim_next.set(v, i, _preim_init.get(lhs, i));
          _preim_init.set(lhs, i, v);
          v = u;
        }
        v = _table.get(rhs, i);
        if (v != UNDEFINED) {
          coset_t u = _preim_init.get(v, i);
          if (u == rhs) {
            _preim_init.set(v, i, _preim_next.get(rhs, i));
          } else {
            while (_preim_next.get(u, i) != rhs) {
              u = _preim_next.get(u, i);
            }
            _preim_next.set(u, i, _preim_next.get(rhs, i));
          }
          u = _table.get(lhs, i);
          if (u == UNDEFINED) {
            _table.set(lhs, i, v);
            _preim_next.set(lhs, i, _preim_init.get(v, i));
            _preim_init.set(v, i, lhs);
          } else {
            _lhs_stack.push(std::min(u, v));
            _rhs_stack.push(std::max(u, v));
          }
        }
      }
    }
    if (_lhs_stack.empty()) {
      break;
    }
    lhs = _lhs_stack.top(); _lhs_stack.pop();
    rhs = _rhs_stack.top(); _rhs_stack.pop();
  }
}

void Congruence::trace (coset_t const& c, relation_t const& rel, bool add) {

  coset_t lhs = c;
  for (auto it = rel.first.begin(); it < rel.first.end() - 1; it++) {
    if (_table.get(lhs, *it) != UNDEFINED) {
      lhs = _table.get(lhs, *it);
    } else if (add) {
      new_coset(lhs, *it);
      lhs = _last;
    } else {
      return;
    }
  }

  coset_t rhs = c;
  for (auto it = rel.second.begin(); it < rel.second.end() - 1; it++) {
    if (_table.get(rhs, *it) != UNDEFINED) {
      rhs = _table.get(rhs, *it);
    } else if (add) {
      new_coset(rhs, *it);
      rhs = _last;
    } else {
      return;
    }
  }

  _next_report++;

  if (_next_report > 4000000) {
    if (_report) {
      std::cout << _defined << " defined, "
        << _forwd.size() << " max, "
        << _active << " active, "
        << (_defined - _active) - _killed << " killed, "
        << "current ";
      if (add) {
        std::cout << _current;
      } else {
        std::cout << _current_no_add;
      }
      std::cout << std::endl;
    }
    if ((_defined - _active) - _killed < 100) {
      _stop_packing = true;
    }
    _next_report = 0;
    _killed = _defined - _active;
  }

  letter_t a = rel.first.back();
  letter_t b = rel.second.back();
  coset_t  u = _table.get(lhs, a);
  coset_t  v = _table.get(rhs, b);

  if (u == UNDEFINED && v == UNDEFINED) {
    if (add) {
      new_coset(lhs, a);
      _table.set(rhs, b, _last);
      if (a == b) {
        _preim_next.set(lhs, a, rhs);
        _preim_next.set(rhs, a, UNDEFINED);
      } else {
        _preim_init.set(_last, b, rhs);
        _preim_next.set(rhs, b, UNDEFINED);
      }
    } else {
      return;
    }
  } else if (u == UNDEFINED && v != UNDEFINED) {
    _table.set(lhs, a, v);
    _preim_next.set(lhs, a, _preim_init.get(v, a));
    _preim_init.set(v, a, lhs);
  } else if (u != UNDEFINED && v == UNDEFINED) {
    _table.set(rhs, b, u);
    _preim_next.set(rhs, b, _preim_init.get(u, b));
    _preim_init.set(u, b, rhs);
  } else {
    // |lhs a| and |rhs b| are both defined
    identify_cosets(u, v);
  }
}

void Congruence::todd_coxeter (size_t limit) {

  do {
    for (relation_t const& rel: _relations) {
      trace(_current, rel);
    }

    if (_active > _pack) {
      if (_report) {
        std::cout << _defined << " defined, "
          << _forwd.size() << " max, "
          << _active << " active, "
          << (_defined - _active) - _killed << " killed, "
          << "current " << _current << std::endl;
        std::cout << "Entering lookahead phase . . .\n";
        _killed = _defined - _active;
      }
      size_t oldactive = _active;
      _current_no_add = _current;
      do {
        for (relation_t const& rel: _relations) {
          trace(_current_no_add, rel, false);
        }
        _current_no_add = _forwd[_current_no_add];
      } while (_current_no_add != _next && !_stop_packing);
      if (_report) {
        std::cout << "Lookahead phase complete " << oldactive - _active <<
          " killed " << std::endl;
      }
      _pack += _pack / 10;
      _stop_packing = false;
      _current_no_add = UNDEFINED;
    }
    _current = _forwd[_current];
  } while (_current != _next);
  if (_report) {
    std::cout << _defined << " cosets defined, maximum " << _forwd.size();
    std::cout <<  ", " <<_active << " survived" << std::endl;
  }
}
