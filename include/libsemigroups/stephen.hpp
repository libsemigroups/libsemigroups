//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2025 James D. Mitchell
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

#ifndef LIBSEMIGROUPS_STEPHEN_HPP_
#define LIBSEMIGROUPS_STEPHEN_HPP_

#include <array>    // for array
#include <cstddef>  // for size_t
#include <cstdint>  // for uint32_t
#include <memory>   // for shared_ptr
#include <string>   // for string
#include <tuple>    // for tie
#include <utility>  // for make_pair
#include <vector>   // for vector

#include "constants.hpp"     // for Max, UNDEFINED
#include "debug.hpp"         // for LIBSEMIGROUPS_ASSERT
#include "dot.hpp"           // for Dot
#include "exception.hpp"     // for LIBSEMIGROUPS_EXCEPTION
#include "paths.hpp"         // for Paths
#include "presentation.hpp"  // for Presentation
#include "runner.hpp"        // for Runner
#include "types.hpp"         // for word_type
#include "word-graph.hpp"    // for WordGraph

#include "detail/felsch-graph.hpp"             // for DoNotReg...
#include "detail/node-managed-graph.hpp"       // for NodeMana...
#include "detail/node-manager.hpp"             // for NodeManager
#include "detail/report.hpp"                   // for report_n...
#include "detail/string.hpp"                   // for group_di...
#include "detail/word-graph-with-sources.hpp"  // for WordGrap...

// TODO(2)
// * update so that run_for, run_until work properly (at present basically
//   run_impl starts again from scratch every time)
// * minimal rep (as per Reinis) (named normal_form?)
// * invert() - just swap the initial and accept states and re-standardize
// * idempotent() - just make the accept state = initial state.
// * class_of for inverse Stephen (i.e. all walks in the graph through all
// nodes) (not sure how to do this just yet). This is different than
// words_accepted see Corollary 3.2 in Stephen's "Presentations of inverse
// monoids" paper (not thesis).
// * canonical_form (as per Howie's book)

//! \defgroup stephen_group Stephen
//!
//! On this page we describe the functionality in `libsemigroups` relating
//! to Stephen's procedure for finitely presented semigroups.

namespace libsemigroups {

  //! \ingroup stephen_group
  //!
  //! \brief For constructing the word graph of left factors of a word in an
  //! f.p. semigroup.
  //!
  //! Defined in `sims.hpp`.
  //!
  //! This class implements Stephen's procedure for constructing
  //! the WordGraph corresponding to the left factors of a word in a finitely
  //! presented semigroup or a finitely presented inverse semigroup. The
  //! algorithm implemented in this class is closely related to the Todd-Coxeter
  //! algorithm  (as implemented in ToddCoxeter) and originates in
  //! \cite Stephen1987aa.
  //!
  //! \tparam PresentationType a type derived from \ref PresentationBase. This
  //! is the type of the underlying presentation used in the Stephen algorithm.
  //! Common choices include \ref Presentation<word_type>,
  //! \ref Presentation<std::string>, \ref InversePresentation<word_type> and
  //! \ref InversePresentation<std::string>. If an \ref InversePresentation is
  //! supplied, then the \ref Stephen class will use the Stephen procedure for
  //! inverse semigroups when run. Otherwise the Stephen procedure for general
  //! semigroups is used instead.
  template <typename PresentationType>
  class Stephen : public Runner {
    template <typename Q>
    static constexpr bool is_valid_presentation() {
      return std::is_same_v<Q, Presentation<word_type>>
             || std::is_same_v<Q, InversePresentation<word_type>>;
      // TODO(2): uncomment when we figure out how to handle std::string
      // presentations
      //|| std::is_same_v<Q, Presentation<std::string>>
      //|| std::is_same_v<Q, InversePresentation<std::string>>;
    }

    // TODO (2): Change the error message once std::string presentations are
    // supported
    static_assert(is_valid_presentation<PresentationType>(),
                  "the template parameter PresentationType must be "
                  "Presentation<word_type> or InversePresentation<word_type>");

   public:
    //! The underlying presentation type.
    using presentation_type = PresentationType;

    //! The return type of the function \ref libsemigroups::Stephen::word_graph.
    using word_graph_type = WordGraph<uint32_t>;
    //! The node type of \ref libsemigroups::Stephen::word_graph_type.
    using node_type = word_graph_type::node_type;

   private:
    class StephenGraph;  // forward decl

    // Data members
    node_type                         _accept_state;
    bool                              _finished;
    bool                              _is_word_set;
    std::shared_ptr<PresentationType> _presentation;
    word_type                         _word;
    StephenGraph                      _word_graph;

   public:
    //! \brief Default constructor.
    //!
    //! Default constructs an empty instance, use \ref init and \ref set_word
    //! to specify the presentation and the word, respectively.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Stephen();

    //! \brief Reinitialize an existing Stephen instance.
    //!
    //! This function puts a Stephen instance back into the same state as if
    //! it had been newly default constructed.
    //!
    //! \returns A reference to \c this.
    //!
    //! \exceptions
    //! \no_libsemigroups_except
    Stephen& init();

    // TODO(2): Implement make<Stephen> function which checks args
    // Should presentation::throw_if_not_normalized (until we separate the
    // implementation and external interface later on).
    // Should also throw if `p.throw_if_bad_alphabet_or_rules()` throws.
    // Should also throw if if `p.alphabet().size()` is `0`.

    //! \brief Construct from a presentation (copy).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! copies \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param p the presentation.
    // Not noexcept because allocated memory
    explicit Stephen(PresentationType const& p) : Stephen() {
      init(p);
    }

    //! \brief Construct from a presentation (move).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! moves \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param p the presentation.
    explicit Stephen(PresentationType&& p) : Stephen() {
      init(std::move(p));
    }

    //! \brief Construct from a shared pointer to presentation (copy).
    //!
    //! Construct an instance from the shared pointer \p ptr to a shared
    //! presentation object. This constructor copies \p ptr.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param ptr a shared pointer to a presentation.
    explicit Stephen(std::shared_ptr<PresentationType> const& ptr) : Stephen() {
      init(ptr);
    }

    //! Default copy constructor
    Stephen(Stephen const& that) = default;

    //! Default move constructor
    Stephen(Stephen&&) = default;

    //! Default copy assignment operator
    Stephen& operator=(Stephen const&) = default;

    //! Default move assignment operator
    Stephen& operator=(Stephen&&) = default;

    ~Stephen();

    //! \brief Initialize from a presentation (copy).
    //!
    //! This function puts a Stephen instance back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! copies \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    Stephen& init(PresentationType const& p) {
      return init(std::make_shared<PresentationType>(p));
    }

    //! \brief Initialize from a presentation (move).
    //!
    //! This function puts a Stephen instance back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! moves \p p.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    Stephen& init(PresentationType&& p) {
      return init(std::make_shared<PresentationType>(std::move(p)));
    }

    //! \brief Initialize from a shared pointer to presentation (copy).
    //!
    //! This function puts a Stephen instance back into the same state as if it
    //! had been newly constructed from the shared pointer to a presentation
    //! \p ptr. This initializer copies \p ptr.
    //!
    //! \tparam PresentationType a type derived from \ref PresentationBase.
    //!
    //! \param ptr a shared pointer to a presentation.
    //!
    //! \returns A reference to \c this.
    Stephen& init(std::shared_ptr<PresentationType> const& ptr);

    //! \brief Get the input presentation.
    //!
    //! This function returns a const reference to the input presentation.
    //!
    //! \returns A const reference to a \ref presentation_type.
    //!
    //! \exceptions
    //! \noexcept
    presentation_type const& presentation() const noexcept {
      return *_presentation;
    }

    //! \brief Set the initial word.
    //!
    //! This function sets the word whose left factors, or equivalent words, are
    //! sought.
    //!
    //! \tparam Iterator1 the type of the argument \p first1.
    //! \tparam Iterator2 the type of the argument \p last1.
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter in the word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \cong_common_throws_if_letters_out_of_bounds
    //!
    //! \sa stephen::set_word
    template <typename Iterator1, typename Iterator2>
    Stephen& set_word(Iterator1 first, Iterator2 last) {
      presentation().throw_if_letter_not_in_alphabet(first, last);
      return set_word_no_checks(first, last);
    }

    //! \brief Set the initial word (no checks).
    //!
    //! This function sets the word whose left factors, or equivalent words, are
    //! sought.
    //!
    //! \tparam Iterator1 the type of the argument \p first1.
    //! \tparam Iterator2 the type of the argument \p last1.
    //! \param first iterator pointing at the first letter of the word.
    //! \param last iterator pointing one beyond the last letter in the word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \cong_common_warn_assume_letters_in_bounds
    //!
    //! \sa stephen::set_word_no_checks
    template <typename Iterator1, typename Iterator2>
    Stephen& set_word_no_checks(Iterator1 first, Iterator2 last);

    //! \brief Check if the initial word is set.
    //!
    //! Returns true if a word has been set with \ref set_word since the last
    //! presentation change and false otherwise.
    //!
    //! \returns A bool.
    //!
    //! \exceptions
    //! \noexcept
    bool is_word_set() const noexcept {
      return _is_word_set;
    }

    //! \brief Get the initial word.
    //!
    //! Returns a const reference to the word set by \ref set_word.
    //!
    //! \returns A const reference to a \ref word_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    word_type const& word() const {
      throw_if_not_ready();
      return _word;
    }

    //! \brief Get the word graph.
    //!
    //! Returns a const reference to the word graph in its present state. The
    //! algorithm implemented in this class is not triggered by calls to this
    //! function.
    //!
    //! \returns A const reference to a \ref word_graph_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    word_graph_type const& word_graph() const {
      throw_if_not_ready();
      return _word_graph;
    }

    //! \brief Get the accept state of the word graph.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns the accept state of
    //! the produced word graph.
    //!
    //! \returns A \ref node_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    // Throws if run throws, also this is not in the helper namespace because
    // we cache the return value.
    node_type accept_state();

    //! \brief Get the initial state of the word graph.
    //!
    //! This function return the initial state of \ref word_graph.
    //!
    //! \returns A \ref node_type.
    //!
    //! \exceptions
    //! \noexcept
    static constexpr node_type initial_state() noexcept {
      return 0;
    }

    //! \brief Append a Stephen instance.
    //!
    //! This function appends the Stephen instance \p that to \c this.
    //! This modifies the current Stephen instance in-place. The result is a
    //! Stephen instance with underlying word equal to the concatenation of
    //! \c this.word() and \c that.word().
    //!
    //! The advantage of this is that if either \c this or \p that have already
    //! been (partially) run, then we can reuse the underlying word graphs
    //! instead of having to recompute them completely from scratch.
    //!
    //! \param that the Stephen instance to append.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \c this or \p that or with Stephen::init or if no
    //! word was set with Stephen::set_word .
    //!
    //! \throws LibsemigroupsException if the presentations for \c this and
    //! \p that differ.
    void operator*=(Stephen<PresentationType>& that);

    //! \brief Append a Stephen instance (no checks).
    //!
    //! This function appends the Stephen instance \p that to \c this.
    //! This modifies the current Stephen instance in-place. The result is a
    //! Stephen instance with underlying word equal to the concatenation of
    //! \c this.word() and \c that.word().
    //!
    //! The advantage of this is that if either \c this or \p that have already
    //! been (partially) run, then we can reuse the underlying word graphs
    //! instead of having to recompute them completely from scratch.
    //!
    //! \param that the Stephen instance to append.
    //!
    //! \warning
    //! No checks are made on the validity of the parameters to this function.
    //! Bad things may happen if \c this and \p that have different underlying
    //! presentations or if either of them is not ready.
    //!
    //! \sa Stephen::operator*= .
    void append_no_checks(Stephen<PresentationType>& that);

   private:
    ////////////////////////////////////////////////////////////////////////
    // Reporting
    ////////////////////////////////////////////////////////////////////////

    void report_after_run() const;
    void report_before_run() const;
    void report_progress_from_thread() const;

    Stephen& init_after_presentation_set();

    void throw_if_presentation_empty(presentation_type const& p) const {
      if (p.alphabet().empty()) {
        LIBSEMIGROUPS_EXCEPTION("the presentation must not have 0 generators");
      }
    }

    void throw_if_not_ready() const;

    void init_word_graph_from_word_no_checks() {
      _word_graph.init(presentation());
      _word_graph.report_prefix("Stephen");
      _word_graph.complete_path(
          presentation(), 0, _word.cbegin(), _word.cend());
      // Here so we have accurate data when using to_human_readable_repr
      _word_graph.number_of_active_nodes(_word_graph.number_of_nodes_active());
    }

    void run_impl() override;
    void really_run_impl();

    bool finished_impl() const noexcept override {
      return _finished;
    }

    void standardize() {
      word_graph::standardize(_word_graph);
      _word_graph.induced_subgraph_no_checks(
          0, _word_graph.number_of_nodes_active());
    }
  };

  // Deduction guides
  // The following is not a mistake but intentional, if no presentation type is
  // explicitly used, then we use Presentation<word_type>.
  // Presentation<std::string> is not allowed.
  // TODO(2): allow for Presentation<std::string> by templating on Word (after
  // we separate implementation and interface)

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<Presentation<word_type>>` from a
  //! `Presentation<word_type> const&`.
  Stephen(Presentation<word_type> const&)->Stephen<Presentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<Presentation<word_type>>` from a
  //! `Presentation<word_type>&`.
  Stephen(Presentation<word_type>&)->Stephen<Presentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<Presentation<word_type>>` from a
  //! `Presentation<word_type>&&`.
  Stephen(Presentation<word_type>&&)->Stephen<Presentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<Presentation<word_type>>` from a
  //! `std::shared_ptr<Presentation<word_type>>&&`.
  Stephen(std::shared_ptr<Presentation<word_type>>&&)
      ->Stephen<Presentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<InversePresentation<word_type>>`
  //! from an `InversePresentation<word_type> const&`.
  Stephen(InversePresentation<word_type> const&)
      ->Stephen<InversePresentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<InversePresentation<word_type>>`
  //! from an `InversePresentation<word_type>&`.
  Stephen(InversePresentation<word_type>&)
      ->Stephen<InversePresentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<InversePresentation<word_type>>`
  //! from an `InversePresentation<word_type>&&`.
  Stephen(InversePresentation<word_type>&&)
      ->Stephen<InversePresentation<word_type>>;

  //! \ingroup stephen_group
  //!
  //! \brief Deduction guide.
  //!
  //! Defined in `stephen.hpp`
  //!
  //! Deduction guide to construct a `Stephen<InversePresentation<word_type>>`
  //! from a `std::shared_ptr<InversePresentation<word_type>>&&`.
  Stephen(std::shared_ptr<InversePresentation<word_type>>&&)
      ->Stephen<InversePresentation<word_type>>;
  // TODO(2): other shared_ptr guides?

}  // namespace libsemigroups

namespace libsemigroups {
  //! \ingroup stephen_group
  //!
  //! \brief Helper functions for the \ref Stephen class.
  //!
  //! Defined in \c stephen.hpp.
  //!
  //! This page contains documentation for some helper functions for the
  //! \ref Stephen class. The helpers documented on this page all belong to the
  //! namespace `stephen`.
  namespace stephen {

    //! \brief Check if a word is accepted by a Stephen instance.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true if \p w
    //! labels a path in Stephen::word_graph with source \c 0 and target
    //! Stephen::accept_state.
    //!
    //! For a \c Stephen<Presentation> instance, a word \p w is accepted if and
    //! only if \p w is equivalent to Stephen::word in the semigroup defined by
    //! Stephen::presentation.
    //!
    //! For a \c Stephen<InversePresentation> instance, a word \p w is accepted
    //! if and only if \f$uu^{-1}w\f$ is equivalent to \f$u\f$ in the semigroup
    //! defined by Stephen::presentation, where \f$u\f$ is the value of
    //! Stephen::word.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \note
    //! The interpretation of results returned by this function differs between
    //! \c Stephen<Presentation> and \c Stephen<InversePresentation>."
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    template <typename PresentationType>
    bool accepts(Stephen<PresentationType>& s, word_type const& w);

    //! \brief Check if a word is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true
    //! if it labels a path in Stephen::word_graph with source \c 0.
    //!
    //! A word \p w labels such a path if and only if \p w is a left factor of
    //! Stephen::word in the semigroup defined by Stephen::presentation. Note
    //! that this is true for both \c Stephen<Presentation> and
    //! \c Stephen<InversePresentation> instances.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    template <typename PresentationType>
    bool is_left_factor(Stephen<PresentationType>& s, word_type const& w);

    //! \brief Returns a range object containing all words accepted by a
    //! Stephen instance in short-lex order.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns a range object
    //! containing all words accepted by a Stephen instance in short-lex order.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A range object containing all words accepted by \p s
    //! in short-lex order.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \note
    //! The interpretation of results returned by this function differs between
    //! \c Stephen<Presentation> and \c Stephen<InversePresentation>."
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::accepts
    template <typename PresentationType>
    auto words_accepted(Stephen<PresentationType>& s) {
      s.run();
      Paths paths(s.word_graph());
      return paths.source(s.initial_state()).target(s.accept_state());
    }

    //! \brief Returns a range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns a range object
    //! containing all the words (in short-lex order) that are left factors of
    //! Stephen::word.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::is_left_factor
    template <typename PresentationType>
    auto left_factors(Stephen<PresentationType>& s) {
      s.run();
      Paths paths(s.word_graph());
      return paths.source(s.initial_state());
    }

    //! \brief Returns the number of words accepted with length in a given
    //! range.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns the the number of paths
    //! in Stephen::word_graph with source \c 0, target Stephen::accept_state,
    //! and length in the range \p min to \p max.
    //!
    //! For a \c Stephen<Presentation> instance this is the same as the number
    //! of words are equivalent to Stephen::word with length between \p min and
    //! \p max.
    //!
    //! For a \c Stephen<InversePresentation> instance this is the same as the
    //! number of words \f$w\f$ such that \f$uu^{-1}w\f$ is equivalent to
    //! \f$u\f$ with length between \p min and \p max, where \f$u\f$ is
    //! Stephen::word.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \note
    //! The interpretation of results returned by this function differs between
    //! \c Stephen<Presentation> and \c Stephen<InversePresentation>."
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::accepts
    // Not noexcept because number_of_paths isn't
    template <typename PresentationType>
    uint64_t number_of_words_accepted(Stephen<PresentationType>& s,
                                      size_t                     min = 0,
                                      size_t max = POSITIVE_INFINITY) {
      s.run();
      using node_type = typename Stephen<PresentationType>::node_type;
      return number_of_paths(
          s.word_graph(), node_type(0), s.accept_state(), min, max);
    }

    //! \brief Returns the number of left factors with length in a given range.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns the the number of left
    //! factors of the Stephen::word in the instance \p s with length between
    //! \p min and \p max. This is the same as the number of paths in
    //! Stephen::word_graph with source \c 0 and length in the range \p min to
    //! \p max.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns A \c uint64_t.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word .
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::is_left_factor
    // Not noexcept because number_of_paths isn't
    template <typename PresentationType>
    uint64_t number_of_left_factors(Stephen<PresentationType>& s,
                                    size_t                     min = 0,
                                    size_t max = POSITIVE_INFINITY) {
      s.run();
      return number_of_paths(s.word_graph(), 0, min, max);
    }

    //! \brief Returns a \ref Dot object representing the Stephen word graph.
    //!
    //! This function returns a \ref Dot object representing the underlying word
    //! graph of the Stephen instance \p s.
    //!
    //! \tparam PresentationType the type of presentation defining the Stephen
    //! instance, must be derived from \ref PresentationBase.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A \ref Dot object.
    template <typename PresentationType>
    Dot dot(Stephen<PresentationType>& s);

    //! \brief Set the initial word.
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \param w a const reference to the input word.
    //!
    //! \param s the Stephen instance.
    //! \returns A reference to \c this.
    //!
    //! \throws LibsemigroupsException if the letters in \p w do not all belong
    //! to the alphabet of the \ref presentation.
    // things?
    template <typename PresentationType>
    Stephen<PresentationType>& set_word(Stephen<PresentationType>& s,
                                        word_type const&           w) {
      return s.set_word(w.cbegin(), w.cend());
    }

    //! \brief Set the initial word (no checks).
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A reference to \c this.
    //!
    //! \warning
    //! This function does not argument checking whatsoever. It assumes that all
    //! letters of \p w belong to the alphabet of \ref presentation. Bad things
    //! may happen if this assumption does not hold.
    template <typename PresentationType>
    Stephen<PresentationType>& set_word_no_checks(Stephen<PresentationType>& s,
                                                  word_type const& w) {
      return s.set_word_no_checks(w.cbegin(), w.cend());
    }

  }  // namespace stephen

  //! \brief Check equality of two Stephen instances.
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \param x a Stephen instance.
  //! \param y a Stephen instance.
  //!
  //! \returns A \c const_iterator_left_factors.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set with
  //! Stephen::set_word .
  //!
  //! \throws LibsemigroupsException if the presentations for \p x and \p y
  //! differ.
  //!
  //! \cong_common_warn_undecidable{Stephen}.
  template <typename PresentationType>
  bool operator==(Stephen<PresentationType> const& x,
                  Stephen<PresentationType> const& y) {
    if (x.presentation() != y.presentation()) {
      LIBSEMIGROUPS_EXCEPTION(
          "x.presentation() must equal y.presentation() when comparing "
          "Stephen instances")
    }
    return equal_to_no_checks(x, y);
  }

  //! \brief Check equality of two Stephen instances (no checks).
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \param x a Stephen instance.
  //! \param y a Stephen instance.
  //!
  //! \returns A \c const_iterator_left_factors.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set with
  //! Stephen::set_word .
  //!
  //! \cong_common_warn_undecidable{Stephen}.
  //!
  //! \warning
  //! No checks are made on the validity of the parameters to this function. Bad
  //! things may happen if \p x and \p y have different underlying
  //! presentations.
  template <typename PresentationType>
  bool equal_to_no_checks(Stephen<PresentationType> const& x,
                          Stephen<PresentationType> const& y) {
    return stephen::accepts(const_cast<Stephen<PresentationType>&>(x), y.word())
           && stephen::accepts(const_cast<Stephen<PresentationType>&>(y),
                               x.word());
  }

  //! \brief Return a human readable representation of a Stephen instance.
  //!
  //! Return a human readable representation of a Stephen instance.
  //!
  //! \param x the Stephen instance.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename PresentationType>
  [[nodiscard]] std::string
  to_human_readable_repr(Stephen<PresentationType> const& x);
}  // namespace libsemigroups

#include "stephen.tpp"

#endif  // LIBSEMIGROUPS_STEPHEN_HPP_
