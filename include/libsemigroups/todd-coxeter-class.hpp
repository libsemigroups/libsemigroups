//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2025 James D. Mitchell
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

// This file contains the declaration of the ToddCoxeter class. This class
// exists mostly to wrap ToddCoxeterImpl (where the action happens) to make it
// more user friendly. In particular, ToddCoxeter presentation()
// and generating_pairs() return the input presentation and generating pairs,
// whatever their type, and not the normalized Presentation<word_type> required
// by ToddCoxeterImpl.

#ifndef LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_
#define LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_

#include <algorithm>    // for equal
#include <cstddef>      // for ptrdiff_t, size_t
#include <iterator>     // for bidirectional_iterator_tag
#include <string>       // for basic_string, string
#include <type_traits>  // for is_same_v
#include <utility>      // for move
#include <vector>       // for vector

#include "presentation.hpp"  // for Presentation
#include "types.hpp"         // for congruence_kind

#include "detail/citow.hpp"              // for detail::citow and detail::itow
#include "detail/cong-common-class.hpp"  // for detail::CongruenceCommon
#include "detail/fmt.hpp"                // for fmt
#include "detail/todd-coxeter-impl.hpp"  // for ToddCoxeterImpl

namespace libsemigroups {

  //! \defgroup todd_coxeter_group Todd-Coxeter
  //!
  //! This page contains documentation related to the implementation of the
  //! Todd-Coxeter algorithm \cite Coleman2022aa in `libsemigroups`.
  //!
  //! The purpose of this algorithm is to find the WordGraph of the action of
  //! a semigroup or monoid on the classes of a 1-sided (right), or 2-sided
  //! congruence; see \cite Coleman2022aa for more details.

  //! \defgroup todd_coxeter_class_group ToddCoxeter class
  //! \ingroup todd_coxeter_group
  //!
  //! \brief Class containing an implementation of the Todd-Coxeter Algorithm.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! This class contains an implementation of the Todd-Coxeter
  //! algorithm for computing 1-sided (right), and 2-sided congruences on
  //! a semigroup or monoid.
  //!
  //! In this documentation we use the term "congruence enumeration" to mean
  //! the execution of (any version of) the Todd-Coxeter algorithm. Some of
  //! the features of this class were inspired by similar features in
  //! [ACE](https://staff.itee.uq.edu.au/havas/) by George Havas and Colin
  //! Ramsay.
  //!
  //! For information about the output printed by a ToddCoxeter instance during
  //! a congruence enumeration, see \ref todd-coxeter-report.md "this page".
  //!
  //! \sa congruence_kind and tril.
  //!
  //! \par Example 1
  //! \code
  //! Presentation<word_type> p;
  //! p.alphabet(2);
  //! presentation::add_rule(p, 00_w, 0_w);
  //! presentation::add_rule(p, 0_w, 1_w);
  //! ToddCoxeter tc(congruence_kind::onesided, p);
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
  template <typename Word>
  class ToddCoxeter : public detail::ToddCoxeterImpl {
   private:
    std::vector<Word>  _generating_pairs;
    Presentation<Word> _presentation;

   public:
#ifdef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Struct containing various options that can be used to control
    //! the behaviour of Todd-Coxeter.
    //!
    //! This struct containing various options that can be used to control the
    //! behaviour of Todd-Coxeter.
    struct options : public FelschGraphSettings_::options {
      //! \brief Enum class containing various strategies.
      //!
      //! The values in this enum can be passed to the member function
      //! \ref strategy to define the strategy to be used when performing a
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
        //! divided by \f$N\f$ nodes have been defined, where \f$N\f$ is the
        //! sum
        //! of the lengths of the words in the presentation and generating
        //! pairs. These steps are repeated until the enumeration terminates.
        CR,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy R/C. The HLT
        //! strategy is run until the first lookahead is triggered (when the
        //! number of active nodes in \ref current_word_graph is at least
        //! \ref lookahead_next). A full lookahead is then performed, and then
        //! the CR
        //! strategy is used.
        R_over_C,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Cr. The Felsch
        //! strategy is run until at least f_defs() new nodes have been
        //! defined, then the HLT strategy is run until at least hlt_defs()
        //! divided by \f$N\f$ nodes have been defined, where \f$N\f$ is the
        //! sum
        //! of the lengths of the words in the presentation and generating
        //! pairs. Then the Felsch strategy is run.
        Cr,

        //! This strategy is meant to mimic the
        //! [ACE](https://staff.itee.uq.edu.au/havas/) strategy Rc. The HLT
        //! strategy is run until at least hlt_defs() divided by \f$N\f$ new
        //! nodes have been defined (where \f$N\f$ is the sum of the lengths
        //! of
        //! the words in the presentation and generating pairs) the Felsch
        //! strategy is then run until at least f_defs() new nodes are
        //! defined,
        //! and then the HLT strategy is run.
        Rc
      };

      //! \brief Enum class for specifying the extent of any lookahead
      //! performed.
      //!
      //! The values in this enum can be used as the argument for
      //! \ref ToddCoxeter::lookahead_extent to specify the extent of any
      //! lookahead that should be performed.
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

      //! \brief Enum class containing values for specifying how to handle
      //! edge definitions.
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
      //! The values in this enum represent what to do if the number of
      //! definitions in the stack exceeds the value \ref def_max.
      enum class def_policy : uint8_t {
        //! Do not put newly generated definitions in the stack if the stack
        //! already has size \ref def_max.
        no_stack_if_no_space,
        //! If the definition stack has size \ref def_max and a new
        //! definition is generated, then definitions with dead source node
        //! are
        //! are popped from the top of the stack (if any).
        purge_from_top,
        //! If the definition stack has size \ref def_max and a new
        //! definition is generated, then definitions with dead source node
        //! are
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

      // NOTE: The next enum (def_version) is actually defined in FelschGraph,
      // not in ToddCoxeter

      //! \brief Values for specifying how to handle edge definitions.
      //!
      //! The values in this enum can be used as the argument for
      //! \ref def_version.
      //!
      //! For our purposes, a definition is a recently defined edge in the
      //! word graph that we are attempting to construct in an instance of
      //! \ref_todd_coxeter. The values in this enum influence how these
      //! definitions are stored and processed.
      //!
      //! For every definition held in the definition stack, a depth first
      //! search through the Felsch tree of the generating pairs is performed.
      //! The aim is to only follow paths from nodes in the word graph
      //! labelled by generating pairs that actually pass through the edge
      //! described by a definition. There are two versions of this
      //! represented by the values def_version::one and def_version::two. The
      //! first version is simpler, but may involve following the same path
      //! that leads nowhere multiple times. The second version is more
      //! complex, and attempts to avoid following the same path multiple
      //! times if it is found to lead nowhere once.
      enum class def_version : uint8_t {
        //! Version 1 definition processing.
        one,
        //! Version 2 definition processing.
        two,
      };
    };

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief The kind of the congruence (1- or 2-sided).
    //!
    //! This function returns the kind of the congruence represented by a
    //! \ref_todd_coxeter instance; see \ref congruence_kind for details.
    //!
    //! \return The kind of the congruence (1- or 2-sided).
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] congruence_kind kind() const noexcept;

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Returns the number of generating pairs.
    //!
    //! This function returns the number of generating pairs, which is the
    //! size of \ref generating_pairs divided by \f$2\f$.
    //!
    //! \returns
    //! The number of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    //!
    //! \complexity
    //! Constant.
    [[nodiscard]] size_t number_of_generating_pairs() const noexcept;
#endif

    //! \ingroup todd_coxeter_class_mem_types_group
    //!
    //! \brief Type of the letters in the relations of the presentation stored
    //! in a \ref_todd_coxeter instance.
    //!
    //! Type of the letters in the relations of the presentation stored
    //! in a \ref_todd_coxeter instance.
    using native_word_type = Word;

    //! \ingroup todd_coxeter_class_mem_types_group
    //! \hideinitializer
    //!
    //! \brief The type of the nodes in the word graph.
    //!
    //! The type of the nodes in the word graph.
    using node_type = typename detail::ToddCoxeterImpl::node_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //! \hideinitializer
    //!
    //! \brief The type of the underlying WordGraph.
    //!
    //! The type of the underlying WordGraph.
    using word_graph_type = typename detail::ToddCoxeterImpl::word_graph_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //! \hideinitializer
    //!
    //! \brief The type of the index of a class.
    //!
    //! The type of the indices of classes in the congruence represented by a
    //! \ref_todd_coxeter instance.
    //!
    //! This alias is the same as \ref node_type, and is included to because
    //! if a \ref_todd_coxeter instance is created from a Presentation, and
    //! that presentation does not \ref Presentation::contains_empty_word,
    //! then there is always at least one more node (the node representing the
    //! empty word) in the \ref current_word_graph than there are classes in
    //! the congruence. This alias is used to delineate the cases when we are
    //! referring to a node or a class index.
    using index_type = typename detail::ToddCoxeterImpl::index_type;

    //! \ingroup todd_coxeter_class_mem_types_group
    //! \hideinitializer
    //!
    //! \brief The type of the edge-labels in the word graph.
    //!
    //! The type of the edge-labels in the word graph.
    using label_type = typename detail::ToddCoxeterImpl::label_type;

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Default constructor.
    //!
    //! This function default constructs an uninitialised \ref_todd_coxeter
    //! instance.
    ToddCoxeter() = default;

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Re-initialize a \ref_todd_coxeter instance.
    //!
    //! This function puts a \ref_todd_coxeter instance back into the state
    //! that it would have been in if it had just been newly default
    //! constructed.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    ToddCoxeter& init();

    //! \ingroup todd_coxeter_class_init_group
    //! Copy constructor.
    ToddCoxeter(ToddCoxeter const&) = default;

    //! \ingroup todd_coxeter_class_init_group
    //! Move constructor.
    ToddCoxeter(ToddCoxeter&&) = default;

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Copy assignment operator.
    //!
    //! Copy assignment operator.
    ToddCoxeter& operator=(ToddCoxeter const&) = default;

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Move assignment operator.
    //!
    //! Move assignment operator.
    ToddCoxeter& operator=(ToddCoxeter&&) = default;

    ~ToddCoxeter();

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Construct from \ref congruence_kind and Presentation.
    //!
    //! This function constructs a  \ref_todd_coxeter instance representing a
    //! congruence of kind \p knd over the semigroup or monoid defined by the
    //! presentation \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    ToddCoxeter(congruence_kind knd, Presentation<Word>&& p) : ToddCoxeter() {
      init(knd, std::move(p));
    }

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Re-initialize a \ref_todd_coxeter
    //! instance.
    //!
    //! This function puts a  \ref_todd_coxeter
    //! instance back into the state that it would have been in if it had just
    //! been newly constructed from \p knd and \p p.
    //!
    //! \param knd the kind (onesided or twosided) of the congruence.
    //! \param p the presentation.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throws LibsemigroupsException if \p p is not valid.
    ToddCoxeter& init(congruence_kind knd, Presentation<Word>&& p);

    //! \ingroup todd_coxeter_class_init_group
    //! \copydoc ToddCoxeter(congruence_kind, Presentation<Word>&&)
    ToddCoxeter(congruence_kind knd, Presentation<Word> const& p)
        // call the rval ref constructor
        : ToddCoxeter(knd, Presentation<Word>(p)) {}

    //! \ingroup todd_coxeter_class_init_group
    //! \copydoc init(congruence_kind, Presentation<Word>&&)
    ToddCoxeter& init(congruence_kind knd, Presentation<Word> const& p) {
      // call the rval ref init
      return init(knd, Presentation<Word>(p));
    }

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Construct from \ref congruence_kind and \ref_todd_coxeter.
    //!
    //! This function constructs a  \ref_todd_coxeter instance representing a
    //! congruence of kind \p knd over the \ref_todd_coxeter instance \p tc.
    //! The \ref_todd_coxeter instance constructed in this way represents a
    //! quotient of the word graph represented by \p tc.
    //!
    //! \param knd the kind (onesided, or twosided) of the congruence.
    //! \param tc the \ref_todd_coxeter instance.
    //!
    //! \throw LibsemigroupsException if the arguments \p knd and \p tc are
    //! not compatible. If the first item is `tc.kind()` and the second is the
    //! parameter \p knd, then compatible arguments are (one-sided,
    //! one-sided), (two-sided, one-sided), and (two-sided, two-sided).
    ToddCoxeter(congruence_kind knd, ToddCoxeter const& tc) : ToddCoxeter() {
      init(knd, tc);
    }

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Re-initialize a \ref_todd_coxeter instance.
    //!
    //! This function puts a \ref_todd_coxeter instance back into the state
    //! that it would have been in if it had just been newly constructed from
    //! \p knd and \p tc.
    //!
    //! \param knd the kind (onesided, or twosided) of the congruence.
    //! \param tc the \ref_todd_coxeter instance.
    //!
    //! \returns A reference to `*this`.
    //!
    //! \throw LibsemigroupsException if the arguments \p knd and \p tc are
    //! not compatible. If the first item is `tc.kind()` and the second is the
    //! parameter \p knd, then compatible arguments are (one-sided,
    //! one-sided), (two-sided, one-sided), and (two-sided, two-sided).
    ToddCoxeter& init(congruence_kind knd, ToddCoxeter const& tc) {
      ToddCoxeterImpl::init(knd, tc);
      _presentation = tc.presentation();
      _presentation.rules.insert(_presentation.rules.end(),
                                 tc.generating_pairs().cbegin(),
                                 tc.generating_pairs().cend());
      // Clear generating pairs last, in case &tc == this!!!
      _generating_pairs.clear();
      // TODO(1) check KnuthBendix et al
      return *this;
    }

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Construct from \ref congruence_kind and \ref WordGraph.
    //!
    //! This function constructs a  \ref_todd_coxeter instance representing a
    //! congruence of kind \p knd over the WordGraph \p wg. The
    //! \ref_todd_coxeter instance constructed in this way represents a
    //! quotient of the word graph \p wg. If \p wg happens to be the left or
    //! right Cayley graph of a semigroup or monoid, then the
    //! \ref_todd_coxeter instance will represent a quotient of that
    //! semigroup.
    //!
    //! \tparam Node the type of the nodes in the 2nd argument.
    //!
    //! \param knd the kind (1- or 2-sided) of the congruence.
    //! \param wg the word graph.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    template <typename Node>
    ToddCoxeter(congruence_kind knd, WordGraph<Node> const& wg)
        : ToddCoxeter() {
      init(knd, wg);
    }

    //! \ingroup todd_coxeter_class_init_group
    //! \brief Re-initialize a \ref_todd_coxeter instance.
    //!
    //! This function puts a  \ref_todd_coxeter instance back into the state
    //! that it would have been in if it had just been newly constructed from
    //! \p knd and \p wg.
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
    template <typename Node>
    ToddCoxeter& init(congruence_kind knd, WordGraph<Node> const& wg);

#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // Used in Sims
    // TODO(1) could this and the next function be removed, and replaced
    // with something else?
    template <typename Node>
    ToddCoxeter(congruence_kind           knd,
                Presentation<Word> const& p,
                WordGraph<Node> const&    wg) {
      init(knd, p, wg);
    }

    // TODO(1) a "make" variant that throws if params are valid
    template <typename Node>
    ToddCoxeter& init(congruence_kind           knd,
                      Presentation<Word> const& p,
                      WordGraph<Node> const&    wg);
#endif

    //! \ingroup todd_coxeter_class_init_group
    //!
    //! \brief Throws if any letter in a range is out of bounds.
    //!
    //! This function throws a LibsemigroupsException if any value pointed
    //! at by an iterator in the range \p first to \p last is out of
    //! bounds (i.e. does not belong to the alphabet of the
    //! \ref presentation used to construct the \ref_todd_coxeter instance).
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the
    //! word.
    //!
    //! \throw LibsemigroupsException if any letter in the range from
    //! \p first to \p last is out of bounds.
    template <typename Iterator1, typename Iterator2>
    void throw_if_letter_not_in_alphabet(Iterator1 first,
                                         Iterator2 last) const {
      presentation().throw_if_letter_not_in_alphabet(first, last);
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Get the generating pairs of the congruence.
    //!
    //! This function returns the generating pairs of the congruence. The
    //! words comprising the generating pairs are converted to
    //! \ref native_word_type as they are added via \ref add_generating_pair.
    //! This function returns the std::vector of these \ref native_word_type.
    //!
    //! \returns The std::vector of generating pairs.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] std::vector<Word> const& generating_pairs() const noexcept {
      return _generating_pairs;
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Get the presentation used to define a \ref_todd_coxeter
    //! instance (if any).
    //!
    //! If a \ref_todd_coxeter instance is constructed or initialised using a
    //! presentation, then a const reference to this presentation is returned
    //! by this function.
    //!
    //! If the \ref_todd_coxeter instance was constructed or initialised from
    //! a WordGraph, then this presentation will be empty.
    //!
    //! \returns A const reference to the presentation.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] Presentation<Word> const& presentation() const noexcept {
      return _presentation;
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - add_generating_pair
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented
    //! by a
    //! \ref_todd_coxeter instance.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \warning It is assumed that \ref started returns \c false. Adding
    //! generating pairs after \ref started is not permitted (but also not
    //! checked by this function).
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_generating_pair_no_checks(Iterator1 first1,
                                               Iterator2 last1,
                                               Iterator3 first2,
                                               Iterator4 last2);

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Add generating pair via iterators.
    //!
    //! This function adds a generating pair to the congruence represented by
    //! a \ref_todd_coxeter instance.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns A reference to `*this`.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_throws_if_started
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    ToddCoxeter& add_generating_pair(Iterator1 first1,
                                     Iterator2 last1,
                                     Iterator3 first2,
                                     Iterator4 last2) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in ToddCoxeter and not ToddCoxeterImpl
      return detail::CongruenceCommon::add_generating_pair<ToddCoxeter>(
          first1, last1, first2, last2);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - contains
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to
    //! be contained in the congruence represented by a \ref_todd_coxeter
    //! instance. This function performs no enumeration, so it is possible for
    //! the words to be contained in the congruence, but that this is not
    //! currently known.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains_no_checks(Iterator1 first1,
                                      Iterator2 last1,
                                      Iterator3 first2,
                                      Iterator4 last2) const {
      return ToddCoxeterImpl::currently_contains_no_checks(
          detail::citow(this, first1),
          detail::citow(this, last1),
          detail::citow(this, first2),
          detail::citow(this, last2));
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are already known to
    //! be contained in the congruence represented by a \ref_todd_coxeter.
    //! This function performs no enumeration, so it is possible for the words
    //! to be contained in the congruence, but that this is not currently
    //! known.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns
    //! * tril::TRUE if the words are known to belong to the congruence;
    //! * tril::FALSE if the words are known to not belong to the congruence;
    //! * tril::unknown otherwise.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    tril currently_contains(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) const {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in ToddCoxeter and not ToddCoxeterImpl
      return detail::CongruenceCommon::currently_contains<ToddCoxeter>(
          first1, last1, first2, last2);
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref_todd_coxeter instance. This function
    //! triggers a full enumeration, which may never terminate.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains_no_checks(Iterator1 first1,
                            Iterator2 last1,
                            Iterator3 first2,
                            Iterator4 last2) {
      return ToddCoxeterImpl::contains_no_checks(detail::citow(this, first1),
                                                 detail::citow(this, last1),
                                                 detail::citow(this, first2),
                                                 detail::citow(this, last2));
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Check containment of a pair of words via iterators.
    //!
    //! This function checks whether or not the words represented by the
    //! ranges
    //! \p first1 to \p last1 and \p first2 to \p last2 are contained in the
    //! congruence represented by a \ref_todd_coxeter instance. This function
    //! triggers a full enumeration, which may never terminate.
    //!
    //! \cong_common_params_contains
    //!
    //! \returns Whether or not the pair belongs to the congruence.
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename Iterator1,
              typename Iterator2,
              typename Iterator3,
              typename Iterator4>
    bool contains(Iterator1 first1,
                  Iterator2 last1,
                  Iterator3 first2,
                  Iterator4 last2);

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - interface requirements - reduce
    ////////////////////////////////////////////////////////////////////////

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Reduce a word with no enumeration or checks.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence
    //! defined by a
    //! \ref_todd_coxeter instance. If the \ref_todd_coxeter instance is
    //! \ref finished, then the output word is a normal form for the input word.
    //! If the \ref_todd_coxeter instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run_no_checks(OutputIterator d_first,
                                           InputIterator1 first,
                                           InputIterator2 last) const {
      return ToddCoxeterImpl::reduce_no_run_no_checks(
                 detail::itow(this, d_first),
                 detail::citow(this, first),
                 detail::citow(this, last))
          .get();
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Reduce a word with no enumeration.
    //!
    //! This function writes a reduced word equivalent to the input word
    //! described by the iterator \p first and \p last to the output iterator
    //! \p d_first. This function triggers no enumeration. The word output by
    //! this function is equivalent to the input word in the congruence
    //! defined by a
    //! \ref_todd_coxeter instance. If the \ref_todd_coxeter instance is
    //! \ref finished, then the output word is a normal form for the input word.
    //! If the \ref_todd_coxeter instance is not \ref finished, then it might be
    //! that equivalent input words produce different output words.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_run(OutputIterator d_first,
                                 InputIterator1 first,
                                 InputIterator2 last) const {
      return detail::CongruenceCommon::reduce_no_run<ToddCoxeter>(
          d_first, first, last);
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Reduce a word with no checks.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first
    //! and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by
    //! a
    //! \ref_todd_coxeter instance. In other
    //! words, the output word is a normal form for the input word or
    //! equivalently a canconical representative of its congruence class.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    //!
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce_no_checks(OutputIterator d_first,
                                    InputIterator1 first,
                                    InputIterator2 last) {
      return ToddCoxeterImpl::reduce_no_checks(detail::itow(this, d_first),
                                               detail::citow(this, first),
                                               detail::citow(this, last))
          .get();
    }

    //! \ingroup todd_coxeter_class_intf_group
    //!
    //! \brief Reduce a word.
    //!
    //! This function triggers a full enumeration and then writes a reduced
    //! word equivalent to the input word described by the iterator \p first
    //! and
    //! \p last to the output iterator \p d_first. The word output by this
    //! function is equivalent to the input word in the congruence defined by
    //! a
    //! \ref_todd_coxeter instance. In other words, the output word is a
    //! normal form for the input word or equivalently a canconical
    //! representative of its congruence class.
    //!
    //! \cong_common_params_reduce
    //!
    //! \returns An \p OutputIterator pointing one beyond the last letter
    //! inserted into \p d_first.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \cong_common_warn_undecidable{Todd-Coxeter}
    //!
    template <typename OutputIterator,
              typename InputIterator1,
              typename InputIterator2>
    OutputIterator reduce(OutputIterator d_first,
                          InputIterator1 first,
                          InputIterator2 last) {
      // Call detail::CongruenceCommon version so that we perform bound checks
      // in ToddCoxeter and not ToddCoxeterImpl
      return detail::CongruenceCommon::reduce<ToddCoxeter>(
          d_first, first, last);
    }

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeterImpl - word -> index
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_word_index_group Word to class index
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting a word into the index of a
    //! class in a \ref_todd_coxeter instance.
    //!
    //! This page contains documentation for the member functions of
    //! \ref_todd_coxeter that can be used to convert a word into the index of
    //! congruence class.
    //!
    //! \sa \ref todd_coxeter_class_index_word_group for the inverses of the
    //! functions described on this page.
    //!
    //! @{

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing the
    //! word described by the iterators \p first and \p last. No enumeration
    //! is triggered by calls to this function. Unless \ref finished returns
    //! \c true, the index returned by this function is essentially arbitrary,
    //! and can only really be used to check whether or not two words are
    //! currently known to belong to the congruence.
    //!
    //! The returned index is obtained by following the path in
    //! \ref current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. If there is no such path, then
    //! \ref UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    // NOTE THAT: the graph contains one more node than there are element if
    // the underlying presentation does not contain the empty word
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of_no_checks(Iterator1 first,
                                          Iterator2 last) const {
      return ToddCoxeterImpl::current_index_of_no_checks(
          detail::citow(this, first), detail::citow(this, last));
    }

    //! \brief Returns the current index of the class containing a word.
    //!
    //! This function returns the current index of the class containing the
    //! word described by the iterators \p first and \p last. No enumeration
    //! is triggered by calls to this function. Unless \ref finished returns
    //! \c true, the index returned by this function is essentially arbitrary,
    //! and can only really be used to check whether or not two words are
    //! currently known to belong to the congruence.
    //!
    //! The returned index is obtained by following the path in
    //! \ref current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. If there is no such path, then
    //! \ref UNDEFINED is returned.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The current index of the class containing the word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type current_index_of(Iterator1 first, Iterator2 last) const {
      throw_if_letter_not_in_alphabet(first, last);
      return current_index_of_no_checks(first, last);
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full enumeration is
    //! triggered by calls to this function. If the \ref current_word_graph
    //! has not already been standardized, then this function first
    //! standardizes it with respect to Order::shortlex; otherwise the
    //! existing standardization order is used.
    //!
    //! The returned index is obtained by following the path in
    //! \ref current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. Since a full enumeration is triggered
    //! by calls to this function, the word graph is complete, and so the
    //! return value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of_no_checks(Iterator1 first, Iterator2 last) {
      return ToddCoxeterImpl::index_of_no_checks(detail::citow(this, first),
                                                 detail::citow(this, last));
    }

    //! \brief Returns the index of the class containing a word.
    //!
    //! This function returns the index of the class containing the word
    //! described by the iterators \p first and \p last. A full enumeration is
    //! triggered by calls to this function. If the \ref current_word_graph
    //! has not already been standardized, then this function first
    //! standardizes it with respect to Order::shortlex; otherwise the
    //! existing standardization order is used.
    //!
    //! The returned index is obtained by following the path in
    //! \ref current_word_graph from node \c 0 labelled by the word given by the
    //! arguments \p first and \p last. Since a full enumeration is triggered
    //! by calls to this function, the word graph is complete, and so the
    //! return value is never \ref UNDEFINED.
    //!
    //! \tparam Iterator1 the type of first argument \p first.
    //! \tparam Iterator2 the type of second argument \p last.
    //!
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter of the word.
    //!
    //! \returns The index of the class containing the word.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    template <typename Iterator1, typename Iterator2>
    index_type index_of(Iterator1 first, Iterator2 last) {
      throw_if_letter_not_in_alphabet(first, last);
      return index_of_no_checks(first, last);
    }

    //! @}

    ////////////////////////////////////////////////////////////////////////
    // ToddCoxeter - index -> word
    ////////////////////////////////////////////////////////////////////////

    //! \defgroup todd_coxeter_class_index_word_group Class index to word
    //! \ingroup todd_coxeter_class_group
    //!
    //! \brief Member functions for converting the index of a class
    //! into a word in a \ref_todd_coxeter
    //! instance.
    //!
    //! This page contains documentation for the member functions of
    //!  \ref_todd_coxeter that can be used to
    //!  convert the index of a congruence class
    //! to a representative word belonging to that congruence class.
    //!
    //! \sa \ref todd_coxeter_class_word_index_group for the inverses of the
    //! functions described on this page.
    //!
    //! @{

    //! \brief Insert a current word representing a class with given index
    //! into an output iterator.
    //!
    //! This function appends a current word representing the class with index
    //! \p i to the output iterator \p d_first. No enumeration is triggered by
    //! calls to this function, but \ref current_word_graph is standardized
    //! (using Order::shortlex) if it is not already standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c i
    //! back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that \p i is a valid index of a current class.
    //!
    // NOTE THAT: the graph contains one more node than there are element if
    // the underlying presentation does not contain the empty word
    template <typename OutputIterator>
    OutputIterator current_word_of_no_checks(OutputIterator d_first,
                                             index_type     i) const {
      return ToddCoxeterImpl::current_word_of_no_checks(
                 detail::itow(this, d_first), i)
          .get();
    }

    //! \brief Insert a current word representing a class with given index
    //! into an output iterator.
    //!
    //! This function appends a current word representing the class with index
    //! \p i to the output iterator \p d_first. No enumeration is triggered by
    //! calls to this function, but \ref current_word_graph is standardized
    //! (using Order::shortlex) if it is not already standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c i
    //! back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    template <typename OutputIterator>
    OutputIterator current_word_of(OutputIterator d_first, index_type i) const {
      return ToddCoxeterImpl::current_word_of(detail::itow(this, d_first), i)
          .get();
    }

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is
    //! triggered by calls to this function, and \ref current_word_graph is
    //! standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c i
    //! back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \throws LibsemigroupsException if \p i is out of bounds.
    template <typename Iterator>
    Iterator word_of_no_checks(Iterator d_first, index_type i) {
      return ToddCoxeterImpl::word_of_no_checks(detail::itow(this, d_first), i)
          .get();
    }

    //! \brief Insert the word representing a class with given index into
    //! an output iterator.
    //!
    //! This function appends the word representing the class with index
    //! \p i to the output iterator \p d_first. A full enumeration is
    //! triggered by calls to this function, and \ref current_word_graph is
    //! standardized.
    //!
    //! The word appended to \p d_first is obtained by following a path in
    //! \ref current_spanning_tree from the node corresponding to index \c i
    //! back to the root of that tree.
    //!
    //! \tparam OutputIterator the type of the first argument.
    //!
    //! \param d_first output iterator pointing at the first letter of the
    //! destination.
    //! \param i the index of the class.
    //!
    //! \returns An output iterator pointing one beyond the last letter of the
    //! output word.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that \p i is a valid index of a current class.
    template <typename Iterator>
    Iterator word_of(Iterator d_first, index_type i) {
      return ToddCoxeterImpl::word_of(detail::itow(this, d_first), i).get();
    }

    //! @}
  };  // class ToddCoxeter

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<Word>` from a
  //! `Presentation<Word>` const reference.
  template <typename Word>
  ToddCoxeter(congruence_kind, Presentation<Word> const&) -> ToddCoxeter<Word>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<Word>` from a
  //! `Presentation<Word>` rvalue reference.
  template <typename Word>
  ToddCoxeter(congruence_kind, Presentation<Word>&&) -> ToddCoxeter<Word>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<Word>` from a
  //! `ToddCoxeter<Word>` const reference.
  template <typename Word>
  ToddCoxeter(congruence_kind, ToddCoxeter<Word> const&) -> ToddCoxeter<Word>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<word_type>` from a
  //! `WordGraph<Node>` const reference.
  template <typename Node>
  ToddCoxeter(congruence_kind, WordGraph<Node> const&)
      -> ToddCoxeter<word_type>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<word_type>` from a
  //! `WordGraph<Node>` rvalue reference.
  template <typename Node>
  ToddCoxeter(congruence_kind, WordGraph<Node>&&) -> ToddCoxeter<word_type>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<Word>` from a
  //! `ToddCoxeter<Word>` const reference.
  template <typename Word>
  ToddCoxeter(ToddCoxeter<Word> const&) -> ToddCoxeter<Word>;

  //! \ingroup todd_coxeter_class_group
  //
  //! \brief Deduction guide.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! Deduction guide to construct a `ToddCoxeter<Word>` from a
  //! `ToddCoxeter<Word>` rvalue reference.
  template <typename Word>
  ToddCoxeter(ToddCoxeter<Word>&&) -> ToddCoxeter<Word>;

  //! \ingroup todd_coxeter_group
  //!
  //! \brief Return a human readable representation of a \ref_todd_coxeter
  //! object.
  //!
  //! Defined in `todd-coxeter-class.hpp`.
  //!
  //! This function returns a human readable representation of a
  //! \ref_todd_coxeter object.
  //!
  //! \param tc the \ref_todd_coxeter object.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename Word>
  std::string to_human_readable_repr(ToddCoxeter<Word> const& tc);
}  // namespace libsemigroups

#include "todd-coxeter-class.tpp"
#endif  // LIBSEMIGROUPS_TODD_COXETER_CLASS_HPP_
