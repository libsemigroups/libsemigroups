/*******************************************************************************
 * Semigroups++
 *
 * This file contains ...
 *
 *******************************************************************************/

#ifndef TC_H_
#define TC_H_

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
  Congruence (Semigroup*, std::vector<relation_t> const&);
  explicit Congruence (Semigroup*);

  void todd_coxeter (size_t limit);

  void todd_coxeter () {
    todd_coxeter(INFTY);
  }

  size_t nr_active_cosets () {
    return _active;
  }

 private:
  void new_coset(coset_t const&, letter_t const&);
  void identify_cosets(coset_t, coset_t);
  void trace(coset_t const&, relation_t const&, bool add = true);

  size_t                                  _nrgens;
  std::vector<relation_t>                 _relations;

  size_t                                  _active;

  std::vector<coset_t>                    _forwd;
  std::vector<signed_coset_t>             _bckwd;
  coset_t                                 _current;
  coset_t                                 _current_no_add;
  coset_t                                 _next;
  coset_t                                 _last;

  RecVec<coset_t>                         _table; // coset table
  RecVec<coset_t>                         _preim_init;
  RecVec<coset_t>                         _preim_next;

  std::stack<coset_t>                     _lhs_stack;
  std::stack<coset_t>                     _rhs_stack;

  // statistics etc
  bool                                    _report;
  size_t                                  _defined;
  size_t                                  _next_report;

  static size_t                           INFTY;
  static size_t                           UNDEFINED;
};

#endif // TC_H_
