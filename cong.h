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

#ifndef LIBSEMIGROUPS_CONG_H_
#define LIBSEMIGROUPS_CONG_H_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "semigroups.h"
#include "util/report.h"

namespace libsemigroups {

  // Non-abstract
  // Class for representing a congruence on a semigroup defined either as an
  // instance of a <Semigroup> object or as a finitely presented semigroup
  // defined by generators and relations.
  //
  // This class and its implemented methods are rather rudimentary in the
  // current version of Semigroups++.
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

   public:
    typedef size_t                      class_index_t;
    typedef std::vector<Element const*> class_t;
    typedef std::vector<class_t>        partition_t;

    // 5 parameters (for finitely presented semigroup)
    // @type string describing the type of congruence (left/right/twosided)
    // @nrgens the number of generators
    // @relations a vector of <relation_t>
    // @extra     a vector of extra <relation_t> (such as those used to define a
    // congruence on a finitely presented semigroup)
    //
    // This constructor returns an instance of a congruence object whose type
    // is described by the string <type>. The congruence is defined over the
    // semigroup defined by the generators <nrgens> and relations <relations>
    // and is the least congruence containing the generating pairs in <extra>.
    //
    // Note that cldoc mangles the types of the parameters <relations> and
    // <extra>, which are both **std::vector&lt;&lt;relation_t&gt;&gt;**.
    Congruence(std::string                    type,
               size_t                         nrgens,
               std::vector<relation_t> const& relations,
               std::vector<relation_t> const& extra);

    // 6 parameters (for a <Semigroup>)
    // @type      string describing the type of congruence (left/right/twosided)
    // @semigroup pointer to a <Semigroup>.
    // @genpairs a vector of <relation_t> used to define a congruence on
    //            <semigroup>.
    //
    // This constructor returns an instance of a congruence object whose type is
    // described by the string <type>. The congruence is defined over the
    // semigroup <semigroup> and is the least congruence containing the
    // generating pairs in <extra>.
    //
    // If the value <prefill> is **true**, then the left or right Cayley graph
    // of <semigroup> is used to completely prefill the coset table in
    // <todd_coxeter>, so that only coincidences are processed during
    // <todd_coxeter>. This can be faster than simply running <todd_coxeter> on
    // the presentation defining <semigroup> in the case that the congruence
    // being defined has lots of equivalence classes when compared to the size
    // of <semigroup> (for some definition of "lots").
    //
    // Note that cldoc mangles the types of the parameter <extra>, which is
    // **std::vector&lt;&lt;relation_t&gt;&gt;**.
    Congruence(std::string                    type,
               Semigroup*                     semigroup,
               std::vector<relation_t> const& genpairs);

    //
    // A default destructor.
    ~Congruence() {
      delete _data;
      // The caller is responsible for deleting the _semigroup (if any)
    }

    // non-const
    // @word   a <word_t> in the (indices of) the generators of the semigroup
    //         that **this** is defined over.
    //
    // This method is non-const because it will call <todd_coxeter> if it has
    // not already been run to completion.
    //
    // @return the index of the coset corresponding to <word>.
    class_index_t word_to_class_index(word_t const& word) {
      DATA* data = get_data();
      if (!data->is_done()) {
        data->run();
      }
      return data->word_to_class_index(word);
    }

    // non-const
    //
    // This method is non-const because it will call <todd_coxeter> if it has
    // not already been run to completion.
    //
    // @return the number of congruences classes (or cosets) of the congruence.
    size_t nr_classes() {
      DATA* data = get_data();
      if (!data->is_done()) {
        data->run();
      }
      return data->nr_classes();
    }

    partition_t nontrivial_classes() {
      DATA* data = get_data();
      if (!data->is_done()) {
        data->run();
      }
      return data->nontrivial_classes();
    }

    bool is_done() {
      if (_data == nullptr) {
        return false;
      }
      return get_data()->is_done();
    }

    cong_t get_type() {
      return _type;
    }

    std::vector<relation_t> const& relations() {
      init_relations(_semigroup);
      return _relations;
    }

    std::vector<relation_t> const& extra() {
      return _extra;
    }

    void set_relations(std::vector<relation_t> const& relations) {
      assert(_relations.empty());  // _extra can be non-empty!
      _relations = relations;
    }

    void set_report(bool val) {
      glob_reporter.set_report(val);
      assert(glob_reporter.get_report() == val);
    }

    void run() {
      get_data();
    }

    void compress() {
      DATA* data = get_data();
      if (!data->is_done()) {
        data->run();
      }
      data->compress();
    }

    void set_prefill(RecVec<class_index_t>& table) {
      _prefill = table;
    }

    void set_max_threads(size_t nr_threads) {
      unsigned int n =
          static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
      _max_threads = std::min(n, std::thread::hardware_concurrency());
    }

    // non-const
    // This runs the [Todd-Coxeter
    // algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) on
    // **this**. Note that this may never terminate, but can be killed using
    // <kill>. The implementation is based on one by Goetz Pfeiffer in GAP.
    void force_tc();
    void force_tc_prefill();
    void force_p();
    void force_kbp();
    void force_kbfp();

   private:
    // Abstract base class for nested classes containing methods for actually
    // enumerating the classes etc of a congruence
    class DATA {
     public:
      explicit DATA(Congruence& cong, size_t report_interval = 1000)
          : _cong(cong),
            _killed(false),
            _report_interval(report_interval),
            _report_next(0) {}

      virtual ~DATA() {}

      virtual void          run()                                   = 0;
      virtual bool          is_done() const                         = 0;
      virtual size_t        nr_classes()                            = 0;
      virtual class_index_t word_to_class_index(word_t const& word) = 0;
      virtual partition_t nontrivial_classes()                      = 0;

      void kill() {
        // TODO add killed-by-thread
        _killed = true;
      }

      std::atomic<bool>& get_killed() {
        return _killed;
      }

      virtual void compress() {}

     protected:
      Congruence&       _cong;
      std::atomic<bool> _killed;
      size_t            _report_interval;
      size_t            _report_next;
    };

    // Subclasses of DATA
    class KBFP;  // Knuth-Bendix followed by Froidure-Pin
    class TC;    // Todd-Coxeter
    class P;     // Orbit of pairs
    class KBP;   // Knuth-Bendix followed by P

    // Set the relations of a Congruence object to the relations of the
    // semigroup over which the Congruence is defined (if any). Report is here
    // in case of any enumeration of the underlying semigroup.
    void init_relations(Semigroup* semigroup, std::atomic<bool>& killed);

    void init_relations(Semigroup* semigroup) {
      std::atomic<bool> killed(false);
      init_relations(semigroup, killed);
    }

    DATA* get_data();
    DATA* winning_data(std::vector<DATA*>&                      data,
                       std::vector<std::function<void(DATA*)>>& funcs,
                       bool ignore_max_threads = false);

    Congruence(cong_t                         type,
               size_t                         nrgens,
               std::vector<relation_t> const& relations,
               std::vector<relation_t> const& extra);

    Congruence(cong_t                         type,
               Semigroup*                     semigroup,
               std::vector<relation_t> const& extra);

    cong_t type_from_string(std::string);

    DATA*                   _data;
    std::vector<relation_t> _extra;
    size_t                  _max_threads;
    size_t                  _nrgens;
    RecVec<class_index_t>   _prefill;
    std::vector<relation_t> _relations;
    std::atomic<bool>       _relations_done;
    Semigroup*              _semigroup;
    cong_t                  _type;
    std::mutex              _mtx;

    static size_t const INFTY;
    static size_t const UNDEFINED;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_CONG_H_
