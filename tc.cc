/*******************************************************************************
 * Semigroups++
 *
 * This file contains methods for Todd-Coxeter coset enumeration
 *
 *******************************************************************************/

#include "tc.h"

#include <thread>

size_t Congruence::INFTY = -1;
size_t Congruence::UNDEFINED = -1;
std::mutex Congruence::_report_mtx;

#define REPORT(str) \
  _report_mtx.lock(); \
  std::cout << "Thread #" << _thread_id << ": " << __func__ << ": " \
    << str << std::endl; \
  _report_mtx.unlock();

Congruence::Congruence (cong_t                         type,
                        size_t                         nrgens,
                        // TODO default these to empty vectors
                        std::vector<relation_t> const& relations,
                        std::vector<relation_t> const& extra,
                        size_t                         thread_id) :
  _type(type),
  _tc_done(false),
  _id_coset(0),
  _nrgens(nrgens),
  _relations(relations),
  _active(1),
  _pack(120000),
  _stop(false),
  _forwd(1, UNDEFINED),
  _bckwd(1, 0),
  _current(0),
  _current_no_add(UNDEFINED),
  _last(0),
  _next(UNDEFINED),
  _table(_nrgens, 1, UNDEFINED),
  _preim_init(_nrgens, 1, UNDEFINED),
  _preim_next(_nrgens, 1, UNDEFINED),
  _report(true),
  _defined(1),
  _killed(0),
  _stop_packing(false),
  _next_report(0),
  _use_known(false),
  _thread_id(thread_id) {

  // TODO: check that the entries in extra/relations are properly defined
  // i.e. that every entry is at most nrgens - 1

    switch (_type) {
      case LEFT:
        for (relation_t& rel: _relations) {
          std::reverse(rel.first.begin(), rel.first.end());
          std::reverse(rel.second.begin(), rel.second.end());
        }
        _extra = extra;
        for (relation_t& rel: _extra) {
          std::reverse(rel.first.begin(), rel.first.end());
          std::reverse(rel.second.begin(), rel.second.end());
        }
        break;
      case RIGHT: // do nothing
        _extra = extra;
        break;
      case TWOSIDED:
        _relations.insert(_relations.end(),
                          extra.begin(),
                          extra.end());
        _extra = std::vector<relation_t>();
        break;
      default:
        assert(false);
    }
}

// FIXME remove this, it's for testing only
Congruence::Congruence (Semigroup* semigroup) :
  Congruence(TWOSIDED, semigroup, std::vector<relation_t>(), false) {}

Congruence::Congruence (cong_t                         type,
                        Semigroup*                     semigroup,
                        std::vector<relation_t> const& extra,
                        bool                           use_known,
                        size_t                         thread_id) :
  Congruence(type,
             semigroup->nrgens(),
             std::vector<relation_t>(),
             extra,
             thread_id) {

  if (use_known) { // use the right or left Cayley table of semigroup
    _use_known = true;
    _active += semigroup->size();

    if (type == LEFT) {
      _table.adjoin(*semigroup->left_cayley_graph(_report));
    } else {
      _table.adjoin(*semigroup->right_cayley_graph(_report));
    }

    for (auto it = _table.begin(); it < _table.end(); it++) {
      (*it)++;
    }

    _id_coset = 0;

    for (size_t i = 0; i < _nrgens; i++) {
      _table.set(_id_coset, i, semigroup->genslookup(i) + 1);
    }

    _forwd.reserve(_active);
    _bckwd.reserve(_active);

    for (size_t i = 1; i < _active; i++) {
      _forwd.push_back(i + 1);
      _bckwd.push_back(i - 1);
    }
    _forwd[0] = 1;
    _forwd[_active - 1] = UNDEFINED;

    _last = _active - 1;

    _preim_init.add_rows(semigroup->size());
    _preim_next.add_rows(semigroup->size());

    for (coset_t c = 0; c < _active; c++) {
      for (letter_t i = 0; i < _nrgens; i++) {
        coset_t b = _table.get(c, i);
        _preim_next.set(c, i, _preim_init.get(b, i));
        _preim_init.set(b, i, c);
      }
    }

    _defined = _active;
  } else { // Don't use the right/left Cayley graph
    _use_known = false;
    std::vector<size_t> relation;

    semigroup->reset_next_relation();
    semigroup->next_relation(relation, _report);

    if (relation.size() == 2) {
      // This is for the case when there are duplicate gens
      assert(false); // FIXME
    }
    if (_type == LEFT) {
      while (! relation.empty()) {
        word_t lhs = *(semigroup->factorisation(relation[0]));
        lhs.push_back(relation[1]);
        std::reverse(lhs.begin(), lhs.end());
        word_t rhs = *(semigroup->factorisation(relation[2]));
        std::reverse(rhs.begin(), rhs.end());
        _relations.push_back(std::make_pair(lhs, rhs));
        semigroup->next_relation(relation, _report);
      }
    } else {
      while (! relation.empty()) {
        word_t lhs = *(semigroup->factorisation(relation[0]));
        lhs.push_back(relation[1]);
        word_t rhs = *(semigroup->factorisation(relation[2]));
        _relations.push_back(std::make_pair(lhs, rhs));
        semigroup->next_relation(relation, _report);
      }
    }
  }
}

//
// new_coset( c, a )
// Create a new active coset for coset c to map to under generator a
//
void Congruence::new_coset (coset_t const& c, letter_t const& a) {

  if (_stop) {
    return;
  }
  _active++;
  _defined++;
  _next_report++;

  if (_next == UNDEFINED) {
    // There are no free cosets to recycle: make a new one
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

  // Mark one more coset as active
  _last = _next;
  _next = _forwd[_last];

  // Clear the new coset's row in each table
  for (letter_t i = 0; i < _nrgens; i++) {
    _table.set(_last, i, UNDEFINED);
    _preim_init.set(_last, i, UNDEFINED);
  }

  // Set the new coset as the image of c under a
  _table.set(c, a, _last);

  // Set c as the one preimage of the new coset
  _preim_init.set(_last, a, c);
  _preim_next.set(c, a, UNDEFINED);

  // TODO something with pos?
}

//
// identify_cosets( lhs, rhs )
// Identify lhs with rhs, and process any further coincidences
//
void Congruence::identify_cosets (coset_t lhs, coset_t rhs) {
  if (_stop) {
    return;
  }

  assert(_lhs_stack.empty() && _rhs_stack.empty());

  // Make sure lhs < rhs
  if (lhs == rhs) {
    return;
  } else if (rhs < lhs) {
    coset_t tmp = lhs;
    lhs = rhs;
    rhs = tmp;
    // TODO: Should this be done after the (lhs = -_bckwd[lhs];) step?
  }

  // TODO something with pos?

  while (!_stop) {
    // If <lhs> is not active, use the coset it was identified with
    while (_bckwd[lhs] < 0) {
      lhs = -_bckwd[lhs];
    }
    // Same with <rhs>
    while (_bckwd[rhs] < 0) {
      rhs = -_bckwd[rhs];
    }

    if (lhs != rhs) {
      _active--;
      // If any "controls" point to <rhs>, move them back one in the list
      if (rhs == _current) {
        // TODO: should we go forwd instead?
        _current = _bckwd[_current];
      }
      if (rhs == _current_no_add) {
        _current_no_add = _bckwd[_current_no_add];
      }

      assert(rhs != _next);
      if (rhs == _last) {
        // Simply move the start of the free list back by 1
        _last = _bckwd[_last];
      } else {
        // Remove <rhs> from the active list
        _bckwd[_forwd[rhs]] = _bckwd[rhs];
        _forwd[_bckwd[rhs]] = _forwd[rhs];
        // Add <rhs> to the start of the free list
        _forwd[rhs] = _next;
        _forwd[_last] = rhs;
      }
      _next = rhs;

      // Leave a "forwarding address" so we know what <rhs> was identified with
      _bckwd[rhs] = -lhs;

      for (letter_t i = 0; i < _nrgens; i++) {
        // Let <v> be the first PREIMAGE of <rhs>
        coset_t v = _preim_init.get(rhs, i);
        while (v != UNDEFINED) {
          _table.set(v, i, lhs);  // Replace <rhs> by <lhs> in the table
          coset_t u = _preim_next.get(v, i); // Get <rhs>'s next preimage
          _preim_next.set(v, i, _preim_init.get(lhs, i));
          _preim_init.set(lhs, i, v); // v is now a preimage of <lhs>, not <rhs>
          v = u; // Let <v> be <rhs>'s next preimage, and repeat
        }

        // Now let <v> be the IMAGE of <rhs>
        v = _table.get(rhs, i);
        if (v != UNDEFINED) {
          coset_t u = _preim_init.get(v, i);
          assert(u != UNDEFINED);
          if (u == rhs) {
            // Remove <rhs> from the start of the list of <v>'s preimages
            _preim_init.set(v, i, _preim_next.get(rhs, i));
          } else {
            // Go through all <v>'s preimages until we find <rhs>
            while (_preim_next.get(u, i) != rhs) {
              u = _preim_next.get(u, i);
            }
            // Remove <rhs> from the list of <v>'s preimages
            _preim_next.set(u, i, _preim_next.get(rhs, i));
          }

          // Let <u> be the image of <lhs>, and ensure <u> = <v>
          u = _table.get(lhs, i);
          if (u == UNDEFINED) {
            _table.set(lhs, i, v);
            _preim_next.set(lhs, i, _preim_init.get(v, i));
            _preim_init.set(v, i, lhs);
          } else {
            // Add (u,v) to the stack of pairs to be identified
            _lhs_stack.push(std::min(u, v));
            _rhs_stack.push(std::max(u, v));
            // TODO: Is the min/max thing necessary?
          }
        }
      }
    }
    if (_lhs_stack.empty()) {
      break;
    }
    // Get the next pair to be identified
    lhs = _lhs_stack.top(); _lhs_stack.pop();
    rhs = _rhs_stack.top(); _rhs_stack.pop();
  }
}

//
// trace( c, rel[, add] )
// Take the two words of the relation <rel>, apply them both to the coset <c>,
// and identify the two results.  If <add> is true (the default) then new cosets
// will be created whenever necessary; if false, then we are "packing", and this
// function will not create any new cosets.
//
void Congruence::trace (coset_t const& c, relation_t const& rel, bool add) {

  if (_stop) {
    return;
  }

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
  // <lhs> is the image of <c> under <rel>[1] (minus the last letter)

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
    if (_stop) {
      return;
    }
  }
  // <rhs> is the image of <c> under <rel>[2] (minus the last letter)

  // Statistics and packing
  _next_report++;
  if (_next_report > 4000000) {
    if (_report) {
      REPORT(_defined << " defined, " <<
             _forwd.size() << " max, " <<
             _active << " active, " <<
             (_defined - _active) - _killed << " killed, " <<
             "current " << (add ? _current : _current_no_add));
    }
    // If we are killing cosets too slowly, then stop packing
    if ((_defined - _active) - _killed < 100) {
      _stop_packing = true;
    }
    _next_report = 0;
    _killed = _defined - _active;
  }

  if (_stop) {
    return;
  }
  letter_t a = rel.first.back();
  letter_t b = rel.second.back();
  coset_t  u = _table.get(lhs, a);
  coset_t  v = _table.get(rhs, b);
  // u = lhs^a = c^rel[1]
  // v = rhs^b = c^rel[2]

  // We must now ensure lhs^a == rhs^b.
  if (u == UNDEFINED && v == UNDEFINED) {
    if (add) {
      // Create a new coset and set both lhs^a and rhs^b to it
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
      return;  // Packing phase: do nothing
    }
  } else if (u == UNDEFINED && v != UNDEFINED) {
    // Set lhs^a to v
    _table.set(lhs, a, v);
    _preim_next.set(lhs, a, _preim_init.get(v, a));
    _preim_init.set(v, a, lhs);
  } else if (u != UNDEFINED && v == UNDEFINED) {
    // Set rhs^b to u
    _table.set(rhs, b, u);
    _preim_next.set(rhs, b, _preim_init.get(u, b));
    _preim_init.set(u, b, rhs);
  } else {
    // lhs^a and rhs^b are both defined
    identify_cosets(u, v);
  }
}

//
// todd_coxeter( limit )
// Apply the TC algorithm until the coset table is complete.
// TODO: <limit> should be the max table size; it is currently ignored.
//
void Congruence::todd_coxeter (size_t limit) {

  // TODO this should be put into a method that is only called once

  // If we have already run this before, then we are done
  if (_tc_done) {
    return;
  }

  // Apply each "extra" relation to the first coset only
  for (relation_t const& rel: _extra) {
    trace(_id_coset, rel);  // Allow new cosets
    if (_stop) {
      return;
    }
  }
  if (_relations.empty()) {
    _tc_done = true;
    return;
  }
  do {
    // Apply each relation to the "_current" coset
    for (relation_t const& rel: _relations) {
      trace(_current, rel);  // Allow new cosets
    }

    // If the number of active cosets is too high, start a packing phase
    if (_active > _pack) {
      if (_report) {
        REPORT(_defined << " defined, " <<
               _forwd.size() << " max, " <<
               _active << " active, " <<
               (_defined - _active) - _killed << " killed, " <<
               "current " << _current);
        REPORT("Entering lookahead phase . . .");
        _killed = _defined - _active;
      }

      size_t oldactive = _active;  // Keep this for stats
      _current_no_add = _current;  // Start packing from _current
      // TODO: Should this be "_current + 1"?

      do {
        // Apply every relation to the "_current_no_add" coset
        for (relation_t const& rel: _relations) {
          trace(_current_no_add, rel, false);  // Don't allow new cosets
        }
        _current_no_add = _forwd[_current_no_add];

        // Quit loop if we reach an inactive coset OR we get a "stop" signal
        if (_stop) {
          return;
        }
      } while (_current_no_add != _next && !_stop_packing);
      if (_report) {
        REPORT("Lookahead phase complete " <<
               oldactive - _active << " killed");
      }
      _pack += _pack / 10;  // Raise packing threshold 10%
      _stop_packing = false;
      _current_no_add = UNDEFINED;
    }

    // Move onto the next coset
    _current = _forwd[_current];

    // Quit loop when we reach an inactive coset
    if (_stop) {
      return;
    }
  } while (_current != _next);

  // Final report
  if (_report) {
    REPORT(_defined << " cosets defined," <<
           " maximum " << _forwd.size() <<
           _active << " survived");
  }

  _tc_done = true;

  // No return value: all info is now stored in the class
}

/*void Congruence::todd_coxeter_finite () {
  if (_use_known) {
    switch (_type) {
      case LEFT: // TODO
        break;
      case RIGHT:
        for (relation_t const& rel: _extra) {
          trace(_id_coset, rel);
          if (_stop) {
            return;
          }
        }
        _tc_done = true;
        break;
      case TWOSIDED:
        break;
      default:
        assert(false);
    }
  } else {
    todd_coxeter();
  }
}*/

//
// word_to_coset( w )
// We assume that todd_coxeter has already been run.
// Return the coset which corresponds to the word <w>.
//
size_t Congruence::word_to_coset (word_t w) {
  coset_t c = _id_coset;
  if (_type == LEFT) {
    // Iterate in reverse order
    for (auto rit = w.rbegin(); rit >= w.rend(); rit++) {
      c = _table.get(c, *rit);
      assert(c != UNDEFINED);
    }
  } else {
    // Iterate in sequential order
    for (auto it = w.begin(); it < w.end(); it++) {
      c = _table.get(c, *it);
      assert(c != UNDEFINED);
    }
  }
  return c;
}

void Congruence::terminate () {
  _stop = true;
}

void Congruence::set_report (bool val) {
  _report = val;
}

bool Congruence::is_tc_done () {
  return _tc_done;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Congruence* finite_cong_enumerate (cong_t type,
                                   Semigroup* S,
                                   std::vector<relation_t> const& extra,
                                   bool report) {

  Timer timer;
  if (report) {
    timer.start();
  }

  Congruence* cong_t(new Congruence(type, S, extra, true, 1));
  Congruence* cong_f(new Congruence(type, S, extra, false, 2));

  cong_t->set_report(report);
  cong_f->set_report(report);

  auto go = [] (Congruence& this_cong, Congruence& that_cong) {
    this_cong.todd_coxeter();
    that_cong.terminate();
  };

  std::thread thread_t(go, std::ref(*cong_t), std::ref(*cong_f));
  std::thread thread_f(go, std::ref(*cong_f), std::ref(*cong_t));

  thread_t.join();
  thread_f.join();

  if (report) {
    timer.stop(std::string(__func__) + ": ");
  }

  if (cong_t->is_tc_done()) {
    if (report) {
      std::cout << __func__ << ": Using the Cayley graph (Thread #1) won!" <<
        std::endl;
    }
    delete cong_f;
    return cong_t;
  } else {
    assert(cong_f->is_tc_done());
    if (report) {
      std::cout << __func__ << ": Using the Cayley graph (Thread #1) lost!" <<
        std::endl;
    }
    delete cong_t;
    return cong_f;
  }
}
