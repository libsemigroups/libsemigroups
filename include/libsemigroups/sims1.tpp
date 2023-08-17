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
      : _exclude(),
        _include(),
        _longs(),
        _num_threads(),
        _report_interval(),
        _shorts(),
        _stats() {
    number_of_threads(1);
    report_interval(999);
  }

  template <typename T>
  template <typename S>
  Sims1Settings<T>::Sims1Settings(Sims1Settings<S> const& that)
      : _exclude(that.exclude()),
        _include(that.include()),
        _longs(that.long_rules()),
        _num_threads(that.number_of_threads()),
        _report_interval(that.report_interval()),
        _shorts(that.short_rules()),
        _stats(that.stats()) {}

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::short_rules(P const& p) {
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
    validate_presentation(normal_p, long_rules());
    validate_presentation(normal_p, include());
    _shorts = normal_p;
    return static_cast<T&>(*this);
  }

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::long_rules(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    // We call make in the next two lines to ensure that the generators of
    // the presentation are {0, ..., n - 1} where n is the size of the
    // alphabet.
    auto normal_p = to_presentation<word_type>(p);
    validate_presentation(normal_p, short_rules());
    validate_presentation(normal_p, include());
    _longs = normal_p;
    return static_cast<T&>(*this);
  }

  template <typename T>
  template <typename P>
  T& Sims1Settings<T>::include(P const& p) {
    static_assert(std::is_base_of<PresentationBase, P>::value,
                  "the template parameter P must be derived from "
                  "PresentationBase");
    auto normal_p = to_presentation<word_type>(p);
    validate_presentation(normal_p, short_rules());
    validate_presentation(normal_p, long_rules());
    _include = normal_p;
    return static_cast<T&>(*this);
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
    auto partition = [&val](auto first, auto last) {
      for (; first != last; first += 2) {
        if (first->size() + (first + 1)->size() >= val) {
          break;
        }
      }
      if (first == last) {
        return first;
      }

      for (auto lhs = first + 2; lhs < last; lhs += 2) {
        auto rhs = lhs + 1;
        if (lhs->size() + rhs->size() < val) {
          std::iter_swap(lhs, first++);
          std::iter_swap(rhs, first++);
        }
      }
      return first;
    };

    // points at the lhs of the first rule of length at least val
    auto its = partition(_shorts.rules.begin(), _shorts.rules.end());
    _longs.rules.insert(_longs.rules.end(),
                        std::make_move_iterator(its),
                        std::make_move_iterator(_shorts.rules.end()));
    auto lastl = _longs.rules.end() - std::distance(its, _shorts.rules.end());
    _shorts.rules.erase(its, _shorts.rules.end());

    // points at the lhs of the first rule of length at least val
    auto itl = partition(_longs.rules.begin(), lastl);
    _shorts.rules.insert(_shorts.rules.end(),
                         std::make_move_iterator(_longs.rules.begin()),
                         std::make_move_iterator(itl));
    _longs.rules.erase(_longs.rules.begin(), itl);
    return static_cast<T&>(*this);
  }

  template <typename T>
  Sims1Settings<T>& Sims1Settings<T>::split_at(size_t val) {
    if (val > _shorts.rules.size() / 2 + _longs.rules.size() / 2) {
      LIBSEMIGROUPS_EXCEPTION("expected a value in the range [0, {}), found {}",
                              _shorts.rules.size() / 2
                                  + _longs.rules.size() / 2,
                              val);
    }

    val *= 2;
    if (val < _shorts.rules.size()) {
      _longs.rules.insert(_longs.rules.begin(),
                          _shorts.rules.begin() + val,
                          _shorts.rules.end());
      _shorts.rules.erase(_shorts.rules.begin() + val, _shorts.rules.end());
    } else {
      val -= _shorts.rules.size();
      _shorts.rules.insert(_shorts.rules.end(),
                           _longs.rules.begin(),
                           _longs.rules.begin() + val);
      _longs.rules.erase(_longs.rules.begin(), _longs.rules.begin() + val);
    }
    return *this;
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
