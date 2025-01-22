//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-23 James D. Mitchell
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
// algorithm for semigroups and monoids.
//
// TODO(1)
// * re-implement reserve
// * remove preferred_defs from FelschGraph etc (except where they are really
// needed)? Or possibly reintroduce PrefDefs here
// * re-add report why stopped

#ifndef LIBSEMIGROUPS_TODD_COXETER_BASE_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_BASE_HPP_

#include <chrono>    // for nanoseconds
#include <cstddef>   // for size_t
#include <cstdint>   // for uint32_t
#include <iterator>  // for bidirect...
#include <memory>    // for unique_ptr
#include <numeric>   // for iota
#include <string>    // for string
#include <utility>   // for move, pair
#include <vector>    // for vector

#include "cong-intf-class.hpp"  // for Congruen...
#include "constants.hpp"        // for operator!=
#include "debug.hpp"            // for LIBSEMIG...
#include "forest.hpp"           // for Forest
#include "order.hpp"            // for Order
#include "presentation.hpp"     // for Presenta...
#include "ranges.hpp"           // for operator|
#include "runner.hpp"           // for Reporter
#include "to-presentation.hpp"  // for to_prese...
#include "types.hpp"            // for word_type
#include "word-graph.hpp"       // for WordGraph

#include "detail/felsch-graph.hpp"             // for FelschGr...
#include "detail/felsch-graph.hpp"             // for FelschGraph
#include "detail/node-managed-graph.hpp"       // for NodeMana...
#include "detail/node-manager.hpp"             // for NodeMana...
#include "detail/word-graph-with-sources.hpp"  // for WordGrap...

////////////////////////////////////////////////////////////////////////
// This file is organised as follows:
// 0.  ToddCoxeterBase - member types - public
// 1.  ToddCoxeterBase - nested classes - private
// 2.  ToddCoxeterBase - data members - private
// 3.  ToddCoxeterBase - constructors + initializers - public
// 4.  ToddCoxeterBase - interface requirements - add_generating_pair
// 5.  ToddCoxeterBase - interface requirements - number_of_classes
// 6.  ToddCoxeterBase - interface requirements - contains
// 7.  ToddCoxeterBase - interface requirements - reduce
// 8.  ToddCoxeterBase - settings - public
// 9.  ToddCoxeterBase - accessors - public
// 10. ToddCoxeterBase - modifiers - public
// 11. ToddCoxeterBase - word -> index
// 12. ToddCoxeterBase - index -> word
// 13. Runner      - pure virtual member functions - private
// 14. ToddCoxeterBase - member functions - private
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// Documentation macros
////////////////////////////////////////////////////////////////////////

namespace libsemigroups {
  //! \defgroup todd_coxeter_group Todd-Coxeter
  //!
  //! This page contains documentation related to the implementation of the
  //! Todd-Coxeter algorithm \cite Coleman2022aa in `libsemigroups`.
  //!
  //! The purpose of this algorithm is to find the WordGraph of the action of a
  //! semigroup or monoid on the classes of a 1-sided (right), or 2-sided
  //! congruence; see \cite Coleman2022aa for more details.

  //! \defgroup todd_coxeter_class_group ToddCoxeterBase class
  //! \ingroup todd_coxeter_group
  //!
  //! \brief Class containing an implementation of the Todd-Coxeter Algorithm.
  //!
  //! Defined in `todd-coxeter.hpp`.
  //!
  //! This class contains an implementation of the Todd-Coxeter
  //! algorithm for computing 1-sided (right), and 2-sided congruences on
  //! a semigroup or monoid.
  //!
  //! In this documentation we use the term "congruence enumeration" to mean
  //! the execution of (any version of) the Todd-Coxeter algorithm. Some of the
  //! features of this class were inspired by similar features in
  //! [ACE](https://staff.itee.uq.edu.au/havas/) by George Havas and Colin
  //! Ramsay.
  //!
  //! \sa congruence_kind and tril.
  //!
  //! \par Example 1
  //! \code
  //! Presentation<word_type> p;
  //! p.alphabet(2);
  //! presentation::add_rule(p, 00_w, 0_w);
  //! presentation::add_rule(p, 0_w, 1_w);
  //! ToddCoxeterBase tc(congruence_kind::onesided, p);
  //! tc.strategy(options::strategy::felsch);
  //! tc.number_of_classes();
  //! tc.contains(0000_w, 00_w);
  //! tc.index_of(0000_w);
  //! \endcode
  //!
  //! \par Example 2
  //! \code
  //! Presentation<word_type> p;
  //! p.alphabet(4);
  //! presentation::add_rule(p, 00_w, 0_w);
  //! presentation::add_rule(p, 10_w, 1_w);
  //! presentation::add_rule(p, 01_w, 1_w);
  //! presentation::add_rule(p, 20_w, 2_w);
  //! presentation::add_rule(p, 02_w, 2_w);
  //! presentation::add_rule(p, 30_w, 3_w);
  //! presentation::add_rule(p, 03_w, 3_w);
  //! presentation::add_rule(p, 11_w, 0_w);
  //! presentation::add_rule(p, 23_w, 0_w);
  //! presentation::add_rule(p, 222_w, 0_w);
  //! presentation::add_rule(p, 12121212121212_w, 0_w);
  //! presentation::add_rule(p, 12131213121312131213121312131213_w, 0_w);
  //! ToddCoxeterBase tc(congruence_kind::twosided, p);
  //! tc.strategy(options::strategy::hlt)
  //!    .lookahead_extent(options::lookahead_extent::partial)
  //!    .save(false);
  //! tc.number_of_classes()  // 10'752
  //! tc.standardize(order::recursive);
  //! normal_forms(tc) | rx::take(10) | rx::to_vector()
  //! // {0_w,
  //! //  1_w,
  //! //  2_w,
  //! //  21_w,
  //! //  12_w,
  //! //  121_w,
  //! //  22_w,
  //! //  221_w,
  //! //  212_w,
  //! //  2121_w}
  //! tc.standardize(order::lex);
  //! normal_forms(tc) | rx::take(10) | rx::to_vector()
  //! // {0_w,
  //! //  01_w,
  //! //  012_w,
  //! //  0121_w,
  //! //  01212_w,
  //! //  012121_w,
  //! //  0121212_w,
  //! //  01212121_w,
  //! //  012121212_w,
  //! //  0121212121_w};
  //! \endcode
  class ToddCoxeterBase : public CongruenceInterface,
                          public detail::FelschGraphSettings<ToddCoxeterBase> {
    using FelschGraphSettings_ = FelschGraphSettings<ToddCoxeterBase>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // 0. ToddCoxeterBase - member types - public
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_mem_types_group Member types
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Public member types
    //!
    //! This page contains the documentation of the public member types of a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Struct containing various options that can be used to control the
    //! behaviour of Todd-Coxeter.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Todd-Coxeter.
    struct options : public FelschGraphSettings_::options {
      //! \brief Enum class containing various strategies.
      //!
      //! The values in this enum can be passed to the member function \ref
      //! strategy to define the strategy to be used when performing a
      //! congruence enumeration.
      //!
      //! Several of the strategies mimic
      //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies of the same
      //! name. The [ACE](https://staff.itee.uq.edu.au/havas/) strategy \"R*\"
      //! is equivalent to \c strategy(options::strategy::hlt).save(true).
      enum class strategy {
        //! This value indicates that the HLT (Hazelgrove-Leech-Trotter)
        //! strategy should be used. This is analogous to
        //! [ACE](https://staff.itee.uq.edu.au/havas/)'s R-style.
        hlt,

        //! This value indicates that the Felsch strategy should be used.
        //! This is analogous to [ACE](https://staff.itee.uq.edu.au/havas/)'s
        //! C-style.
        felsch,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy of the same
        //! name. The Felsch is run until at least f_defs() nodes are
        //! defined, then the HLT strategy is run until at least hlt_defs()
        //! divided by \f$N\f$ nodes have been defined, where \f$N\f$ is the sum
        //! of the lengths of the words in the presentation and generating
        //! pairs. These steps are repeated until the enumeration terminates.
        CR,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy R/C. The HLT
        //! strategy is run until the first lookahead is triggered (when
        //! \ref number_of_nodes_active is at least \ref lookahead_next). A full
        //! lookahead is then performed, and then the CR strategy is used.
        R_over_C,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Cr. The Felsch
        //! strategy is run until at least f_defs() new nodes have been
        //! defined, then the HLT strategy is run until at least hlt_defs()
        //! divided by \f$N\f$ nodes have been defined, where \f$N\f$ is the sum
        //! of the lengths of the words in the presentation and generating
        //! pairs. Then the Felsch strategy is run.
        Cr,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Rc. The HLT
        //! strategy is run until at least hlt_defs() divided by \f$N\f$ new
        //! nodes have been defined (where \f$N\f$ is the sum of the lengths of
        //! the words in the presentation and generating pairs) the Felsch
        //! strategy is then run until at least f_defs() new nodes are defined,
        //! and then the HLT strategy is run.
        Rc
      };

      //! \brief Enum class for specifying the extent of any lookahead
      //! performed.
      //!
      //! The values in this enum can be used as the argument for
      //! \ref lookahead_extent to specify the extent of any lookahead that
      //! should be performed.
      enum class lookahead_extent {
        //! Perform a full lookahead from every node in the word graph.
        //! Full lookaheads are therefore sometimes slower but may
        //! detect more coincidences than a partial lookahead.
        full,
        //! Perform a partial lookahead starting from the current node in the
        //! word graph. Partial lookaheads are sometimes faster but may not
        //! detect as many coincidences as a full lookahead.
        partial
      };

      //! \brief Enum class for specifying the style of any lookahead
      //! performed.
      //!
      //! The values in this enum can be used as the argument for
      //! \ref lookahead_style to specify the style of any lookahead that
      //! should be performed.
      enum class lookahead_style {
        //! The lookahead will be done in HLT style by following the paths
        //! labelled by every relation from every node in the range
        //! specified by lookahead_extent::full or lookahead_extent::partial.
        hlt,

        //! The lookahead will be done in Felsch style where every edge is
        //! considered in every path labelled by a relation in which it
        //! occurs.
        felsch
      };

      //! \brief Enum class containing values for specifying how to handle edge
      //! definitions.
      //!
      //! The values in this enum can be used as the argument for
      //! \ref def_policy.
      //!
      //! For our purposes, a *definition* is a recently defined edge in the
      //! word graph that we are attempting to construct in an instance of
      //! ToddCoxeterBase. The values in this enum influence how these
      //! definitions are stored and processed.
      //!
      //! For every definition held in the definition stack, a depth first
      //! search through the Felsch tree of the generating pairs is
      //! performed. The aim is to only follow paths from nodes in the word
      //! graph labelled by generating pairs that actually pass through the
      //! edge described by a definition.
      //!
      //! The values in this enum represent what to do if the number of
      //! definitions in the stack exceeds the value \ref def_max.
      enum class def_policy : uint8_t {
        //! Do not put newly generated definitions in the stack if the stack
        //! already has size \ref def_max.
        no_stack_if_no_space,
        //! If the definition stack has size \ref def_max and a new
        //! definition is generated, then definitions with dead source node are
        //! are popped from the top of the stack (if any).
        purge_from_top,
        //! If the definition stack has size \ref def_max and a new
        //! definition is generated, then definitions with dead source node are
        //! are popped from the entire of the stack (if any).
        purge_all,
        //! If the definition stack has size \ref def_max and a new
        //! definition is generated, then all definitions in the stack are
        //! discarded.
        discard_all_if_no_space,
        //! There is no limit to the number of definitions that can be put in
        //! the stack.
        unlimited
      };
    };  // struct options

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief The type of the nodes in the word graph.
    using node_type = typename WordGraph<uint32_t>::node_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief The type of the index of a class.
    //!
    //! The type of the indices of classes in the congruence represented by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! This alias is the same as \ref node_type, and is included to because
    //! if a \ref todd_coxeter_class_group "ToddCoxeterBase" instance is created
    //! from a Presentation, and that presentation does not \ref
    //! Presentation::contains_empty_word, then there is always at least one
    //! more node (the node representing the empty word) in the \ref
    //! current_word_graph than there are classes in the congruence. This alias
    //! is used to delineate the cases when we are referring to a node or a
    //! class index.
    using index_type = node_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief The type of the edge-labels in the word graph.
    using label_type = typename WordGraph<uint32_t>::label_type;

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - native-types
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! A \ref todd_coxeter_class_group "ToddCoxeterBase" instance can be
    //! constructed or initialised from a presentation of arbitrary types of
    //! letters and words. Internally the letters are converted to \ref
    //! native_letter_type.
    using native_letter_type = letter_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Type of the words in the relations of the presentation stored in
    //! a \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! A \ref todd_coxeter_class_group "ToddCoxeterBase" instance can be
    //! constructed or initialised from a presentation with arbitrary types
    //! of letters and words. Internally the words are converted to \ref
    //! native_word_type.
    using native_word_type = word_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Type of the presentation stored in a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! A \ref todd_coxeter_class_group "ToddCoxeterBase" instance can be
    //! constructed or initialised from a presentation of arbitrary types of
    //! letters and words. Internally the presentation is stored as a \ref
    //! native_presentation_type.
    using native_presentation_type = Presentation<native_word_type>;

   private:
    ////////////////////////////////////////////////////////////////////////
    // 1. ToddCoxeterBase - nested classes - private
    ////////////////////////////////////////////////////////////////////////

    struct Settings;

    class SettingsGuard;
    friend class SettingsGuard;

    class Definitions {
      using Definition = std::pair<node_type, label_type>;

     private:
      bool                    _any_skipped;
      std::vector<Definition> _definitions;
      ToddCoxeterBase const*  _tc;

     public:
      Definitions() : _any_skipped(false), _definitions(), _tc(nullptr) {}
      // TODO(1) init()

      Definitions(Definitions const&)                 = default;
      Definitions(Definitions&&)                      = default;
      Definitions& operator=(Definitions const& that) = default;
      Definitions& operator=(Definitions&&)           = default;

      // TODO(1) corresponding constructor
      void init(ToddCoxeterBase const* tc) {
        _any_skipped = false;
        _definitions.clear();
        _tc = tc;
      }

      void emplace_back(node_type c, label_type x);

      [[nodiscard]] bool any_skipped() const noexcept {
        return _any_skipped;
      }

      [[nodiscard]] bool empty() const noexcept {
        return _definitions.empty();
      }

      void pop(Definition& d) {
        d = std::move(_definitions.back());
        _definitions.pop_back();
      }

      void clear() {
        _definitions.clear();
      }

     private:
      bool is_active_node(node_type n) const noexcept {
        return _tc->current_word_graph().is_active_node(n);
      }
    };  // class Definitions

    class Graph : public detail::NodeManagedGraph<
                      detail::FelschGraph<word_type, uint32_t, Definitions>> {
      using FelschGraph_
          = detail::FelschGraph<word_type, uint32_t, Definitions>;
      using NodeManagedGraph_ = NodeManagedGraph<FelschGraph_>;

     public:
      using node_type = typename NodeManagedGraph_::node_type;

      Graph()                        = default;
      Graph(Graph const&)            = default;
      Graph(Graph&&)                 = default;
      Graph& operator=(Graph const&) = default;
      Graph& operator=(Graph&&)      = default;
      // TODO(1) init()

      Graph& operator=(WordGraph<node_type> const& wg) {
        NodeManagedGraph_::operator=(wg);
        return *this;
      }

      using FelschGraph_::target_no_checks;
      using NodeManagedGraph_::NodeManagedGraph;

      Graph& init();
      // TODO(1) corresponding constructors
      Graph& init(Presentation<word_type> const& p);
      Graph& init(Presentation<word_type>&& p);

      void process_definitions();

      template <bool RegDefs>
      void push_definition_hlt(node_type const& c,
                               word_type const& u,
                               word_type const& v);

      template <typename Iterator>
      size_t make_compatible(node_type&               current,
                             Iterator                 first,
                             Iterator                 last,
                             bool                     stop_early,
                             std::chrono::nanoseconds stop_early_interval,
                             float                    stop_early_ratio);
    };  // class Graph

    ////////////////////////////////////////////////////////////////////////
    // 2. ToddCoxeterBase - data members - private
    ////////////////////////////////////////////////////////////////////////

    bool                                   _finished;
    Forest                                 _forest;
    std::vector<std::unique_ptr<Settings>> _setting_stack;
    Order                                  _standardized;
    Graph                                  _word_graph;

   public:
    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief The type of the underlying WordGraph.
    using word_graph_type = Graph;

    ////////////////////////////////////////////////////////////////////////
    // 3. ToddCoxeterBase - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_init_group Constructors + initializers
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Construct or re-initialize a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance (public member function).
    //!
    //! This page documents the constructors and initialisers for the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" class.
    //!
    //! Every constructor (except the move + copy constructors, and the move +
    //! copy assignment operators) has a matching `init` function with the same
    //! signature that can be used to re-initialize a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance as if it had just
    //! been constructed; but without necessarily releasing any previous
    //! allocated memory.
    //!
    //! @{

    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance.
    ToddCoxeterBase();

    //! \brief Re-initialize a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This function puts a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance back into the state that it would have been in if it had just
    //! been newly default constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    ToddCoxeterBase& init();

    //! Copy constructor.
    ToddCoxeterBase(ToddCoxeterBase const& that);

    //! Move constructor.
    ToddCoxeterBase(ToddCoxeterBase&&);

    //! Copy assignment operator.
    ToddCoxeterBase& operator=(ToddCoxeterBase const&);

    //! Move assignment operator.
    ToddCoxeterBase& operator=(ToddCoxeterBase&&);

    ~ToddCoxeterBase();

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance representing a congruence of kind \p knd over
    //! the semigroup or monoid defined by the presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    ToddCoxeterBase(congruence_kind knd, Presentation<word_type>&& p);

    //! \brief Re-initialize a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This function puts a  \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance back into the state that it would have been in if it had just
    //! been newly constructed from \p knd and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    ToddCoxeterBase& init(congruence_kind knd, Presentation<word_type>&& p);

    //! \copydoc ToddCoxeterBase(congruence_kind, Presentation<word_type>&&)
    ToddCoxeterBase(congruence_kind knd, Presentation<word_type> const& p);

    //! \copydoc init(congruence_kind, Presentation<word_type>&&)
    ToddCoxeterBase& init(congruence_kind                knd,
                          Presentation<word_type> const& p);

    //! \brief Construct from \ref congruence_kind and \ref WordGraph.
    //!
    //! This function constructs a  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance representing a congruence of kind \p knd over
    //! the WordGraph \p wg. The  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance constructed in this way represents a quotient
    //! of the word graph \p wg. If \p wg happens to be the left or right Cayley
    //! graph of a semigroup or monoid, then the  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance will represent a quotient of that semigroup.
    //!
    //! \tparam Node the type of the nodes in the 2nd argument.
    //!
    //! \param knd the kind (1- or 2-sided) of the congruence.
    //! \param wg the word graph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(1) a to_todd_coxeter variant that throws if wg is not valid
    // see below
    template <typename Node>
    ToddCoxeterBase(congruence_kind knd, WordGraph<Node> const& wg)
        : ToddCoxeterBase() {
      LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
      init(knd, wg);
    }

    //! \brief Re-initialize a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This function puts a  \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance back into the state that it would have been in if it had just
    //! been newly constructed from \p knd and \p wg.
    //!
    //! \tparam Node the type of the nodes in the 2nd argument.
    //!
    //! \param knd the kind (1- or 2-sided) of the congruence.
    //! \param wg the word graph.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(1) a to_todd_coxeter variant that throws if wg is not valid
    // i.e. any target is out of bounds.
    template <typename Node>
    ToddCoxeterBase& init(congruence_kind knd, WordGraph<Node> const& wg);

    // TODO(1) rvalue ref WordGraph init + constructor

    //! \brief Construct from \ref congruence_kind and \ref
    //! todd_coxeter_class_group "ToddCoxeterBase".
    //!
    //! This function constructs a  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance representing a congruence of kind \p knd over
    //! the  \ref todd_coxeter_class_group "ToddCoxeterBase" instance \p tc. The
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance constructed in
    //!  this way represents a quotient of the word graph represented by \p tc.
    //!
    //! \param knd the kind (onesided, or twosided) of the congruence.
    //! \param tc the  \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! \throw LibsemigroupsException if the arguments \p knd and \p tc are
    //! not compatible. If the first item is `tc.kind()` and the second is the
    //! parameter \p knd, then compatible arguments are (one-sided, one-sided),
    //! (two-sided, one-sided), and (two-sided, two-sided).
    ToddCoxeterBase(congruence_kind knd, ToddCoxeterBase const& tc);

    //! \brief Re-initialize a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This function puts a  \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance back into the state that it would have been in if it had just
    //! been newly constructed from \p knd and \p tc.
    //!
    //! \param knd the kind (onesided, or twosided) of the congruence.
    //! \param tc the  \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throw LibsemigroupsException if the arguments \p knd and \p tc are
    //! not compatible. If the first item is `tc.kind()` and the second is the
    //! parameter \p knd, then compatible arguments are (one-sided, one-sided),
    //! (two-sided, one-sided), and (two-sided, two-sided).
    ToddCoxeterBase& init(congruence_kind knd, ToddCoxeterBase const& tc);

    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance representing a congruence of kind \p knd over
    //! the semigroup or monoid defined by the presentation \p p. The type of
    //! the words in \p p can be anything, but will be converted in to \ref
    //! native_word_type. This means that if the input presentation uses
    //! std::string, for example, as the word type, then this presentation is
    //! converted into a \ref native_presentation_type. This converted
    //! presentation can be recovered using \ref presentation.
    //!
    //! \tparam Word the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    // This is a constructor and not a helper so that everything that takes a
    // presentation has the same constructors, regardless of what they use
    // inside.
    // TODO(0) reuse or rm doc

    //! \brief Re-initialize a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance.
    //!
    //! This function re-initializes a  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance as if it had been newly constructed
    //! from \p knd and \p p.
    //!
    //! \tparam Word the type of the words in the presentation \p p.
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    // TODO(0) reuse or rm doc
    //! @}

#ifndef PARSED_BY_DOXYGEN
    // Used in Sims
    // TODO(1) could this and the next function be removed, and replaced
    // with something else?
    template <typename Node>
    ToddCoxeterBase(congruence_kind                knd,
                    Presentation<word_type> const& p,
                    WordGraph<Node> const&         wg) {
      init(knd, p, wg);
    }

    // TODO(1) a to_todd_coxeter variant that throws if p is not valid
    template <typename Node>
    ToddCoxeterBase& init(congruence_kind                knd,
                          Presentation<word_type> const& p,
                          WordGraph<Node> const&         wg);
#endif

    //! \ingroup todd_coxeter_class_init_group
    //!
    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of
    //! bounds (i.e. does not belong to the alphabet of the \ref
    //! presentation used to construct the \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from \p
    //! first to \p last is out of bounds.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      internal_presentation().validate_word(first, last);
    }

    static void
    throw_if_presentation_not_normalized(Presentation<word_type> const&,
                                         std::string_view arg = "1st");

    ////////////////////////////////////////////////////////////////////////
    // 4. ToddCoxeterBase - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_intf_group Common member functions
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Documentation of common member functions of \ref
    //! Congruence, Kambites, KnuthBendixBase, and \ref
    //! todd_coxeter_class_group "ToddCoxeterBase".
    //!
    //! This page contains documentation of the member functions of
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" that are
    //!  implemented in all of the classes Congruence,
    //! Kambites, KnuthBendixBase, and \ref todd_coxeter_class_group
    //! "ToddCoxeterBase".
    //!
    //! @{

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented
    //! by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed that \ref started returns \c false. Adding
    //! generating pairs after \ref started is not permitted (but also not
    //! checked by this function).
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeterBase& add_generating_pair_no_checks(Iterator1 first1,
                                                   Iterator2 last1,
                                                   Iterator3 first2,
                                                   Iterator4 last2) {
      return CongruenceInterface::add_internal_generating_pair_no_checks<
          ToddCoxeterBase>(first1, last1, first2, last2);
    }

    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented
    //! by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_throws_if_started
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeterBase& add_generating_pair(Iterator1 first1,
                                         Iterator2 last1,
                                         Iterator3 first2,
                                         Iterator4 last2) {
      return CongruenceInterface::add_generating_pair<ToddCoxeterBase>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // 5. ToddCoxeterBase - interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! \brief Compute the number of classes in the congruence.
    //!
    //! This function computes the number of classes in the congruence
    //! represented by a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance by running the congruence enumeration until it
    //! terminates.
    //!
    //! \returns The number of congruences classes of a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance if this number
    //! is finite, or \ref POSITIVE_INFINITY in some cases if this number
    //! is not finite.
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    [[nodiscard]] uint64_t number_of_classes();

    ////////////////////////////////////////////////////////////////////////
    // 6. ToddCoxeterBase - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known
    //! to be contained in the congruence represented by a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. This function
    //! performs no enumeration, so it is possible for the words to be
    //! contained in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the
    //! congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(Iterator1 first1,
                                      Iterator2 last1,
                                      Iterator3 first2,
                                      Iterator4 last2) const;

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known
    //! to be contained in the congruence represented by a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. This function
    //! performs no enumeration, so it is possible for the words to be
    //! contained in the congruence, but that this is not currently known.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the
    //! congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      return CongruenceInterface::currently_contains<ToddCoxeterBase>(
          first1, last1, first2, last2);
    }

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in
    //! the congruence represented by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. This function triggers a full
    //! enumeration, which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains_no_checks(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2);

    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in
    //! the congruence represented by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. This function triggers a full
    //! enumeration, which may never terminate.
    //!
    //! \cong_intf_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // 7. ToddCoxeterBase - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \brief Reduce a word with no enumeration or checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output
    //! iterator \p d_first. This function triggers no enumeration. The
    //! word output by this function is equivalent to the input word in
    //! the congruence defined by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. If the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" instance is \ref
    //!  finished, then the output word is a normal
    //! form for the input word. If the  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const;

    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output
    //! iterator \p d_first. This function triggers no enumeration. The
    //! word output by this function is equivalent to the input word in
    //! the congruence defined by a \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance. If the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" instance is \ref
    //!  finished, then the output word is a normal
    //! form for the input word. If the  \ref todd_coxeter_class_group
    //! "ToddCoxeterBase" instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      return CongruenceInterface::reduce_no_run<ToddCoxeterBase>(
          d_first, first, last);
    }

    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a
    //! reduced word equivalent to the input word described by the
    //! iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined
    //! by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return CongruenceInterface::reduce_no_checks<ToddCoxeterBase>(
          d_first, first, last);
    }

    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a
    //! reduced word equivalent to the input word described by the
    //! iterator \p first and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined
    //! by a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_intf_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    //!
    //! \cong_intf_warn_undecidable{Todd-Coxeter}
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      return CongruenceInterface::reduce<ToddCoxeterBase>(d_first, first, last);
    }
    //! @}

    ////////////////////////////////////////////////////////////////////////
    // 8. ToddCoxeterBase - settings - public
    ////////////////////////////////////////////////////////////////////////

#ifndef PARSED_BY_DOXYGEN
    // This is documented in Runner, so we don't duplicate the doc here.
    template <typename T>
    void report_every(T val) {
      CongruenceInterface::report_every(val);
      _word_graph.report_every(val);
    }
    using Reporter::report_every;
#endif

    //! \defgroup todd_coxeter_class_settings_group Settings
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Settings that control the behaviour of a  \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! This page contains information about the member functions of the
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" that control
    //!  various settings that influence the congruence
    //! enumeration process.
    //!
    //! There are a fairly large number of settings, they can profoundly
    //! alter the run time of a congruence enumeration process, but it is
    //! hard to predict what settings will work best for any particular
    //! input.
    //!
    //! See also \ref Runner for further settings.
    //!
    //! @{

    //! \brief Set the maximum number of definitions in the stack.
    //!
    //! This setting specifies the maximum number of definitions that can
    //! be in the stack at any given time. What happens if there are the
    //! maximum number of definitions in the stack and a new definition is
    //! generated is governed by \ref def_policy.
    //!
    //! The default value of this setting is \c 2'000.
    //!
    //! \param val the maximum size of the definition stack.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& def_max(size_t val) noexcept;

    //! \brief Get the current value of the setting for the maximum number
    //! of definitions.
    //!
    //! \returns The current value of the setting, a value of type
    //! `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t def_max() const noexcept;

    //! \brief Set the definition policy.
    //!
    //! This function can be used to specify how to handle definitions.
    //! For details see options::def_policy.
    //!
    //! The default value of this setting is
    //! \ref options::def_policy::no_stack_if_no_space.
    //!
    //! \param val the policy to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    ToddCoxeterBase& def_policy(options::def_policy val);

    //! \brief Get the current value of the definition policy.
    //!
    //! This function returns the current value of the definition policy
    //! which specifies how to handle definitions. For details see
    //! options::def_policy.
    //!
    //! \returns The current value of the setting, a value of type
    //! \ref options::def_policy.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::def_policy def_policy() const noexcept;

    //! \brief Set the number of Felsch style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! This function can be used to set the approximate number of nodes
    //! defined in Felsch style in each phase of the
    //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
    //! * \ref options::strategy::CR;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::Cr; and
    //! * \ref options::strategy::Rc.
    //!
    //! If the strategy is not one of those listed above, then this
    //! setting is ignored.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is \c 0.
    ToddCoxeterBase& f_defs(size_t val);

    //! \brief Get the number of Felsch style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! This function returns the approx number of Felsch style
    //! definitions in each phase of the
    //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
    //! * \ref options::strategy::CR;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::Cr; and
    //! * \ref options::strategy::Rc.
    //!
    //! If the strategy is not one of those listed above, then this
    //! setting is ignored.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \returns The current value of the setting, a value of type
    //! `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t f_defs() const noexcept;

    //! \brief Set the number of HLT style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! This function can be used to set the approximate number nodes
    //! defined in HLT style in each phase of the
    //! [ACE](https://staff.itee.uq.edu.au/havas/) style strategies:
    //! * \ref options::strategy::CR;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::Cr; and
    //! * \ref options::strategy::Rc.
    //!
    //! If the strategy is not one of those listed above, then this
    //! setting is ignored.
    //!
    //! The default value of this setting is \c 200'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is \c 0.
    ToddCoxeterBase& hlt_defs(size_t val);

    //! \brief Get the number of HLT style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! This function returns the approx number of HLT style definitions
    //! in each phase of the [ACE](https://staff.itee.uq.edu.au/havas/)
    //! style strategies:
    //! * \ref options::strategy::CR;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::R_over_C;
    //! * \ref options::strategy::Cr; and
    //! * \ref options::strategy::Rc.
    //!
    //! If the strategy is not one of those listed above, then this
    //! setting is ignored.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \returns The current value of the setting, a value of type
    //! `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t hlt_defs() const noexcept;

    //! \brief Set the size of a large collapse.
    //!
    //! This function can be used to set what should be considered a
    //! \"large\" collapse.
    //!
    //! By default when processing coincidences nodes are merged in the
    //! word graph one pair at a time, and the in-neighbours of the
    //! surviving node are updated at the same time. If the number of
    //! coincidences is large, then it might be that a pair of nodes are
    //! merged at one step, then the surviving node is merged with another
    //! node at a future step, and this may happen many many times. This
    //! results in the in-neighbours of the surviving nodes being
    //! repeatedly traversed, which can result in a significant
    //! performance penalty. It can be beneficial to stop updating the
    //! in-neighbours as nodes are merged, and to just rebuild the entire
    //! in-neighbours data structure by traversing the entire word graph
    //! after all coincidences have been processed. This is beneficial if
    //! the number of surviving nodes is relatively small in comparison to
    //! the number of nodes merged. The purpose of this setting is to
    //! specify what should be considered a \"large\" collapse, or more
    //! precisely, what number of coincidences in the stack will trigger a
    //! change from updating the in-neighbours one-by-one to traversing
    //! the entire graph once after all coincidences have been processed.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& large_collapse(size_t val) noexcept;

    //! \brief Get the current size of a large collapse.
    //!
    //! This function can be used to get what is currently considered a
    //! \"large\" collapse. See \ref large_collapse(size_t) for the
    //! meaning of this setting.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \returns The current value of the setting, a value of type
    //! `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t large_collapse() const noexcept;

    //! \brief Set the lookahead extent.
    //!
    //! This function can be used to specify the extent of any lookaheads
    //! that might take place in a congruence enumeration. The possible
    //! values are options::lookahead_extent::partial or
    //! options::lookahead_extent::full.
    //!
    //! The default value of this setting is \ref
    //! options::lookahead_extent::partial.
    //!
    //! \param val the extent.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lookahead_extent(options::lookahead_extent val) noexcept;

    //! \brief Get the current value of the lookahead extent.
    //!
    //! This function returns the current value of the lookahead extent
    //! setting.
    //!
    //! The default value of this setting is \ref
    //! options::lookahead_extent::partial.
    //!
    //! \returns The current lookahead extent.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::lookahead_extent lookahead_extent() const noexcept;

    //! \brief Set the lookahead growth factor.
    //!
    //! This setting determines by what factor the number of nodes
    //! required to trigger a lookahead grows. More specifically, at the
    //! end of any lookahead if the number of active nodes already exceeds
    //! the value of
    //! \ref lookahead_next or the number of nodes killed during the
    //! lookahead is less than the number of active nodes divided by
    //! \ref lookahead_growth_threshold, then the value of
    //! \ref lookahead_next is increased by a multiple of \p val.
    //!
    //! The default value is of this setting is `2.0`.
    //!
    //! \param val the value indicating the lookahead growth factor.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than `1.0`.
    ToddCoxeterBase& lookahead_growth_factor(float val);

    //! \brief Get the current value of the lookahead growth factor.
    //!
    //! This function returns the current value of the lookahead growth
    //! factor. See lookahead_growth_factor(float) for a full explanation
    //! of this setting.
    //!
    //! \returns The lookahead growth factor.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] float lookahead_growth_factor() const noexcept;

    //! \brief Set the lookahead growth threshold.
    //!
    //! This setting determines the threshold for the number of nodes
    //! required to trigger a lookahead. More specifically, at the end of
    //! any lookahead if the number of active nodes already exceeds the
    //! value of \ref lookahead_next or the number of nodes killed during
    //! the lookahead is less than the number of active nodes divided by
    //! \ref lookahead_growth_threshold, then the value of \ref
    //! lookahead_next is increased.
    //!
    //! The default value is `4`.
    //!
    //! \param val the value indicating the lookahead growth threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lookahead_growth_threshold(size_t val) noexcept;

    //! \brief Get the current value of the lookahead growth threshold.
    //!
    //! This function returns the current value of the lookahead growth
    //! threshold. See \ref lookahead_growth_threshold for a full
    //! description of this setting.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t lookahead_growth_threshold() const noexcept;

    //! \brief Set the minimum value of \ref lookahead_next.
    //!
    //! After a lookahead is performed the value of \ref lookahead_next is
    //! modified depending on the outcome of the current lookahead. If the
    //! return value of \ref lookahead_next is too small or too large,
    //! then the value is adjusted according to \ref
    //! lookahead_growth_factor and
    //! \ref lookahead_growth_threshold. This setting specified the
    //! minimum possible value for lookahead_next().
    //!
    //! The default value is \c 10'000.
    //!
    //! \param val value indicating the minimum value of \ref
    //! lookahead_next.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lookahead_min(size_t val) noexcept;

    //! \brief Get the current value of the minimum lookahead setting.
    //!
    //! This function returns the current value of the minimum lookahead.
    //! See \ref lookahead_min(size_t) for a full description
    //! of this setting.
    //!
    //! The default value is \c 10'000.
    //!
    //! \returns The current value of the minimum lookahead.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t lookahead_min() const noexcept;

    //! \brief Set the threshold that will trigger a lookahead.
    //!
    //! If the number of active nodes exceeds the value set by this
    //! function, then a lookahead of style \ref lookahead_style and
    //! extent \ref lookahead_extent will be triggered.
    //!
    //! The default value is 5 million.
    //!
    //! \param val value indicating the initial threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lookahead_next(size_t val) noexcept;

    //! \brief Get the current value of the lookahead next setting.
    //!
    //! This function returns the current value of the lookahead next
    //! setting. See \ref lookahead_next(size_t) for a full description of
    //! this setting.
    //!
    //! \returns The number of active nodes that will trigger the next
    //! lookahead.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t lookahead_next() const noexcept;

    //! \brief Set the lookahead stop early interval.
    //!
    //! During any lookaheads that are performed, it is periodically
    //! checked what proportion of the active nodes have been killed since
    //! the previous such check. This function can be used to set the
    //! interval between these checks. The purpose of this setting is to
    //! allow lookaheads to be stopped early if the number of nodes being
    //! killed is too small (for example, if \f$<1%\f$ of nodes were
    //! killed in the previous second, then we might want to stop the
    //! lookahead early, since lookaheads take some time but may not
    //! result in many nodes being killed).
    //!
    //! The default value is 1 second.
    //!
    //! \param val the new value for the interval.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase&
    lookahead_stop_early_interval(std::chrono::nanoseconds val) noexcept;

    //! \brief Get the current value of the lookahead stop early interval.
    //!
    //! This function returns the current value of the lookahead stop
    //! early interval. See \ref
    //! lookahead_stop_early_interval(std::chrono::nanoseconds) for a full
    //! description of this setting.
    //!
    //! \returns The length of the interval in nanoseconds.
    //!
    //! \exceptions
    //! \noexcept
    std::chrono::nanoseconds lookahead_stop_early_interval() const noexcept;

    //! \brief Set the lookahead stop early ratio.
    //!
    //! During any lookaheads that are performed, it is periodically
    //! checked what proportion of the active nodes have been killed since
    //! the previous such check. This function can be used to set the
    //! minimum proportion of the active nodes that must be killed every
    //! \ref lookahead_stop_early_interval to avoid the lookahead being
    //! stopped early. The purpose of this setting is to allow lookaheads
    //! to be stopped early if the number of nodes being killed is too
    //! small (for example, if no nodes were killed in the previous
    //! second, then we might want to stop the lookahead early, since
    //! lookaheads take some time but may not result in many nodes being
    //! killed).
    //!
    //! \param val the proportion of active nodes.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is not in the interval
    //! \f$[0, 1)\f$.
    ToddCoxeterBase& lookahead_stop_early_ratio(float val);

    //! \brief Get the current value of the lookahead stop early ratio.
    //!
    //! This function returns the current value of the lookahead stop
    //! early ratio. See \ref lookahead_stop_early_ratio(float) for a full
    //! description of this setting.
    //!
    //! \returns The ratio.
    //!
    //! \exceptions
    //! \noexcept
    float lookahead_stop_early_ratio() const noexcept;

    //! \brief Set the style of lookahead.
    //!
    //! This function can be used to set the style of any lookaheads that
    //! are performed during the congruence enumeration. The possible
    //! values are options::lookahead_style::HLT and
    //! options::lookahead_style::felsch.
    //!
    //! The default value of this setting is
    //! options::lookahead_style::HLT.
    //!
    //! \param val the style of lookahead to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lookahead_style(options::lookahead_style val) noexcept;

    //! \brief Get the current value of the lookahead style.
    //!
    //! This function returns the current value of the lookahead style.
    //! See \ref lookahead_style(options::lookahead_style) for
    //! a full description of this setting.
    //!
    //! \returns The current lookahead style.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::lookahead_style lookahead_style() const noexcept;

    //! \brief Specify the minimum number of classes that may permit any
    //! enumeration early stop.
    //!
    //! This function can be used to set a lower bound for the number of
    //! classes of the congruence represented by a  \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. If the number
    //! of active nodes becomes at least the value of the argument, and
    //! the word graph is complete (\ref word_graph::is_complete returns
    //! \c true), then the enumeration is terminated. When the given bound
    //! is equal to the number of classes, this may prevent following the
    //! paths labelled by relations at many nodes when there is no
    //! possibility of finding coincidences.
    //!
    //! The default value is \ref UNDEFINED.
    //!
    //! \param val value indicating the lower bound.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& lower_bound(size_t val) noexcept;

    //! \brief Get the current value of the lower bound.
    //!
    //! This function returns the current value of the lower bound.
    //! See \ref lower_bound(size_t) for a full description of this
    //! setting.
    //!
    //! \returns The current lower bound.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t lower_bound() const noexcept;

    //! \brief Set whether or not to process definitions during HLT.
    //!
    //! If the argument of this function is \c true and the HLT strategy
    //! is being used, then definitions are processed during any
    //! enumeration.
    //!
    //! The default value is \c false.
    //!
    //! \param val value indicating whether or not to process deductions.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& save(bool val) noexcept;

    //! \brief Get the current value of the save setting.
    //!
    //! This function returns the current value of the save setting.
    //! See \ref save(bool) for a full description of this setting.
    //!
    //! \returns The current value.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool save() const noexcept;

    //! \brief Specify the congruence enumeration strategy.
    //!
    //! The strategy used during the enumeration can be specified using
    //! this function.
    //!
    //! The default value is options::strategy::hlt.
    //!
    //! \param val value indicating which strategy to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& strategy(options::strategy val) noexcept;

    //! \brief Get the current value of the strategy setting.
    //!
    //! This function returns the current value of the strategy setting.
    //! See \ref strategy(options::strategy) for a full description of
    //! this setting.
    //!
    //! \returns The current value.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::strategy strategy() const noexcept;

    //! \brief Set whether or not to perform an HLT-style push of the
    //! defining relations at the identity.
    //!
    //! If a  \ref todd_coxeter_class_group "ToddCoxeterBase" instance is
    //! defined over a finitely presented semigroup or monoid and the
    //! Felsch strategy is being used, it can be useful to follow all the
    //! paths from the identity labelled by the underlying relations. The
    //! setting specifies whether or not to do this.
    //!
    //! The default value of this setting is \c false.
    //!
    //! \param val the boolean value.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeterBase& use_relations_in_extra(bool val) noexcept;

    //! \brief Get the current value of the \"use relations in extra\"
    //! setting.
    //!
    //! This function returns the current value of the "use relations in
    //! extra" setting. See \ref use_relations_in_extra(bool) for a fuller
    //! description of this setting.
    //!
    //! \returns The current value.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool use_relations_in_extra() const noexcept;
    //! @}

#ifdef PARSED_BY_DOXYGEN
    //! \ingroup todd_coxeter_class_settings_group
    //!
    //! \brief Set the value of the definition version setting.
    //!
    //! There are two versions of definition processing represented by the
    //! values options::def_version::one and options::def_version::two.
    //! The first version is simpler, but may involve following the same
    //! path that leads nowhere multiple times. The second version is more
    //! complex, and attempts to avoid following the same path multiple
    //! times if it is found to lead nowhere once.
    //!
    //! \param val the version to use.
    //!
    //! \returns A reference to `*this`.
    ToddCoxeterBase& def_version(options::def_version val);

    //! \ingroup todd_coxeter_class_settings_group
    //!
    //! \brief Get the current value of the definition version setting.
    //!
    //! This function returns the current version of the definition
    //! version setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! `options::def_version`.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::def_version def_version() const noexcept;
#else
    using FelschGraphSettings_::def_version;
    using FelschGraphSettings_::settings;
#endif

    ////////////////////////////////////////////////////////////////////////
    // 9. ToddCoxeterBase - accessors - public
    ////////////////////////////////////////////////////////////////////////

#ifndef PARSED_BY_DOXYGEN
    [[nodiscard]] native_presentation_type const&
    internal_presentation() const noexcept {
      return _word_graph.presentation();
    }
#endif

    //! \brief Get the generating pairs of the congruence.
    //!
    //! This function returns the generating pairs of the congruence. The
    //! words comprising the generating pairs are converted to \ref
    //! native_word_type as they are added via \ref add_generating_pair.
    //! This function returns the std::vector of these \ref
    //! native_word_type.
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    // TODO(0) rm or use doc

    //! \defgroup todd_coxeter_class_accessors_group Accessors
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions that can be used to access the state of a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! This page contains the documentation of the various member
    //! functions of the \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! class that can be used to access the state of an instance.
    //!
    //! Those functions with the prefix `current_` do not perform any
    //! further enumeration.
    //!
    //! @{

    // [[nodiscard]] bool empty() const {
    //   return (internal_presentation().rules.empty() &&
    //   generating_pairs().empty()
    //           && current_word_graph().number_of_nodes_active() == 1);
    //   // FIXME(1) there's an issue where the word graph can have 0
    //   nodes but
    //   1
    //   // active node.
    // }

    //! \brief Get the presentation used to define a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance (if any).
    //!
    //! If a \ref todd_coxeter_class_group "ToddCoxeterBase" instance is
    //! constructed or initialised using a presentation, then a const
    //! reference to the \ref native_presentation_type version of this
    //! presentation is returned by this function.
    //!
    //! If the \ref todd_coxeter_class_group "ToddCoxeterBase" instance
    //! was constructed or initialised from a WordGraph, then this
    //! presentation will be empty.
    //!
    //! \returns A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    // TODO(0) use or rm doc

    //! \brief Get the current word graph.
    //!
    //! In some sense, the purpose of the Todd-Coxeter algorithm is to
    //! produce a WordGraph of the action of a set of generators on the
    //! classes of a congruence. This function can be used to obtain a
    //! reference to that WordGraph as it currently exists within a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. This function
    //! does not trigger any enumeration.
    //!
    //! The WordGraph returned by this function may be in a rather
    //! complicated state. No guarantees are given: about the values of
    //! the active nodes (i.e. they may be any non-negative integers in
    //! any order); that the number of nodes (including those that are
    //! inactive) should coincide with the number of active nodes; that
    //! the graph is complete; or that the graph is compatible with the
    //! relations of the \ref presentation or with the \ref
    //! generating_pairs.
    //!
    //! The functions \ref standardize(Order) and \ref shrink_to_fit can
    //! be used to modify the returned word graph in-place to have
    //! (possibly) more reasonable characteristics.
    //!
    //! \returns A const reference to the underlying WordGraph.
    //!
    //! \exceptions
    //! \noexcept
    word_graph_type const& current_word_graph() const noexcept {
      return _word_graph;
    }

    //! \brief Get the word graph after performing a full congruence
    //! enumeration.
    //!
    //! In some sense, the purpose of the Todd-Coxeter algorithm is to
    //! produce a WordGraph of the action of a set of generators on the
    //! classes of a congruence. This function can be used to obtain a
    //! reference to that WordGraph instance. This function triggers a
    //! full enumeration.
    //!
    //! The WordGraph returned by this function may be in a rather
    //! complicated state. The active nodes (and nodes) will be \f$\{0,
    //! \ldots, n - 1\}\f$ where \f$n\f$ is the number of classes in the
    //! congruence if \ref presentation contains the empty word; or the
    //! number of classes plus one if \ref presentation does not contain
    //! the empty word. The returned WordGraph is also short-lex
    //! standardized. The returned WordGraph will usually be complete and
    //! compatible with the relations of the \ref presentation and with
    //! the \ref generating_pairs. The WordGraph may not be complete or
    //! compatible for some values of the settings. For example, if the
    //! setting \ref lower_bound is used but is not the same as the number
    //! of classes in the congruence, then the WordGraph returned by this
    //! function may not be compatible with the relations of \ref
    //! presentation or \ref generating_pairs.
    //!
    //! \returns A const reference to the underlying WordGraph.
    word_graph_type const& word_graph();

    //! \brief Get the current possible spanning tree of the underlying
    //! word graph.
    //!
    //! This function returns a const reference to the current value of a
    //! possible spanning tree (a \ref Forest) for the underlying
    //! WordGraph (returned by \ref current_word_graph). This spanning
    //! tree is only populated during calls to \ref standardize and as
    //! such might contain nothing, or a spanning tree of a previous value
    //! of \ref current_word_graph. Some care should be used with the
    //! return value of this function, and it might be better to use the
    //! function \ref spanning_tree, which has none of these limitation.
    //!
    //! If \ref finished returns \c true, and \ref standardize(Order) has
    //! been called prior to a call to this function, then the returned
    //! \ref Forest will represent a valid spanning tree for the WordGraph
    //! returned by \ref current_word_graph or \ref word_graph.
    //!
    //! \returns A const reference to a possible spanning tree of the
    //! underlying \ref WordGraph.
    //!
    //! \exceptions
    //! \noexcept
    Forest const& current_spanning_tree() const noexcept {
      return _forest;
    }

    //! \brief Get the spanning tree of the underlying word graph.
    //!
    //! This function returns a const reference to a spanning tree (a \ref
    //! Forest) for the underlying WordGraph (returned by \ref word_graph)
    //! with the nodes appearing in short-lex order. This function
    //! triggers a full congruence enumeration.
    //!
    //! \returns A const reference to a spanning tree of the underlying
    //! \ref WordGraph.
    Forest const& spanning_tree();

    //! \brief Get the current standardization order of the underlying
    //! word graph.
    //!
    //! This function returns the standardization order currently used in
    //! the underlying word graph. The return value of this function will
    //! be the argument of the most recent call to \ref
    //! standardize(Order); or Order::none.
    //!
    //! The return value of this function indicates the following:
    //! * Order::none implies that no standardization has been performed
    //! and:
    //!   - the return value of \ref reduce will essentially arbitrary;
    //!   - the return values of \ref todd_coxeter::normal_forms will be
    //!   essentially arbitrary;
    //!   - the classes of the congruence will be indexed in an arbitrary
    //!   order;
    //! * Order::shortlex implies that:
    //!   - the return value of \ref reduce will be the short-lex least
    //!   word belonging to a given congruence class;
    //!   - the return values of \ref todd_coxeter::normal_forms will be
    //!   in short-lex order;
    //!   - the classes of the congruence will be indexed in short-lex
    //!   order
    //!     on the short-lex least word;
    //! * Order::lex implies that:
    //!   - the return values of \ref todd_coxeter::normal_forms will be
    //!   ordered lexicographically.
    //!   - the return values of \ref reduce and the indexes of class are
    //!     essentially arbitrary because there is not necessarily a
    //!     lexicographically least word in every class;
    //! * Order::recursive implies that:
    //!   - the return value of \ref reduce will be the recursive path
    //!   least word belonging to a given congruence class;
    //!   - the return values of \ref todd_coxeter::normal_forms will be
    //!   in recursive path order;
    //!   - the classes of the congruence will be indexed in recursive
    //!   path order on the recursive path least word.
    //!
    //! \returns The current standardization order.
    //!
    //! \exceptions
    //! \noexcept
    inline Order standardization_order() const noexcept {
      return _standardized;
    }

    //! \brief Check if the word graph is currently standardized with
    //! respect to a given order.
    //!
    //! This function returns \c true if the \ref current_word_graph has
    //! been standardized with respect to the order \p val; and \c false
    //! if not.
    //!
    //! \param val the \ref Order to check for.
    //!
    //! \returns Whether or not the current word graph is standardized
    //! with respect to a given order.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    bool is_standardized(Order val) const;

    //! \brief Check if the word graph is currently standardized with
    //! respect to any order.
    //!
    //! This function returns \c true if the \ref current_word_graph has
    //! been standardized with respect to the any \ref Order other than
    //! Order::none.
    //!
    //! \returns Whether or not the current word graph is standardized
    //! with respect to any order.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    bool is_standardized() const;
    //! @}

    ////////////////////////////////////////////////////////////////////////
    // 10. ToddCoxeterBase - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_mod_group Modifiers
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions that can be used to modify the state of a
    //! \ref todd_coxeter_class_group "ToddCoxeterBase" instance.
    //!
    //! This page contains documentation of the member functions of
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" that can be used
    //!  to modify the state of a \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" instance. In other
    //! words, for modifying the WordGraph that is the output of the
    //! algorithm in a way that preserves it up to isomorphism.
    //!
    //! @{

    //! \brief Shrink the underlying word graph to remove all dead nodes.
    //!
    //! This function triggers a full enumeration, and standardization,
    //! and removes from \ref word_graph any dead nodes.
    //!
    //! If \ref finished returns \c false, then this function does
    //! nothing.
    void shrink_to_fit();

    //! \brief Standardize the \ref current_word_graph.
    //!
    //! This function standardizes the return value of \ref
    //! current_word_graph, and does not trigger any enumeration. See \ref
    //! standardization_order for a full description. The return value of
    //! this function indicates whether or not the \ref current_word_graph
    //! was modified. In other words, if this function returns \c true,
    //! then the word graph was not previously standardized with respect
    //! to \p val, and was modified by calling this function if \c false
    //! is returned, then the word graph was previously standardized with
    //! respect to \p val (although this might not have been known), and
    //! was not modified by calling this function.
    //!
    //! \param val the order of the standardization.
    //!
    //! \returns Whether or not the word graph was modified by the
    //! standardization.
    //!
    //! \note If \p val is Order::none, then this function does nothing.
    //!
    //! \sa \ref word_graph::standardize
    //! \sa \ref current_spanning_tree.
    bool standardize(Order val);

    //! \brief Perform a lookahead.
    //!
    //! This function can be used to explicitly perform a lookahead. The
    //! style and extent of this lookahead are controlled by the settings
    //! \ref lookahead_style and \ref lookahead_extent.
    //!
    //! If the argument \p stop_early is \c true, then the settings \ref
    //! lookahead_stop_early_interval and \ref lookahead_stop_early_ratio
    //! are used to determine whether or not the lookahead should be
    //! aborted early. If \p stop_early is \c false, then these settings
    //! are ignored.
    //!
    //! \param stop_early whether or not to consider stopping the
    //! lookahead early if too few nodes are killed.
    void perform_lookahead(bool stop_early);
    //! @}

    ////////////////////////////////////////////////////////////////////////
    // 11. ToddCoxeterBase - word -> index
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_word_index_group Word to class index
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting a word into the index of a
    //! class in a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This page contains documentation for the member functions of \ref
    //! todd_coxeter_class_group "ToddCoxeterBase" that can be used to
    //! convert a word into the index of congruence class.
    //!
    //! \sa \ref todd_coxeter_class_index_word_group for the inverses of
    //! the functions described on this page.
    //!
    //! @{

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing
    //! the word described by the iterators \p first and \p last. No
    //! enumeration is triggered by calls to this function. Unless \ref
    //! finished returns \c true, the index returned by this function is
    //! essentially arbitrary, and can only really be used to check
    //! whether or not two words are currently known to belong to the
    //! congruence.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by
    //! the arguments \p first and \p last. If there is no such path, then
    //! \ref UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    //!
    // NOTE THAT: the graph contains one more node than there are element
    // if the underlying presentation does not contain the empty word
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of_no_checks(Iterator1 first,
                                          Iterator2 last) const;

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing
    //! the word described by the iterators \p first and \p last. No
    //! enumeration is triggered by calls to this function. Unless \ref
    //! finished returns \c true, the index returned by this function is
    //! essentially arbitrary, and can only really be used to check
    //! whether or not two words are currently known to belong to the
    //! congruence.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by
    //! the arguments \p first and \p last. If there is no such path, then
    //! \ref UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of(Iterator1 first, Iterator2 last) const {
      throw_if_letter_out_of_bounds(first, last);
      return current_index_of_no_checks(first, last);
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full
    //! enumeration is triggered by calls to this function. If the \ref
    //! current_word_graph has not already been standardized, then this
    //! function first standardizes it with respect to Order::shortlex;
    //! otherwise the existing standardization order is used.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by
    //! the arguments \p first and \p last. Since a full enumeration is
    //! triggered by calls to this function, the word graph is complete,
    //! and so the return value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_intf_warn_assume_letters_in_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of_no_checks(Iterator1 first, Iterator2 last);

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full
    //! enumeration is triggered by calls to this function. If the \ref
    //! current_word_graph has not already been standardized, then this
    //! function first standardizes it with respect to Order::shortlex;
    //! otherwise the existing standardization order is used.
    //!
    //! The returned index is obtained by following the path in \ref
    //! current_word_graph from node \c 0 labelled by the word given by
    //! the arguments \p first and \p last. Since a full enumeration is
    //! triggered by calls to this function, the word graph is complete,
    //! and so the return value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_intf_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of(Iterator1 first, Iterator2 last) {
      throw_if_letter_out_of_bounds(first, last);
      return index_of_no_checks(first, last);
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // 12. ToddCoxeterBase - index -> word
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_index_word_group Class index to word
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting the index of a class
    //! into a word in a \ref todd_coxeter_class_group "ToddCoxeterBase"
    //! instance.
    //!
    //! This page contains documentation for the member functions of
    //!  \ref todd_coxeter_class_group "ToddCoxeterBase" that can be used
    //!  to convert the index of a congruence class
    //! to a representative word belonging to that congruence class.
    //!
    //! \sa \ref todd_coxeter_class_word_index_group for the inverses of
    //! the functions described on this page.
    //!
    //! @{

    //! \brief Insert a current word representing a class with given index
    //! into an output iterator.
    //!
    //! This function appends a current word representing the class with
    //! index
    //! \p i to the output iterator \p d_first. No enumeration is
    //! triggered by calls to this function, but \ref current_word_graph
    //! is standardized (using Order::shortlex) if it is not already
    //! standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c
    //! i back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of
    //! the output word.
    //!
    //! \warning This function does not check its arguments. In
    //! particular, it is assumed that \p i is a valid index of a current
    //! class.
    //!
    //! \todd_coxeter_note_reverse
    // NOTE THAT: the graph contains one more node than there are element
    // if the underlying presentation does not contain the empty word
    template <typename OutputIterator>
    OutputIterator current_word_of_no_checks(OutputIterator d_first,
                                             index_type     i) const;

    //! \brief Insert a current word representing a class with given index
    //! into an output iterator.
    //!
    //! This function appends a current word representing the class with
    //! index
    //! \p i to the output iterator \p d_first. No enumeration is
    //! triggered by calls to this function, but \ref current_word_graph
    //! is standardized (using Order::shortlex) if it is not already
    //! standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c
    //! i back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of
    //! the output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    //!
    //! \todd_coxeter_note_reverse
    template <typename OutputIterator>
    OutputIterator current_word_of(OutputIterator d_first, index_type i) const;

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is
    //! triggered by calls to this function, and \ref current_word_graph
    //! is standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c
    //! i back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of
    //! the output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    //!
    //! \todd_coxeter_note_reverse
    template <typename Iterator>
    Iterator word_of_no_checks(Iterator d_first, index_type i) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return current_word_of_no_checks(d_first, i);
    }

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is
    //! triggered by calls to this function, and \ref current_word_graph
    //! is standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c
    //! i back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of
    //! the output word.
    //!
    //! \warning This function does not check its arguments. In
    //! particular, it is assumed that \p i is a valid index of a current
    //! class.
    //!
    //! \todd_coxeter_note_reverse
    template <typename Iterator>
    Iterator word_of(Iterator d_first, index_type i) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return current_word_of(d_first, i);
    }

    //! @}

   private:
    ////////////////////////////////////////////////////////////////////////
    // 13. Runner - pure virtual member functions - private
    ////////////////////////////////////////////////////////////////////////

    void really_run_impl();

    void run_impl() override;

    bool finished_impl() const override {
      return _finished;
    }

    ////////////////////////////////////////////////////////////////////////
    // 14. ToddCoxeterBase - member functions - private
    ////////////////////////////////////////////////////////////////////////

    void copy_settings_into_graph();

    // This function has prefix tc_ because there's already a settings
    // function in a base class
    Settings&       tc_settings();
    Settings const& tc_settings() const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterBase - main strategies - private
    ////////////////////////////////////////////////////////////////////////

    void init_run();
    void finalise_run();

    void felsch();
    void hlt();
    void CR_style();
    void R_over_C_style();
    void Cr_style();
    void Rc_style();

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterBase - reporting - private
    ////////////////////////////////////////////////////////////////////////

    void report_next_lookahead(size_t old_value) const;
    void report_nodes_killed(int64_t number) const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterBase - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    static constexpr bool StopEarly      = true;
    static constexpr bool DoNotStopEarly = false;

    size_t hlt_lookahead(bool stop_early);
    size_t felsch_lookahead();
  };  // class ToddCoxeterBase

  //! \ingroup todd_coxeter_group
  //!
  //! \brief Return a human readable representation of a \ref
  //! todd_coxeter_class_group "ToddCoxeterBase" object.
  //!
  //! Defined in `todd-coxeter.hpp`.
  //!
  //! This function returns a human readable representation of a \ref
  //! todd_coxeter_class_group "ToddCoxeterBase" object.
  //!
  //! \param tc the \ref todd_coxeter_class_group "ToddCoxeterBase"
  //! object.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  std::string to_human_readable_repr(ToddCoxeterBase const& tc);

}  // namespace libsemigroups

#include "todd-coxeter-base.tpp"
#endif  // LIBSEMIGROUPS_TODD_COXETER_BASE_HPP_
