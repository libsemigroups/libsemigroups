//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2022 James D. Mitchell
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

// This file contains a declaration of a class for performing the "low-index
// congruence" algorithm for semigroups and monoid.

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Sims1Settings
  ////////////////////////////////////////////////////////////////////////

  template <typename Subclass>
  Sims1Settings<Subclass>::Sims1Settings()
      :  // protected
        _exclude(),
        _include(),
        _presentation(),
        // private
        _longs_begin(),
        _num_threads(1),
        _stats() {
    _longs_begin = _presentation.rules.cend();
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::init() {
    _exclude.clear();
    _include.clear();
    _presentation.init();
    _longs_begin = _presentation.rules.cend();
    _num_threads = 1;
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  template <typename OtherSubclass>
  Sims1Settings<Subclass>&
  Sims1Settings<Subclass>::init_from(Sims1Settings<OtherSubclass> const& that) {
    // protected
    _exclude      = that.exclude();
    _include      = that.include();
    _presentation = that.presentation();

    _longs_begin = _presentation.rules.cbegin()
                   + std::distance(that.presentation().rules.cbegin(),
                                   that.cbegin_long_rules());
    // private
    _num_threads = that.number_of_threads();
    _stats       = that.stats();
    return *this;
  }

  template <typename Subclass>
  template <typename PresentationOfSomeKind>
  Subclass&
  Sims1Settings<Subclass>::presentation(PresentationOfSomeKind const& p) {
    static_assert(
        std::is_base_of<PresentationBase, PresentationOfSomeKind>::value,
        "the template parameter PresentationOfSomeKind must be derived from "
        "PresentationBase");
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) must not have 0 generators");
    }
    // This normalises the rules in the case they are of the right type but
    // not normalised
    auto p_copy = to_presentation<word_type>(p);
    p_copy.validate();
    try {
      presentation::validate_rules(
          p_copy, include().cbegin(), include().cend());
      presentation::validate_rules(
          p_copy, exclude().cbegin(), exclude().cend());
    } catch (LibsemigroupsException const& e) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) is not compatible with include() and "
          "exclude(), the following exception was thrown:\n{}",
          e.what());
    }
    _presentation = std::move(p_copy);
    _longs_begin  = _presentation.rules.cend();
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::cbegin_long_rules(
      std::vector<word_type>::const_iterator it) {
    auto const& rules = presentation().rules;
    if (!(rules.cbegin() <= it && it <= rules.cend())) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected an iterator pointing into presentation().rules()");
    } else if (std::distance(it, rules.cend()) % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected an iterator pointing at the left hand side of a rule "
          "(an even distance from the end of the rules), found distance {}",
          std::distance(it, rules.cend()));
    }
    _longs_begin = it;
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::cbegin_long_rules(size_t pos) {
    return cbegin_long_rules(presentation().rules.cbegin() + pos);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::number_of_threads(size_t val) {
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (number of threads) must be non-zero");
    }
    _num_threads = val;
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::long_rule_length(size_t val) {
    presentation::sort_rules(_presentation);
    auto& rules = _presentation.rules;
    auto  it    = rules.cbegin();

    for (; it < rules.cend(); it += 2) {
      if (it->size() + (it + 1)->size() >= val) {
        break;
      }
    }

    _longs_begin = it;
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  template <typename Iterator>
  Subclass& Sims1Settings<Subclass>::include(Iterator first, Iterator last) {
    if (std::distance(first, last) % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION("expected the distance between the 1st and 2nd "
                              "arguments to be even, found {}",
                              std::distance(first, last));
    }
    for (auto it = first; it != last; ++it) {
      presentation().validate_word(it->cbegin(), it->cend());
    }
    _include.assign(first, last);
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::include(word_type const& lhs,
                                             word_type const& rhs) {
    presentation().validate_word(lhs.cbegin(), lhs.cend());
    presentation().validate_word(rhs.cbegin(), rhs.cend());
    _include.push_back(lhs);
    _include.push_back(rhs);
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  template <typename Iterator>
  Subclass& Sims1Settings<Subclass>::exclude(Iterator first, Iterator last) {
    if (std::distance(first, last) % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION("expected the distance between the 1st and 2nd "
                              "arguments to be even, found {}",
                              std::distance(first, last));
    }
    for (auto it = first; it != last; ++it) {
      presentation().validate_word(it->cbegin(), it->cend());
    }
    _exclude.assign(first, last);
    return static_cast<Subclass&>(*this);
  }

  template <typename Subclass>
  Subclass& Sims1Settings<Subclass>::exclude(word_type const& lhs,
                                             word_type const& rhs) {
    presentation().validate_word(lhs.cbegin(), lhs.cend());
    presentation().validate_word(rhs.cbegin(), rhs.cend());
    _exclude.push_back(lhs);
    _exclude.push_back(rhs);
    return static_cast<Subclass&>(*this);
  }
}  // namespace libsemigroups
