//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2018 James D. Mitchell
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

// This file contains a declaration of a class for performing the Todd-Coxeter
// algorithm for semigroups.

// TODO - add constructor from RWS, see comments in cong.cc

#ifndef LIBSEMIGROUPS_SRC_TODD_COXETER_H_
#define LIBSEMIGROUPS_SRC_TODD_COXETER_H_

#include <stack>
#include <unordered_map>
#include <vector>

#include "cong-intf.h"
#include "fpsemi-intf.h"
#include "race.h"
#include "recvec.h"

namespace libsemigroups {
  namespace fpsemigroup {
    // Forward declaratation
    class ToddCoxeter;
  }  // namespace fpsemigroup

  namespace congruence {
    using congruence_t  = Interface::congruence_t;
    using class_index_t = Interface::class_index_t;

    class ToddCoxeter : public Interface {
     public:
      enum policy { none = 0, use_relations = 1, use_cayley_graph = 2 };

      //////////////////////////////////////////////////////////////////////////
      // Constructors and destructor
      //////////////////////////////////////////////////////////////////////////

      ToddCoxeter(congruence_t                   type,
                  SemigroupBase*                 S,
                  std::vector<relation_t> const& genpairs,
                  policy                         p);
      ToddCoxeter(congruence_t type, SemigroupBase* S, policy p);
      ~ToddCoxeter();

      // TODO ToddCoxeter(congruence_t type, FpSemigroup* S, policy p);

      /*ToddCoxeter(congruence_t                   type,
                  size_t                         nrgens,
                  std::vector<relation_t> const& relations,
                  std::vector<relation_t> const& extra = {});*/

      // TODO double check if the following are still required
      // ToddCoxeter(congruence_t type, size_t nrgens);
      // ToddCoxeter(ToddCoxeter const& copy);
      ToddCoxeter(fpsemigroup::ToddCoxeter const*,
                  std::vector<relation_t> const&);

      //////////////////////////////////////////////////////////////////////////
      // Overridden virtual methods from Runner
      //////////////////////////////////////////////////////////////////////////

      void run() override;

      //////////////////////////////////////////////////////////////////////////
      // Overridden public pure virtual methods from congruence::Interface
      //////////////////////////////////////////////////////////////////////////

      class_index_t              word_to_class_index(word_t const&) override;
      size_t                     nr_classes() override;
      void                       add_pair(word_t, word_t) override;
      SemigroupBase*             quotient_semigroup() override;
      non_trivial_class_iterator cbegin_non_trivial_classes() override;
      non_trivial_class_iterator cend_non_trivial_classes() override;
      size_t                     nr_non_trivial_classes() override;

      //////////////////////////////////////////////////////////////////////////
      // Overridden public non-pure virtual methods from congruence::Interface
      //////////////////////////////////////////////////////////////////////////

      bool is_quotient_obviously_infinite() const override;

      //////////////////////////////////////////////////////////////////////////
      // Public methods
      //////////////////////////////////////////////////////////////////////////

      size_t        char_to_uint(char) const;
      char          uint_to_char(size_t) const;
      word_t        string_to_word(std::string const&) const;
      std::string   word_to_string(word_t const&) const;
      class_index_t right(class_index_t, letter_t);
      void          prefill(RecVec<class_index_t> const&);
      void          set_pack(size_t);
      bool          validate_relations() const;
      bool          validate_table() const;

     private:
      //////////////////////////////////////////////////////////////////////////
      // Overridden private pure virtual methods from congruence::Interface
      //////////////////////////////////////////////////////////////////////////
      class_index_t const_word_to_class_index(word_t const&) const override;

      //////////////////////////////////////////////////////////////////////////
      // Private constructors
      //////////////////////////////////////////////////////////////////////////

      explicit ToddCoxeter(congruence_t type);

      //////////////////////////////////////////////////////////////////////////
      // Private methods
      //////////////////////////////////////////////////////////////////////////

      typedef int64_t signed_class_index_t;
      void            init();
      void            init_after_prefill();
      void            init_relations();
      void            init_non_trivial_classes();
      void            prefill(SemigroupBase*);
      void            use_relations_or_cayley_graph();
      bool            empty() const;
      void            reset_quotient_semigroup();

      void new_coset(class_index_t const&, letter_t const&);
      void identify_cosets(class_index_t, class_index_t);
      inline void
           trace(class_index_t const&, relation_t const&, bool add = true);
      void compress();

      //////////////////////////////////////////////////////////////////////////
      // Private data
      //////////////////////////////////////////////////////////////////////////

      size_t                             _active;  // Number of active cosets
      std::string                        _alphabet;
      std::unordered_map<char, letter_t> _alphabet_map;
      // _base is the semigroup over which the congruence is defined.
      SemigroupBase*            _base;
      std::vector<signed_class_index_t>  _bckwd;
      size_t                             _cosets_killed;
      class_index_t                      _current;
      class_index_t                      _current_no_add;
      size_t                             _defined;
      std::vector<relation_t>            _extra;
      std::vector<class_index_t>         _forwd;
      class_index_t                      _id_coset;
      bool _init_done;  // Has init() been run yet?
      // Is _extra non-empty upon construction?
      class_index_t             _last;
      std::stack<class_index_t> _lhs_stack;  // Stack for identifying cosets
      class_index_t             _next;
      std::vector<std::vector<word_t>> _non_trivial_classes;
      size_t                           _nrgens;
      size_t _pack;  // Nr of active cosets allowed before a
                     // packing phase starts
      policy                    _policy;
      bool                      _prefilled;
      RecVec<class_index_t>     _preim_init;
      RecVec<class_index_t>     _preim_next;
      SemigroupBase*            _quotient_semigroup;
      std::vector<relation_t>   _relations;
      std::stack<class_index_t> _rhs_stack;  // Stack for identifying cosets
      bool                      _stop_packing;
      RecVec<class_index_t>     _table;
      congruence_t              _type;
    };
  }  // namespace congruence

  namespace fpsemigroup {
    class ToddCoxeter : public fpsemigroup::Interface {
     public:
      // TODO add constructors
      ToddCoxeter();

      //////////////////////////////////////////////////////////////////////////
      // Overridden virtual methods from Runner
      //////////////////////////////////////////////////////////////////////////

      void run() override;

      //////////////////////////////////////////////////////////////////////////
      // Overridden virtual methods from fpsemigroup::Interface
      //////////////////////////////////////////////////////////////////////////

      void               set_nr_generators(size_t) override;
      size_t             nr_generators() const override;
      void               set_alphabet(std::string) override;
      std::string const& alphabet() const override;

     private:
      bool validate_word(word_t const&) const override;
      bool validate_word(std::string const&) const override;
      void set_isomorphic_non_fp_semigroup(SemigroupBase*) override;
      void internal_add_relation(word_t, word_t) override;

     public:
      void add_relation(word_t, word_t) override;
      void add_relation(std::string, std::string) override;

      bool           is_obviously_finite() const override;
      bool           is_obviously_infinite() const override;
      size_t         size() override;
      SemigroupBase* isomorphic_non_fp_semigroup() override;
      bool           has_isomorphic_non_fp_semigroup() override;

      bool equal_to(word_t const&, word_t const&) override;
      bool equal_to(std::string const&, std::string const&) override;

      word_t      normal_form(word_t const&) override;
      std::string normal_form(std::string const&) override;

     private:
      congruence::ToddCoxeter* _tcc;
    };
  }  // namespace fpsemigroup */
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_SRC_TODD_COXETER_H_
