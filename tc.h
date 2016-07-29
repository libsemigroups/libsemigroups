/*******************************************************************************
 * Semigroups++
 *
 * This file contains methods for Todd-Coxeter coset enumeration
 *
 *******************************************************************************/

#ifndef TC_H_
#define TC_H_

#include <atomic>
#include <forward_list>
#include <stack>
#include <utility>
#include <vector>

#include "elements.h"
#include "semigroups.h"

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef size_t                    letter_t;
typedef std::vector<letter_t>     word_t;
typedef std::pair<word_t, word_t> relation_t;

class Congruence {

 typedef size_t                    coset_t;
 typedef int64_t                   signed_coset_t;

 public:
  explicit Congruence (size_t,
                       std::vector<relation_t> const&,
                       std::vector<relation_t> const&);
  Congruence (Semigroup*, std::vector<relation_t> const&, bool);
  explicit Congruence (Semigroup*);

  void todd_coxeter (size_t limit = INFTY);

  void todd_coxeter_finite ();

  size_t nr_active_cosets () {
    return _active;
  }

  coset_t word_to_coset (word_t);

  void terminate ();
  bool is_tc_done ();
  void set_report (bool);

 private:
  void new_coset(coset_t const&, letter_t const&);
  void identify_cosets(coset_t, coset_t);
  inline void trace(coset_t const&, relation_t const&, bool add = true);
  void check_forwd();

  bool                         _tc_done;  // Has todd_coxeter already been run?

  coset_t                      _id_coset; // TODO: Remove?
  size_t                       _nrgens;
  std::vector<relation_t>      _relations;
  std::vector<relation_t>      _extra;

  size_t                       _active;  // Number of active cosets

  size_t                       _pack;    // Nr of active cosets allowed before a
                                         // packing phase starts

  std::atomic_bool             _stop;

  //
  // COSET LISTS:
  //
  // We use these two arrays to simulate a doubly-linked list of active cosets
  // (the "active list") with deleted cosets attached to the end (the "free
  // list").  If <c> is an active coset:
  //   _forwd[c] is the coset that comes after <c> in the list.
  //   _bckwd[c] is the coset that comes before <c> in the list.
  // If <c> is a free coset (has been deleted) the backward reference is not
  // needed, and so instead, _bckwd[c] is set to the coset <c> was identified
  // with.  To indicate this alternative use of the list, the entry is negated
  // (_backwd[c] == -3 indicates that <c> was identified with coset 3).
  //
  std::vector<coset_t>         _forwd;
  std::vector<signed_coset_t>  _bckwd;
  //
  // We also store some special locations in the list:
  //   _current is the coset to which we are currently applying relations.
  //   _current_no_add is used instead of _current if we are in a packing phase.
  //   _last points to the final active coset in the list.
  //   _next points to the first free coset in the list.
  // Hence usually _next == _last + 1.
  //
  coset_t                      _current;
  coset_t                      _current_no_add;
  coset_t                      _last;
  coset_t                      _next;

  //
  // COSET TABLES:
  //
  // We use these three tables to store all a coset's images and preimages.
  //   _table[c][i] is coset c's image under generator i.
  //   _preim_init[c][i] is ONE of coset c's preimages under generator i.
  //   _preim_next[c][i] is a coset that has THE SAME IMAGE as coset c (under i)
  //
  // Hence to find all the preimages of c under i:
  //   - Let u = _preim_init[c][i] ONCE.
  //   - Let u = _preim_next[u][i] REPEATEDLY until it becomes UNDEFINED.
  // Each u is one preimage.
  //
  // To add v, a new preimage of c under i:
  //   - Set _preim_next[v][i] to point to the current _preim_init[c][i].
  //   - Then change _preim_init[c][i] to point to v.
  // Now the new preimage and all the old preimages are stored.
  //
  RecVec<coset_t>              _table;
  RecVec<coset_t>              _preim_init;
  RecVec<coset_t>              _preim_next;

  // Stacks for identifying cosets
  std::stack<coset_t>          _lhs_stack;
  std::stack<coset_t>          _rhs_stack;

  // Statistics etc.
  bool                         _report;
  size_t                       _defined;
  size_t                       _killed;
  size_t                       _stop_packing;  //TODO: make this a bool?
  size_t                       _next_report;

  // Determines whether we pre-populate the table with known info
  bool                         _use_known;

  static size_t                INFTY;
  static size_t                UNDEFINED;
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Congruence* finite_cong_enumerate (Semigroup*, std::vector<relation_t> const&, bool report = false);

#endif // TC_H_
