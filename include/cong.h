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

#ifndef LIBSEMIGROUPS_INCLUDE_CONG_H_
#define LIBSEMIGROUPS_INCLUDE_CONG_H_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <queue>
#include <stack>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

#include "misc/partition.h"
#include "misc/report.h"
#include "misc/uf.h"

#include "constants.h"
#include "semigroup-traits.h"
#include "semigroup.h"

#define RETURN_FALSE nullptr

namespace libsemigroups {

  //! Class for congruence on a semigroup or fintely presented semigroup.
  //!
  //! This class represents a congruence on a semigroup defined either as an
  //! instance of a Semigroup object or as a finitely presented semigroup
  //! defined by generators and relations.
  //!
  //! The structure of a Congruence is determined by running several different
  //! methods concurrently until one method returns an answer. The number of
  //! different threads used can be controlled, in part, by the value passed to
  //! Congruence::set_max_threads. The use of multiple threads to find the
  //! structure of a Congruence can cause the return values of certain
  //! methods to differ for different instances of mathematically equal
  //! objects.
  //!
  //! This class and its implemented methods are somewhat rudimentary in
  //! the current version of libsemigroups.
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
    //! Type for indices of congruence classes in a Congruence object.
    typedef size_t class_index_t;

    //! Constructor for congruences over a finitely presented semigroup.
    //!
    //! The parameters are as follows:
    //!
    //! * \p type: a std::string describing the type of congruence, must be one
    //!   of \c "left", \c "right", or \c "twosided".
    //!
    //! * \p nrgens: the number of generators.
    //!
    //! * \p relations: the defining relations of the semigroup over which the
    //! congruence being constructed is defined. Every relation_type in this
    //! parameter must consist of positive integers less than \p nrgens.
    //!
    //! * \p extra: additional relations corresponding to the generating pairs
    //! of the congruence being constructed.  Every relation_type in this
    //! parameter must consist of positive integers less than \p nrgens.
    //!
    //! This constructor returns an instance of a Congruence object whose type
    //! is described by the string \p type. The congruence is defined over the
    //! semigroup defined by \p nrgens generators and the relations \p relations
    //! and is the least congruence containing the generating pairs in \p extra.
    //!
    //! For example, to compute a congruence over the free semigroup the
    //! parameter \p relations should be empty, and the relations defining the
    //! congruence to be constructed should be passed as the parameter \p
    //! extra. To compute a congruence over a finitely presented semigroup the
    //! relations defining the fintely presented semigroup must be passed as
    //! the parameter \p relations and the relations defining the
    //! congruence to be constructed should be passed as the parameter \p
    //! extra.
    Congruence(std::string                       type,
               size_t                            nrgens,
               std::vector<relation_type> const& relations,
               std::vector<relation_type> const& extra);

    //! Constructor for congruences over a Semigroup object.
    //!
    //! The parameters are as follows:
    //!
    //! * \p type: a std::string describing the type of congruence, must be one
    //!   of \c "left", \c "right", or \c "twosided".
    //!
    //! * \p semigroup: a pointer to an instance of Semigroup. It is the
    //! responsibility of the caller to delete \p semigroup.
    //!
    //! * \p genpairs: additional relations corresponding to the generating
    //! pairs
    //! of the congruence being constructed.  Every relation_type in this
    //! parameter must consist of positive integers less than the number of
    //! generators of \p semigroup (Semigroup::nrgens()).
    //!
    //! This constructor returns an instance of a Congruence object whose type
    //! is described by the string \p type. The congruence is defined over the
    //! Semigroup \p semigroup and is the least congruence containing the
    //! generating pairs in \p extra.
    Congruence(std::string                       type,
               SemigroupBase*                    semigroup,
               std::vector<relation_type> const& genpairs);

    //! A default destructor.
    //!
    //! The caller is responsible for deleting the semigroup used to construct
    //! \c this, if any.
    ~Congruence() {
      delete_data();
    }

    //! Returns the index of the congruence class corresponding to \p word.
    //!
    //! The parameter \p word must be a libsemigroups::word_type consisting of
    //! indices of the generators of the semigroup over which \c this is
    //! defined.
    //!
    //! If \c this is defined over a semigroup with generators \f$A\f$, then
    //! Congruence::word_to_class_index defines a surjective function from the
    //! set of all words over \f$A\f$ to either \f$\{0, 1, \ldots, n - 1\}\f$,
    //! where \f$n\f$ is the number of classes, or to the non-negative integers
    //! \f$\{0, 1, \ldots\}\f$ if \c this has infinitely many classes.
    //!
    //! \warning The method for finding the structure of a congruence is
    //! non-deterministic, and the return value of this method may vary
    //! between different instances of the same congruence.
    class_index_t word_to_class_index(word_type const& word) {
      DATA* data = get_data();
      LIBSEMIGROUPS_ASSERT(data->is_done());
      return data->word_to_class_index(word);
    }

    //!  Returns \c true if the words \p w1 and \p w2 belong to the
    //! same congruence class.
    //!
    //! The parameters  \p w1  and \p w2 must be libsemigroups::word_type's
    //! consisting of indices of generators of the semigroup over which \c this
    //! is defined.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    bool test_equals(word_type const& w1, word_type const& w2) {
      if (w1 == w2) {
        return true;
      }
      DATA* data;
      if (is_done()) {
        data = _data;
      } else {
        std::function<bool(DATA*)> words_func = [&w1, &w2](DATA* d) {
          return d->current_equals(w1, w2) != DATA::result_t::UNKNOWN;
        };
        data = get_data(words_func);
      }
      DATA::result_t result = data->current_equals(w1, w2);
      LIBSEMIGROUPS_ASSERT(result != DATA::result_t::UNKNOWN);
      return result == DATA::result_t::TRUE;
    }

    //!  Returns \c true if the congruence class of \p w1 is less than
    //! that of \p w2.
    //!
    //! This method returns \c true if the congruence class of \p w1 is less
    //! than
    //! the class of \p w2 in a total ordering of congruence classes.
    //!
    //! The parameters \p w1 and \p w2 should be libsemigroups::word_type's
    //! consisting of indices of the generators of the semigroup over which \c
    //! this is defined.
    //!
    //! \warning The method for finding the structure of a congruence is
    //! non-deterministic, and the total order of congruences classes may vary
    //! between different instances of the same congruence.
    //!
    //! \warning The problem of determining the return value of this method is
    //! undecidable in general, and this method may never terminate.
    bool test_less_than(word_type const& w1, word_type const& w2) {
      DATA* data;
      if (is_done()) {
        data = _data;
      } else {
        std::function<bool(DATA*)> words_func = [&w1, &w2](DATA* d) {
          return d->current_less_than(w1, w2) != DATA::result_t::UNKNOWN;
        };
        data = get_data(words_func);
      }

      if (!_partial_data.empty()) {
        LIBSEMIGROUPS_ASSERT(_data == nullptr);
        // Delete the losers and clear _partial_data
        for (size_t i = 0; i < _partial_data.size(); i++) {
          if (_partial_data[i] != data) {
            delete _partial_data[i];
          }
        }
        _partial_data.clear();
      }
      _data = data;

      DATA::result_t result = data->current_less_than(w1, w2);
      LIBSEMIGROUPS_ASSERT(result != DATA::result_t::UNKNOWN);
      return result == DATA::result_t::TRUE;
    }

    //! Returns the number of congruences classes of \c this.
    //!
    //! This method is non-const because it may fully compute a data structure
    //! for the congruence.
    //!
    //! \warning The problem of determining the number of classes of a
    //! congruence over a finitely presented semigroup is undecidable in
    //! general, and this method may never terminate.
    size_t nr_classes() {
      DATA* data = get_data();
      LIBSEMIGROUPS_ASSERT(data->is_done());
      return data->nr_classes();
    }

    //! Returns the non-trivial classes of the congruence.
    //!
    //! The elements in these classes are represented as words in the
    //! generators of the semigroup over which the congruence is defined.
    //!
    //! \warning If \c this has infinitely many non-trivial congruence classes,
    //! then this method will only terminate when it can no longer allocate
    //! memory.
    Partition<word_type>* nontrivial_classes();

    //! Returns \c true if the structure of the congruence is known.
    bool is_done() const {
      if (_data == nullptr) {
        return false;
      }
      return _data->is_done();
    }

    //!  Returns the vector of relations used to define the semigroup
    //! over which the congruence is defined.
    //!
    //! This method is non-const since if the congruence is defined over a
    //! Semigroup object, then we may have to compute and store its relations.
    std::vector<relation_type> const& relations() {
      init_relations(_semigroup);
      return _relations;
    }

    //!  Returns the vector of extra relations (or equivalently,
    //! generating pairs) used to define the congruence.
    std::vector<relation_type> const& extra() const {
      return _extra;
    }

    //!  Define the relations defining the semigroup over which \c this
    //! is defined.
    //!
    //! This method allows the relations of the semigroup over which the
    //! congruence is defined to be specified. This method asserts that the
    //! relations have not previously been specified.

    // FIXME it would be better to provide a constructor from another
    // congruence that copied the relations.
    void set_relations(std::vector<relation_type> const& relations) {
      LIBSEMIGROUPS_ASSERT(_relations.empty());  // _extra can be non-empty!
      _relations = relations;
    }

    //! Specify a partial coset table for the Todd-Coxeter algorithm.
    //!
    //! The parameter \p table should be a partial coset table for use in the
    //! Todd-Coxeter algorithm:
    //!
    //! * \p table should have RecVec::nr_cols equal to the number of generators
    //! of the semigroup over which \c this is defined
    //!
    //! * every entry in \p table must be less than the number of rows in
    //! \p table.
    //!
    //! For example, \p table can represent the right Cayley graph of a
    //! finite semigroup.
    //!
    //! If this method is called after anything has been computed about the
    //! congruence, it has no effect.
    void set_prefill(RecVec<class_index_t> const& table) {
      if (_data == nullptr) {
        _prefill = table;
      }
    }

    //! Set the maximum number of threads for a Congruence over a Semigroup.
    //!
    //! This method sets the maximum number of threads to be used by any method
    //! of a Congruence object which is defined over a Semigroup. The
    //! number of threads is limited to the maximum of 1 and the minimum of
    //! \p nr_threads and the number of threads supported by the hardware.
    //!
    //! If the congruence is not defined over a Semigroup, then the number of
    //! threads is not limited by this method.
    void set_max_threads(size_t nr_threads) {
      unsigned int n
          = static_cast<unsigned int>(nr_threads == 0 ? 1 : nr_threads);
      _max_threads = std::min(n, std::thread::hardware_concurrency());
    }

    //! Set the maximum number of active cosets in Todd-Coxeter before entering
    //! packing phase.
    //!
    //! This method only has any effect if used after Congruence::force_tc.
    void set_pack(size_t val) {
      if (_data != nullptr) {
        _data->set_pack(val);
      }
    }

    //! Sets how often the core methods of Congruence report.
    //!
    //! The smaller this value, the more often information will be reported.
    void set_report_interval(size_t val) {
      if (_data != nullptr) {
        _data->set_report_interval(val);
      }
    }

    //! Use the Todd-Coxeter algorithm.
    //!
    //! This methods forces the use of the
    //! [Todd-Coxeter
    //! algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) to
    //! compute the congruence.  The implementation is based on one by Goetz
    //! Pfeiffer in GAP.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Todd-Coxeter Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_tc();

    //! Use the Todd-Coxeter algorithm after prefilling the table.
    //!
    //! This methods forces the use of the
    //! [Todd-Coxeter
    //! algorithm](https://en.wikipedia.org/wiki/Todd–Coxeter_algorithm) to
    //! compute the congruence.
    //!
    //! When applied to a congruence defined over a Semigroup,
    //! this method differs from Congruence::force_tc in that the so-called
    //! coset table used in the Todd-Coxeter algorithm is initialised to
    //! contain the right (or left) Cayley graph of the semigroup over which
    //! \c this is defined.
    //!
    //! If \c this is not defined over a Semigroup (i.e. it is defined over a
    //! finitely presented semigroup), then this method does the same as
    //! force_tc.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Todd-Coxeter Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_tc_prefill();

    //! Use an elementary orbit algorithm which enumerates pairs of
    //! Element objects that are related in \c this.
    //!
    //! The method forced by this is unlikely
    //! to terminate, or to be faster than the other methods, unless there are
    //! a relatively few pairs of elements related by the congruence.
    //!
    //! This method only applies to a congruence created using a Semigroup
    //! object, and does not apply to finitely presented semigroups.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The worst case space complexity of these algorithms
    //! is the square of the size of the semigroup over which \c this is
    //! defined.
    template <typename TElementType  = Element const*,
              typename TElementHash  = libsemigroups::hash<TElementType>,
              typename TElementEqual = libsemigroups::equal_to<TElementType>>
    void force_p() {
      LIBSEMIGROUPS_ASSERT(_semigroup != nullptr);
      delete_data();
      _data = new P<TElementType, TElementHash, TElementEqual>(*this);
    }

    //! Use the Knuth-Bendix algorithm on a rewriting system RWS with rules
    //! obtained from Congruence::relations followed by an elementary orbit on
    //! pairs method on the resulting semigroup.
    //!
    //! This method forces the use of the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! to compute the congruence defined by the generators and relations of \c
    //! this followed by an elementary orbit algorithm which enumerates pairs
    //! of Element objects of that semigroup which are related in \c this.
    //! Knuth-Bendix is applied to a rewriting system obtained from
    //! Congruence::relations.
    //!
    //! This method only applies to a congruence over a finitely presented
    //! semigroups, and does not apply to a congruence defined using a concrete
    //! Semigroup object.
    //!
    //! Note that this algorithm can be applied to left, right, and two-sided
    //! congruences (unlike KBFP).
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Knuth-Bendix Algorithm may never terminate when applied to
    //! a finitely presented semigroup. Even if Knuth-Bendix completes
    //! the other algorithm that this method forces a Congruence to use
    //! will only terminate if there are finitely many pairs of elements
    //! related by \c this. The worst case space complexity of these algorithms
    //! is the square of the size of the semigroup over which \c this is
    //! defined.
    void force_kbp();

    //! Use the Knuth-Bendix algorithm on a rewriting system RWS with rules
    //! obtained from Congruence::relations and Congruence::extra, followed by
    //! the Froidure-Pin algorithm on the resulting semigroup.
    //!
    //! This method forces the use of the [Knuth-Bendix
    //! algorithm](https://en.wikipedia.org/wiki/Knuth–Bendix_completion_algorithm)
    //! to compute the congruence defined by the generators,
    //! Congruence::relations, and Congruence::extra of \c this followed by the
    //! Froidure-Pin algorithm on the resulting semigroup. The resulting
    //! semigroup consists of RWSE's and is enumerated using
    //! Semigroup::enumerate.
    //!
    //! At present the Knuth-Bendix algorithm can only be applied to two-sided
    //! congruences, and this method asserts that \c this is a two-sided
    //! congruence.
    //!
    //! \warning Any existing data for the congruence is deleted by this
    //! method, and may have to be recomputed. The return values and runtimes
    //! of other methods applied to \c this may also be affected.
    //!
    //! \warning The Knuth-Bendix Algorithm may never terminate when applied to
    //! a finitely presented semigroup.
    void force_kbfp();

    //! This method tries to quickly determine whether or not the Congruence
    //! has infinitely many classes.
    //!
    //! If \c true is returned, then there are infinitely many classes in the
    //! congruence, but if \c false is returned, then the method could not
    //! determine whether or not there are infinitely many classes.
    bool is_obviously_infinite();

   private:
    // Subclasses of DATA
    class KBFP;  // Knuth-Bendix followed by Froidure-Pin
    class KBP;   // Knuth-Bendix followed by P
    template <typename TElementType,
              typename TElementHash,
              typename TElementEqual,
              class TTraits>
    class P;   // Orbit of pairs
    class TC;  // Todd-Coxeter

    // Abstract base class for nested classes containing methods for actually
    // enumerating the classes etc of a congruence
    class DATA {
      friend KBFP;
      friend KBP;
      template <typename TElementType,
                typename TElementHash,
                typename TElementEqual,
                class TTraits>
      friend class P;
      friend TC;

     public:
      // Default constructor
      DATA(Congruence& cong,
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
      // Todd-Coxeter, Knuth-Bendix, etc., until completion.
      virtual void run() = 0;

      // This method runs the algorithm for a while, then stops after a
      // certain amount of work or when done \p steps the amount of work to do
      // before returning.
      virtual void run(size_t steps) = 0;

      // This method returns true if a DATA object's run method has been run to
      // conclusion, i.e. that it has not been killed by another instance.
      virtual bool is_done() const = 0;

      // This method returns the number of classes of the congruence.
      virtual size_t nr_classes() = 0;

      // This method returns the index of the congruence class containing the
      // element of the semigroup defined by word.
      virtual class_index_t word_to_class_index(word_type const& word) = 0;

      // Possible result of questions that might not be answerable.
      enum result_t { TRUE = 0, FALSE = 1, UNKNOWN = 2 };

      // This method returns \c true if the two words are known to describe
      // elements in the same congruence class, \c false if they are known to
      // lie in different classes, and **UNKNOWN** if the information has not
      // yet been discovered.
      virtual result_t current_equals(word_type const& w1, word_type const& w2)
          = 0;

      // This method returns \c true if the two words are known to be in
      // distinct classes, where w1's class is less than w2's class by some
      // total ordering; \c false if this is known to be untrue; and
      // **UNKNOWN** if the information has not yet been discovered.
      // \p w1 const reference to the first word
      // \p w2 const reference to the second word
      virtual result_t current_less_than(word_type const& w1,
                                         word_type const& w2) {
        if (is_done()) {
          return word_to_class_index(w1) < word_to_class_index(w2)
                     ? result_t::TRUE
                     : result_t::FALSE;
        } else if (current_equals(w1, w2) == result_t::TRUE) {
          return result_t::FALSE;  // elements are equal
        }
        return result_t::UNKNOWN;
      }

      // This method returns the non-trivial classes of the congruence.
      virtual Partition<word_type>* nontrivial_classes();

      // This method kills a given instance of a DATA object.
      void kill() {
        // TODO add killed-by-thread
        _killed = true;
      }

      // This method sets a given instance of a DATA object to "not killed"
      void unkill() {
        _killed = false;
      }

      // This method can be used to tell whether or not a given DATA object has
      // been killed by another instance.
      std::atomic<bool>& is_killed() {
        return _killed;
      }

      // \p goal_func a function to test whether we can stop running
      //
      // This function calls DATA::run in batches until goal_func returns
      // true.  If goal_func is RETURN_FALSE, then the object ! is instead run
      // to completion.
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

      virtual void set_pack(size_t val) {
        (void) val;
      }

      void set_report_interval(size_t val) {
        _report_interval = val;
      }

     private:
      Congruence&                     _cong;
      size_t                          _default_nr_steps;
      std::atomic<bool>               _killed;
      size_t                          _report_interval;
      size_t                          _report_next;
      typedef std::vector<word_type*> class_t;
      typedef std::vector<class_t*>   partition_t;
    };

    template <
        typename TElementType,
        typename TElementHash,
        typename TElementEqual,
        class TTraits
        = SemigroupTraitsHashEqual<TElementType, TElementHash, TElementEqual>>
    class P : public DATA, TTraits {
      using element_type       = typename TTraits::element_type;
      using const_element_type = typename TTraits::const_element_type;
      using reference          = typename TTraits::reference;
      using const_reference    = typename TTraits::const_reference;

      using internal_element_type = typename TTraits::internal_element_type;
      using internal_const_element_type =
          typename TTraits::internal_const_element_type;

      using internal_equal_to = typename TTraits::internal_equal_to;
      using internal_hash     = typename TTraits::internal_hash;

      using product = ::libsemigroups::product<internal_element_type>;

     public:
      explicit P(Congruence& cong)
          : DATA(cong, 2000, 40000),
            _class_lookup(),
            _done(false),
            _found_pairs(),
            _lookup(0),
            _map(),
            _map_next(0),
            _next_class(0),
            _pairs_to_mult(),
            _reverse_map(),
            _tmp1(),
            _tmp2() {
        LIBSEMIGROUPS_ASSERT(cong._semigroup != nullptr);
        // FIXME all the static_cast's of the form below must include all 3
        // template parameters, not just the first.
        auto semigroup = static_cast<Semigroup<TElementType>*>(cong._semigroup);
        _tmp1          = this->internal_copy(
            this->to_internal_const(semigroup->generator(0)));
        _tmp2 = this->internal_copy(_tmp1);

        // Set up _pairs_to_mult
        for (relation_type const& rel : cong._extra) {
          element_type x = semigroup->word_to_element(rel.first);
          element_type y = semigroup->word_to_element(rel.second);
          add_pair(this->to_internal(x), this->to_internal(y));
          this->external_free(x);
          this->external_free(y);
        }
      }

      void delete_tmp_storage() {
        std::unordered_set<
            std::pair<internal_element_type, internal_element_type>,
            PHash,
            PEqual>()
            .swap(_found_pairs);
        std::queue<std::pair<internal_element_type, internal_element_type>>()
            .swap(_pairs_to_mult);
      }

      ~P() {
        delete_tmp_storage();
        this->internal_free(_tmp1);
        this->internal_free(_tmp2);
        for (auto& x : _map) {
          this->internal_free(const_cast<internal_element_type>(x.first));
        }
      }

      bool is_done() const final {  // TODO noexcept
        return _done;
      }

      size_t nr_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        return this->_cong._semigroup->size() - _class_lookup.size()
               + _next_class;
      }

      class_index_t word_to_class_index(word_type const& w) final {
        LIBSEMIGROUPS_ASSERT(is_done());
        auto semigroup
            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        element_type x     = semigroup->word_to_element(w);
        size_t       ind_x = get_index(this->to_internal(x));
        this->external_free(x);
        LIBSEMIGROUPS_ASSERT(ind_x < _class_lookup.size());
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() == _map.size());
        return _class_lookup[ind_x];
      }

      result_t current_equals(word_type const& w1, word_type const& w2) final {
        if (is_done()) {
          return word_to_class_index(w1) == word_to_class_index(w2) ? TRUE
                                                                    : FALSE;
        }
        auto semigroup
            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        element_type x     = semigroup->word_to_element(w1);
        element_type y     = semigroup->word_to_element(w2);
        size_t       ind_x = get_index(this->to_internal(x));
        size_t       ind_y = get_index(this->to_internal(y));
        this->external_free(x);
        this->external_free(y);
        return _lookup.find(ind_x) == _lookup.find(ind_y) ? TRUE : UNKNOWN;
      }

      Partition<word_type>* nontrivial_classes() final {
        LIBSEMIGROUPS_ASSERT(is_done());
        LIBSEMIGROUPS_ASSERT(_reverse_map.size() >= _nr_nontrivial_elms);
        LIBSEMIGROUPS_ASSERT(_class_lookup.size() >= _nr_nontrivial_elms);

        Partition<word_type>* classes
            = new Partition<word_type>(_nr_nontrivial_classes);
        auto semigroup
            = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
        for (size_t ind = 0; ind < _nr_nontrivial_elms; ind++) {
          word_type* word = new word_type(
              semigroup->factorisation(this->to_external(_reverse_map[ind])));
          (*classes)[_class_lookup[ind]]->push_back(word);
        }
        return classes;
      }

      void run() final {
        run(this->_killed);
      }

      void run(size_t steps) final {
        run(steps, this->_killed);
      }

      void run(std::atomic<bool>& killed) {
        while (!killed && !is_done()) {
          run(LIMIT_MAX, killed);
        }
      }

      void run(size_t steps, std::atomic<bool>& killed) {
        REPORT("number of steps = " << steps);
        size_t tid = REPORTER.thread_id(std::this_thread::get_id());
        while (!_pairs_to_mult.empty()) {
          // Get the next pair
          auto current_pair = _pairs_to_mult.front();
          // TODO can the previous be auto&, or does the pop make this UB?
          _pairs_to_mult.pop();

          // Add its left and/or right multiples
          auto semigroup
              = static_cast<Semigroup<TElementType>*>(_cong._semigroup);
          for (size_t i = 0; i < this->_cong._nrgens; i++) {
            const_reference gen = semigroup->generator(i);
            if (this->_cong._type == LEFT || this->_cong._type == TWOSIDED) {
              product()(
                  _tmp1, this->to_internal_const(gen), current_pair.first, tid);
              product()(_tmp2,
                        this->to_internal_const(gen),
                        current_pair.second,
                        tid);
              add_pair(_tmp1, _tmp2);
            }
            if (this->_cong._type == RIGHT || this->_cong._type == TWOSIDED) {
              product()(
                  _tmp1, current_pair.first, this->to_internal_const(gen), tid);
              product()(_tmp2,
                        current_pair.second,
                        this->to_internal_const(gen),
                        tid);
              add_pair(_tmp1, _tmp2);
            }
          }
          if (this->_report_next++ > this->_report_interval) {
            REPORT("found " << _found_pairs.size() << " pairs: " << _map_next
                            << " elements in " << _lookup.nr_blocks()
                            << " classes, " << _pairs_to_mult.size()
                            << " pairs on the stack");
            this->_report_next = 0;
            if (tid != 0 && this->_cong._semigroup->is_done()
                && _found_pairs.size() > this->_cong._semigroup->size()) {
              // If the congruence is only using 1 thread, then this will never
              // happen, if the congruence uses > 1 threads, then it is ok for
              // P to kill itself, because another thread will complete and
              // return the required DATA*.
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
          for (size_t i = 1; i < _lookup.get_size(); i++) {
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
          REPORT("finished with " << _found_pairs.size()
                                  << " pairs: " << _map_next << " elements in "
                                  << _lookup.nr_blocks() << " classes");
          _done = true;
          delete_tmp_storage();
        } else {
          REPORT("killed");
        }
      }

     private:
      struct PHash {
       public:
        size_t
        operator()(std::pair<internal_const_element_type,
                             internal_const_element_type> const& pair) const {
          return internal_hash()(pair.first)
                 + 17 * internal_hash()(pair.second);
        }
      };

      struct PEqual {
        size_t operator()(std::pair<internal_const_element_type,
                                    internal_const_element_type> pair1,
                          std::pair<internal_const_element_type,
                                    internal_const_element_type> pair2) const {
          return internal_equal_to()(pair1.first, pair2.first)
                 && internal_equal_to()(pair1.second, pair2.second);
        }
      };

      void add_pair(internal_const_element_type x,
                    internal_const_element_type y) {
        if (!internal_equal_to()(x, y)) {
          internal_element_type xx, yy;
          bool                  xx_new = false, yy_new = false;
          size_t                i, j;

          auto it_x = _map.find(x);
          if (it_x == _map.end()) {
            xx_new = true;
            xx     = this->internal_copy(x);
            i      = add_index(xx);
          } else {
            i = it_x->second;
          }

          auto it_y = _map.find(y);
          if (it_y == _map.end()) {
            yy_new = true;
            yy     = this->internal_copy(y);
            j      = add_index(yy);
          } else {
            j = it_y->second;
          }
          // TODO avoid unnecessary copying in the next block
          LIBSEMIGROUPS_ASSERT(i != j);
          std::pair<internal_element_type, internal_element_type> pair;
          if (xx_new || yy_new) {  // it's a new pair
            xx   = internal_element_type(xx_new ? xx : it_x->first);
            yy   = internal_element_type(yy_new ? yy : it_y->first);
            pair = (i < j ? std::make_pair(xx, yy) : std::make_pair(yy, xx));
          } else {
            pair = (i < j ? std::make_pair(internal_element_type(it_x->first),
                                           internal_element_type(it_y->first))
                          : std::make_pair(internal_element_type(it_y->first),
                                           internal_element_type(it_x->first)));
            if (_found_pairs.find(pair) != _found_pairs.end()) {
              return;
            }
          }
          _found_pairs.insert(pair);
          _pairs_to_mult.push(pair);
          _lookup.unite(i, j);
        }
      }

      size_t get_index(internal_const_element_type x) {
        auto it = _map.find(x);
        if (it == _map.end()) {
          return add_index(this->internal_copy(x));
        }
        return it->second;
      }

      size_t add_index(internal_element_type x) {
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

      std::vector<class_index_t> _class_lookup;
      bool                       _done;
      std::unordered_set<
          std::pair<internal_element_type, internal_element_type>,
          PHash,
          PEqual>
          _found_pairs;
      UF  _lookup;
      std::unordered_map<internal_const_element_type,
                         size_t,
                         internal_hash,
                         internal_equal_to>
                    _map;
      size_t        _map_next;
      class_index_t _next_class;
      size_t        _nr_nontrivial_classes;
      size_t        _nr_nontrivial_elms;
      std::queue<std::pair<internal_element_type, internal_element_type>>
                                         _pairs_to_mult;
      std::vector<internal_element_type> _reverse_map;
      internal_element_type              _tmp1;
      internal_element_type              _tmp2;
    };

    // This deletes all DATA objects stored in this congruence, including
    // finished objects and partially-enumerated objects.
    void delete_data();

    // Set the relations of a Congruence object to the relations of the
    // semigroup over which the Congruence is defined (if any). Report is here
    // in case of any enumeration of the underlying semigroup.
    void init_relations(SemigroupBase* semigroup, std::atomic<bool>& killed);

    void init_relations(SemigroupBase* semigroup) {
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

    Congruence(cong_t                            type,
               size_t                            nrgens,
               std::vector<relation_type> const& relations,
               std::vector<relation_type> const& extra);

    Congruence(cong_t                            type,
               SemigroupBase*                    semigroup,
               std::vector<relation_type> const& extra);

    cong_t type_from_string(std::string);

    DATA*                      _data;
    std::vector<relation_type> _extra;
    std::mutex                 _init_mtx;
    std::mutex                 _kill_mtx;
    size_t                     _max_threads;
    size_t                     _nrgens;
    std::vector<DATA*>         _partial_data;
    RecVec<class_index_t>      _prefill;
    std::vector<relation_type> _relations;
    std::atomic<bool>          _relations_done;
    SemigroupBase*             _semigroup;
    cong_t                     _type;
  };
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_CONG_H_
