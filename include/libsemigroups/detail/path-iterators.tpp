//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2024-2025 James D. Mitchell
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

// This file contains implementations of the iterator classes for
// paths in an WordGraph.

namespace libsemigroups {
  namespace detail {
    template <typename Node>
    const_pilo_iterator<Node>::~const_pilo_iterator() = default;

    template <typename Node>
    const_pilo_iterator<Node>::const_pilo_iterator(const_pilo_iterator const&)
        = default;

    template <typename Node>
    const_pilo_iterator<Node>::const_pilo_iterator() noexcept = default;

    template <typename Node>
    const_pilo_iterator<Node>&
    const_pilo_iterator<Node>::operator=(const_pilo_iterator const&)
        = default;

    template <typename Node>
    const_pilo_iterator<Node>&
    const_pilo_iterator<Node>::operator=(const_pilo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pilo_iterator<Node>::const_pilo_iterator(
        const_pilo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pilo_iterator<Node>::const_pilo_iterator(WordGraph<Node> const* ptr,
                                                   Node      source,
                                                   size_type min,
                                                   size_type max)
        : _edges(),
          _word_graph(ptr),
          _edge(UNDEFINED),
          _min(min),
          _max(max),
          _nodes() {
      if (_min < _max) {
        _nodes.push_back(source);
        if (_min != 0) {
          ++(*this);
        }
      }
    }

    template <typename Node>
    const_pilo_iterator<Node> const& const_pilo_iterator<Node>::operator++() {
      if (_nodes.empty()) {
        return *this;
      } else if (_edge == UNDEFINED) {
        // first call
        _edge = 0;
      }

      do {
        node_type next;
        std::tie(_edge, next) = _word_graph->next_label_and_target_no_checks(
            _nodes.back(), _edge);
        if (next != UNDEFINED && _edges.size() < _max - 1) {
          _nodes.push_back(next);
          _edges.push_back(_edge);
          _edge = 0;
          if (_edges.size() >= _min) {
            return *this;
          }
        } else {
          _nodes.pop_back();
          if (!_edges.empty()) {
            _edge = _edges.back() + 1;
            _edges.pop_back();
          }
        }
      } while (!_nodes.empty());

      return *this;
    }

    template <typename Node>
    void const_pilo_iterator<Node>::swap(const_pilo_iterator& that) noexcept {
      std::swap(_edges, that._edges);
      std::swap(_word_graph, that._word_graph);
      std::swap(_edge, that._edge);
      std::swap(_min, that._min);
      std::swap(_max, that._max);
      std::swap(_nodes, that._nodes);
    }

    // Assert that the forward iterator requirements are met
    static_assert(
        std::is_default_constructible<const_pilo_iterator<size_t>>::value,
        "forward iterator requires default-constructible");
    static_assert(
        std::is_copy_constructible<const_pilo_iterator<size_t>>::value,
        "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pilo_iterator<size_t>>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pilo_iterator<size_t>>::value,
                  "forward iterator requires destructible");

    template <typename Node>
    const_pislo_iterator<Node>::~const_pislo_iterator() = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(
        const_pislo_iterator const&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator() = default;

    template <typename Node>
    const_pislo_iterator<Node>&
    const_pislo_iterator<Node>::operator=(const_pislo_iterator const&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>&
    const_pislo_iterator<Node>::operator=(const_pislo_iterator&&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(const_pislo_iterator&&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(
        WordGraph<Node> const*                         ptr,
        Node                                           source,
        typename const_pislo_iterator<Node>::size_type min,
        typename const_pislo_iterator<Node>::size_type max)
        : _length(min >= max ? UNDEFINED : min),
          _it(),
          _max(max),
          _source(source) {
      if (_length != UNDEFINED) {
        _it = cbegin_pilo(*ptr, source, _length, _length + 1);
      } else {
        _it = cend_pilo(*ptr);
      }
    }

    template <typename Node>
    const_pislo_iterator<Node> const& const_pislo_iterator<Node>::operator++() {
      ++_it;
      if (_it == cend_pilo(_it.word_graph())) {
        if (_length < _max - 1) {
          ++_length;
          _it = cbegin_pilo(_it.word_graph(), _source, _length, _length + 1);
          if (_it == cend_pilo(_it.word_graph())) {
            _length = UNDEFINED;
          }
        } else {
          _length = UNDEFINED;
        }
      }
      return *this;
    }

    template <typename Node>
    void const_pislo_iterator<Node>::swap(const_pislo_iterator& that) noexcept {
      std::swap(_length, that._length);
      std::swap(_it, that._it);
      std::swap(_max, that._max);
      std::swap(_source, that._source);
    }

    static_assert(
        std::is_default_constructible<const_pislo_iterator<size_t>>::value,
        "forward iterator requires default-constructible");
    static_assert(
        std::is_copy_constructible<const_pislo_iterator<size_t>>::value,
        "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pislo_iterator<size_t>>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pislo_iterator<size_t>>::value,
                  "forward iterator requires destructible");

    template <typename Node>
    const_pstilo_iterator<Node>::const_pstilo_iterator() noexcept = default;

    template <typename Node>
    const_pstilo_iterator<Node>::const_pstilo_iterator(
        const_pstilo_iterator const&)
        = default;

    template <typename Node>
    const_pstilo_iterator<Node>::const_pstilo_iterator(
        const_pstilo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pstilo_iterator<Node>&
    const_pstilo_iterator<Node>::operator=(const_pstilo_iterator const&)
        = default;

    template <typename Node>
    const_pstilo_iterator<Node>&
    const_pstilo_iterator<Node>::operator=(const_pstilo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pstilo_iterator<Node>::~const_pstilo_iterator() = default;

    template <typename Node>
    const_pstilo_iterator<Node>::const_pstilo_iterator(
        WordGraph<Node> const* ptr,
        node_type              source,
        node_type              target,
        size_type              min,
        size_type              max)
        : _edges({}),
          _word_graph(ptr),
          _edge(UNDEFINED),
          _min(min),
          _max(max),
          _nodes(),
          _target(target) {
      if (_min < _max) {
        _nodes.push_back(source);
        ++(*this);
      }
    }

    template <typename Node>
    const_pstilo_iterator<Node> const&
    const_pstilo_iterator<Node>::operator++() {
      if (_nodes.empty()) {
        return *this;
      } else if (_edge == UNDEFINED) {
        // first call
        _edge = 0;
        init_can_reach_target();
        if (_min == 0 && _nodes.front() == _target) {
          // special case if the source == target, and we allow words of
          // length 0, then we return the empty word here.
          return *this;
        }
      }

      do {
        node_type next;
        std::tie(_edge, next) = _word_graph->next_label_and_target_no_checks(
            _nodes.back(), _edge);
        if (next != UNDEFINED && _edges.size() < _max - 1) {
          // Avoid infinite loops when we can never reach _target
          if (_can_reach_target[next]) {
            _nodes.push_back(next);
            _edges.push_back(_edge);
            _edge = 0;
            if (_edges.size() >= _min && next == _target) {
              return *this;
            }
          } else {
            _edge++;
          }
        } else {
          _nodes.pop_back();
          if (!_edges.empty()) {
            _edge = _edges.back() + 1;
            _edges.pop_back();
          }
        }
      } while (!_nodes.empty());
      return *this;
    }

    template <typename Node>
    void
    const_pstilo_iterator<Node>::swap(const_pstilo_iterator& that) noexcept {
      std::swap(_edges, that._edges);
      std::swap(_word_graph, that._word_graph);
      std::swap(_edge, that._edge);
      std::swap(_min, that._min);
      std::swap(_max, that._max);
      std::swap(_nodes, that._nodes);
      std::swap(_target, that._target);
    }

    template <typename Node>
    void const_pstilo_iterator<Node>::init_can_reach_target() {
      if (_can_reach_target.empty()) {
        _can_reach_target.resize(_word_graph->number_of_nodes(), false);
        auto ancestors
            = v4::word_graph::ancestors_of_no_checks(*_word_graph, _target);
        std::for_each(ancestors.begin(), ancestors.end(), [this](auto n) {
          _can_reach_target[n] = true;
        });
      }
    }

    static_assert(
        std::is_default_constructible<const_pstilo_iterator<size_t>>::value,
        "forward iterator requires default-constructible");
    static_assert(
        std::is_copy_constructible<const_pstilo_iterator<size_t>>::value,
        "forward iterator requires copy-constructible");
    static_assert(std::is_copy_assignable<const_pstilo_iterator<size_t>>::value,
                  "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pstilo_iterator<size_t>>::value,
                  "forward iterator requires destructible");

    template <typename Node>
    const_pstislo_iterator<Node>::~const_pstislo_iterator() = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator(
        const_pstislo_iterator const&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator() = default;

    template <typename Node>
    const_pstislo_iterator<Node>&
    const_pstislo_iterator<Node>::operator=(const_pstislo_iterator const&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>&
    const_pstislo_iterator<Node>::operator=(const_pstislo_iterator&&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator(
        const_pstislo_iterator&&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node> const&
    const_pstislo_iterator<Node>::operator++() {
      // FIXME(1) the following is a hack to avoid the situation where there are
      // only finitely many paths from _source to _target but there are
      // infinitely many paths rooted at _source. Without the hack this function
      // will run forever in such situations. The fixme is to remove the hack
      // and to properly implement pislo (not using pilo) and pstislo iterators
      // (analogous to pstilo iterator).
      if (_count == 0) {
        _it = _end;
        return *this;
      }

      if (_it.target() != UNDEFINED) {
        ++_it;

        while (_it.target() != _target && _it != _end) {
          ++_it;
        }
        if (_it == _end) {
          _target = UNDEFINED;
        }
      }
      if (_count != POSITIVE_INFINITY) {
        _count--;
      }
      return *this;
    }
    static_assert(
        std::is_default_constructible<const_pstislo_iterator<size_t>>::value,
        "forward iterator requires default-constructible");
    static_assert(
        std::is_copy_constructible<const_pstislo_iterator<size_t>>::value,
        "forward iterator requires copy-constructible");
    static_assert(
        std::is_copy_assignable<const_pstislo_iterator<size_t>>::value,
        "forward iterator requires copy-assignable");
    static_assert(std::is_destructible<const_pstislo_iterator<size_t>>::value,
                  "forward iterator requires destructible");
  }  // namespace detail
}  // namespace libsemigroups
