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
// TODO: (1)
// * re-implement reserve
// * remove preferred_defs from FelschGraph etc (except where they are really
// needed)? Or possibly reintroduce PrefDefs here
// * TODO(0) re-add report why stopped

#ifndef LIBSEMIGROUPS_TODD_COXETER_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_HPP_

#include "cong-intf.hpp"
#include "exception.hpp"  // for LIBSEMIGROUPS_EXCEPTION
#include "libsemigroups/detail/iterator.hpp"
#include "obvinf.hpp"           // for is_obviously_infinite
#include "order.hpp"            // for order
#include "paths.hpp"            // for Paths
#include "presentation.hpp"     // for Presentation
#include "to-presentation.hpp"  // for to_presentation
#include "types.hpp"            // for word_type

#include "detail/felsch-graph.hpp"        // for FelschGraph
#include "detail/node-managed-graph.hpp"  // for NodeManagedGraph
#include "detail/report.hpp"              // for LIBSEMIGROUPS_EXCEPTION
#include <cstddef>
#include <type_traits>

////////////////////////////////////////////////////////////////////////
// This file is organised as follows:
// 0.  ToddCoxeter - member types - public
// 1.  ToddCoxeter - nested classes - private
// 2.  ToddCoxeter - data members - private
// 3.  ToddCoxeter - constructors + initializers - public
// 4.  ToddCoxeter - interface requirements - add_pair
// 5.  ToddCoxeter - interface requirements - number_of_classes
// 6.  ToddCoxeter - interface requirements - contains
// 7.  ToddCoxeter - interface requirements - reduce
// 8.  ToddCoxeter - settings - public
// 9.  ToddCoxeter - accessors - public
// 10. ToddCoxeter - modifiers - public
// 11. ToddCoxeter - word -> index
// 12. ToddCoxeter - index -> word
// 13. Runner      - pure virtual member functions - private
// 14. ToddCoxeter - member functions - private

namespace libsemigroups {
  //! \defgroup todd_coxeter_group Todd-Coxeter
  //!
  //! This page contains documentation related to the implementation of the
  //! Todd-Coxeter algorithm \cite Coleman2022aa in ``libsemigroups``.
  //!
  //! The purpose of this algorithm is to find the WordGraph of the action of a
  //! semigroup or monoid on the classes of a left, right, or 2-sided
  //! congruence; see \cite Coleman2022aa for more details.

  //! \ingroup todd_coxeter_group
  //!
  //! \brief Class containing an implementation of the Todd-Coxeter Algorithm.
  //!
  //! Defined in ``todd-coxeter.hpp``.
  //!
  //! This class contains an implementation of the Todd-Coxeter
  //! algorithm for computing left, right, and 2-sided congruences on
  //! a semigroup or monoid.
  //!
  //! In this documentation we use the term "congruence enumeration" to mean the
  //! execution of (any version of) the Todd-Coxeter algorithm.
  //! Some of the features of this class were inspired by similar features in
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
  //! ToddCoxeter tc(congruence_kind::left, p);
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
  //! ToddCoxeter tc(congruence_kind::twosided, p);
  //! tc.strategy(options::strategy::hlt)
  //!    .lookahead_extent(options::lookahead_extent::partial)
  //!    .save(false);
  //! tc.number_of_classes()  // 10'752
  //! tc.complete();          // true
  //! tc.compatible();        // true
  //! todd_coxeter::number_of_idempotents(tc); // 1
  //! tc.standardize(order::recursive);
  //! std::vector<word_type>(tc.cbegin_normal_forms(),
  //!                        tc.cbegin_normal_forms() + 10);
  //! // {0_w,
  //! //  1_w,
  //! //  2_w,
  //! //  21_w,
  //! //  12_w,
  //! //  121_w,
  //! //  22_w,
  //! //  221_w,
  //! //  212_w,
  //! //  2121_w};
  //! tc.standardize(order::lex);
  //! std::vector<word_type>(tc.cbegin_normal_forms(),
  //!                        tc.cbegin_normal_forms() + 10);
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
  class ToddCoxeter : public CongruenceInterface,
                      public detail::FelschGraphSettings<ToddCoxeter> {
    using FelschGraphSettings_ = FelschGraphSettings<ToddCoxeter>;

   public:
    ////////////////////////////////////////////////////////////////////////
    // 0. ToddCoxeter - member types - public
    ////////////////////////////////////////////////////////////////////////

    //! The type of the nodes in the word graph.
    using node_type = typename WordGraph<uint32_t>::node_type;

    //! The type of the edge-labels in the word graph.
    using label_type = typename WordGraph<uint32_t>::label_type;

    ////////////////////////////////////////////////////////////////////////
    // Interface requirements - native-types
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) comment on what these are
    using native_letter_type       = letter_type;
    using native_word_type         = word_type;
    using native_presentation_type = Presentation<native_word_type>;

    //! \ingroup todd_coxeter_group
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
        //! divided by length_of_generating_pairs() nodes have been defined.
        //! These steps are repeated until the enumeration terminates.
        CR,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy R/C. The HLT
        //! strategy is run until the first lookahead is triggered (when
        //! number_of_cosets_active() is at least next_lookhead()). A full
        //! lookahead is then performed, and then the CR strategy is used.
        R_over_C,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Cr. The Felsch
        //! strategy is run until at least f_defs() new nodes have been
        //! defined, the HLT strategy is then run until at least hlt_defs()
        //! divided by length_of_generating_pairs() new nodes are defined,
        //! and then the Felsch strategy is run.
        Cr,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Rc. The HLT
        //! strategy is run until at least hlt_defs() divided by
        //! length_of_generating_pairs() new nodes have been
        //! defined, the Felsch strategy is then run until at least f_defs()
        //! new nodes are defined, and then the HLT strategy is run.
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
      //! ToddCoxeter. The values in this enum influence how these
      //! definitions are stored and processed.
      //!
      //! For every definition held in the definition stack, a depth first
      //! search through the Felsch tree of the generating pairs is
      //! performed. The aim is to only follow paths from nodes in the word
      //! graph labelled by generating pairs that actually pass through the
      //! edge described by a definition.
      //!
      // TODO(0) delete or use
      // There are two versions of this
      // represented by the values options::definitions::v1 and
      // options::definitions::v2. The first version is simpler, but may
      // involve following the same path that leads nowhere multiple times.
      // The second version is more complex, and attempts to avoid following
      // the same path multiple times if it is found to lead nowhere once.
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
    };

   private:
    ////////////////////////////////////////////////////////////////////////
    // 1. ToddCoxeter - nested classes - private
    ////////////////////////////////////////////////////////////////////////

    struct Settings;

    class SettingsGuard;
    friend class SettingsGuard;

    class Definitions {
      using Definition = std::pair<node_type, label_type>;

     private:
      bool                    _any_skipped;
      std::vector<Definition> _definitions;
      ToddCoxeter const*      _tc;

     public:
      Definitions() : _any_skipped(false), _definitions(), _tc(nullptr) {}
      // TODO(1) init()

      Definitions(Definitions const&)                 = default;
      Definitions(Definitions&&)                      = default;
      Definitions& operator=(Definitions const& that) = default;
      Definitions& operator=(Definitions&&)           = default;

      // TODO(1) corresponding constructor
      void init(ToddCoxeter const* tc) {
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

    // The following is a class for wrapping iterators. This is used by the
    // member functions that accept iterators (that point at possibly non-native
    // types) to convert the values pointed at into native types, and in the
    // class itow, to allow assignment of these values too.
    // CITOW = const_iterator_to_word
    template <typename Iterator>
    class citow {
     protected:
      Iterator           _it;
      ToddCoxeter const* _tc;

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = letter_type;
      using const_reference        = value_type;
      using const_pointer          = value_type const*;
      using pointer                = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      citow(ToddCoxeter const* tc, Iterator it) : _it(it), _tc(tc) {}

      reference operator*() const {
        return _tc->_input_presentation.index_no_checks(*_it);
      }

      // TODO operator-> ??

      bool operator==(citow<Iterator> that) const noexcept {
        return _it == that._it;
      }

      bool operator<=(citow<Iterator> that) const noexcept {
        return _it <= that._it;
      }

      bool operator>=(citow<Iterator> that) const noexcept {
        return _it >= that._it;
      }

      bool operator!=(citow<Iterator> that) const noexcept {
        return _it != that._it;
      }

      citow& operator++() {
        ++_it;
        return *this;
      }

      citow& operator--() {
        --_it;
        return *this;
      }

      [[nodiscard]] Iterator get() const noexcept {
        return _it;
      }

    };  // class citow

    // itow only differs from citow in the dereference member function
    // returning a (non-const) reference. A proxy is returned instead which
    // permits assignment to an output iterator.
    template <typename Iterator>
    class itow : public citow<Iterator> {
      // Proxy class for reference to the returned values
      class proxy_ref {
       private:
        Iterator           _it;
        ToddCoxeter const* _tc;

       public:
        // Constructor from ToddCoxeter and iterator
        // that byte
        proxy_ref(ToddCoxeter const* tc, Iterator it) noexcept
            : _it(it), _tc(tc) {}

        // Assignment operator to allow setting the value via the proxy
        Iterator operator=(letter_type i) noexcept {
          *_it = _tc->_input_presentation.letter_no_checks(i);
          return _it;
        }

        // Conversion operator to obtain the letter corresponding to the
        // letter_type
        [[nodiscard]] operator letter_type() const noexcept {
          return _tc->_input_presentation.index_no_checks(*_it);
        }
      };  // class proxy_ref

     public:
      using internal_iterator_type = Iterator;
      using value_type             = letter_type;
      using reference              = proxy_ref;
      using const_reference        = value_type;

      // TODO use proxy for pointers too?
      using const_pointer = value_type const*;
      using pointer       = value_type*;

      using size_type         = size_t;
      using difference_type   = std::ptrdiff_t;
      using iterator_category = std::bidirectional_iterator_tag;

      using citow<Iterator>::citow;

      reference operator*() {
        return reference(this->_tc, this->_it);
      }

    };  // class itow

    // Helpers for constructing citow + itow
    template <typename Iterator>
    citow<Iterator> make_citow(Iterator it) const {
      return citow<Iterator>(this, it);
    }

    template <typename Iterator>
    itow<Iterator> make_itow(Iterator it) const {
      return itow<Iterator>(this, it);
    }

    ////////////////////////////////////////////////////////////////////////
    // 2. ToddCoxeter - data members - private
    ////////////////////////////////////////////////////////////////////////

    Presentation<word_type>                _input_presentation;
    bool                                   _finished;
    Forest                                 _forest;
    std::vector<std::unique_ptr<Settings>> _setting_stack;
    Order                                  _standardized;
    Graph                                  _word_graph;

   public:
    //! The type of the underlying WordGraph.
    using word_graph_type = Graph;

    ////////////////////////////////////////////////////////////////////////
    // 3. ToddCoxeter - constructors + initializers - public
    ////////////////////////////////////////////////////////////////////////

    //! Default constructor.
    ToddCoxeter();

    //! \brief Re-initialize a ToddCoxeter instance.
    //!
    //! This function puts a ToddCoxeter instance back into the state that it
    //! would have been in if it had just been newly default constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    ToddCoxeter& init();

    //! Copy constructor.
    ToddCoxeter(ToddCoxeter const& that);

    //! Move constructor.
    ToddCoxeter(ToddCoxeter&&);

    //! Copy assignment operator.
    ToddCoxeter& operator=(ToddCoxeter const&);

    //! Move assignment operator.
    ToddCoxeter& operator=(ToddCoxeter&&);

    ~ToddCoxeter();

    //! \brief Construct from \ref congruence_kind and \ref
    //! Presentation<word_type>.
    //!
    //! This function constructs a ToddCoxeter instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! \param knd the kind (left, right, or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p is not valid.
    ToddCoxeter(congruence_kind knd, Presentation<word_type>&& p);

    //! \brief Re-initialize a ToddCoxeter instance.
    //!
    //! This function puts a ToddCoxeter instance back into the state that it
    //! would have been in if it had just been newly constructed from \p knd
    //! and \p p.
    //!
    //! \param knd the kind (left, right, or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type>&& p);

    //! \copydoc ToddCoxeter(congruence_kind, Presentation<word_type>&&)
    ToddCoxeter(congruence_kind knd, Presentation<word_type> const& p);

    //! \copydoc init(congruence_kind, Presentation<word_type>&&)
    ToddCoxeter& init(congruence_kind knd, Presentation<word_type> const& p);

    //! \brief Construct from \ref congruence_kind and \ref WordGraph.
    //!
    //! This function constructs a ToddCoxeter instance representing a
    //! congruence of kind \p knd over the WordGraph \p wg. The ToddCoxeter
    //! instance constructed in this way represents a quotient of the word
    //! graph
    //! \p wg. If \p wg happens to be the left or right Cayley graph of a
    //! semigroup or monoid, then the ToddCoxeter instance will represent a
    //! quotient of that semigroup.
    //!
    //! \tparam Node the type of the nodes in the 2nd argument.
    //!
    //! \param knd the kind (left, right, or twosided) of the congruence.
    //! \param wg the word graph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(0) a to_todd_coxeter variant that throws if wg is not valid
    // see below
    template <typename Node>
    ToddCoxeter(congruence_kind knd, WordGraph<Node> const& wg)
        : ToddCoxeter() {
      LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
      init(knd, wg);
    }

    //! \brief Re-initialize a ToddCoxeter instance.
    //!
    //! This function puts a ToddCoxeter instance back into the state that it
    //! would have been in if it had just been newly constructed from \p knd
    //! and \p wg.
    //!
    //! \tparam Node the type of the nodes in the 2nd argument.
    //!
    //! \param knd the kind (left, right, or twosided) of the congruence.
    //! \param wg the word graph.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(0) out of line
    // TODO(0) a to_todd_coxeter variant that throws if wg is not valid
    // i.e. any target is out of bounds
    template <typename Node>
    ToddCoxeter& init(congruence_kind knd, WordGraph<Node> const& wg) {
      LIBSEMIGROUPS_ASSERT(!_setting_stack.empty());
      CongruenceInterface::init(knd);
      _word_graph = wg;
      _word_graph.presentation().alphabet(wg.out_degree());
      copy_settings_into_graph();
      _input_presentation = _word_graph.presentation();
      return *this;
    }

    //! \brief Construct from \ref congruence_kind and \ref ToddCoxeter.
    //!
    //! This function constructs a ToddCoxeter instance representing a
    //! congruence of kind \p knd over the ToddCoxeter instance \p tc. The
    //! ToddCoxeter instance constructed in this way represents a quotient of
    //! the word graph represented by \p tc.
    //!
    //! \param knd the kind (left, right, or twosided) of the congruence.
    //! \param tc the ToddCoxeter instance.
    //!
    //! \throw LibsemigroupsException if the arguments \p knd and \p tc are
    //! not compatible. If the first item is `tc.kind()` and the second is the
    //! parameter \p knd, then compatible arguments are (right, right), (left,
    //! left), (two-sided, left), (two-sided, right), and (two-sided,
    //! two-sided).
    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc);

    // TODO(0) doc
    ToddCoxeter& init(congruence_kind knd, ToddCoxeter const& tc);

    // Used in Sims
    // TODO(0) could this and the next function be removed, and replaced with
    // something else?
    template <typename Node>
    ToddCoxeter(congruence_kind                knd,
                Presentation<word_type> const& p,
                WordGraph<Node> const&         wg) {
      init(knd, p, wg);
    }

    // TODO(0) a to_todd_coxeter variant that throws if p is not valid
    // TODO(0) out of line
    template <typename Node>
    ToddCoxeter& init(congruence_kind                knd,
                      Presentation<word_type> const& p,
                      WordGraph<Node> const&         wg) {
      init(knd, p);
      _word_graph = wg;
      _word_graph.presentation(p);  // TODO does this throw if p is invalid?
      copy_settings_into_graph();
      return *this;
    }

    // This is a constructor and not a helper so that everything that takes a
    // presentation has the same constructors, regardless of what they use
    // inside.
    // TODO(0) doc
    //! \throws LibsemigroupsException if \p p is not valid.
    template <typename Word>
    ToddCoxeter(congruence_kind knd, Presentation<Word> const& p)
        : ToddCoxeter(knd, to_presentation<word_type>(p)) {
      // to_presentation throws in the previous line if p isn't valid

      // TODO(0) we copy the input presentation twice here once in the
      // constructor above, and again in this function, better not duplicate
      // the next line looks weird but we are usually taking in char's and
      // returning letter_types
      _input_presentation
          = to_presentation<word_type>(p, [](auto const& x) { return x; });
    }

    //! \throws LibsemigroupsException if \p p is not valid.
    // TODO(0) doc
    template <typename Word>
    ToddCoxeter& init(congruence_kind knd, Presentation<Word> const& p) {
      // to_presentation throws if p isn't valid
      init(knd, to_presentation<word_type>(p));
      // TODO(0) we copy the input presentation twice here once in the
      // constructor above, and again in this function, better not duplicate
      // the next line looks weird but we are usually taking in char's and
      // returning letter_types
      _input_presentation
          = to_presentation<word_type>(p, [](auto const& x) { return x; });
      return *this;
    }

    // TODO(0) doc
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_out_of_bounds(Iterator1 first, Iterator2 last) const {
      presentation().validate_word(first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // 4. ToddCoxeter - interface requirements - add_pair
    ////////////////////////////////////////////////////////////////////////

    // using CongruenceInterface::add_pair_no_checks; TODO use or rm

   private:
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_pair_no_checks(citow<Iterator1> first1,
                                    citow<Iterator2> last1,
                                    citow<Iterator3> first2,
                                    citow<Iterator4> last2) {
      if (kind() == congruence_kind::left) {
        add_pair_no_checks_no_reverse(std::make_reverse_iterator(last1),
                                      std::make_reverse_iterator(first1),
                                      std::make_reverse_iterator(last2),
                                      std::make_reverse_iterator(first2));
      } else {
        add_pair_no_checks_no_reverse(first1, last1, first2, last2);
      }
      return *this;
    }

   public:
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_pair_no_checks(Iterator1 first1,
                                    Iterator2 last1,
                                    Iterator3 first2,
                                    Iterator4 last2) {
      return add_pair_no_checks(make_citow(first1),
                                make_citow(last1),
                                make_citow(first2),
                                make_citow(last2));
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_pair(Iterator1 first1,
                          Iterator2 last1,
                          Iterator3 first2,
                          Iterator4 last2) {
      CongruenceInterface::add_pair<ToddCoxeter>(first1, last1, first2, last2);
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // 5. ToddCoxeter - interface requirements - number_of_classes
    ////////////////////////////////////////////////////////////////////////

    //! Compute the number of classes in the congruence.
    //!
    //! \returns The number of congruences classes of \c this if this number
    //! is finite, or \ref POSITIVE_INFINITY in some cases if \c this
    //! number is not finite.
    //!
    //! \throws std::bad_alloc if the (possibly infinite) computation uses all
    //! the available memory.
    //!
    //! \complexity
    //! See warning.
    //!
    //! \warning The problem of determining the number of classes of a
    //! congruence over a finitely presented semigroup is undecidable in
    //! general, and this function may never terminate.
    [[nodiscard]] uint64_t number_of_classes();

    ////////////////////////////////////////////////////////////////////////
    // 6. ToddCoxeter - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

   private:
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(citow<Iterator1> first1,
                                      citow<Iterator2> last1,
                                      citow<Iterator3> first2,
                                      citow<Iterator4> last2) const {
      if (std::equal(first1, last1, first2, last2)) {
        return tril::TRUE;
      }
      auto i1 = current_index_of_no_checks(first1, last1);
      auto i2 = current_index_of_no_checks(first2, last2);
      if (i1 == UNDEFINED || i2 == UNDEFINED) {
        return tril::unknown;
      } else if (i1 == i2) {
        return tril::TRUE;
      } else if (finished()) {
        return tril::FALSE;
      } else {
        return tril::unknown;
      }
    }

   public:
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(Iterator1 first1,
                                      Iterator2 last1,
                                      Iterator3 first2,
                                      Iterator4 last2) const {
      return currently_contains_no_checks(make_citow(first1),
                                          make_citow(last1),
                                          make_citow(first2),
                                          make_citow(last2));
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains_no_checks(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) {
      if (native_presentation().rules.empty() && generating_pairs().empty()
          && current_word_graph().number_of_nodes_active() == 1) {
        return std::equal(first1, last1, first2, last2);
      }
      return currently_contains_no_checks(first1, last1, first2, last2)
             == tril::TRUE;
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return currently_contains_no_checks(first1, last1, first2, last2);
    }

    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2) {
      throw_if_letter_out_of_bounds(first1, last1);
      throw_if_letter_out_of_bounds(first2, last2);
      return contains_no_checks(first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // 7. ToddCoxeter - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) {
      return current_word_of_no_checks(d_first,
                                       current_index_of_no_checks(first, last));
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) {
      return current_word_of(d_first, current_index_of(first, last));
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return word_of_no_checks(d_first, index_of_no_checks(first, last));
    }

    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      return word_of(d_first, index_of(first, last));
    }

    ////////////////////////////////////////////////////////////////////////
    // 8. ToddCoxeter - settings - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename T>
    void report_every(T val) {
      CongruenceInterface::report_every(val);
      _word_graph.report_every(val);
    }

    using FelschGraphSettings_::def_version;
    using FelschGraphSettings_::settings;
    using Reporter::report_every;

    //! The maximum number of definitions in the stack.
    //!
    //! This setting specifies the maximum number of definitions that can be
    //! in the stack at any given time. What happens if there are the maximum
    //! number of definitions in the stack and a new definition is generated
    //! is governed by definition_policy().
    //!
    //! The default value of this setting is \c 2'000.
    //!
    //! \param val the maximum size of the definition stack.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& def_max(size_t val) noexcept;

    //! The current value of the setting for the maximum number of
    //! definitions.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t def_max() const noexcept;

    //! Specify how to handle definitions.
    //!
    //! This function can be used to specify how to handle definitions. For
    //! details see options::definitions.
    //!
    //! The default value of this setting is
    //! ``options::definitions::no_stack_if_no_space |
    //! options::definitions::v2``.
    //!
    //! \param val the policy to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is not valid (i.e. if for
    //! example ``options::definitions::v1 & options::definitions::v2``
    //! returns
    //! ``true``).
    ToddCoxeter& def_policy(options::def_policy val);

    //! The current value of the definition policy setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``options::definitions``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::def_policy def_policy() const noexcept;

    //! The approx number of Felsch style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! If the strategy being used is any of those mimicking
    //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
    //! setting is used to determine the number of nodes defined in any Felsch
    //! phase of the strategy.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is \c 0.
    ToddCoxeter& f_defs(size_t val);

    //! The current value of the f_defs setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t f_defs() const noexcept;

    //! The approx number of HLT style definitions in
    //! [ACE](https://staff.itee.uq.edu.au/havas/) strategies.
    //!
    //! If the strategy being used is any of those mimicking
    //! [ACE](https://staff.itee.uq.edu.au/havas/), then the value of this
    //! setting is used to determine the number of nodes defined in any HLT
    //! phase of the strategy.
    //!
    //! The default value of this setting is \c 200'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than
    //! length_of_generating_pairs().
    ToddCoxeter& hlt_defs(size_t val);

    //! The current value of the hlt_defs setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t hlt_defs() const noexcept;

    //! Specify what should be considered a large collapse.
    //!
    //! By default when processing coincidences nodes are merged in the word
    //! graph one pair at a time, and the in-neighbours of the surviving node
    //! are updated at the same time. If the number of coincidences is
    //! large, then it might be that a pair of nodes are merged at one step,
    //! then the surviving node is merged with another node at a future step,
    //! and this may happen many many times. This results in the
    //! in-neighbours of the surviving nodes being repeatedly traversed,
    //! which can result in a significant performance penalty.
    //! It can be beneficial to stop updating the in-neighbours as nodes are
    //! merged, and to just rebuild the entire in-neighbours data structure
    //! by traversing the entire word graph after all coincidences have been
    //! processed. This is beneficial if the number of surviving nodes is
    //! relatively small in comparison to the number of nodes merged.
    //! The purpose of this setting is to specify what should be
    //! considered a \"large\" collapse, or more precisely, what number of
    //! coincidences in the stack will trigger a change from updating the
    //! in-neighbours one-by-one to traversing the entire graph once after
    //! all coincidences have been processed.
    //!
    //! The default value of this setting is \c 100'000.
    //!
    //! \param val the value to use.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& large_collapse(size_t val) noexcept;

    //! The current value of the large collapse setting.
    //!
    //! \returns The current value of the setting, a value of type
    //! ``size_t``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] size_t large_collapse() const noexcept;

    // TODO(0) doc
    ToddCoxeter& lookahead_extent(options::lookahead_extent val) noexcept;

    // TODO(0) doc
    [[nodiscard]] options::lookahead_extent lookahead_extent() const noexcept;

    //! Set the lookahead growth factor.
    //!
    //! This setting determines by what factor the number of nodes required
    //! to trigger a lookahead grows. More specifically, at the end of any
    //! lookahead if the number of active nodes already exceeds the value of
    //! lookahead_next() or the number of nodes killed during the lookahead
    //! is less than the number of active nodes divided by
    //! lookahead_growth_threshold(), then the value of
    //! ToddCoxeter::next_lookhead is increased by a multiple of the \p value.
    //!
    //! \param val the value indicating the lookahead growth factor.
    //! The default value is ``2.0``.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p val is less than ``1.0``.
    ToddCoxeter& lookahead_growth_factor(float val);

    //! The current value of the lookahead growth factor.
    //!
    //! \returns A value of type `float`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_factor(float)
    [[nodiscard]] float lookahead_growth_factor() const noexcept;

    //! Set the lookahead growth threshold.
    //!
    //! This setting determines the threshold for the number of nodes required
    //! to trigger a lookahead. More specifically, at the end of any lookahead
    //! if the number of active nodes already exceeds the value of
    //! lookahead_next() or the number of nodes killed during the lookahead is
    //! less than the number of active nodes divided by \ref
    //! lookahead_growth_threshold, then the value of
    //! ToddCoxeter::next_lookhead() is increased.
    //!
    //! The default value is ``4``.
    //!
    //! \param val the value indicating the lookahead growth threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_growth_threshold(size_t val) noexcept;

    //! The current value of the lookahead growth threshold.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_growth_threshold()
    [[nodiscard]] size_t lookahead_growth_threshold() const noexcept;

    //! Set the minimum value of lookahead_next().
    //!
    //! After a lookahead is performed the value of lookahead_next() is
    //! modified depending on the outcome of the current lookahead. If the
    //! return value of lookahead_next() is too small or too large, then the
    //! value is adjusted according to lookahead_growth_factor() and
    //! lookahead_growth_threshold(). This setting specified the minimum
    //! possible value for lookahead_next().
    //!
    //! The default value is \c 10'000.
    //!
    //! \param val value indicating the minimum value of lookahead_next().
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_min(size_t val) noexcept;

    //! The current value of the minimum lookahead setting.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_min(size_t)
    [[nodiscard]] size_t lookahead_min() const noexcept;

    //! Set the threshold that will trigger a lookahead in HLT.
    //!
    //! If the number of cosets active exceeds the value set by this
    //! function, then a lookahead, of the type set using the function
    //! \ref lookahead, is triggered. This only applies when using
    //! the HLT strategy.
    //!
    //! The default value is 5 million.
    //!
    //! \param val value indicating the initial threshold.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_next(size_t val) noexcept;

    //! The current value of the next lookahead setting.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lookahead_next(size_t)
    [[nodiscard]] size_t lookahead_next() const noexcept;

    // Throws if val >= 1 or < 0
    ToddCoxeter& lookahead_stop_early_ratio(float val);
    float        lookahead_stop_early_ratio() const noexcept;

    ToddCoxeter&
    lookahead_stop_early_interval(std::chrono::nanoseconds val) noexcept;
    std::chrono::nanoseconds lookahead_stop_early_interval() const noexcept;

    //! Set the style of lookahead to use in HLT.
    //!
    //! If the strategy is not HLT, then the value of this setting is
    //! ignored.
    //!
    //! The default value is options::lookahead::partial |
    //! options::lookahead::hlt. The other
    //! possible value are documented in options::lookahead.
    //!
    //! \param val value indicating whether to perform a full or partial
    //! lookahead in HLT or Felsch style.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lookahead_style(options::lookahead_style val) noexcept;

    //! The current value of the setting for lookaheads.
    //!
    //! \returns A value of type options::lookahead.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::lookahead_style lookahead_style() const noexcept;

    //! Specify minimum number of classes that may trigger early stop.
    //!
    //! Set a lower bound for the number of classes of the congruence
    //! represented by a ToddCoxeter instance. If the number of active cosets
    //! becomes at least the value of the argument, and the table is complete
    //! (\ref complete returns \c true), then the enumeration is terminated.
    //! When the given bound is equal to the number of classes, this may save
    //! tracing relations at many cosets when there is no possibility of
    //! finding coincidences.
    //!
    //! The default value is \ref UNDEFINED.
    //!
    //! \param val value indicating the lower bound.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& lower_bound(size_t val) noexcept;

    //! The current value of the lower bound setting.
    //!
    //! \returns A value of type `size_t`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa lower_bound(size_t)
    [[nodiscard]] size_t lower_bound() const noexcept;

    //! Process deductions during HLT.
    //!
    //! If the argument of this function is \c true and the HLT strategy is
    //! being used, then deductions are processed during the enumeration.
    //!
    //! The default value is \c false.
    //!
    //! \param val value indicating whether or not to process deductions.
    //!
    //! \returns A reference to `*this`.
    ToddCoxeter& save(bool val);

    //! The current value of save setting.
    //!
    //! \returns A value of type `bool`.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \sa save(bool)
    [[nodiscard]] bool save() const noexcept;

    //! Specify the strategy.
    //!
    //! The strategy used during the enumeration can be specified using
    //! this function.
    //!
    //! The default value is options::strategy::hlt.
    //!
    //! \param val value indicating which strategy to use
    //!
    //! \returns A reference to `*this`.
    ToddCoxeter& strategy(options::strategy val);

    //! The current strategy for enumeration.
    //!
    //! \returns The current strategy, a value of type options::strategy.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] options::strategy strategy() const noexcept;

    //! Perform an HLT-style push of the defining relations at the identity.
    //!
    //! If a ToddCoxeter instance is defined over a finitely presented
    //! semigroup and the Felsch strategy is being used, it can be useful
    //! to follow all the paths from the identity labelled by the underlying
    //! relations of the semigroup (if any). The setting specifies whether or
    //! not to do this.
    //!
    //! The default value of this setting is \c false.
    //!
    //! \param val the boolean value.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \noexcept
    ToddCoxeter& use_relations_in_extra(bool val) noexcept;

    //! The current value of the setting for using relations.
    //!
    //! \returns The current value of the setting, a value of type ``bool``.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool use_relations_in_extra() const noexcept;

    ////////////////////////////////////////////////////////////////////////
    // 9. ToddCoxeter - accessors - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    Presentation<word_type> const& native_presentation() const noexcept {
      return _word_graph.presentation();
    }

    // TODO(0) doc
    Presentation<word_type> const& presentation() const noexcept {
      return _input_presentation;
    }

    // TODO(0) doc
    word_graph_type const& current_word_graph() const noexcept {
      return _word_graph;
    }

    // TODO(0) to cpp
    word_graph_type const& word_graph() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      shrink_to_fit();
      return current_word_graph();
    }

    // TODO(0) doc
    Forest const& current_spanning_tree() const noexcept {
      return _forest;
    }

    // TODO(0) to cpp
    Forest const& spanning_tree() {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      shrink_to_fit();
      return current_spanning_tree();
    }

    // TODO(0) doc
    inline Order standardization_order() const noexcept {
      return _standardized;
    }

    // TODO(0) doc
    bool is_standardized(Order val) const;

    // TODO(0) doc
    bool is_standardized() const;

    ////////////////////////////////////////////////////////////////////////
    // 10. ToddCoxeter - modifiers - public
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    void shrink_to_fit();

    // Returns true if anything changed
    // TODO:(0) The documentation for Order used to contain a description of
    // what each order means with respect to this function. However, the Order
    // enum is now used in other places, so those descriptions should be
    // written here instead. That documentation is placed here for reference:
    // No standardization has been done.
    // none = 0,
    // Normal forms are the short-lex least word belonging to a given
    // congruence class.
    // shortlex,
    // The congruence classes are ordered lexicographically by their
    // normal form. The normal forms themselves are essentially arbitrary
    // because there is not necessarily a lexicographically least word in
    // every class.
    // lex,
    // Normal forms are the recursive-path least word belonging to a given
    // congruence class.
    // recursive
    bool standardize(Order val);

    // stop_early indicates that if too few nodes are killed in 1 second, then
    // the lookahead aborts, this should not happen if we are doing a final
    // lookahead because we skipped some deductions
    // TODO(0) doc
    void perform_lookahead(bool stop_early);

    ////////////////////////////////////////////////////////////////////////
    // 11. ToddCoxeter - word -> index
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    // NOTE THAT: the graph contains one more node than there are element if
    // the underlying presentation does not contain the empty word
   private:
    template <typename Iterator1, typename Iterator2>
    node_type current_index_of_no_checks(citow<Iterator1> first,
                                         citow<Iterator2> last) const {
      node_type c = current_word_graph().initial_node();

      if (kind() != congruence_kind::left) {
        c = word_graph::follow_path_no_checks(
            current_word_graph(), c, first, last);
      } else {
        c = word_graph::follow_path_no_checks(
            current_word_graph(),
            c,
            std::make_reverse_iterator(last),
            std::make_reverse_iterator(first));
      }
      size_t const offset
          = (native_presentation().contains_empty_word() ? 0 : 1);
      return (c == UNDEFINED ? UNDEFINED : static_cast<node_type>(c - offset));
    }

   public:
    template <typename Iterator1, typename Iterator2>
    node_type current_index_of_no_checks(Iterator1 first,
                                         Iterator2 last) const {
      return current_index_of_no_checks(make_citow(first), make_citow(last));
    }

    template <typename Iterator1, typename Iterator2>
    node_type current_index_of(Iterator1 first, Iterator2 last) const {
      throw_if_letter_out_of_bounds(first, last);
      return current_index_of_no_checks(first, last);
    }

    template <typename Iterator1, typename Iterator2>
    node_type index_of_no_checks(Iterator1 first, Iterator2 last) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      if (!is_standardized()) {
        standardize(Order::shortlex);
      }
      // c is in the range 1, ..., number_of_cosets_active() because 0
      // represents the identity coset, and does not correspond to an element,
      // unless native_presentation().contains_empty_word()
      return current_index_of_no_checks(first, last);
    }

    template <typename Iterator1, typename Iterator2>
    node_type index_of(Iterator1 first, Iterator2 last) {
      throw_if_letter_out_of_bounds(first, last);
      return index_of_no_checks(first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // 12. ToddCoxeter - index -> word
    ////////////////////////////////////////////////////////////////////////

   private:
    // TODO(2) maybe this isn't great, because we always wrap the incoming
    // iterators, even the _input_presentation, and the native_presentation are
    // identical, and the wrapping isn't necessary.
    template <typename OutputIterator>
    itow<OutputIterator> current_word_of_no_checks(itow<OutputIterator> d_first,
                                                   node_type            i) {
      if (!is_standardized()) {
        standardize(Order::shortlex);
      }
      if (!native_presentation().contains_empty_word()) {
        ++i;
      }

      return _forest.path_to_root_no_checks(d_first, i);
      // TODO(0) maybe withdraw left congs altogether?
      // This doesn't compile with the call to std::reverse below
      // if (kind() != congruence_kind::left) {
      //   std::reverse(d_first, d_last);
      // }
    }

   public:
    template <typename OutputIterator>
    OutputIterator current_word_of_no_checks(OutputIterator d_first,
                                             node_type      i) {
      return current_word_of_no_checks(make_itow(d_first), i).get();
    }

    template <typename OutputIterator>
    OutputIterator current_word_of(OutputIterator d_first, node_type i) {
      size_t const offset
          = (native_presentation().contains_empty_word() ? 0 : 1);
      if (i >= _word_graph.number_of_nodes_active() - offset) {
        LIBSEMIGROUPS_EXCEPTION("invalid class index, expected a value in "
                                "the range [0, {}), found {}",
                                _word_graph.number_of_nodes_active() - offset,
                                i);
      }
      return current_word_of_no_checks(d_first, i);
    }

    // Note that the output of this needs to be reversed if and only if kind()
    // != left.
    template <typename Iterator>
    Iterator word_of_no_checks(Iterator d_first, node_type i) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return current_word_of_no_checks(d_first, i);
    }

    template <typename Iterator>
    Iterator word_of(Iterator d_first, node_type i) {
      run();
      LIBSEMIGROUPS_ASSERT(finished());
      return current_word_of(d_first, i);
    }

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
    // 14. ToddCoxeter - member functions - private
    ////////////////////////////////////////////////////////////////////////

    void copy_settings_into_graph();

    // This function has prefix tc_ because there's already a settings function
    // in a base class
    Settings&       tc_settings();
    Settings const& tc_settings() const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - main strategies - private
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
    // ToddCoxeter - reporting - private
    ////////////////////////////////////////////////////////////////////////

    void report_next_lookahead(size_t old_value) const;
    void report_nodes_killed(int64_t number) const;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - lookahead - private
    ////////////////////////////////////////////////////////////////////////

    static constexpr bool StopEarly      = true;
    static constexpr bool DoNotStopEarly = false;

    size_t hlt_lookahead(bool stop_early);
    size_t felsch_lookahead();
  };

  ////////////////////////////////////////////////////////////////////////
  // ToddCoxeter helpers
  ////////////////////////////////////////////////////////////////////////

  namespace todd_coxeter {
    using node_type = typename ToddCoxeter::node_type;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter add_generating_pairs helpers
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::add_pair;
    using congruence_interface::add_pair_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter specific helpers - word -> index
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Word>
    node_type current_index_of_no_checks(ToddCoxeter const& tc, Word const& w) {
      return tc.current_index_of_no_checks(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Word>
    node_type current_index_of(ToddCoxeter const& tc, Word const& w) {
      return tc.current_index_of(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Word>
    node_type index_of_no_checks(ToddCoxeter& tc, Word const& w) {
      return tc.index_of_no_checks(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Word>
    node_type index_of(ToddCoxeter& tc, Word const& w) {
      return tc.index_of(std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Int = size_t>
    node_type current_index_of_no_checks(ToddCoxeter&                      tc,
                                         std::initializer_list<Int> const& w) {
      return index_of<std::initializer_list<Int>>(tc, w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    node_type current_index_of(ToddCoxeter&                      tc,
                               std::initializer_list<Int> const& w) {
      return current_index_of<std::initializer_list<Int>>(tc, w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    node_type index_of_no_checks(ToddCoxeter&                      tc,
                                 std::initializer_list<Int> const& w) {
      return index_of<std::initializer_list<Int>>(tc, w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    node_type index_of(ToddCoxeter& tc, std::initializer_list<Int> const& w) {
      return index_of<std::initializer_list<Int>>(tc, w);
    }

    // TODO(0) versions of these for char const*

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter specific helpers - index -> word
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename Word = word_type>
    Word word_of_no_checks(ToddCoxeter& tc, node_type i) {
      Word result;
      tc.word_of_no_checks(std::back_inserter(result), i);
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) doc
    template <typename Word = word_type>
    Word word_of(ToddCoxeter& tc, node_type i) {
      Word result;
      tc.word_of(std::back_inserter(result), i);
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) 2x current_word_of

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - contains
    ////////////////////////////////////////////////////////////////////////

    using congruence_interface::contains;
    using congruence_interface::contains_no_checks;
    using congruence_interface::currently_contains;
    using congruence_interface::currently_contains_no_checks;

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - reduce
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    template <typename InputWord, typename OutputWord = InputWord>
    OutputWord reduce_no_run_no_checks(ToddCoxeter& tc, InputWord const& w) {
      OutputWord result;
      tc.reduce_no_run_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) doc
    template <typename InputWord, typename OutputWord = InputWord>
    OutputWord reduce_no_run(ToddCoxeter& tc, InputWord const& w) {
      OutputWord result;
      tc.reduce_no_run(std::back_inserter(result), std::begin(w), std::end(w));
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) doc
    template <typename InputWord, typename OutputWord = InputWord>
    OutputWord reduce_no_checks(ToddCoxeter& tc, InputWord const& w) {
      OutputWord result;
      tc.reduce_no_checks(
          std::back_inserter(result), std::begin(w), std::end(w));
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) doc
    template <typename InputWord, typename OutputWord = InputWord>
    OutputWord reduce(ToddCoxeter& tc, InputWord const& w) {
      OutputWord result;
      tc.reduce(std::back_inserter(result), std::begin(w), std::end(w));
      if (tc.kind() != congruence_kind::left) {
        std::reverse(std::begin(result), std::end(result));
      }
      return result;
    }

    // TODO(0) doc
    template <typename Int = size_t>
    auto reduce_no_run_no_checks(ToddCoxeter&                      tc,
                                 std::initializer_list<Int> const& w) {
      return reduce_no_run_no_checks<std::initializer_list<Int>,
                                     std::vector<Int>>(tc, w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    auto reduce_no_run(ToddCoxeter& tc, std::initializer_list<Int> const& w) {
      return reduce_no_run<std::initializer_list<Int>, std::vector<Int>>(tc, w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    auto reduce_no_checks(ToddCoxeter&                      tc,
                          std::initializer_list<Int> const& w) {
      return reduce_no_checks<std::initializer_list<Int>, std::vector<Int>>(tc,
                                                                            w);
    }

    // TODO(0) doc
    template <typename Int = size_t>
    auto reduce(ToddCoxeter& tc, std::initializer_list<Int> const& w) {
      return reduce<std::initializer_list<Int>, std::vector<Int>>(tc, w);
    }

    inline auto reduce(ToddCoxeter& tc, char const* w) {
      // TODO is this consistent?  Should the return type be word_type?
      return reduce<std::string, std::string>(tc, w);
    }

    // TODO(0) the other 3 reduce functions for char const*

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - class_of
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    inline auto class_of(ToddCoxeter& tc, node_type n) {
      size_t const offset = (tc.presentation().contains_empty_word() ? 0 : 1);
      tc.run();
      // We call run and then current_word_graph, because the word
      // graph does not need to be standardized for this to work.
      return Paths(tc.current_word_graph()).source(0).target(n + offset);
    }

    // TODO(0) doc
    inline auto class_of_no_checks(ToddCoxeter& tc, node_type n) {
      size_t const offset = (tc.presentation().contains_empty_word() ? 0 : 1);
      tc.run();
      // We call run and then current_word_graph, because the word
      // graph does not need to be standardized for this to work.
      return Paths(tc.current_word_graph())
          .source_no_checks(0)
          .target_no_checks(n + offset);
    }

    // TODO(0) doc
    template <typename Iterator1, typename Iterator2>
    auto class_of(ToddCoxeter& tc, Iterator1 first, Iterator2 last) {
      return class_of(tc, tc.index_of(first, last));
    }

    // TODO(0) doc
    template <typename Iterator1, typename Iterator2>
    auto class_of_no_checks(ToddCoxeter& tc, Iterator1 first, Iterator2 last) {
      return class_of_no_checks(tc, tc.index_of_no_checks(first, last));
    }

    // TODO(0) doc
    template <typename Word,
              typename = std::enable_if_t<!std::is_integral_v<Word>>>
    inline auto class_of(ToddCoxeter& tc, Word const& w) {
      return class_of(tc, std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Word,
              typename = std::enable_if_t<!std::is_integral_v<Word>>>
    inline auto class_of_no_checks(ToddCoxeter& tc, Word const& w) {
      return class_of_no_checks(tc, std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Int = size_t>
    inline auto class_of(ToddCoxeter& tc, std::initializer_list<Int> const& w) {
      return class_of(tc, std::begin(w), std::end(w));
    }

    // TODO(0) doc
    template <typename Int = size_t>
    inline auto class_of_no_checks(ToddCoxeter&                      tc,
                                   std::initializer_list<Int> const& w) {
      return class_of_no_checks(tc, std::begin(w), std::end(w));
    }

    // TODO(0) class_of (ToddCoxeter, char const*)
    // TODO(0) class_of_no_checks (ToddCoxeter, char const*)

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - normal_forms
    ////////////////////////////////////////////////////////////////////////

    //! Returns a \ref normal_form_iterator pointing at the first normal
    //! form.
    //!
    //! Returns a const iterator pointing to the normal form of the first
    //! class of the congruence represented by an instance of ToddCoxeter.
    //! The order of the classes, and the normal form, that is returned are
    //! controlled by standardize(Order).
    //!
    //! \returns A value of type \ref normal_form_iterator.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    // TODO(0): redo the doc
    // TODO(0) note in the doc that the output is always word_type, and to use
    // transform if that's not what's wanted; or add some sort of template here
    // to specify the output type
    inline auto normal_forms(ToddCoxeter& tc) {
      // TODO(1) avoid allocations here.
      // To do this we'll have to make a custom range object that stores a
      // word_type _word that gets updated by calls to
      // tc.word_of_no_checks(std::back_inserter(_word), i);
      return rx::seq() | rx::take(tc.number_of_classes())
             | rx::transform(
                 [&tc](auto i) { return word_of_no_checks(tc, i); });
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - non_trivial_classes
    ////////////////////////////////////////////////////////////////////////

    // TODO(0) doc
    // TODO(0) template <word_type>
    // TODO(0) remove?
    // TODO(0) update as we did for partition below, or remove it
    std::vector<std::vector<word_type>> non_trivial_classes(ToddCoxeter& tc1,
                                                            ToddCoxeter& tc2);

    // TODO(0) doc
    // TODO(0) remove?
    uint64_t number_of_idempotents(ToddCoxeter& tc);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - first_equivalent_pair
    ////////////////////////////////////////////////////////////////////////
    // TODO(0) doc
    // TODO(1) range version
    template <typename Iterator>
    std::pair<Iterator, Iterator> first_equivalent_pair(ToddCoxeter& tc,
                                                        Iterator     first,
                                                        Iterator     last) {
      std::unordered_map<ToddCoxeter::node_type, Iterator> map;
      size_t                                               index = 0;
      for (auto it = first; it != last; ++it, ++index) {
        auto [map_it, inserted]
            = map.emplace(todd_coxeter::index_of(tc, *it), it);
        if (!inserted) {
          return std::pair(map_it->second, it);
        }
      }
      return std::pair(last, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - is_traversal
    ////////////////////////////////////////////////////////////////////////
    // TODO(0) doc
    template <typename Iterator>
    bool is_traversal(ToddCoxeter& tc, Iterator first, Iterator last) {
      return first_equivalent_pair(tc, first, last) == std::pair(last, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - is_non_trivial
    ////////////////////////////////////////////////////////////////////////

    //! Check if the congruence has more than one class.
    //!
    //! Returns tril::TRUE if it is possible to show that the congruence is
    //! non-trivial; tril::FALSE if the congruence is already known to be
    //! trivial; and tril::unknown if it is not possible to show that the
    //! congruence is non-trivial.
    //!
    //! This function attempts to find a non-trivial congruence containing
    //! the congruence represented by a ToddCoxeter instance by repeating the
    //! following steps on a copy until the enumeration concludes:
    //! 1. running the enumeration for the specified amount of time
    //! 2. repeatedly choosing a random pair of cosets and identifying them,
    //!    until the number of cosets left in the quotient is smaller than
    //!    \p threshold times the initial number of cosets for this step.
    //! If at the end of this process, the ToddCoxeter instance is
    //! non-trivial, then the original ToddCoxeter is also non-trivial.
    //! Otherwise, the entire process is repeated again up to a total of \p
    //! tries times.
    //!
    //! \param tries the number of attempts to find non-trivial
    //! super-congruence.
    //! \param try_for the amount of time in millisecond to enumerate the
    //! congruence after choosing a random pair of representatives and
    //! identifying them.
    //! \param threshold the threshold (see description).
    //!
    //! \returns A value of type \ref tril
    // TODO(0) redo the doc
    tril is_non_trivial(ToddCoxeter&              tc,
                        size_t                    tries = 10,
                        std::chrono::milliseconds try_for
                        = std::chrono::milliseconds(100),
                        float threshold = 0.99);

    ////////////////////////////////////////////////////////////////////////
    // Interface helpers - redundant_rule
    ////////////////////////////////////////////////////////////////////////
    // FIXME run_for seems to not function properly here.
    // TODO(0) doc
    // TODO(0) out of line this
    template <typename Word, typename Time>
    [[nodiscard]] auto redundant_rule(Presentation<Word> const& p, Time t) {
      constexpr static congruence_kind twosided = congruence_kind::twosided;

      p.validate();
      Presentation<Word> q;
      q.alphabet(p.alphabet());
      q.contains_empty_word(p.contains_empty_word());
      ToddCoxeter tc;
      ToWord      to_word(p.alphabet());

      for (auto omit = p.rules.crbegin(); omit != p.rules.crend(); omit += 2) {
        q.rules.clear();
        q.rules.insert(q.rules.end(), p.rules.crbegin(), omit);
        q.rules.insert(q.rules.end(), omit + 2, p.rules.crend());
        tc.init(twosided, q);
        tc.run_for(t);
        if (todd_coxeter::index_of(tc, to_word(*omit))
            == todd_coxeter::index_of(tc, to_word(*(omit + 1)))) {
          return (omit + 1).base() - 1;
        }
      }
      return p.rules.cend();
    }

  }  // namespace todd_coxeter

  // TODO move into todd_coxeter namespace
  // TODO(0) doc
  // TODO(0) out of line
  template <typename Range,
            typename = std::enable_if_t<rx::is_input_or_sink_v<Range>>>
  std::vector<std::vector<std::decay_t<typename Range::output_type>>>
  partition(ToddCoxeter& tc, Range r) {
    using return_type
        = std::vector<std::vector<std::decay_t<typename Range::output_type>>>;

    if (tc.number_of_classes() == POSITIVE_INFINITY) {
      LIBSEMIGROUPS_EXCEPTION(
          "the 1st argument defines a congruence with infinitely many "
          "classes, the non-trivial classes cannot be determined!");
      // They really can't be determined because we cannot run ToddCoxeter at
      // all
    } else if (!r.is_finite) {
      LIBSEMIGROUPS_EXCEPTION("the 2nd argument (a range) must be finite, "
                              "found an infinite range");
    }
    return_type         result;
    std::vector<size_t> lookup;
    size_t              next_index = 0;

    while (!r.at_end()) {
      auto       next  = r.get();
      auto const index = todd_coxeter::index_of(tc, next);
      if (index >= lookup.size()) {
        lookup.resize(index + 1, UNDEFINED);
      }
      if (lookup[index] == UNDEFINED) {
        lookup[index] = next_index++;
        result.emplace_back();
      }
      result[lookup[index]].push_back(std::move(next));
      r.next();
    }
    return result;
  }

  // TODO(0) to_human_readable_repr

}  // namespace libsemigroups
#endif  // LIBSEMIGROUPS_TODD_COXETER_HPP_
