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

    ////////////////////////////////////////////////////////////////////////
    // pilo = Paths In Lex Order
    ////////////////////////////////////////////////////////////////////////

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
        : _edge(0), _edges(), _min(min), _max(max), _nodes(), _word_graph(ptr) {
      _nodes.push_back(source);
      if (_min != 0) {
        ++(*this);
      }
    }

    template <typename Node>
    const_pilo_iterator<Node>::~const_pilo_iterator() = default;

    template <typename Node>
    const_pilo_iterator<Node> const& const_pilo_iterator<Node>::operator++() {
      if (_nodes.empty()) {
        return *this;
      }

      do {
        node_type next;
        std::tie(_edge, next) = _word_graph->next_label_and_target_no_checks(
            _nodes.back(), _edge);
        if (next != UNDEFINED && _edges.size() < _max) {
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
    Node const_pilo_iterator<Node>::source() const noexcept {
      if (!_nodes.empty()) {
        return _nodes.front();
      } else {
        return UNDEFINED;
      }
    }

    template <typename Node>
    Node const_pilo_iterator<Node>::target() const noexcept {
      if (!_nodes.empty()) {
        return _nodes.back();
      } else {
        return UNDEFINED;
      }
    }

    template <typename Node>
    void const_pilo_iterator<Node>::swap(const_pilo_iterator& that) noexcept {
      std::swap(_edge, that._edge);
      std::swap(_edges, that._edges);
      std::swap(_min, that._min);
      std::swap(_max, that._max);
      std::swap(_nodes, that._nodes);
      std::swap(_word_graph, that._word_graph);
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

    ////////////////////////////////////////////////////////////////////////
    // pislo = Paths In Short-Lex Order
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator() noexcept = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(
        const_pislo_iterator const&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(
        const_pislo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pislo_iterator<Node>&
    const_pislo_iterator<Node>::operator=(const_pislo_iterator const&)
        = default;

    template <typename Node>
    const_pislo_iterator<Node>&
    const_pislo_iterator<Node>::operator=(const_pislo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pislo_iterator<Node>::const_pislo_iterator(
        WordGraph<Node> const*                         ptr,
        Node                                           source,
        typename const_pislo_iterator<Node>::size_type min,
        typename const_pislo_iterator<Node>::size_type max)
        : _it(), _max(max) {
      if (min <= max) {
        _it = const_pilo_iterator(ptr, source, min, min);
      }
    }

    template <typename Node>
    const_pislo_iterator<Node>::~const_pislo_iterator() = default;

    template <typename Node>
    const_pislo_iterator<Node> const& const_pislo_iterator<Node>::operator++() {
      node_type const src    = source();
      size_type       length = _it->size();
      ++_it;
      // if _it is at the end of the range of paths in lex order of the current
      // length.
      if (_it == const_pilo_iterator<Node>()) {
        if (length < _max) {
          ++length;
          _it = const_pilo_iterator(&_it.word_graph(), src, length, length);
        }
      }
      return *this;
    }

    template <typename Node>
    void const_pislo_iterator<Node>::swap(const_pislo_iterator& that) noexcept {
      std::swap(_it, that._it);
      std::swap(_max, that._max);
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

    ////////////////////////////////////////////////////////////////////////
    // pstilo = Paths Source Target In Lex Order
    ////////////////////////////////////////////////////////////////////////

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
        : _can_reach_target(),
          _edge(0),
          _edges(),
          _min(min),
          _max(max),
          _nodes(),
          _target(target),
          _word_graph(ptr) {
      _nodes.push_back(source);
      if (_min != 0 || source != _target) {
        ++(*this);
      }
    }

    template <typename Node>
    const_pstilo_iterator<Node>&
    const_pstilo_iterator<Node>::init(WordGraph<Node> const* ptr,
                                      node_type              source,
                                      node_type              target,
                                      size_type              min,
                                      size_type              max) {
      _edge = 0;
      _edges.clear();
      _min   = min;
      _max   = max;
      _nodes = {source};
      if (ptr != _word_graph || target != _target) {
        _can_reach_target.clear();
        _target     = target;
        _word_graph = ptr;
      }
      if (_min != 0 || source != _target) {
        ++(*this);
      }
      return *this;
    }

    template <typename Node>
    const_pstilo_iterator<Node> const&
    const_pstilo_iterator<Node>::operator++() {
      if (_nodes.empty()) {
        return *this;
      }
      init_can_reach_target();

      do {
        node_type next;
        std::tie(_edge, next) = _word_graph->next_label_and_target_no_checks(
            _nodes.back(), _edge);
        if (next != UNDEFINED && _edges.size() < _max) {
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
    Node const_pstilo_iterator<Node>::source() const noexcept {
      if (!_nodes.empty()) {
        return _nodes.front();
      } else {
        return UNDEFINED;
      }
    }

    template <typename Node>
    void
    const_pstilo_iterator<Node>::swap(const_pstilo_iterator& that) noexcept {
      std::swap(_can_reach_target, that._can_reach_target);
      std::swap(_edge, that._edge);
      std::swap(_edges, that._edges);
      std::swap(_min, that._min);
      std::swap(_max, that._max);
      std::swap(_nodes, that._nodes);
      std::swap(_target, that._target);
      std::swap(_word_graph, that._word_graph);
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

    ////////////////////////////////////////////////////////////////////////
    // pstislo = Paths Source Target In Short-Lex Order
    ////////////////////////////////////////////////////////////////////////

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator() noexcept = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator(
        const_pstislo_iterator const&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator(
        const_pstislo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>&
    const_pstislo_iterator<Node>::operator=(const_pstislo_iterator const&)
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>&
    const_pstislo_iterator<Node>::operator=(const_pstislo_iterator&&) noexcept
        = default;

    template <typename Node>
    const_pstislo_iterator<Node>::const_pstislo_iterator(
        WordGraph<node_type> const* ptr,
        node_type                   source,
        node_type                   target,
        size_type                   min,
        size_type                   max)
        : _it(), _max(max), _num(0) {
      if (min <= max) {
        _num = v4::paths::count(*ptr, source, target, min, max);
        if (_num == 0) {
          return;
        }
        auto last = const_pstilo_iterator<node_type>();
        _it       = const_pstilo_iterator(ptr, source, target, min, min);
        // TODO(1) optimize to avoid this, by simply computing the distance from
        // source to target (could use is_reachable)
        while (_it == last && min <= max) {
          _it.init(ptr, source, target, min, min);
          ++min;
        }
        if (min > max) {
          _it = last;
        }
      }
    }

    template <typename Node>
    const_pstislo_iterator<Node>::~const_pstislo_iterator() = default;

    template <typename Node>
    const_pstislo_iterator<Node> const&
    const_pstislo_iterator<Node>::operator++() {
      if (_num == 0) {
        return *this;
      } else if (_num == 1) {
        --_num;
        _it = const_pstilo_iterator<node_type>();
        return *this;
      }
      node_type const src    = source();
      node_type const trgt   = target();
      size_type       length = _it->size();
      ++_it;
      auto last = const_pstilo_iterator<node_type>();
      if (_it == last) {
        // TODO(1) optimize to avoid this, by simply computing the distance from
        // source to target (could use is_reachable)
        do {
          ++length;
          _it.init(&_it.word_graph(), src, trgt, length, length);
        } while (_it == last && length < _max);
      }
      _num--;
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
