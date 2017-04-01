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

#ifndef LIBSEMIGROUPS_CONG_H_
#define LIBSEMIGROUPS_CONG_H_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "partition.h"
#include "semigroups.h"
#include "util/report.h"

#define RETURN_FALSE nullptr

namespace libsemigroups {

  // Non-abstract
  // Class for representing a congruence on a semigroup defined either as an
  // instance of a <Semigroup> object or as a finitely presented semigroup
  // defined by generators and relations.
  //
  // This class and its implemented methods are rather rudimentary in the
  // current version of libsemigroups.
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
    //
    // Type for indices of congruence classes in a Congruence object.
    typedef size_t class_index_t;

    // The maximum number of steps that can be done in one batch when processing
    static const size_t LIMIT_MAX = std::numeric_limits<size_t>::max();

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
    // @semigroup pointer to a <Semigroup>
    // @genpairs  a vector of <relation_t> used to define a congruence on
    //            <semigroup>.
    //
    // This constructor returns an instance of a congruence object whose type is
    // described by the string <type>. The congruence is defined over the
    // semigroup <semigroup> and is the least congruence containing the
    // generating pairs in <genpairs>.
    //
    // Note that cldoc mangles the types of the parameter <genpairs>, which is
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
    // This method is non-const because it may fully compute a data structure
    // for the congruence.
    //
    // @return the index of the coset corresponding to <word>.
    class_index_t word_to_class_index(word_t const& word) {
      DATA* data = get_data();
      assert(data->is_done());
      return data->word_to_class_index(word);
    }

    // non-const
    //
    // This method is non-const because it may fully compute a data structure
    // for the congruence.
    //
    // @return the number of congruences classes (or cosets) of the congruence.
    size_t nr_classes() {
      DATA* data = get_data();
      assert(data->is_done());
      return data->nr_classes();
    }

    // non-const
    //
    // This method is non-const because it involves fully computing the
    // congruence.
    //
    // @return this method returns the non-trivial classes of the congruence,
    // the elements in these classes are represented as words in the
    // generators of the semigroup over which the congruence is defined.
    Partition<word_t> nontrivial_classes() {
      DATA* data = get_data();
      assert(data->is_done());
      return data->nontrivial_classes();
    }

    // non-const
    //
    // @return **true** if the congruence is fully determined, and **false** if
    // it is not.
    bool is_done() const {
      if (_data == nullptr) {
        return false;
      }
      return _data->is_done();
    }

    // non-const
    //
    // This method is non-const since if the congruence is defined over a
    // <Semigroup> object, then we may have to obtain the relations of
    // <Semigroup>.
    //
    // @return the vector of relations (or equivalently, generating
    // pairs) used to define the congruence.
    std::vector<relation_t> const& relations() {
      init_relations(_semigroup);
      return _relations;
    }

    // const
    //
    // @return the vector of extra relations (or equivalently, generating
    // pairs) used to define the congruence.
    std::vector<relation_t> const& extra() const {
      return _extra;
    }

    // non-const
    // @relations defining relations of the semigroup over which the congruence
    // is defined.
    //
    // This method allows the relations of the semigroup over which the
    // congruence is defined to be specified. This method asserts that the
    // relations have not previously be specified.
    void set_relations(std::vector<relation_t> const& relations) {
      assert(_relations.empty());  // _extra can be non-empty!
      _relations = relations;
    }

    // const
    // @val a boolean value
    //
    // If @val is true, then some methods for a Congruence object may report
    // information about the progress of the computation.
    void set_report(bool val) const {
      glob_reporter.set_report(val);
      assert(glob_reporter.get_report() == val);
    }

    // non-const
    // @table a partial coset table for Todd-Coxeter
    //
    // This method allows a partial coset table, for the Todd-Coxeter
    // algorithm, to be specified. This must be done before anything is
    // computed about the congruence, otherwise it has not effect.
    void set_prefill(RecVec<class_index_t> const& table) {
      if (_data == nullptr) {
        _prefill = table;
      }
    }

    // non-const
    // @nr_threads the maximum number of threads
    //
    // This method sets the maximum number of threads to be used by any method
    // of a Congruence object which is defined over a <Semigroup>. The
    // number of threads is limited to the minimum of <nr_threads> and the
    // number of threads supported by the hardware.
    //
    // If the congruence is not defined over a <Semigroup>, then the number of
    // threads is not limited by this method.
    void set_max_threads(size_t nr_threads) {
      unsigned int n =
          static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
      _max_threads = std::min(n, std::thread::hardware_concurrency());
    }

    // non-const
    // This forces the congruence to use the [Todd-Coxeter
    // algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm)
    // to compute the congruence. This is non-const because any existing data
    // structure for the congruence will be deleted, and replaced with a
    // Todd-Coxeter data structure.
    //
    // Note that when applied to an arbitrary finitely presented semigroup this
    // may never terminate.
    //
    // The implementation is based on one by Goetz Pfeiffer in GAP.
    void force_tc();

    // non-const
    // This forces the congruence to use the [Todd-Coxeter
    // algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm)
    // to compute the congruence. This is non-const because any existing data
    // structure for the congruence will be deleted, and replaced with a
    // Todd-Coxeter data structure.
    //
    // When applied to a congruence defined over a <Semigroup> denoted *S*,
    // this method differs from <force_tc> in that the so-called coset table
    // used in the Todd-Coxeter algorithm is initialised to contain the right
    // (or left) Cayley graph of *S*.
    //
    // If the congruence is not defined over a finite semigroup, then this
    // method does the same as <force_tc>.
    //
    // Note that when applied to an arbitrary finitely presented semigroup this
    // may never terminate.
    void force_tc_prefill();

    // non-const
    // This forces the congruence to use an elementary orbit algorithm which
    // enumerates pairs of <Element>s that are related in the congruence.
    //
    // This method is unlikely to terminate, or to be faster than the other
    // methods, unless there are a relatively few pairs of elements related by
    // the congruence.
    //
    // This is non-const because any existing data structure for the congruence
    // will be deleted, and replaced with a data structure specific to the
    // orbit algorithm mentioned above.
    //
    // Note that when applied to an arbitrary finitely presented semigroup this
    // may never terminate.
    void force_p();

    // non-const
    // This forces the congruence to use the [Knuth-Bendix
    // algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    // to compute the congruence defined by the generators and <relations> of
    // **this** followed by an elementary orbit algorithm which
    // enumerates pairs of <Element>s of this semigroup that are
    // related in **this**.
    //
    // This is non-const because any existing data structure for the congruence
    // will be deleted, and replaced with a data structure specific to this
    // method.
    //
    // Note that when applied to an arbitrary finitely presented semigroup this
    // may never terminate.
    void force_kbp();

    // non-const
    // This forces the congruence to use the [Knuth-Bendix
    // algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    // to compute the congruence defined by the generators, <relations> and
    // <extra> of **this** followed by the Froidure-Pin algorithm (implemented
    // in
    // <Semigroup::enumerate>) on this semigroup.
    //
    // This is non-const because any existing data structure for the congruence
    // will be deleted, and replaced with a data structure specific to this
    // method.
    //
    // Note that when applied to an arbitrary finitely presented semigroup this
    // will only terminate if the finitely presented semigroup is finite.
    void force_kbfp();

   private:
    // Subclasses of DATA
    class KBFP;  // Knuth-Bendix followed by Froidure-Pin
    class KBP;   // Knuth-Bendix followed by P
    class P;     // Orbit of pairs
    class TC;    // Todd-Coxeter

    // Abstract base class for nested classes containing methods for actually
    // enumerating the classes etc of a congruence
    class DATA {
      friend KBFP;
      friend KBP;
      friend P;
      friend TC;

     public:
      // Default constructor
      // @cong keeping cldoc happy
      // @report_interval keeping cldoc happy
      explicit DATA(Congruence& cong,
                    size_t      default_nr_steps,
                    size_t      report_interval = 1000)
          : _cong(cong),
            _default_nr_steps(default_nr_steps),
            _killed(false),
            _report_interval(report_interval),
            _report_next(0) {}

      // Default destructor, does nothing
      virtual ~DATA() {}

      // This method runs the algorithm used to determine the congruence, i.e.
      // Todd-Coxeter, Knuth-Bendix, etc., until completion
      virtual void run() = 0;

      // This method runs the algorithm for a while, then stops after a certain
      // amount of work or when done
      // @steps the amount of work to do before returning
      virtual void run(size_t steps) = 0;

      // This method returns true if a DATA object's run method has been run to
      // conclusion, i.e. that it has not been killed by another instance.
      // @return keeping cldoc happy
      virtual bool is_done() const = 0;

      // This method returns the number of classes of the congruence.
      //
      // @return keeping cldoc happy
      virtual size_t nr_classes() = 0;

      //
      // @word keeping cldoc happy
      // This method returns the index of the congruence class containing the
      // element of the semigroup defined by <word>.
      //
      // @return keeping cldoc happy
      virtual class_index_t word_to_class_index(word_t const& word) = 0;

      // This method returns the non-trivial classes of the congruence.
      //
      // @return keeping cldoc happy
      virtual Partition<word_t> nontrivial_classes();

      // This method kills a given instance of a DATA object.
      void kill() {
        // TODO add killed-by-thread
        _killed = true;
      }

      // This method can be used to tell whether or not a given DATA object has
      // been killed by another instance.
      // @return keeping cldoc happy
      std::atomic<bool>& is_killed() {
        return _killed;
      }

      // Non-const
      // @goal_func a function to test whether we can stop running
      //
      // This function calls **run** on the DATA object in batches until
      // goal_func returns true.  If goal_func is RETURN_FALSE, then the object
      // is instead run to completion.
      void run_until(std::function<bool(DATA*)> goal_func) {
        if (is_done()) {
          return;
        }
        if (goal_func != RETURN_FALSE) {
          while (!_killed && !is_done() && !goal_func(this)) {
            run(_default_nr_steps);
          }
        } else {
          run();
        }
      }

      // Compress the data structure, this does nothing by default, but
      // rewriting systems and the Todd-Coxeter data structures can be
      // compressed, to use less memory.
      virtual void compress() {}

     private:
      // This initialises the data structure, and can be run from inside a
      // thread after construction.  If already initialised, this does nothing.
      virtual void init() = 0;

      Congruence&                   _cong;
      size_t                        _default_nr_steps;
      std::atomic<bool>             _killed;
      size_t                        _report_interval;
      size_t                        _report_next;
      typedef std::vector<word_t*>  class_t;
      typedef std::vector<class_t*> partition_t;
    };

    // Set the relations of a Congruence object to the relations of the
    // semigroup over which the Congruence is defined (if any). Report is here
    // in case of any enumeration of the underlying semigroup.
    void init_relations(Semigroup* semigroup, std::atomic<bool>& killed);

    void init_relations(Semigroup* semigroup) {
      std::atomic<bool> killed(false);
      init_relations(semigroup, killed);
    }

    DATA* cget_data() const {
      return _data;
    }

    DATA* get_data(std::function<bool(DATA*)> goal_func = RETURN_FALSE);

    DATA* winning_data(std::vector<DATA*>&                      data,
                       std::vector<std::function<void(DATA*)>>& funcs,
                       bool                       ignore_max_threads = false,
                       std::function<bool(DATA*)> goal_func = RETURN_FALSE);

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
