//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022-2026 James D. Mitchell
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

#include <cstddef>      // for size_t
#include <cstdint>      // for uint32_t
#include <memory>       // for shared_ptr
#include <string>       // for string
#include <type_traits>  // for is_same_v
#include <utility>      // for move

#include "constants.hpp"             // for Max, UNDEFINED
#include "dot.hpp"                   // for Dot
#include "exception.hpp"             // for LIBSEMIGROUPS_EXCEPTION
#include "is_specialization_of.hpp"  // for is_specialization_of
#include "paths-count.hpp"           // for count
#include "presentation.hpp"          // for Presentation
#include "ranges.hpp"                // for rx
#include "runner.hpp"                // for Runner
#include "types.hpp"                 // for word_type
#include "word-graph.hpp"            // for WordGraph
#include "word-range.hpp"            // for ToString

#include "detail/citow.hpp"         // for citow
#include "detail/fmt.hpp"           // for format
#include "detail/stephen-impl.hpp"  // for StephenImpl, left_...
#include "detail/string.hpp"        // for group_digits

//! \defgroup stephen_group Stephen
//!
//! On this page we describe the functionality in `libsemigroups` relating
//! to Stephen's procedure for finitely presented semigroups and inverse
//! semigroups (and monoids).

namespace libsemigroups {

  namespace detail {
    template <typename Thing>
    struct InternalPresentation;

    template <typename Word>
    struct InternalPresentation<Presentation<Word>> {
      using type = Presentation<word_type>;
    };

    template <typename Word>
    struct InternalPresentation<InversePresentation<Word>> {
      using type = InversePresentation<word_type>;
    };
  }  // namespace detail

  //! \ingroup stephen_group
  //!
  //! \brief For constructing the word graph of left factors of a word in an
  //! f.p. semigroup.
  //!
  //! Defined in `stephen.hpp`.
  //!
  //! This class implements Stephen's procedure for constructing
  //! the WordGraph corresponding to the left factors of a word in a finitely
  //! presented semigroup or a finitely presented inverse semigroup. The
  //! algorithm implemented in this class is closely related to the Todd-Coxeter
  //! algorithm  (as implemented in \ref_todd_coxeter) and originates in
  //! \cite Stephen1987aa.
  //!
  //! \tparam PresentationType must be a specialisation of \ref Presentation or
  //! \ref InversePresentation. If an \ref InversePresentation is supplied, then
  //! the \ref Stephen class will use Stephen's procedure for inverse semigroups
  //! when run. Otherwise Stephen's procedure for general semigroups is used
  //! instead.
  template <typename PresentationType>
  class Stephen
      : public detail::StephenImpl<
            typename detail::InternalPresentation<PresentationType>::type> {
    using internal_presentation_type =
        typename detail::InternalPresentation<PresentationType>::type;
    using StephenImpl_ = detail::StephenImpl<internal_presentation_type>;

    static_assert(
        is_specialization_of_v<PresentationType, Presentation>
            || is_specialization_of_v<PresentationType, InversePresentation>,
        "the template parameter PresentationType must be a specialization of "
        "Presentation or InversePresentation");

   public:
    //! The underlying presentation type.
    using presentation_type = PresentationType;

    //! The return type of the function \ref Stephen::word_graph_no_run.
    using word_graph_type = WordGraph<uint32_t>;

    //! The node type of \ref Stephen::word_graph_type.
    using node_type = word_graph_type::node_type;

    //! The word type of \ref presentation_type.
    using native_word_type = typename PresentationType::word_type;

   private:
    class StephenGraph;  // forward decl

    // Data members
    PresentationType _presentation;
    native_word_type _word;

   public:
    ////////////////////////////////////////////////////////////////////////
    // Constructors + initializers
    ////////////////////////////////////////////////////////////////////////

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

    //! \brief Construct from a presentation (copy).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! copies \p p.
    //!
    //! \param p the presentation.
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    // Not noexcept because allocated memory
    explicit Stephen(PresentationType const& p) : Stephen() {
      init(p);
    }

    //! \brief Construct from a presentation (move).
    //!
    //! Construct an instance from the presentation \p p. This constructor
    //! moves \p p.
    //!
    //! \param p the presentation.
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    explicit Stephen(PresentationType&& p) : Stephen() {
      init(std::move(p));
    }

    // NOTE: StephenImpl still operates as before but there doesn't seem an easy
    // way to have both the Stephen and StephenImpl using shared_ptrs --- to
    // avoid copying the presentation multiple times --- because if a shared_ptr
    // is given to Stephen, then underlying presentation will be copied into
    // StephenImpl anyway. So I opted to remove them from Stephen. This means
    // that when/if we re-add Cutting, we'll need to use StephenImpl objects not
    // Stephen objects to avoid using lots of memory.

    //! \brief Construct from a shared pointer to presentation (copy).
    //!
    //! Construct an instance from the shared pointer \p ptr to a shared
    //! presentation object. This constructor copies \p ptr.
    //!
    //! \param p the std::shared_ptr.
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    //!
    //! \deprecated_warning{function}
    [[deprecated]] explicit Stephen(std::shared_ptr<PresentationType> const& p)
        : Stephen() {
      init(*p);
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
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    Stephen& init(PresentationType const& p);

    //! \brief Initialize from a presentation (move).
    //!
    //! This function puts a Stephen instance back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! moves \p p.
    //!
    //! \param p the presentation.
    //!
    //! \returns A reference to \c this.
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    Stephen& init(PresentationType&& p);

    //! \brief Initialize from a presentation (copy).
    //!
    //! This function puts a Stephen instance back into the same state as if it
    //! had been newly constructed from the presentation \p p. This initializer
    //! copies \p p.
    //!
    //! \param p the std::shared_ptr.
    //!
    //! \returns A reference to \c this.
    //!
    //! \deprecated_warning{function}
    //!
    //! \throws if \ref Presentation::throw_if_bad_alphabet_or_rules throws for
    //! \p p or if \p has empty alphabet.
    [[deprecated]] Stephen& init(std::shared_ptr<PresentationType> const& p) {
      init(*p);
      return *this;
    }

    ////////////////////////////////////////////////////////////////////////
    // StephenImpl mem fns
    ////////////////////////////////////////////////////////////////////////

    // NOTE: none of the functions in this section really need to exist, but
    // they are here for the documentation.

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
    //! with Stephen::set_word.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    [[nodiscard]] node_type accept_state() {
      return StephenImpl_::accept_state();
    }

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
    //! \sa Stephen::operator*=.
    void append_no_checks(Stephen<PresentationType>& that) {
      _word.insert(_word.end(), that._word.cbegin(), that._word.cend());
      StephenImpl_::append_no_checks(that);
    }

    //! \brief Check if the initial word is set.
    //!
    //! Returns true if a word has been set with \ref set_word since the last
    //! presentation change and false otherwise.
    //!
    //! \returns A bool.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] bool is_word_set() const noexcept {
      return StephenImpl_::is_word_set();
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
    //! with Stephen::set_word.
    //!
    //! \deprecated_warning{function}
    [[deprecated("Use word_graph_no_run() "
                 "instead!")]] [[nodiscard]] word_graph_type const&
    word_graph() const {
      return StephenImpl_::word_graph_no_run();
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
    //! with Stephen::set_word.
    [[nodiscard]] word_graph_type const& word_graph_no_run() const {
      return StephenImpl_::word_graph_no_run();
    }

    //! \brief Get the initial state of the word graph.
    //!
    //! This function return the initial state of \ref word_graph.
    //!
    //! \returns A \ref node_type.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] static constexpr node_type initial_state() noexcept {
      return StephenImpl_::initial_state();
    }

    ////////////////////////////////////////////////////////////////////////
    // Stephen mem fns
    ////////////////////////////////////////////////////////////////////////

    //! \brief Get the input presentation.
    //!
    //! This function returns a const reference to the input presentation.
    //!
    //! \returns A const reference to a \ref presentation_type.
    //!
    //! \exceptions
    //! \noexcept
    [[nodiscard]] presentation_type const& presentation() const noexcept {
      return _presentation;
    }

    //! \brief Set the initial word.
    //!
    //! This function sets the word whose left factors, or equivalent words, are
    //! sought.
    //!
    //! \tparam Iterator1 the type of the argument \p first.
    //! \tparam Iterator2 the type of the argument \p last.
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

    //! \brief Get the initial word.
    //!
    //! Returns a const reference to the word set by \ref set_word.
    //!
    //! \returns A const reference to a \ref native_word_type.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    [[nodiscard]] native_word_type const& word() const {
      StephenImpl_::throw_if_not_ready();
      return _word;
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
    //! word was set with Stephen::set_word.
    //!
    //! \throws LibsemigroupsException if the presentations for \c this and
    //! \p that differ.
    void operator*=(Stephen<PresentationType>& that);
  };  // class Stephen

  //! \ingroup stephen_group
  //!
  //! \brief Helper functions for the \ref Stephen class.
  //!
  //! Defined in `stephen.hpp`.
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
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \note
    //! The interpretation of results returned by this function differs between
    //! \c Stephen<Presentation> and \c Stephen<InversePresentation>.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the letters in \p w belong to the alphabet of
    //! `s.presentation()`. If the letters of \p w do not belong to the
    //! alphabet of `s.presentation()`, then bad things will happen.
    template <typename PresentationType>
    [[nodiscard]] bool
    accepts_no_checks(Stephen<PresentationType>&                  s,
                      typename PresentationType::word_type const& w);

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
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \throws LibsemigroupsException if the word \p w is not defined over the
    //! same alphabet as \ref Stephen::presentation.
    //!
    //! \note
    //! The interpretation of results returned by this function differs between
    //! \c Stephen<Presentation> and \c Stephen<InversePresentation>.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    template <typename PresentationType>
    [[nodiscard]] bool accepts(Stephen<PresentationType>&                  s,
                               typename PresentationType::word_type const& w);

    //! \brief Check if a word is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true
    //! if it labels a path in Stephen::word_graph_no_run with source \c 0.
    //!
    //! A word \p w labels such a path if and only if \p w is a left factor of
    //! Stephen::word in the semigroup defined by Stephen::presentation. Note
    //! that this is true for both \c Stephen<Presentation> and
    //! \c Stephen<InversePresentation> instances.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \warning This function does not check its arguments. In particular, it
    //! is assumed that the letters in \p w belong to the alphabet of
    //! `s.presentation()`. If the letters of \p w do not belong to the
    //! alphabet of `s.presentation()`, then bad things will happen.
    template <typename PresentationType>
    [[nodiscard]] bool
    is_left_factor_no_checks(Stephen<PresentationType>&                  s,
                             typename PresentationType::word_type const& w);

    //! \brief Check if a word is a left factor of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already), and then returns \c true
    //! if it labels a path in Stephen::word_graph_no_run with source \c 0.
    //!
    //! A word \p w labels such a path if and only if \p w is a left factor of
    //! Stephen::word in the semigroup defined by Stephen::presentation. Note
    //! that this is true for both \c Stephen<Presentation> and
    //! \c Stephen<InversePresentation> instances.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A \c bool.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \throws LibsemigroupsException if the word \p w is not defined over the
    //! same alphabet as \ref Stephen::presentation.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    template <typename PresentationType>
    [[nodiscard]] bool
    is_left_factor(Stephen<PresentationType>&                  s,
                   typename PresentationType::word_type const& w);

    //! \brief Returns a range object containing all words accepted by a
    //! Stephen instance in short-lex order.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns a range object
    //! containing all words accepted by a Stephen instance in short-lex order.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation where `PresentationType::word_type` is
    //! either \ref word_type or std::string.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A range object containing all words accepted by \p s
    //! in short-lex order.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \note
    //! The results returned by this function should be interpreted differently
    //! for \c Stephen<Presentation> and \c Stephen<InversePresentation>.
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::accepts
    // TODO explain the note above properly
    template <typename PresentationType>
    [[nodiscard]] auto words_accepted(Stephen<PresentationType>& s) {
      using rx::operator|;
      using native_word_type = typename PresentationType::word_type;
      auto result            = detail::stephen::words_accepted(s);
      if constexpr (std::is_same_v<native_word_type, word_type>) {
        // NOTE: piping "result" into rx::transform below isn't necessary if
        // s.presentation().alphabet() and s.internal_presentation().alphabet()
        // are identical, but we can't do anything else here because then the
        // return type is different (type of "result") versus (type of "result |
        // rx::transform").
        //
        // TODO(1) improve the below to "do nothing" if
        // presentation().alphabet() and internal_presentation().alphabet() are
        // identical. This will require implementing a custom combinator,
        // similar to but not the same as ToWord. This would also remove the
        // requirement to copy everything that result spits out

        // NOTE: we cannot use ToWord here because its output is always
        // normalised, e.g.
        // ~~~
        // ToWord to_word(201_w);
        // REQUIRE(to_word(1120_w) == 2201_w);
        // ~~~
        // 1 -> 2 because "1" is in position "2" of 201_w etc. This is by
        // design in ToWord, but what we want here is that
        // 1 -> 0 because "0" is the value in position "1" and so on.
        return result | rx::transform([&s](auto const& w) {
                 word_type ww;
                 for (auto& letter : w) {
                   ww.push_back(s.presentation().letter_no_checks(letter));
                 }
                 return ww;
               });
      } else {
        static_assert(std::is_same_v<native_word_type, std::string>);
        return result | ToString(s.presentation().alphabet());
      }
    }

    //! \brief Returns a range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns a range object
    //! containing all the words (in short-lex order) that are left factors of
    //! Stephen::word.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation where `PresentationType::word_type` is
    //! either \ref word_type or std::string.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A range object containing all the words (in short-lex
    //! order) that are left factors of Stephen::word.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::is_left_factor
    template <typename PresentationType>
    [[nodiscard]] auto left_factors(Stephen<PresentationType>& s) {
      using rx::operator|;
      using native_word_type = typename PresentationType::word_type;
      auto result            = detail::stephen::left_factors(s);
      if constexpr (std::is_same_v<native_word_type, word_type>) {
        // See the comments above in words_accepted about why we don't use
        // ToWord here.
        return result | rx::transform([&s](auto const& w) {
                 word_type ww;
                 for (auto& letter : w) {
                   ww.push_back(s.presentation().letter_no_checks(letter));
                 }
                 return ww;
               });
      } else {
        static_assert(std::is_same_v<native_word_type, std::string>);
        return result | ToString(s.presentation().alphabet());
      }
    }

    //! \brief Returns the number of words accepted with length in a given
    //! range.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns the number of
    //! paths in Stephen::word_graph_no_run with source \c 0, target
    //! Stephen::accept_state, and length in the range \p min to \p max.
    //!
    //! For a \c Stephen<Presentation> instance this is the same as the number
    //! of words are equivalent to Stephen::word with length between \p min
    //! and \p max.
    //!
    //! For a \c Stephen<InversePresentation> instance this is the same as the
    //! number of words \f$w\f$ such that \f$uu^{-1}w\f$ is equivalent to
    //! \f$u\f$ with length between \p min and \p max, where \f$u\f$ is
    //! Stephen::word.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns The number of words accepted.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
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
    [[nodiscard]] uint64_t
    number_of_words_accepted(Stephen<PresentationType>& s,
                             size_t                     min = 0,
                             size_t max = POSITIVE_INFINITY) {
      s.run();
      using node_type = typename Stephen<PresentationType>::node_type;
      return v4::paths::count(
          s.word_graph_no_run(), node_type(0), s.accept_state(), min, max);
    }

    //! \brief Returns the number of left factors with length in a given range.
    //!
    //! This function triggers the algorithm implemented in this class (if it
    //! hasn't been triggered already) and then returns the number of left
    //! factors of the Stephen::word in the instance \p s with length between
    //! \p min and \p max. This is the same as the number of paths in
    //! Stephen::word_graph_no_run with source \c 0 and length in the range
    //! \p min to \p max.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param min the minimum length of a word (default: 0).
    //! \param max one more than the maximum length of a word (default:
    //! POSITIVE_INFINITY).
    //!
    //! \returns The number of left factors of \ref Stephen::word.
    //!
    //! \throws LibsemigroupsException if no presentation was set at
    //! the construction of \p s or with Stephen::init or if no word was set
    //! with Stephen::set_word.
    //!
    //! \cong_common_warn_undecidable{Stephen}.
    //!
    //! \sa stephen::is_left_factor
    // Not noexcept because number_of_paths isn't
    template <typename PresentationType>
    [[nodiscard]] uint64_t number_of_left_factors(Stephen<PresentationType>& s,
                                                  size_t min = 0,
                                                  size_t max
                                                  = POSITIVE_INFINITY) {
      s.run();
      return v4::paths::count(s.word_graph_no_run(), 0, min, max);
    }

    //! \brief Returns a \ref Dot object representing the Stephen word graph.
    //!
    //! This function returns a \ref Dot object representing the underlying
    //! word graph of the Stephen instance \p s.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //!
    //! \returns A \ref Dot object.
    template <typename PresentationType>
    [[nodiscard]] Dot dot(Stephen<PresentationType>& s);

    //! \brief Set the initial word.
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param w a const reference to the input word.
    //! \param s the Stephen instance.
    //!
    //! \returns A reference to \p s.
    //!
    //! \throws LibsemigroupsException if the letters in \p w do not all
    //! belong to the alphabet of the \ref presentation.
    // TODO(v4) should have return type void for consistency
    template <typename PresentationType>
    Stephen<PresentationType>&
    set_word(Stephen<PresentationType>&                  s,
             typename PresentationType::word_type const& w) {
      return s.set_word(w.cbegin(), w.cend());
    }

    //! \brief Set the initial word (no checks).
    //!
    //! This function can be used to set the word whose left factors, or
    //! equivalent words, are sought. The input word is copied.
    //!
    //! \tparam PresentationType must be a specialisation of \ref Presentation
    //! or \ref InversePresentation.
    //!
    //! \param s the Stephen instance.
    //! \param w a const reference to the input word.
    //!
    //! \returns A reference to \p s.
    //!
    //! \warning
    //! This function does no argument checking whatsoever. It assumes that
    //! all letters of \p w belong to the alphabet of \ref Presentation. Bad
    //! things may happen if this assumption does not hold.
    // TODO(v4) should have return type void for consistency
    template <typename PresentationType>
    Stephen<PresentationType>&
    set_word_no_checks(Stephen<PresentationType>&                  s,
                       typename PresentationType::word_type const& w) {
      return s.set_word_no_checks(w.cbegin(), w.cend());
    }

  }  // namespace stephen

  //! \brief Check equality of two Stephen instances.
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \tparam PresentationType must be a specialisation of \ref Presentation
  //! or \ref InversePresentation.
  //!
  //! \param x a Stephen instance.
  //! \param y a Stephen instance.
  //!
  //! \returns Whether or not the two Stephen instances are equal.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set
  //! with Stephen::set_word.
  //!
  //! \throws LibsemigroupsException if the presentations for \p x and \p y
  //! differ.
  //!
  //! \cong_common_warn_undecidable{Stephen}.
  template <typename PresentationType>
  [[nodiscard]] bool operator==(Stephen<PresentationType> const& x,
                                Stephen<PresentationType> const& y);

  //! \brief Check equality of two Stephen instances (no checks).
  //!
  //! This function triggers a run of the Stephen algorithm of \p x and \p y,
  //! if it hasn't been run already, and then checks that \c x.word() equals
  //! \c y.word() in the underlying semigroup.
  //!
  //! \tparam PresentationType must be a specialisation of \ref Presentation
  //! or \ref InversePresentation.
  //!
  //! \param x a Stephen instance.
  //! \param y a Stephen instance.
  //!
  //! \returns Whether or not the two Stephen instances are equal.
  //!
  //! \throws LibsemigroupsException if no presentation was set at
  //! the construction of \p s or with Stephen::init or if no word was set
  //! with Stephen::set_word.
  //!
  //! \cong_common_warn_undecidable{Stephen}.
  //!
  //! \warning
  //! No checks are made on the validity of the parameters to this function.
  //! Bad things may happen if \p x and \p y have different underlying
  //! presentations.
  // TODO(v4) change params to be references not const references
  template <typename PresentationType>
  [[nodiscard]] bool equal_to_no_checks(Stephen<PresentationType> const& x,
                                        Stephen<PresentationType> const& y) {
    return stephen::accepts(const_cast<Stephen<PresentationType>&>(x), y.word())
           && stephen::accepts(const_cast<Stephen<PresentationType>&>(y),
                               x.word());
  }

  //! \brief Return a human readable representation of a Stephen instance.
  //!
  //! Return a human readable representation of a Stephen instance.
  //!
  //! \tparam PresentationType must be a specialisation of \ref Presentation
  //! or \ref InversePresentation.
  //!
  //! \param x the Stephen instance.
  //!
  //! \returns A std::string containing the representation.
  //!
  //! \exceptions
  //! \no_libsemigroups_except
  template <typename PresentationType>
  [[nodiscard]] std::string
  to_human_readable_repr(Stephen<PresentationType> const& x);
}  // namespace libsemigroups

#include "stephen.tpp"

#endif  // LIBSEMIGROUPS_STEPHEN_HPP_
