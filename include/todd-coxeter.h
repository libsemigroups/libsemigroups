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

// TODO:
// 1. In congruence::ToddCoxeter: add methods for add_pair, set_base, etc, for
// use with 0-parameter constructor

#ifndef LIBSEMIGROUPS_INCLUDE_TODD_COXETER_H_
#define LIBSEMIGROUPS_INCLUDE_TODD_COXETER_H_

#include <stack>
#include <unordered_map>
#include <vector>

#include "internal/recvec.h"

#include "cong-intf.h"
#include "fpsemi-intf.h"

namespace libsemigroups {
  // Forward declaratation
  namespace fpsemigroup {
    class ToddCoxeter;
  }  // namespace fpsemigroup

  namespace congruence {
    class ToddCoxeter : public CongIntf {
      using signed_class_index_type = int64_t;

     public:
      using congruence_type  = CongIntf::congruence_type;
      using class_index_type = CongIntf::class_index_type;

      enum class policy { none = 0, use_relations = 1, use_cayley_graph = 2 };

      /////////////////////////////////
      // Constructors and destructor //
      /////////////////////////////////

      // TODO was private ok?
      explicit ToddCoxeter(congruence_type type);

      ToddCoxeter(congruence_type                   type,
                  SemigroupBase*                    S,
                  std::vector<relation_type> const& genpairs,
                  policy                            p);

      ToddCoxeter(congruence_type type, SemigroupBase* S, policy p);

      // TODO remove the next constructor, or use it as a convenience to first
      // construct an fpsemigroup::ToddCoxeter using relations, and then to
      // make a congruence::ToddCoxeter using that.
      ToddCoxeter(congruence_type                   type,
                  size_t                            nrgens,
                  std::vector<relation_type> const& relations,
                  std::vector<relation_type> const& extra = {});

      ~ToddCoxeter();

      // TODO ToddCoxeter(congruence_type type, FpSemigroup* S, policy p);

      // TODO this constructor should instantiate an FpSemigroup with nrgens,
      // and relations, and then call the last constructor below (FpSemigroup
      // const*, relations).

      // TODO double check if the following are still required
      // ToddCoxeter(congruence_type type, size_t nrgens);
      // ToddCoxeter(ToddCoxeter const& copy);
      // ToddCoxeter(fpsemigroup::ToddCoxeter const*,
      //            std::vector<relation_type> const&);

      // ToddCoxeter(FpSemigroup const*,
      //            std::vector<relation_type> const&);

      ////////////////////////////////////////////
      // Overridden virtual methods from Runner //
      ////////////////////////////////////////////

      void run() override;

      //////////////////////////////////////////////////////////
      // Overridden public pure virtual methods from CongIntf //
      //////////////////////////////////////////////////////////

      void                       add_pair(word_type, word_type) override;
      non_trivial_class_iterator cbegin_non_trivial_classes() override;
      non_trivial_class_iterator cend_non_trivial_classes() override;
      size_t                     nr_classes() override;
      size_t                     nr_non_trivial_classes() override;
      SemigroupBase*             quotient_semigroup() override;
      class_index_type           word_to_class_index(word_type const&) override;
      word_type                  class_index_to_word(class_index_type) override;

      //////////////////////////////////////////////////////////////
      // Overridden public non-pure virtual methods from CongIntf //
      //////////////////////////////////////////////////////////////

      bool contains(word_type const&, word_type const&) override;
      bool is_quotient_obviously_finite() override;
      bool is_quotient_obviously_infinite() override;
      void set_nr_generators(size_t) override;

      ////////////////////
      // Public methods //
      ////////////////////

      bool             empty() const;
      policy           get_policy() const noexcept;
      void             prefill(RecVec<class_index_type> const&);
      class_index_type right(class_index_type, letter_type);
      void             set_pack(size_t);

     private:
      ///////////////////////////////////////////////////////////
      // Overridden private pure virtual methods from CongIntf //
      ///////////////////////////////////////////////////////////
      class_index_type
      const_word_to_class_index(word_type const&) const override;

      //////////////////////////////////
      // Private methods - validation //
      //////////////////////////////////

      void validate_relations() const;
      void validate_table() const;
      void validate_word(word_type const&) const;

      //////////////////////////////////////
      // Private methods - initialisation //
      //////////////////////////////////////

      void init();
      void init_after_prefill();
      void init_non_trivial_classes();
      void init_relations();
      void prefill(SemigroupBase*);
      void use_relations_or_cayley_graph();

      /////////////////////////////
      // Private methods - other //
      /////////////////////////////

      void compress();
      void new_coset(class_index_type const&, letter_type const&);
      void identify_cosets(class_index_type, class_index_type);
      inline void
           trace(class_index_type const&, relation_type const&, bool add = true);

      //////////////////
      // Private data //
      //////////////////
      // TODO use Pimpl
      size_t _active;  // Number of active cosets
      // _base is the semigroup over which the congruence is defined.
      SemigroupBase*                       _base;
      std::vector<signed_class_index_type> _bckwd;
      size_t                               _cosets_killed;
      class_index_type                     _current;
      class_index_type                     _current_no_add;
      size_t                               _defined;
      std::vector<relation_type>           _extra;
      std::vector<class_index_type>        _forwd;
      class_index_type                     _id_coset;
      bool                         _init_done;  // Has init() been run yet?
      class_index_type             _last;
      std::stack<class_index_type> _lhs_stack;
      class_index_type             _next;
      std::vector<std::vector<word_type>> _non_trivial_classes;
      size_t _pack;  // Nr of active cosets allowed before a
                     // packing phase starts
      policy                       _policy;
      bool                         _prefilled;
      RecVec<class_index_type>     _preim_init;
      RecVec<class_index_type>     _preim_next;
      std::vector<relation_type>   _relations;
      std::stack<class_index_type> _rhs_stack;
      bool                         _stop_packing;
      RecVec<class_index_type>     _table;
    };
  }  // namespace congruence

  namespace fpsemigroup {
    class ToddCoxeter : public FpSemiIntf {
     public:
      //////////////////
      // Constructors //
      //////////////////

      ToddCoxeter();
      explicit ToddCoxeter(SemigroupBase*);
      explicit ToddCoxeter(SemigroupBase&);

      ////////////////////////////////////////////
      // Overridden virtual methods from Runner //
      ////////////////////////////////////////////

      void run() override;

      /////////////////////////////////////////////////////
      // Overridden pure virtual methods from FpSemiIntf //
      /////////////////////////////////////////////////////

      void add_rule(std::string const&, std::string const&) override;

      bool   is_obviously_finite() override;
      bool   is_obviously_infinite() override;
      SemigroupBase* isomorphic_non_fp_semigroup() override;
      size_t size() override;

      bool equal_to(std::string const&, std::string const&) override;

      std::string normal_form(std::string const&) override;

      /////////////////////////////////////////////////////////
      // Overridden non-pure virtual methods from FpSemiIntf //
      /////////////////////////////////////////////////////////

      void      add_rule(word_type const&, word_type const&) override;
      bool      equal_to(word_type const&, word_type const&) override;
      word_type normal_form(word_type const&) override;
      void      set_alphabet(std::string const&) override;
      void      set_alphabet(size_t) override;

     private:
      std::unique_ptr<congruence::ToddCoxeter> _tcc;
    };
  }  // namespace fpsemigroup
}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_INCLUDE_TODD_COXETER_H_
