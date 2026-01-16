//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2023-2026 James D. Mitchell
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

// This file contains the implementations of the functionality declared in
// paths.hpp for iterating through paths in an WordGraph.

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Paths class
  ////////////////////////////////////////////////////////////////////////

  template <typename Node>
  Paths<Node>::Paths(Paths const&) = default;

  template <typename Node>
  Paths<Node>::Paths(Paths&&) = default;

  template <typename Node>
  Paths<Node>& Paths<Node>::operator=(Paths const&) = default;

  template <typename Node>
  Paths<Node>& Paths<Node>::operator=(Paths&&) = default;

  template <typename Node>
  Paths<Node>::~Paths() = default;

  template <typename Node>
  void Paths<Node>::throw_if_source_undefined() const {
    if (_source == UNDEFINED) {
      LIBSEMIGROUPS_EXCEPTION("no source node defined, use the member "
                              "function \"source\" to define the source node");
    }
  }

  template <typename Node>
  bool Paths<Node>::set_iterator_no_checks() const {
    size_t const N = _word_graph->number_of_nodes();

    if (!_current_valid && N != 0) {
      _current_valid = true;
      _position      = 0;
      if (_order == Order::shortlex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstislo(*_word_graph, _source, _target, _min, _max);
          _end     = cend_pstislo(*_word_graph);
        } else {
          _current = cbegin_pislo(*_word_graph, _source, _min, _max);
          _end     = cend_pislo(*_word_graph);
        }
      } else if (_order == Order::lex && _source != UNDEFINED) {
        if (_target != UNDEFINED) {
          _current = cbegin_pstilo(*_word_graph, _source, _target, _min, _max);
          _end     = cend_pstilo(*_word_graph);
        } else {
          _current = cbegin_pilo(*_word_graph, _source, _min, _max);
          _end     = cend_pilo(*_word_graph);
        }
      }
      return true;
    }
    return N != 0;
  }

  template <typename Node>
  uint64_t Paths<Node>::size_hint() const {
    uint64_t num_paths = 0;
    if (_word_graph->number_of_nodes() == 0) {
      return num_paths;
    } else if (_target != UNDEFINED) {
      num_paths = v4::paths::count(*_word_graph, _source, _target, _min, _max);
    } else {
      num_paths = v4::paths::count(*_word_graph, _source, _min, _max);
    }

    if (_current_valid && num_paths != POSITIVE_INFINITY) {
      num_paths -= _position;
    }

    return num_paths;
  }

  template <typename Node>
  Paths<Node>& Paths<Node>::init() {
    _current_valid = false;
    _word_graph    = nullptr;
    _order         = Order::shortlex;
    _max           = static_cast<size_type>(POSITIVE_INFINITY);
    _min           = 0;
    _position      = 0;
    _source        = static_cast<Node>(UNDEFINED);
    _target        = static_cast<Node>(UNDEFINED);
    return *this;
  }

  template <typename Node>
  Node Paths<Node>::current_target() const {
    if (_target != UNDEFINED) {
      return _target;
    } else if (_source == UNDEFINED) {
      return UNDEFINED;
    }
    set_iterator_no_checks();
    // We are enumerating all paths with a given source, and so we return
    // the current target node using the iterator.
    return std::visit([](auto& it) { return it.target(); }, _current);
  }

  template <typename Node>
  template <typename Subclass>
  Subclass& Paths<Node>::order(Subclass* obj, Order val) {
    if (val != Order::shortlex && val != Order::lex) {
      LIBSEMIGROUPS_EXCEPTION(
          "the argument must be Order::shortlex or Order::lex, found {}", val);
    }
    _current_valid &= (val == _order);
    _order = val;
    return *obj;
  }

  template <typename Node>
  std::string to_human_readable_repr(Paths<Node> const& p) {
    using detail::group_digits;

    std::string source_target;
    std::string sep;
    if (p.source() != UNDEFINED) {
      source_target += fmt::format(" source {}", p.source());
      sep = ",";
    }
    if (p.target() != UNDEFINED) {
      source_target += fmt::format("{} target {}", sep, p.target());
      sep = ",";
    }

    return fmt::format(
        "<Paths in {} with{}{} length in [{}, {})>",
        v4::to_human_readable_repr(p.word_graph()),
        source_target,
        sep,
        group_digits(p.min()),
        p.max() == POSITIVE_INFINITY ? "\u221e" : group_digits(p.max() + 1));
  }

}  // namespace libsemigroups

template <typename Node>
struct rx::is_input_range<typename libsemigroups::Paths<Node>>
    : std::true_type {};
