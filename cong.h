//
// Semigroups++ - C/C++ library for computing with semigroups and monoids
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

#ifndef SEMIGROUPSPLUSPLUS_CONG_H_
#define SEMIGROUPSPLUSPLUS_CONG_H_

// TODO(?): the congruence object and the Todd-Coxeter data structures should be
// separated (i.e. all the Todd-Coxeter data stuff should go into a member
// class of Congruence). This will mean that we can run multiple instance of
// Todd-Coxeter within a single Congruence object, and also things like small
// overlap and Knuth-Bendix.

#include <atomic>
#include <forward_list>
#include <mutex>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "elements.h"
#include "semigroups.h"
#include "util/report.h"

namespace semigroupsplusplus {

  // Non-abstract
  // Class for representing a congruence on a semigroup defined either as an
  // instance of a <Semigroup> object or as a finitely presented semigroup
  // defined by generators and relations.
  //
  // This class and its implemented methods are rather rudimentary in the
  // current
  // version of Semigroups++.
  //
  // The word "coset" is used throughout this section to mean "congruence
  // class".

  class Congruence {
   private:
    // The different types of congruence.
    enum cong_t {
      // Left congruence
      LEFT = 0,
      // Right congruence
      RIGHT = 1,
      // 2-sided congruence
      TWOSIDED = 2
    };

    typedef size_t  coset_t;
    typedef int64_t signed_coset_t;

   public:
    // 5 parameters (for finitely presented semigroup)
    // @type string describing the type of congruence (left/right/twosided)
    // @nrgens the number of generators
    // @relations a vector of <relation_t>
    // @extra     a vector of extra <relation_t> (such as those used to define a
    // congruence on a finitely presented semigroup)
    // @thread_id an integer identifying the thread which is instantiating the
    // congruence (defaults to 0).
    //
    // This constructor returns an instance of a congruence object whose type is
    // described by the string <type>. The congruence is defined over the
    // semigroup defined by the generators <nrgens> and relations <relations>
    // and
    // is the least congruence containing the generating pairs in <extra>.
    //
    // Note that cldoc mangles the types of the parameters <relations> and
    // <extra>, which are both **std::vector&lt;&lt;relation_t&gt;&gt;**.
    Congruence(std::string                    type,
               size_t                         nrgens,
               std::vector<relation_t> const& relations,
               std::vector<relation_t> const& extra,
               size_t                         thread_id = 0);

    // 6 parameters (for a <Semigroup>)
    // @type      string describing the type of congruence (left/right/twosided)
    // @semigroup pointer to a <Semigroup>.
    // @extra     a vector of <relation_t> used to define a congruence on
    //            <semigroup>.
    // @prefill   prefill the coset table (for Todd-Coxeter) with the left or
    //            right Cayley graph of <semigroup>.
    // @report report during enumeration of <semigroup>, if any (defaults to
    //         <DEFAULT_REPORT_VALUE>)
    // @thread_id an integer identifying the thread which is instantiating the
    // congruence (defaults to 0).
    //
    // This constructor returns an instance of a congruence object whose type is
    // described by the string <type>. The congruence is defined over the
    // semigroup <semigroup> and is the least congruence containing the
    // generating pairs in <extra>.
    //
    // If the value <prefill> is **true**, then the left or right Cayley graph
    // of
    // <semigroup> is used to completely prefill the coset table in
    // <todd_coxeter>, so that only coincidences are processed during
    // <todd_coxeter>. This can be faster than simply running <todd_coxeter> on
    // the presentation defining <semigroup> in the case that the congruence
    // being defined has lots of equivalence classes when compared to the size
    // of
    // <semigroup> (for some definition of "lots").
    //
    // Note that cldoc mangles the types of the parameter <extra>, which is
    // **std::vector&lt;&lt;relation_t&gt;&gt;**.
    Congruence(std::string                    type,
               Semigroup*                     semigroup,
               std::vector<relation_t> const& extra,
               bool                           prefill,
               bool                           report    = DEFAULT_REPORT_VALUE,
               size_t                         thread_id = 0);

    // 5 parameters (for a coset table)
    // @type    string describing the type of congruence (left/right/twosided)
    // @nrgens  the number of generators
    // @extra   a vector of <relation_t> used to define the congruence being
    //          constructed.
    // @prefill a prefilled coset table (for Todd-Coxeter).
    // @thread_id an integer identifying the thread which is instantiating the
    // congruence (defaults to 0).
    //
    // This constructor returns an instance of a congruence object whose type is
    // described by the string <type>. The congruence is the least congruence
    // containing the generating pairs in <extra>, and it is defined on the
    // semigroup whose right or left Cayley graph is described by <prefill>.
    //
    // Note that cldoc mangles the types of the parameters <extra>, which is
    // **std::vector&lt;&lt;relation_t&gt;&gt;**.
    Congruence(std::string                    type,
               size_t                         nrgens,
               std::vector<relation_t> const& extra,
               RecVec<coset_t>&               prefill,
               size_t                         thread_id = 0);

    //
    // A default destructor.
    ~Congruence() {}

    // const
    //
    // @return **true** if the method <todd_coxeter> has been run until its
    // conclusion, and **false** if it has not.
    bool is_tc_done() const {
      return _tc_done;
    }

    // non-const
    // @report report during the algorithm, if any (defaults to
    //         <DEFAULT_REPORT_VALUE>)
    //
    // This runs the
    // [Todd-Coxeter
    // algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm)
    // on **this**. Note that this may never terminate, but can be killed using
    // <kill>. The implementation is based on one by Goetz Pfeiffer in GAP.
    void todd_coxeter(bool report = DEFAULT_REPORT_VALUE);

    // non-const
    // @word   a <word_t> in the (indices of) the generators of the semigroup
    //         that **this** is defined over.
    // @report report during <todd_coxeter>, if any (defaults to
    //         <DEFAULT_REPORT_VALUE>)
    //
    // This method is non-const because it will call <todd_coxeter> if it has
    // not
    // already been run to completion.
    //
    // @return the index of the coset corresponding to <word>.
    coset_t word_to_coset(word_t word, bool report = DEFAULT_REPORT_VALUE);

    // non-const
    // @report report during <todd_coxeter>, if any (defaults to
    //         <DEFAULT_REPORT_VALUE>)
    //
    // This method compresses the coset table used by <todd_coxeter>.
    //
    // This method is non-const because it will call <todd_coxeter> if it has
    // not
    // already been run to completion.
    void compress(bool report = DEFAULT_REPORT_VALUE);

    // non-const
    // @report report during <todd_coxeter>, if any (defaults to
    //         <DEFAULT_REPORT_VALUE>)
    //
    // This method is non-const because it will call <todd_coxeter> if it has
    // not
    // already been run to completion.
    //
    // @return the number of congruences classes (or cosets) of the congruence.

    size_t nr_classes(bool report = DEFAULT_REPORT_VALUE) {
      if (!is_tc_done()) {
        todd_coxeter(report);
      }
      return _active - 1;
    }

    // non-const
    //
    // This method can be used to terminate <todd_coxeter> if it is running for
    // too long, or if an instance running in another thread concludes before
    // this.
    void kill() {
      _killed = true;
    }

   private:
    Congruence(cong_t                         type,
               size_t                         nrgens,
               std::vector<relation_t> const& relations,
               std::vector<relation_t> const& extra,
               size_t                         thread_id = 0);

    Congruence(cong_t                         type,
               Semigroup*                     semigroup,
               std::vector<relation_t> const& extra,
               bool                           prefill,
               bool                           report,
               size_t                         thread_id = 0);

    Congruence(cong_t                         type,
               size_t                         nrgens,
               std::vector<relation_t> const& extra,
               RecVec<coset_t>&               prefill,
               size_t                         thread_id = 0);

    void init_after_prefill();

    void        new_coset(coset_t const&, letter_t const&);
    void        identify_cosets(coset_t, coset_t);
    inline void trace(coset_t const&, relation_t const&, bool add = true);
    void   check_forwd();
    cong_t type_from_string(std::string);

    cong_t _type;

    bool _tc_done;  // Has todd_coxeter already been run?
    bool _is_compressed;

    coset_t                 _id_coset;  // TODO(JDM) Remove?
    size_t                  _nrgens;
    std::vector<relation_t> _relations;
    std::vector<relation_t> _extra;

    size_t _active;  // Number of active cosets

    size_t _pack;  // Nr of active cosets allowed before a
                   // packing phase starts

    std::atomic<bool> _killed;

    //
    // COSET LISTS:
    //
    // We use these two arrays to simulate a doubly-linked list of active cosets
    // (the "active list") with deleted cosets attached to the end (the "free
    // list").  If c is an active coset:
    //   _forwd[c] is the coset that comes after c in the list.
    //   _bckwd[c] is the coset that comes before c in the list.
    // If c is a free coset (has been deleted) the backward reference is not
    // needed, and so instead, _bckwd[c] is set to the coset c was identified
    // with.  To indicate this alternative use of the list, the entry is negated
    // (_backwd[c] == -3 indicates that c was identified with coset 3).
    //
    std::vector<coset_t>        _forwd;
    std::vector<signed_coset_t> _bckwd;
    //
    // We also store some special locations in the list:
    //   _current is the coset to which we are currently applying relations.
    //   _current_no_add is used instead of _current if we are in a packing
    //   phase.
    //   _last points to the final active coset in the list.
    //   _next points to the first free coset in the list.
    // Hence usually _next == _last + 1.
    //
    coset_t _current;
    coset_t _current_no_add;
    coset_t _last;
    coset_t _next;

    //
    // COSET TABLES:
    //
    // We use these three tables to store all a coset's images and preimages.
    //   _table[c][i] is coset c's image under generator i.
    //   _preim_init[c][i] is ONE of coset c's preimages under generator i.
    //   _preim_next[c][i] is a coset that has THE SAME IMAGE as coset c (under
    //   i)
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
    RecVec<coset_t> _table;
    RecVec<coset_t> _preim_init;
    RecVec<coset_t> _preim_next;

    // Stacks for identifying cosets
    std::stack<coset_t> _lhs_stack;
    std::stack<coset_t> _rhs_stack;

    // Statistics etc.
    size_t _defined;
    size_t _cosets_killed;
    size_t _stop_packing;  // TODO(JDM): make this a bool?
    size_t _next_report;

    size_t _thread_id;

    static size_t   INFTY;
    static size_t   UNDEFINED;
    static Reporter _reporter;
  };

  // Competitive Todd Coxeter
  // @cong1 the first congruence to run <Congruence::todd_coxeter>
  // @cong2 the second congruence to run <Congruence::todd_coxeter>
  // @report report during <Congruence::todd_coxeter> and this function itself,
  // if any (defaults to <DEFAULT_REPORT_VALUE>)
  //
  // This function can be used to run <Congruence::todd_coxeter> in parallel on
  // two congruences. For example, if we want to compute a congruence on a
  // <Semigroup> object, then we may want to run the Todd-Coxeter algorithm on
  // an
  // instance of <Congruence> where the coset table is prefilled with the left
  // or
  // right Cayley graph (where appropriate), and on another instance where the
  // coset table is not prefilled.
  //
  // When the <Congruence::todd_coxeter> method of one <Congruence> object
  // terminates it kills the other method, <Congruence::kill>.
  //
  // @return either <cong1> or <cong2> depending on which of them the
  // <Congruence::todd_coxeter> method finished first.

  Congruence* parallel_todd_coxeter(Congruence* cong1,
                                    Congruence* cong2,
                                    bool        report = DEFAULT_REPORT_VALUE);
}  // namespace semigroupsplusplus
#endif  // SEMIGROUPSPLUSPLUS_CONG_H_
