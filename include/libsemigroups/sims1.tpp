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

  // TODO replace T -> Subclass

  template <typename T>
  Sims1Settings<T>::Sims1Settings()
      :  // protected
        _exclude(),
        _include(),
        _longs_begin(),
        _presentation(),
        // private
        _num_threads(),
        _stats() {
    _longs_begin = _presentation.rules.cend();
    number_of_threads(1);
  }

  template <typename T>
  template <typename S>
  Sims1Settings<T>::Sims1Settings(Sims1Settings<S> const& that) {
    init(that);
  }

  template <typename T>
  Sims1Settings<T>::Sims1Settings(Sims1Settings<T> const& that) {
    init(that);
  }

  template <typename T>
  template <typename S>
  Sims1Settings<T>& Sims1Settings<T>::init(Sims1Settings<S> const& that) {
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

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::presentation(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    // This normalises the rules in the case they are of the right type but
    // not normalised
    if (p.alphabet().empty()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (Presentation) must not have 0 generators");
    }
    auto normal_p = to_presentation<word_type>(p);
    // TODO validate against include and exclude
    _presentation = normal_p;
    _longs_begin  = _presentation.rules.cend();
    return static_cast<T&>(*this);
  }

  template <typename T>
  T& Sims1Settings<T>::cbegin_long_rules(
      std::vector<word_type>::const_iterator it) {
    auto const& rules = presentation().rules;
    if (!(rules.cbegin() <= it && it <= rules.cend())) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected an iterator pointing into presentation().rules()");
    } else if (std::distance(it, rules.cend()) % 2 != 0) {
      LIBSEMIGROUPS_EXCEPTION(
          "expected an iterator pointing at the left hand side of a rule "
          "(an "
          "even distance from the end of the rules), found distance {}",
          std::distance(it, rules.cend()));
    }
    _longs_begin = it;
    return static_cast<T&>(*this);
  }

  template <typename T>
  T& Sims1Settings<T>::cbegin_long_rules(size_t pos) {
    return cbegin_long_rules(presentation().rules.cbegin() + pos);
  }

  template <typename T>
  T& Sims1Settings<T>::number_of_threads(size_t val) {
    if (val == 0) {
      LIBSEMIGROUPS_EXCEPTION("the argument (size_t) must be non-zero");
    }
    _num_threads = val;
    return static_cast<T&>(*this);
  }

  template <typename T>
  T& Sims1Settings<T>::long_rule_length(size_t val) {
    presentation::sort_rules(_presentation);
    auto& rules = _presentation.rules;
    auto  it    = rules.cbegin();

    for (; it < rules.cend(); it += 2) {
      if (it->size() + (it + 1)->size() >= val) {
        break;
      }
    }

    _longs_begin = it;
    return static_cast<T&>(*this);
  }

  template <typename T>
  void Sims1Settings<T>::validate_presentation(
      Presentation<word_type> const& arg,
      Presentation<word_type> const& existing) {
    if (!arg.alphabet().empty() && !existing.alphabet().empty()
        && arg.alphabet() != existing.alphabet()) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument (a presentation) is not defined over "
          "the correct alphabet, expected alphabet {} got {}",
          existing.alphabet(),
          arg.alphabet());
    }
    arg.validate();
  }
}  // namespace libsemigroups
