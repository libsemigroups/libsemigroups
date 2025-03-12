//
// libsemigroups - C++ library for semigroups and monoids
// Copyright (C) 2019-2025 Finn Smith
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope That it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

namespace libsemigroups {

  ////////////////////////////////////////////////////////////////////////
  // Konieczny - constructors and initialisers
  ////////////////////////////////////////////////////////////////////////

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::Konieczny()
      : _adjoined_identity_contained(false),
        _can_accept_generators(true),
        _D_classes(),
        _D_rels(),
        _data_initialised(false),
        _degree(UNDEFINED),
        _element_pool(),
        _gens(),
        _group_indices(),
        _group_indices_rev(),
        _lambda_orb(),
        _lambda_to_D_map(),
        _nonregular_reps(),
        _one(),
        _rank_state(nullptr),
        _ranks(),
        _regular_D_classes(),
        _regular_reps(),
        _reps_processed(0),
        _rho_orb(),
        _rho_to_D_map(),
        _run_initialised(false),
        _tmp_lambda_value1(),
        _tmp_lambda_value2(),
        _tmp_rho_value1(),
        _tmp_rho_value2() {
    report_prefix("Konieczny");
    _lambda_orb.cache_scc_multipliers(true);
    _rho_orb.cache_scc_multipliers(true);
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::Konieczny(Konieczny const& that) : Konieczny() {
    *this = that;
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::Konieczny(Konieczny&& that) : Konieczny() {
    *this = std::move(that);
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>&
  Konieczny<Element, Traits>::operator=(Konieczny const& that) {
    Runner::operator=(that);

    free_internals();
    _gens.clear();
    InternalVecCopy()(that._gens, _gens);

    // deal with all the easy data first
    _adjoined_identity_contained = that._adjoined_identity_contained;
    _can_accept_generators       = that._can_accept_generators;
    _data_initialised            = that._data_initialised;
    _degree                      = that._degree;
    _group_indices               = that._group_indices;
    _group_indices_rev           = that._group_indices_rev;
    _lambda_orb                  = that._lambda_orb;
    if (_data_initialised) {
      _one = _gens.back();
    }
    _rank_state      = new rank_state_type(*(that._rank_state));
    _ranks           = that._ranks;
    _reps_processed  = that._reps_processed;
    _rho_orb         = that._rho_orb;
    _run_initialised = that._run_initialised;

    _element_pool.init(_one);

    // Construct the new DClasses, and record a map from the addresses
    // of the old classes to the addresses of the corresponding new classes.
    _D_classes.clear();
    _regular_D_classes.clear();

    std::unordered_map<DClass*, DClass*> DClass_map;

    for (DClass* x : that._D_classes) {
      DClass* y;
      if (x->is_regular_D_class()) {
        y = new RegularDClass(*static_cast<RegularDClass*>(x));
        _regular_D_classes.push_back(static_cast<RegularDClass*>(y));
      } else {
        y = new NonRegularDClass(*static_cast<NonRegularDClass*>(x));
        // the DClass_map for the above regular classes should already be
        // defined, but check to be sure
        static_cast<NonRegularDClass*>(y)->set_left_idem_class(
            static_cast<RegularDClass*>(DClass_map.at(
                static_cast<NonRegularDClass*>(x)->_left_idem_class)));
        static_cast<NonRegularDClass*>(y)->set_right_idem_class(
            static_cast<RegularDClass*>(DClass_map.at(
                static_cast<NonRegularDClass*>(x)->_right_idem_class)));
      }
      y->set_parent(this);
      DClass_map.emplace(x, y);
      _D_classes.push_back(y);
    }

    _D_rels.clear();
    for (std::vector<D_class_index_type> x : that._D_rels) {
      _D_rels.push_back(x);
    }

    _lambda_to_D_map.clear();
    for (auto it = that._lambda_to_D_map.cbegin();
         it != that._lambda_to_D_map.cend();
         ++it) {
      _lambda_to_D_map.insert(std::make_pair(
          it->first, std::vector<D_class_index_type>(it->second)));
    }

    _rho_to_D_map.clear();
    for (auto it = that._rho_to_D_map.cbegin(); it != that._rho_to_D_map.cend();
         ++it) {
      _rho_to_D_map.insert(std::make_pair(
          it->first, std::vector<D_class_index_type>(it->second)));
    }

    // internal reps already freed in free_internals
    _nonregular_reps.clear();
    _regular_reps.clear();

    for (auto v : that._nonregular_reps) {
      _nonregular_reps.push_back(v);
      for (auto x : _nonregular_reps.back()) {
        x._elt = this->internal_copy(x._elt);
      }
    }

    for (auto v : that._regular_reps) {
      _regular_reps.push_back(v);
      for (auto x : _regular_reps.back()) {
        x._elt = this->internal_copy(x._elt);
      }
    }

    return *this;
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>&
  Konieczny<Element, Traits>::operator=(Konieczny&& that) {
    Runner::operator=(that);

    // deal with all the easy data first
    _adjoined_identity_contained = std::move(that._adjoined_identity_contained);
    _can_accept_generators       = std::move(that._can_accept_generators);
    _D_rels                      = std::move(that._D_rels);
    _data_initialised            = std::move(that._data_initialised);
    _degree                      = std::move(that._degree);
    _run_initialised             = std::move(that._run_initialised);
    _group_indices               = std::move(that._group_indices);
    _group_indices_rev           = std::move(that._group_indices_rev);
    _reps_processed              = std::move(that._reps_processed);

    std::swap(_D_classes, that._D_classes);
    for (DClass* x : _D_classes) {
      x->set_parent(this);
    }
    std::swap(
        _regular_D_classes,
        that._regular_D_classes);  // not strictly necessary, but reassuring

    std::swap(_lambda_orb, that._lambda_orb);
    std::swap(_rho_orb, that._rho_orb);
    std::swap(_lambda_to_D_map, that._lambda_to_D_map);
    std::swap(_rho_to_D_map, that._rho_to_D_map);

    std::swap(_nonregular_reps, that._nonregular_reps);
    std::swap(_regular_reps, that._regular_reps);

    std::swap(_gens, that._gens);
    std::swap(_one, that._one);

    std::swap(_rank_state, that._rank_state);
    std::swap(_ranks, that._ranks);  // used in internal_free

    std::swap(_element_pool, that._element_pool);

    return *this;
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>& Konieczny<Element, Traits>::init() {
    Runner::init();

    free_internals();
    _adjoined_identity_contained = false;
    _can_accept_generators       = true;
    _D_classes.clear();
    _D_rels.clear();
    _data_initialised = false;
    _degree           = UNDEFINED;
    _gens.clear();
    _group_indices.clear();
    _group_indices_rev.clear();
    _lambda_orb.init();
    _lambda_to_D_map.clear();
    _nonregular_reps.clear();
    _rank_state = nullptr;
    _ranks.clear();
    _regular_D_classes.clear();
    _regular_reps.clear();
    _reps_processed = 0;
    _rho_orb.init();
    _rho_to_D_map.clear();
    _run_initialised = false;
    _lambda_orb.cache_scc_multipliers(true);
    _rho_orb.cache_scc_multipliers(true);

    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // Konieczny - member functions
  ////////////////////////////////////////////////////////////////////////

  template <typename Element, typename Traits>
  template <typename T>
  void Konieczny<Element, Traits>::add_generators(T const& first,
                                                  T const& last) {
    if (!_can_accept_generators) {
      LIBSEMIGROUPS_EXCEPTION(
          "cannot add generators after the algorithm has begun!");
    }
    validate_element_collection(first, last);
    // If the _one has already been pushed back into _gens
    // (i.e. if data has been initialised), then we should shuffle it to
    // the back of the new generators.
    if (_data_initialised) {
      _gens.pop_back();
    }
    for (auto it = first; it < last; ++it) {
      _gens.push_back(this->internal_copy(this->to_internal_const(*it)));
    }
    if (_data_initialised) {
      _gens.push_back(_one);
    }
    // Once generators are added, the data structures can be initialised.
    // init_data() does nothing if it has already been run.
    init_data();
    // However, the next line does always do something; the rank state and rep
    // vecs depend on the generators.
    init_rank_state_and_rep_vecs();
  }

  template <typename Element, typename Traits>
  template <typename Iterator>
  void
  Konieczny<Element, Traits>::validate_element_collection(Iterator first,
                                                          Iterator last) const {
    if (degree() == UNDEFINED && std::distance(first, last) != 0) {
      auto const n = Degree()(*first);
      for (auto it = first + 1; it < last; ++it) {
        auto const m = Degree()(*it);
        if (m != n) {
          LIBSEMIGROUPS_EXCEPTION(
              "element has degree {} but should have degree {}", n, m);
        }
      }
    } else {
      for (auto it = first; it < last; ++it) {
        validate_element(*it);
      }
    }
  }

  template <typename Element, typename Traits>
  typename Konieczny<Element, Traits>::lambda_orb_index_type
  Konieczny<Element, Traits>::get_lambda_group_index(internal_const_reference x,
                                                     lambda_orb_index_type lpos,
                                                     rho_orb_index_type rpos) {
    lpos = lpos != UNDEFINED ? lpos : get_lpos(x);
    rpos = rpos != UNDEFINED ? rpos : get_rpos(x);
    LIBSEMIGROUPS_ASSERT(lpos != UNDEFINED);

    auto lval_scc_id = _lambda_orb.scc().id(lpos);

    std::pair key(rpos, lval_scc_id);

    if (_group_indices.find(key) != _group_indices.end()) {
      return _group_indices.at(key);
    }

    PoolGuard             cg1(_element_pool);
    PoolGuard             cg2(_element_pool);
    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();

    Product()(this->to_external(tmp1),
              this->to_external_const(x),
              _lambda_orb.multiplier_to_scc_root(lpos));

    for (auto const& val : _lambda_orb.scc().component(lval_scc_id)) {
      Product()(this->to_external(tmp2),
                this->to_external(tmp1),
                _lambda_orb.multiplier_from_scc_root(val));
      if (is_group_index(x, tmp2, lpos, UNDEFINED)) {
        _group_indices.emplace(key, val);
        return val;
      }
    }
    _group_indices.emplace(key, UNDEFINED);
    return UNDEFINED;
  }

  template <typename Element, typename Traits>
  typename Konieczny<Element, Traits>::rho_orb_index_type
  Konieczny<Element, Traits>::get_rho_group_index(internal_const_reference x,
                                                  lambda_orb_index_type    lpos,
                                                  rho_orb_index_type rpos) {
    lpos = lpos != UNDEFINED ? lpos : get_lpos(x);
    rpos = rpos != UNDEFINED ? rpos : get_rpos(x);
    LIBSEMIGROUPS_ASSERT(rpos != UNDEFINED);

    auto rval_scc_id = _rho_orb.scc().id(rpos);

    std::pair key(rval_scc_id, lpos);

    if (_group_indices_rev.find(key) != _group_indices_rev.end()) {
      return _group_indices_rev.at(key);
    }

    PoolGuard cg1(_element_pool);
    PoolGuard cg2(_element_pool);

    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();

    Product()(this->to_external(tmp1),
              _rho_orb.multiplier_to_scc_root(rpos),
              this->to_external_const(x));

    for (auto const& val : _rho_orb.scc().component(rval_scc_id)) {
      Product()(this->to_external(tmp2),
                _rho_orb.multiplier_from_scc_root(val),
                this->to_external(tmp1));
      if (is_group_index(tmp2, x, UNDEFINED, rpos)) {
        _group_indices_rev.emplace(key, val);
        return val;
      }
    }
    _group_indices_rev.emplace(key, UNDEFINED);
    return UNDEFINED;
  }

  // TODO(later): it must be possible to do better than this
  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::idem_in_H_class(
      internal_reference       res,
      internal_const_reference x) const {
    this->to_external(res) = this->to_external_const(x);
    PoolGuard             cg(_element_pool);
    internal_element_type tmp = cg.get();
    do {
      Swap()(this->to_external(res), this->to_external(tmp));
      Product()(this->to_external(res),
                this->to_external_const(tmp),
                this->to_external_const(x));
      Product()(this->to_external(tmp),
                this->to_external_const(res),
                this->to_external_const(res));
    } while (!InternalEqualTo()(res, tmp));
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::make_idem(internal_reference x) {
    LIBSEMIGROUPS_ASSERT(is_regular_element_no_checks(x));
    PoolGuard             cg1(_element_pool);
    internal_element_type tmp1 = cg1.get();

    Product()(this->to_external(tmp1),
              this->to_external_const(x),
              this->to_external_const(x));
    if (EqualTo()(this->to_external(tmp1), this->to_external_const(x))) {
      return;
    }

    Lambda()(_tmp_lambda_value1, this->to_external_const(x));
    auto pos = _lambda_orb.position(_tmp_lambda_value1);

    PoolGuard             cg2(_element_pool);
    internal_element_type tmp2 = cg2.get();

    Product()(this->to_external(tmp1),
              this->to_external_const(x),
              _lambda_orb.multiplier_to_scc_root(pos));

    auto i = get_lambda_group_index(x);
    Product()(this->to_external(tmp2),
              this->to_external(tmp1),
              _lambda_orb.multiplier_from_scc_root(i));

    idem_in_H_class(tmp1, tmp2);
    this->to_external(x) = this->to_external_const(tmp1);
  }

  template <typename Element, typename Traits>
  void
  Konieczny<Element, Traits>::group_inverse(internal_element_type&   res,
                                            internal_const_reference id,
                                            internal_const_reference x) const {
    PoolGuard             cg(_element_pool);
    internal_element_type tmp = cg.get();
    this->to_external(tmp)    = this->to_external_const(x);
    do {
      Swap()(this->to_external(res), this->to_external(tmp));
      Product()(this->to_external(tmp),
                this->to_external_const(res),
                this->to_external_const(x));
    } while (!InternalEqualTo()(tmp, id));
  }

  template <typename Element, typename Traits>
  bool
  Konieczny<Element, Traits>::is_group_index(internal_const_reference x,
                                             internal_const_reference y,
                                             lambda_orb_index_type    lpos,
                                             rho_orb_index_type rpos) const {
    PoolGuard             cg(_element_pool);
    internal_element_type tmp = cg.get();

    Product()(this->to_external(tmp),
              this->to_external_const(y),
              this->to_external_const(x));

    lpos = lpos != UNDEFINED ? lpos : get_lpos(x);
    if (lpos != get_lpos(tmp)) {
      return false;
    }

    rpos = rpos != UNDEFINED ? rpos : get_rpos(y);
    return rpos == get_rpos(tmp);
  }

  template <typename Element, typename Traits>
  typename Konieczny<Element, Traits>::D_class_index_type
  Konieczny<Element, Traits>::get_containing_D_class(internal_const_reference x,
                                                     bool const full_check) {
    if (full_check) {
      rank_type const rnk
          = InternalRank()(_rank_state, this->to_external_const(x));
      run_until([this, rnk]() -> bool { return max_rank() < rnk; });
    }

    Lambda()(_tmp_lambda_value1, this->to_external_const(x));
    Rho()(_tmp_rho_value1, this->to_external_const(x));

    auto lpos = _lambda_orb.position(_tmp_lambda_value1);
    auto rpos = _rho_orb.position(_tmp_rho_value1);
    if (lpos == UNDEFINED || rpos == UNDEFINED) {
      // this should only be possible if this function was called from a
      // public function, and hence full_check is true.
      LIBSEMIGROUPS_ASSERT(full_check);
      return UNDEFINED;
    }
    auto l_it = _lambda_to_D_map.find(lpos);
    auto r_it = _rho_to_D_map.find(rpos);
    if (l_it != _lambda_to_D_map.end() && r_it != _rho_to_D_map.end()) {
      auto l_D_it  = l_it->second.cbegin();
      auto l_D_end = l_it->second.cend();
      auto r_D_it  = r_it->second.cbegin();
      auto r_D_end = r_it->second.cend();
      // the vectors should already be sorted given how we create them
      LIBSEMIGROUPS_ASSERT(std::is_sorted(l_D_it, l_D_end));
      LIBSEMIGROUPS_ASSERT(std::is_sorted(r_D_it, r_D_end));
      while (l_D_it != l_D_end && r_D_it != r_D_end) {
        if (*l_D_it < *r_D_it) {
          ++l_D_it;
        } else {
          if (*r_D_it == *l_D_it) {
            if (full_check) {
              if (_D_classes[*l_D_it]->contains(
                      this->to_external_const(x), lpos, rpos)) {
                return *l_D_it;
              }
            } else {
              if (_D_classes[*l_D_it]->contains_no_checks(x, lpos, rpos)) {
                return *l_D_it;
              }
            }
          }
          ++r_D_it;
        }
      }
    }
    return UNDEFINED;
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::add_to_D_maps(D_class_index_type d) {
    LIBSEMIGROUPS_ASSERT(d < _D_classes.size());
    DClass* D = _D_classes[d];

    {
      auto first = D->cbegin_left_indices();
      auto last  = D->cend_left_indices();

      for (auto it = first; it < last; ++it) {
        _lambda_to_D_map[*it].push_back(d);
      }
    }
    {
      auto first = D->cbegin_right_indices();
      auto last  = D->cend_right_indices();
      for (auto it = first; it < last; ++it) {
        _rho_to_D_map[*it].push_back(d);
      }
    }
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::free_internals() {
    for (DClass* D : _D_classes) {
      delete D;
    }
    // _one is included in _gens
    InternalVecFree()(_gens);
    while (!_ranks.empty()) {
      for (auto& rep_info : _regular_reps[max_rank()]) {
        this->internal_free(rep_info._elt);
      }
      for (auto& rep_info : _nonregular_reps[max_rank()]) {
        this->internal_free(rep_info._elt);
      }
      _ranks.erase(max_rank());
    }
    delete _rank_state;
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::init_run() {
    if (_run_initialised) {
      return;
    }

    // ensure the data is set up correctly
    init_data();

    // no more adding generators
    _can_accept_generators = false;

    // compute orbits (can stop during these enumerations)
    compute_orbs();
    // we might have stopped; then we shouldn't attempt to anything else since
    // the orbs may not have been fully enumerated and hence we cannot compute
    // D classes
    if (stopped()) {
      return;
    }
    // compute the D-class of the adjoined identity and its covering reps
    internal_element_type y   = this->internal_copy(_one);
    RegularDClass*        top = new RegularDClass(this, y);
    add_D_class(top);
    for (auto& rep_info : top->covering_reps()) {
      internal_reference x = rep_info._elt;
      size_t rnk = InternalRank()(_rank_state, this->to_external_const(x));
      _ranks.insert(rnk);
      if (is_regular_element_no_checks(
              x, rep_info._lambda_idx, rep_info._rho_idx)) {
        _regular_reps[rnk].push_back(std::move(rep_info));
      } else {
        _nonregular_reps[rnk].push_back(std::move(rep_info));
      }
    }
    _reps_processed++;
    // Set whether the adjoined one is in the semigroup or not
    // i.e. whether the generators contain multiple elements in the top D
    // class
    bool flag = false;
    for (internal_const_element_type x : _gens) {
      if (_D_classes[0]->contains_no_checks(x)) {
        if (flag) {
          _adjoined_identity_contained = true;
          break;
        } else {
          flag = true;
        }
      }
    }

    _run_initialised = true;
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::init_data() {
    if (_data_initialised) {
      return;
    }
    if (_gens.empty()) {
      LIBSEMIGROUPS_EXCEPTION("no generators have been added!");
    }
    LIBSEMIGROUPS_ASSERT(_degree == UNDEFINED
                         || _degree
                                == Degree()(this->to_external_const(_gens[0])));
    _degree = Degree()(this->to_external_const(_gens[0]));

    element_type x = this->to_external_const(_gens[0]);

    _tmp_lambda_value1 = OneParamLambda()(x);
    _tmp_lambda_value2 = OneParamLambda()(x);

    _tmp_rho_value1 = OneParamRho()(x);
    _tmp_rho_value2 = OneParamRho()(x);

    // if _one is created but not immediately push into _gens
    // it won't be freed if there are exceptions thrown!
    _one = this->to_internal(One()(x));
    _gens.push_back(_one);

    _element_pool.init(_one);

    init_rank_state_and_rep_vecs();

    _data_initialised = true;
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::init_rank_state_and_rep_vecs() {
    if (!_can_accept_generators) {
      // enumeration has already started, so no can't call this anymore
      LIBSEMIGROUPS_EXCEPTION("too late to initialise rank/rep vecs!");
    }

    // We don't necessarily know how to update _rank_state with any
    // new generators, so we will just delete it and create it anew.
    if (_data_initialised) {
      delete _rank_state;
    }

    _rank_state = new rank_state_type(cbegin_generators(), cend_generators());
    LIBSEMIGROUPS_ASSERT((_rank_state == nullptr)
                         == (std::is_void_v<rank_state_type>) );

#ifdef LIBSEMIGROUPS_DEBUG
    // We can safely just replace the vectors without deleting their
    // contents, since we know that the run has not been initialised and
    // they are empty.
    for (auto v : _nonregular_reps) {
      LIBSEMIGROUPS_ASSERT(v.empty());
    }
    for (auto v : _regular_reps) {
      LIBSEMIGROUPS_ASSERT(v.empty());
    }
#endif  // LIBSEMIGROUPS_DEBUG

    _nonregular_reps = std::vector<std::vector<RepInfo>>(
        InternalRank()(_rank_state, this->to_external_const(_one)) + 1,
        std::vector<RepInfo>());

    _regular_reps = std::vector<std::vector<RepInfo>>(
        InternalRank()(_rank_state, this->to_external_const(_one)) + 1,
        std::vector<RepInfo>());
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::compute_orbs() {
    using detail::group_digits;
    using detail::string_time;
    if (_lambda_orb.finished() && _rho_orb.finished()) {
      return;
    }

    report_default("Konieczny: Computing \u03BB- and \u03C1-actions . . .\n");
    if (!_lambda_orb.started()) {
      _lambda_orb.add_seed(OneParamLambda()(this->to_external_const(_one)));
      for (auto const& g : _gens) {
        _lambda_orb.add_generator(this->to_external_const(g));
      }
    }
    if (!_rho_orb.started()) {
      _rho_orb.add_seed(OneParamRho()(this->to_external_const(_one)));
      for (auto const& g : _gens) {
        _rho_orb.add_generator(this->to_external_const(g));
      }
    }
    _lambda_orb.run_until([this]() { return stopped(); });
    _rho_orb.run_until([this]() { return stopped(); });
    auto const time = string_time(delta(start_time()));
    report_default("Konieczny: . . . found {} \u03BB-values and {} "
                   "\u03C1-values in {}\n",
                   group_digits(_lambda_orb.current_size()),
                   group_digits(_rho_orb.current_size()),
                   time);
  }

  // This is a traits class for ConstIteratorStateless in detail/iterator.hpp
  template <typename Element, typename Traits>
  template <typename T>
  struct Konieczny<Element, Traits>::DClassIteratorTraits
      : detail::ConstIteratorTraits<std::vector<T*>> {
    using base_traits_type = detail::ConstIteratorTraits<std::vector<T*>>;

    using internal_iterator_type =
        typename base_traits_type::internal_iterator_type;

    using value_type      = T;
    using reference       = value_type&;
    using const_reference = value_type const&;
    using const_pointer   = value_type const*;
    using pointer         = value_type*;

    struct Deref {
      const_reference
      operator()(internal_iterator_type const& it) const noexcept {
        return **it;
      }
    };

    struct AddressOf {
      const_pointer
      operator()(internal_iterator_type const& it) const noexcept {
        return &(**it);
      }
    };
  };

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::~Konieczny() {
    free_internals();
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::add_D_class(Konieczny::RegularDClass* D) {
    _regular_D_classes.push_back(D);
    _D_classes.push_back(D);
    add_to_D_maps(_D_classes.size() - 1);
    _D_rels.push_back(std::vector<D_class_index_type>());
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::add_D_class(
      Konieczny<Element, Traits>::NonRegularDClass* D) {
    _D_classes.push_back(D);
    add_to_D_maps(_D_classes.size() - 1);
    _D_rels.push_back(std::vector<D_class_index_type>());
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::report_progress() {
    if (!reporting_enabled()) {
      return;
    }

    using detail::group_digits;
    using detail::string_time;

    size_t todo = 0;
    std::for_each(_ranks.cbegin(), _ranks.cend(), [this, &todo](rank_type x) {
      todo += _regular_reps[x].size() + _nonregular_reps[x].size();
    });

    auto const ranks = fmt::format("[{},{})",
                                   group_digits(*_ranks.cbegin()),
                                   group_digits(max_rank() + 1));
    auto const total = string_time(delta(start_time()));

    report_default("Konieczny: {:<5} (D-classes) | {:<11} (elements) | {:<7} "
                   "(todo) | {:<7} (ranks) | {:<7} (total)\n",
                   group_digits(current_number_of_D_classes()),
                   group_digits(current_size()),
                   group_digits(todo),
                   ranks,
                   total);
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::run_impl() {
    if (!running_until() && !running_for() && !finished()) {
      report_default(
          "Konieczny: running until all D-classes are found . . .\n");
    }
    reset_start_time();
    // initialise the required data
    init_run();
    // if we haven't initialised, it should be because we stopped() during
    // init(), and hence we should not continue
    if (!_run_initialised) {
      LIBSEMIGROUPS_ASSERT(stopped());
      return;
    }

    std::vector<RepInfo> next_reps;
    std::vector<RepInfo> tmp_next;

    while (!stopped() && !_ranks.empty()) {
      report_progress();
      LIBSEMIGROUPS_ASSERT(next_reps.empty());
      bool         reps_are_reg = false;
      size_t const mx_rank      = max_rank();
      if (!_regular_reps[mx_rank].empty()) {
        reps_are_reg = true;
        std::swap(next_reps, _regular_reps[mx_rank]);
        _regular_reps[mx_rank].clear();
      } else {
        std::swap(next_reps, _nonregular_reps[mx_rank]);
        _nonregular_reps[mx_rank].clear();
      }

      tmp_next.clear();
      for (auto it = next_reps.begin(); it < next_reps.end(); it++) {
        D_class_index_type i = get_containing_D_class(it->_elt);
        if (i != UNDEFINED) {
          _D_rels[i].push_back(it->_D_idx);
          this->internal_free(it->_elt);
          _reps_processed++;
        } else {
          tmp_next.push_back(*it);
        }
      }
      std::swap(next_reps, tmp_next);

      while (!next_reps.empty()) {
        auto& rep_info = next_reps.back();
        if (reps_are_reg) {
          add_D_class(new RegularDClass(this, rep_info._elt));
        } else {
          add_D_class(new NonRegularDClass(this, rep_info._elt));
        }
        for (auto& rep_info2 : _D_classes.back()->covering_reps()) {
          internal_reference x = rep_info2._elt;
          rank_type          rnk
              = InternalRank()(_rank_state, this->to_external_const(x));
          _ranks.insert(rnk);
          if (is_regular_element_no_checks(
                  x, rep_info2._lambda_idx, rep_info2._rho_idx)) {
            LIBSEMIGROUPS_ASSERT(rnk < mx_rank);
            _regular_reps[rnk].push_back(std::move(rep_info2));
          } else {
            _nonregular_reps[rnk].push_back(std::move(rep_info2));
          }
        }
        next_reps.pop_back();
        _reps_processed++;

        tmp_next.clear();
        for (auto& rep_info2 : next_reps) {
          if (_D_classes.back()->contains_no_checks(
                  rep_info2._elt, rep_info2._lambda_idx, rep_info2._rho_idx)) {
            _D_rels.back().push_back(rep_info2._D_idx);
            this->internal_free(rep_info2._elt);
            _reps_processed++;
          } else {
            tmp_next.push_back(std::move(rep_info2));
          }
        }
        std::swap(next_reps, tmp_next);
      }
      LIBSEMIGROUPS_ASSERT(_regular_reps[mx_rank].empty());
      if (_nonregular_reps[mx_rank].empty()) {
        _ranks.erase(mx_rank);
      }
    }

    report_why_we_stopped();
    if (finished()) {
      report_default("Konieczny: FINISHED!\n");
    }
  }

  /////////////////////////////////////////////////////////////////////////////
  // Konieczny - DClass
  /////////////////////////////////////////////////////////////////////////////

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::DClass::DClass()
      : _class_computed(false),
        _H_class(),
        _H_class_computed(false),
        _is_regular_D_class(),
        _left_indices(),
        _left_mults(),
        _left_mults_inv(),
        _left_reps(),
        _mults_computed(false),
        _parent(),
        _rank(),
        _rep(),
        _reps_computed(false),
        _right_indices(),
        _right_mults(),
        _right_mults_inv(),
        _right_reps(),
        _tmp_internal_set(),
        _tmp_rep_info_vec(),
        _tmp_internal_vec(),
        _tmp_lambda_value(),
        _tmp_rho_value() {}

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::clear() {}

  ////////////////////////////////////////////////////////////////////////
  // DClass - constructors - protected
  ////////////////////////////////////////////////////////////////////////
  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::DClass::DClass(Konieczny*         parent,
                                             internal_reference rep)
      : DClass() {
    _parent = parent;
    _rep    = rep;

    _is_regular_D_class = _parent->is_regular_element_no_checks(_rep);
    _rank = InternalRank()(parent->_rank_state, this->to_external_const(_rep));

    // Why are these needed?
    _tmp_lambda_value = OneParamLambda()(this->to_external_const(_rep));
    _tmp_rho_value    = OneParamRho()(this->to_external_const(_rep));
  }

  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::DClass::DClass(DClass const& that) : DClass() {
    *this = that;
  }

  template <typename Element, typename Traits>
  typename Konieczny<Element, Traits>::DClass&
  Konieczny<Element, Traits>::DClass::operator=(DClass const& that) {
    _class_computed     = that._class_computed;
    _H_class_computed   = that._H_class_computed;
    _is_regular_D_class = that._is_regular_D_class;
    _left_indices       = that._left_indices;
    _mults_computed     = that._mults_computed;
    _parent             = that._parent;
    _rank               = that._rank;

    this->internal_free(_rep);
    _rep           = that._parent->internal_copy(that._rep);
    _reps_computed = that._reps_computed;
    _right_indices = that._right_indices;

    InternalVecCopy()(that._H_class, _H_class);
    InternalVecCopy()(that._left_mults, _left_mults);
    InternalVecCopy()(that._left_mults_inv, _left_mults_inv);
    InternalVecCopy()(that._left_reps, _left_reps);
    InternalVecCopy()(that._right_mults, _right_mults);
    InternalVecCopy()(that._right_mults_inv, _right_mults_inv);
    InternalVecCopy()(that._right_reps, _right_reps);
    return *this;
  }

  ////////////////////////////////////////////////////////////////////////
  // DClass - destructor - public
  ////////////////////////////////////////////////////////////////////////
  template <typename Element, typename Traits>
  Konieczny<Element, Traits>::DClass::~DClass() {
    // the user of _tmp_internal_vec/_tmp_internal_set is responsible for
    // freeing any necessary elements
    InternalVecFree()(_H_class);
    InternalVecFree()(_left_mults);
    InternalVecFree()(_left_mults_inv);
    InternalVecFree()(_left_reps);
    this->internal_free(_rep);
    InternalVecFree()(_right_mults);
    InternalVecFree()(_right_mults_inv);
    InternalVecFree()(_right_reps);
  }

  ////////////////////////////////////////////////////////////////////////
  // DClass - member functions - public
  ////////////////////////////////////////////////////////////////////////

  template <typename Element, typename Traits>
  bool Konieczny<Element, Traits>::DClass::contains(const_reference x) {
    Lambda()(_tmp_lambda_value, x);
    Rho()(_tmp_rho_value, x);
    auto lpos = this->parent()->_lambda_orb.position(_tmp_lambda_value);
    auto rpos = this->parent()->_rho_orb.position(_tmp_rho_value);
    return contains(x, lpos, rpos);
  }

  ////////////////////////////////////////////////////////////////////////
  // DClass - containment - protected
  ////////////////////////////////////////////////////////////////////////

  // Returns whether the element \p x belongs to this
  // \f$\mathscr{D}\f$-class.
  //
  // Given an element \p x of the semigroup represented by \c parent, this
  // function returns whether \p x is an element of the
  // \f$\mathscr{D}\f$-class represented by \c this. If \p x is not an
  // element of the semigroup, then the behaviour is undefined.
  // This member function involved computing most of the frame for
  // \c this, if it is not already known.
  template <typename Element, typename Traits>
  bool Konieczny<Element, Traits>::DClass::contains_no_checks(
      internal_const_reference x) {
    Lambda()(_tmp_lambda_value, this->to_external_const(x));
    LIBSEMIGROUPS_ASSERT(this->parent()->_lambda_orb.position(_tmp_lambda_value)
                         != UNDEFINED);

    Rho()(_tmp_rho_value, this->to_external_const(x));
    LIBSEMIGROUPS_ASSERT(this->parent()->_rho_orb.position(_tmp_rho_value)
                         != UNDEFINED);
    return contains_no_checks(
        x,
        this->parent()->_lambda_orb.position(_tmp_lambda_value),
        this->parent()->_rho_orb.position(_tmp_rho_value));
  }

  // Returns whether the element \p x belongs to this
  // \f$\mathscr{D}\f$-class.
  //
  // This overload of DClass::contains_no_checks is provided in order to avoid
  // recalculating the rank of \p x when it is already known.
  template <typename Element, typename Traits>
  bool Konieczny<Element, Traits>::DClass::contains_no_checks(
      internal_const_reference x,
      size_t                   rank) {
    LIBSEMIGROUPS_ASSERT(this->parent()->InternalRank()(_rank_state, x)
                         == rank);
    return (rank == _rank && contains_no_checks(x));
  }

  // Returns whether the element \p x belongs to this
  // \f$\mathscr{D}\f$-class.
  //
  // This overload of DClass::contains_no_checks is provided in order to avoid
  // recalculating the rank, lambda value, and rho value of \p x when they
  // are already known.
  template <typename Element, typename Traits>
  bool Konieczny<Element, Traits>::DClass::contains_no_checks(
      internal_const_reference x,
      size_t                   rank,
      lambda_orb_index_type    lpos,
      rho_orb_index_type       rpos) {
    LIBSEMIGROUPS_ASSERT(this->parent()->InternalRank()(_rank_state, x)
                         == rank);
    return (rank == _rank && contains_no_checks(x, lpos, rpos));
  }

  ////////////////////////////////////////////////////////////////////////
  // DClass - accessor member functions - protected
  ////////////////////////////////////////////////////////////////////////
  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_left_mult(
      internal_const_reference x) {
    _left_mults.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    PoolGuard             cg2(_parent->element_pool());
    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();
    if (_left_reps.size() >= _left_mults.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(_rep),
                this->to_external_const(x));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external(tmp1))
                           == OneParamLambda()(this->to_external_const(
                               _left_reps[_left_mults.size() - 1])));
    }
    if (_left_mults_inv.size() >= _left_mults.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(_rep),
                this->to_external_const(x));
      Product()(this->to_external(tmp2),
                this->to_external(tmp1),
                this->to_external(_left_mults_inv[_left_mults.size() - 1]));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external_const(_rep))
                           == OneParamLambda()(this->to_external(tmp2)));
    }
#endif
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_left_mult_inv(
      internal_const_reference x) {
    _left_mults_inv.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    PoolGuard             cg2(_parent->element_pool());
    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();
    if (_left_reps.size() >= _left_mults_inv.size()) {
      Product()(this->to_external(tmp1),
                this->to_external(_left_reps[_left_mults.size() - 1]),
                this->to_external_const(x));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external_const(_rep))
                           == OneParamLambda()(this->to_external(tmp1)));
    }
    if (_left_mults.size() >= _left_mults_inv.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(_rep),
                this->to_external(_left_mults[_left_mults_inv.size() - 1]));
      Product()(this->to_external(tmp2),
                this->to_external(tmp1),
                this->to_external_const(x));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external_const(_rep))
                           == OneParamLambda()(this->to_external(tmp2)));
    }
#endif
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_right_mult(
      internal_const_reference x) {
    _right_mults.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    PoolGuard             cg2(_parent->element_pool());
    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();
    if (_right_reps.size() >= _right_mults.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(x),
                this->to_external_const(_rep));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external(tmp1))
                           == OneParamRho()(this->to_external_const(
                               _right_reps[_right_mults.size() - 1])));
    }
    if (_right_mults_inv.size() >= _right_mults.size()) {
      Product()(this->to_external(tmp1),
                this->to_external(_right_mults_inv[_right_mults.size() - 1]),
                this->to_external_const(x));
      Product()(this->to_external(tmp2),
                this->to_external(tmp1),
                this->to_external_const(_rep));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external_const(_rep))
                           == OneParamRho()(this->to_external(tmp2)));
    }
#endif
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_right_mult_inv(
      internal_const_reference x) {
    _right_mults_inv.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    PoolGuard             cg2(_parent->element_pool());
    internal_element_type tmp1 = cg1.get();
    internal_element_type tmp2 = cg2.get();
    if (_right_reps.size() >= _right_mults_inv.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(x),
                this->to_external(_right_reps[_right_mults.size() - 1]));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external_const(_rep))
                           == OneParamRho()(this->to_external(tmp1)));
    }
    if (_right_mults.size() >= _right_mults_inv.size()) {
      Product()(this->to_external(tmp1),
                this->to_external_const(x),
                this->to_external(_right_mults[_right_mults_inv.size() - 1]));
      Product()(this->to_external(tmp2),
                this->to_external(tmp1),
                this->to_external_const(_rep));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external_const(_rep))
                           == OneParamRho()(this->to_external(tmp2)));
    }
#endif
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_left_rep(
      internal_const_reference x) {
    _left_reps.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    internal_element_type tmp = cg1.get();
    if (_left_mults.size() >= _left_reps.size()) {
      Product()(this->to_external(tmp),
                this->to_external_const(_rep),
                this->to_external(_left_mults[_left_reps.size() - 1]));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external(tmp))
                           == OneParamLambda()(this->to_external_const(x)));
    }
    if (_left_mults_inv.size() >= _left_reps.size()) {
      Product()(this->to_external(tmp),
                this->to_external_const(x),
                this->to_external(_left_mults_inv[_left_reps.size() - 1]));
      LIBSEMIGROUPS_ASSERT(OneParamLambda()(this->to_external_const(_rep))
                           == OneParamLambda()(this->to_external(tmp)));
    }
#endif
  }

  template <typename Element, typename Traits>
  void Konieczny<Element, Traits>::DClass::push_right_rep(
      internal_const_reference x) {
    _right_reps.push_back(this->internal_copy(x));
#ifdef LIBSEMIGROUPS_DEBUG
    PoolGuard             cg1(_parent->element_pool());
    internal_element_type tmp = cg1.get();
    if (_right_mults.size() >= _right_reps.size()) {
      Product()(this->to_external(tmp),
                this->to_external(_right_mults[_right_reps.size() - 1]),
                this->to_external_const(_rep));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external(tmp))
                           == OneParamRho()(this->to_external_const(x)));
    }
    if (_right_mults_inv.size() >= _right_reps.size()) {
      Product()(this->to_external(tmp),
                this->to_external(_right_mults_inv[_right_reps.size() - 1]),
                this->to_external_const(x));
      LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external_const(_rep))
                           == OneParamRho()(this->to_external(tmp)));
    }
#endif
  }

  template <typename Element, typename Traits>
  std::vector<typename Konieczny<Element, Traits>::RepInfo>&
  Konieczny<Element, Traits>::DClass::covering_reps() {
    compute_frame();
    _tmp_rep_info_vec.clear();
    _tmp_internal_set.clear();
    // not thread safe (like everything else in here)
    D_class_index_type class_nr = _parent->_D_classes.size();
    // TODO(later): how to best decide which side to calculate? One is
    // often faster
    if (_parent->_lambda_orb.size() < _parent->_rho_orb.size()) {
      PoolGuard             cg(_parent->element_pool());
      internal_element_type tmp = cg.get();
      for (left_indices_index_type i = 0; i < _left_reps.size(); ++i) {
        internal_element_type w = _left_reps[i];
        size_t                j = 0;
        for (auto it = _parent->cbegin_internal_generators();
             it < _parent->cend_internal_generators();
             ++it, ++j) {
          Product()(this->to_external(tmp),
                    this->to_external_const(w),
                    this->to_external_const(*it));
          // TODO(later) this is fragile
          lambda_orb_index_type lpos
              = _parent->_lambda_orb.word_graph().target(_left_indices[i], j);
          Rho()(_tmp_rho_value, this->to_external_const(tmp));
          rho_orb_index_type rpos = _parent->_rho_orb.position(_tmp_rho_value);
          if (!contains_no_checks(tmp, lpos, rpos)) {
            if (_tmp_internal_set.find(tmp) == _tmp_internal_set.end()) {
              internal_element_type x = this->internal_copy(tmp);
              _tmp_internal_set.insert(x);
              _tmp_rep_info_vec.emplace_back(class_nr, x, lpos, rpos);
            }
          }
        }
      }
    } else {
      PoolGuard             cg(_parent->element_pool());
      internal_element_type tmp = cg.get();
      for (right_indices_index_type i = 0; i < _right_reps.size(); ++i) {
        internal_element_type z = _right_reps[i];
        size_t                j = 0;
        for (auto it = _parent->cbegin_internal_generators();
             it < _parent->cend_internal_generators();
             ++it, ++j) {
          Product()(this->to_external(tmp),
                    this->to_external_const(*it),
                    this->to_external_const(z));
          // TODO(later) this is fragile
          rho_orb_index_type rpos
              = _parent->_rho_orb.word_graph().target(_right_indices[i], j);
          Lambda()(_tmp_lambda_value, this->to_external_const(tmp));
          lambda_orb_index_type lpos
              = _parent->_lambda_orb.position(_tmp_lambda_value);
          if (!contains_no_checks(tmp, lpos, rpos)) {
            if (_tmp_internal_set.find(tmp) == _tmp_internal_set.end()) {
              internal_element_type x = this->internal_copy(tmp);
              _tmp_internal_set.insert(x);
              _tmp_rep_info_vec.emplace_back(class_nr, x, lpos, rpos);
            }
          }
        }
      }
    }
    return _tmp_rep_info_vec;
  }

  /////////////////////////////////////////////////////////////////////////////
  // RegularDClass
  /////////////////////////////////////////////////////////////////////////////

  // Defined in `konieczny.hpp`.
  //
  // The nested class Konieczny::RegularDClass inherits from DClass and
  // represents a regular \f$\mathscr{D}\f$-class via a frame as
  // computed in %Konieczny's algorithm. See [here] for more details.
  //
  // A RegularDClass cannot be constructed directly, but may be returned by
  // member functions of the parent semigroup.
  //
  // \sa Konieczny, DClass, and NonRegularDClass
  //
  // [here]:  https://link.springer.com/article/10.1007/BF02573672
  template <typename Element, typename Traits>
  class Konieczny<Element, Traits>::RegularDClass
      : public Konieczny<Element, Traits>::DClass {
    // Konieczny is only a friend of RegularDClass so it can call the private
    // constructor
    friend class Konieczny<Element, Traits>;
#ifndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN
    // NonRegularDClass is a friend of RegularDClass so it can access private
    // iterators
    friend class Konieczny<Element, Traits>::NonRegularDClass;
#endif  // ndef LIBSEMIGROUPS_PARSED_BY_DOXYGEN

   private:
    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - aliases - private
    ////////////////////////////////////////////////////////////////////////
    using left_indices_index_type =
        typename RegularDClass::konieczny_type::left_indices_index_type;
    using right_indices_index_type =
        typename RegularDClass::konieczny_type::right_indices_index_type;
    using const_index_iterator =
        typename std::vector<lambda_orb_index_type>::const_iterator;
    using const_internal_iterator =
        typename std::vector<internal_element_type>::const_iterator;

    void clear() {}

    ////////////////////////////////////////////////////////////////////////
    // DClass - constructors - protected
    ////////////////////////////////////////////////////////////////////////
   protected:
    RegularDClass()
        : Konieczny::DClass(),
          _H_gens(),
          _H_gens_computed(false),
          _idem_reps_computed(false),
          _lambda_index_positions(),
          _left_idem_reps(),
          _left_indices_computed(false),
          _rho_index_positions(),
          _right_idem_reps(),
          _right_indices_computed(false) {}

    // Construct from a pointer to a Konieczny object and an element of the
    // semigroup represented by the Konieczny object.
    //
    // The representative \p rep is not copied by the constructor, and so must
    // not be modified by the user after constructing the RegularDClass. The
    // behaviour of RegularDClass when \p rep is not an element of the
    // semigroup represented by \p parent is undefined.
    //
    // \param parent a pointer to the Konieczny object representing the
    // semigroup of which \c this represents a \f$\mathscr{D}\f$-class.
    //
    // \param rep a regular element of the semigroup represented by \p parent.
    //
    // \throws LibsemigroupsException if \p rep is an element of the semigroup
    // represented by \p parent but is not regular.
    RegularDClass(Konieczny* parent, internal_reference rep)
        : Konieczny::DClass(parent, rep),
          _H_gens(),
          _H_gens_computed(false),
          _idem_reps_computed(false),
          _lambda_index_positions(),
          _left_idem_reps(),
          _left_indices_computed(false),
          _rho_index_positions(),
          _right_idem_reps(),
          _right_indices_computed(false) {
      if (!parent->is_regular_element_no_checks(rep)) {
        LIBSEMIGROUPS_EXCEPTION("the representative given should be regular");
      }
      parent->make_idem(this->unsafe_rep());
      compute_frame();
#ifdef LIBSEMIGROUPS_DEBUG
      PoolGuard cg(this->parent()->element_pool());
      auto      tmp = cg.get();
      Product()(this->to_external(tmp), this->rep(), this->rep());
      LIBSEMIGROUPS_ASSERT(EqualTo()(this->to_external(tmp), this->rep()));
#endif
    }

    RegularDClass(RegularDClass const& that) : RegularDClass() {
      *this = that;
    }

    RegularDClass& operator=(RegularDClass const& that) {
      Konieczny<Element, Traits>::DClass::operator=(that);

      _H_gens_computed        = that._H_gens_computed;
      _idem_reps_computed     = that._idem_reps_computed;
      _left_indices_computed  = that._left_indices_computed;
      _right_indices_computed = that._left_indices_computed;

      _lambda_index_positions = that._lambda_index_positions;
      _rho_index_positions    = that._rho_index_positions;
      InternalVecCopy()(that._left_idem_reps, _left_idem_reps);
      InternalVecCopy()(that._right_idem_reps, _right_idem_reps);

      return *this;
    }

    RegularDClass(RegularDClass&&)            = delete;
    RegularDClass& operator=(RegularDClass&&) = delete;

   public:
    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - destructor - public
    ////////////////////////////////////////////////////////////////////////
    virtual ~RegularDClass() {
      // _H_gens is contained in _H_class which is freed in ~DClass
      InternalVecFree()(_left_idem_reps);
      InternalVecFree()(_right_idem_reps);
    }

    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - member functions - public
    ////////////////////////////////////////////////////////////////////////
    using DClass::contains;

    // \copydoc DClass::contains
    bool contains(const_reference       x,
                  lambda_orb_index_type lpos,
                  rho_orb_index_type    rpos) override {
      LIBSEMIGROUPS_ASSERT(
          this->parent()->_lambda_orb.position(OneParamLambda()(x)) == lpos);
      LIBSEMIGROUPS_ASSERT(this->parent()->_rho_orb.position(OneParamRho()(x))
                           == rpos);
      auto l_it = _lambda_index_positions.find(lpos);
      auto r_it = _rho_index_positions.find(rpos);
      if (l_it == _lambda_index_positions.end()
          || r_it == _rho_index_positions.end()) {
        return false;
      }
      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      Product()(this->to_external(tmp1),
                x,
                this->to_external_const(this->left_mults_inv(l_it->second)));
      Product()(this->to_external(tmp2),
                this->to_external_const(this->right_mults_inv(r_it->second)),
                this->to_external(tmp1));
      std::sort(this->H_class().begin(), this->H_class().end(), InternalLess());
      return std::binary_search(this->H_class().cbegin(),
                                this->H_class().cend(),
                                tmp2,
                                InternalLess());
    }

    size_t number_of_idempotents() const override {
      size_t count = 0;

      auto const lfirst = cbegin_left_idem_reps();
      auto const llast  = cend_left_idem_reps();
      auto const rfirst = cbegin_right_idem_reps();
      auto const rlast  = cend_right_idem_reps();

      for (auto lit = lfirst; lit < llast; ++lit) {
        for (auto rit = rfirst; rit < rlast; ++rit) {
          if (this->parent()->is_group_index(*rit, *lit)) {
            count++;
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(count > 0);
      return count;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - containment - private
    ////////////////////////////////////////////////////////////////////////
    using DClass::contains_no_checks;

    // \copydoc DClass::contains_no_checks
    bool contains_no_checks(internal_const_reference x,
                            lambda_orb_index_type    lpos,
                            rho_orb_index_type       rpos) override {
      // the next line is to suppress compiler warnings
      (void) x;
      LIBSEMIGROUPS_ASSERT(this->parent()->_lambda_orb.position(
                               OneParamLambda()(this->to_external_const(x)))
                           == lpos);
      LIBSEMIGROUPS_ASSERT(this->parent()->_rho_orb.position(
                               OneParamRho()(this->to_external_const(x)))
                           == rpos);
      compute_left_indices();
      compute_right_indices();
      return (_lambda_index_positions.find(lpos)
              != _lambda_index_positions.end())
             && (_rho_index_positions.find(rpos) != _rho_index_positions.end());
    }

    // Returns the indices of the L- and R-classes  that \p bm is in.
    //
    // Returns the indices of the L- and R-classes  that \p bm is in,
    // unless bm is not , in which case returns the pair (UNDEFINED,
    // UNDEFINED). Requires computing part of the frame.
    std::pair<lambda_orb_index_type, rho_orb_index_type>
    index_positions(const_reference bm) {
      compute_left_indices();
      compute_right_indices();
      Lambda()(this->tmp_lambda_value(), bm);
      auto l_it = _lambda_index_positions.find(
          this->parent()->_lambda_orb.position(this->tmp_lambda_value()));
      if (l_it != _lambda_index_positions.end()) {
        Rho()(this->tmp_rho_value(), bm);
        auto r_it = _rho_index_positions.find(
            this->parent()->_rho_orb.position(this->tmp_rho_value()));
        if (r_it != _rho_index_positions.end()) {
          return std::make_pair(l_it->second, r_it->second);
        }
      }
      return std::make_pair(UNDEFINED, UNDEFINED);
    }

    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - initialisation member functions - private
    ////////////////////////////////////////////////////////////////////////

    void compute_left_indices() override {
      if (_left_indices_computed) {
        return;
      }
      auto& lorb = this->parent()->_lambda_orb;
      Lambda()(this->tmp_lambda_value(), this->rep());

      lambda_orb_index_type lval_pos = lorb.position(this->tmp_lambda_value());

      for (auto val : lorb.scc().component_of(lval_pos)) {
        _lambda_index_positions.emplace(val, this->left_indices().size());
        this->left_indices().push_back(val);
#ifdef LIBSEMIGROUPS_DEBUG
        PoolGuard cg(this->parent()->element_pool());
        PoolGuard cg2(this->parent()->element_pool());
        auto      tmp  = cg.get();
        auto      tmp2 = cg2.get();
        Product()(this->to_external(tmp),
                  this->parent()->_lambda_orb.multiplier_to_scc_root(lval_pos),
                  this->parent()->_lambda_orb.multiplier_from_scc_root(val));

        Product()(this->to_external(tmp2), this->rep(), this->to_external(tmp));
        LIBSEMIGROUPS_ASSERT(this->parent()->get_lambda_group_index(tmp2)
                             != UNDEFINED);
#endif
      }
#ifdef LIBSEMIGROUPS_DEBUG
      for (lambda_orb_index_type i : this->left_indices()) {
        LIBSEMIGROUPS_ASSERT(i < this->parent()->_lambda_orb.size());
      }
#endif
      this->_left_indices_computed = true;
    }

    void compute_right_indices() override {
      if (_right_indices_computed) {
        return;
      }
      auto& rorb = this->parent()->_rho_orb;

      Rho()(this->tmp_rho_value(), this->rep());
      rho_orb_index_type rval_pos = rorb.position(this->tmp_rho_value());

      for (auto val : rorb.scc().component_of(rval_pos)) {
        _rho_index_positions.emplace(val, this->right_indices().size());
        this->right_indices().push_back(val);
#ifdef LIBSEMIGROUPS_DEBUG
        PoolGuard cg(this->parent()->element_pool());
        PoolGuard cg2(this->parent()->element_pool());
        auto      tmp  = cg.get();
        auto      tmp2 = cg2.get();
        Product()(this->to_external(tmp),
                  this->parent()->_rho_orb.multiplier_from_scc_root(val),
                  this->parent()->_rho_orb.multiplier_to_scc_root(rval_pos));

        Product()(this->to_external(tmp2), this->to_external(tmp), this->rep());
        LIBSEMIGROUPS_ASSERT(this->parent()->get_lambda_group_index(tmp2)
                             != UNDEFINED);
#endif
      }
#ifdef LIBSEMIGROUPS_DEBUG
      for (rho_orb_index_type i : this->right_indices()) {
        LIBSEMIGROUPS_ASSERT(i < this->parent()->_rho_orb.size());
      }
#endif
      this->_right_indices_computed = true;
    }

    void compute_left_mults() override {
      compute_mults();
    }

    void compute_left_mults_inv() override {
      compute_mults();
    }

    void compute_left_reps() override {
      compute_reps();
    }

    void compute_right_mults() override {
      compute_mults();
    }

    void compute_right_mults_inv() override {
      compute_reps();
    }

    void compute_right_reps() override {
      compute_reps();
    }

    void compute_mults() {
      if (this->mults_computed()) {
        return;
      }

      Lambda()(this->tmp_lambda_value(), this->rep());
      Rho()(this->tmp_rho_value(), this->rep());
      lambda_value_type&    lval = this->tmp_lambda_value();
      lambda_orb_index_type lval_pos
          = this->parent()->_lambda_orb.position(lval);
      rho_value_type     rval     = this->tmp_rho_value();
      rho_orb_index_type rval_pos = this->parent()->_rho_orb.position(rval);

      PoolGuard cg(this->parent()->element_pool());
      auto      tmp = cg.get();

      for (auto lidx_it = this->cbegin_left_indices();
           lidx_it < this->cend_left_indices();
           ++lidx_it) {
        Product()(
            this->to_external(tmp),
            this->parent()->_lambda_orb.multiplier_to_scc_root(lval_pos),
            this->parent()->_lambda_orb.multiplier_from_scc_root(*lidx_it));
        this->push_left_mult(tmp);
        Product()(
            this->to_external(tmp),
            this->parent()->_lambda_orb.multiplier_to_scc_root(*lidx_it),
            this->parent()->_lambda_orb.multiplier_from_scc_root(lval_pos));
        this->push_left_mult_inv(tmp);
      }

      for (auto ridx_it = this->cbegin_right_indices();
           ridx_it < this->cend_right_indices();
           ++ridx_it) {
        // push_right_mult and push_right_mult_inv use tmp_element and
        // tmp_element2
        Product()(this->to_external(tmp),
                  this->parent()->_rho_orb.multiplier_from_scc_root(*ridx_it),
                  this->parent()->_rho_orb.multiplier_to_scc_root(rval_pos));
        this->push_right_mult(tmp);
        Product()(this->to_external(tmp),
                  this->parent()->_rho_orb.multiplier_from_scc_root(rval_pos),
                  this->parent()->_rho_orb.multiplier_to_scc_root(*ridx_it));
        this->push_right_mult_inv(tmp);
      }
      this->set_mults_computed(true);
    }

    void compute_reps() {
      if (this->reps_computed()) {
        return;
      }

      compute_mults();

      PoolGuard cg(this->parent()->element_pool());
      auto      tmp = cg.get();

      for (auto it = this->cbegin_left_mults(); it < this->cend_left_mults();
           ++it) {
        Product()(
            this->to_external(tmp), this->rep(), this->to_external_const(*it));
        this->push_left_rep(tmp);
      }

      for (auto it = this->cbegin_right_mults(); it < this->cend_right_mults();
           ++it) {
        Product()(
            this->to_external(tmp), this->to_external_const(*it), this->rep());
        this->push_right_rep(tmp);
      }
      this->set_reps_computed(true);
    }

    void compute_H_gens() {
      if (_H_gens_computed) {
        return;
      }

      // internal vec represents right inverses
      this->internal_vec().clear();

      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      PoolGuard cg3(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      auto      tmp3 = cg3.get();

      for (auto lrep_it = this->cbegin_left_reps();
           lrep_it < this->cend_left_reps();
           ++lrep_it) {
        LIBSEMIGROUPS_ASSERT(OneParamRho()(this->to_external_const(*lrep_it))
                             == OneParamRho()(this->rep()));
        rho_orb_index_type k = this->parent()->get_rho_group_index(*lrep_it);
        LIBSEMIGROUPS_ASSERT(k != UNDEFINED);
        LIBSEMIGROUPS_ASSERT(_rho_index_positions.find(k)
                             != _rho_index_positions.end());
        right_indices_index_type j = _rho_index_positions.at(k);

        // for p a left rep and q an appropriate right rep,
        // find the product of q with the inverse of pq in H_rep
        Product()(this->to_external(tmp1),
                  this->to_external_const(*lrep_it),
                  this->to_external_const(this->cbegin_right_reps()[j]));

        this->parent()->group_inverse(
            tmp3, this->to_internal_const(this->rep()), tmp1);
        Product()(this->to_external(tmp2),
                  this->to_external_const(this->cbegin_right_reps()[j]),
                  this->to_external_const(tmp3));
        this->internal_vec().push_back(this->internal_copy(tmp2));
      }

      this->internal_set().clear();
      for (size_t i = 0; i < this->left_indices().size(); ++i) {
        for (internal_const_reference g : this->parent()->_gens) {
          Product()(this->to_external(tmp1),
                    this->to_external_const(this->cbegin_left_reps()[i]),
                    this->to_external_const(g));
          Lambda()(this->tmp_lambda_value(), this->to_external(tmp1));
          lambda_orb_index_type lpos
              = this->parent()->_lambda_orb.position(this->tmp_lambda_value());
          LIBSEMIGROUPS_ASSERT(lpos != UNDEFINED);
          if (_lambda_index_positions.find(lpos)
              != _lambda_index_positions.end()) {
            left_indices_index_type j = _lambda_index_positions.at(lpos);
            // TODO(later): this j and internal_vec are a bit mysterious
            Product()(this->to_external(tmp2),
                      this->to_external(tmp1),
                      this->to_external_const(this->internal_vec()[j]));
            if (this->internal_set().find(tmp2) == this->internal_set().end()) {
              internal_element_type x = this->internal_copy(tmp2);
              this->internal_set().insert(x);
              _H_gens.push_back(x);
            }
          }
        }
      }
      InternalVecFree()(this->internal_vec());
      this->_H_gens_computed = true;
    }

    void compute_idem_reps() {
      if (_idem_reps_computed) {
        return;
      }
      compute_left_indices();
      compute_right_indices();

      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      PoolGuard cg3(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      auto      tmp3 = cg3.get();

      // TODO(later): use information from the looping through the left
      // indices in the loop through the right indices
      auto lidx_it = this->cbegin_left_indices();
      for (auto lmult_it = this->cbegin_left_mults();
           lmult_it < this->cend_left_mults();
           ++lmult_it, ++lidx_it) {
        Product()(this->to_external(tmp1),
                  this->rep(),
                  this->to_external_const(*lmult_it));

        rho_orb_index_type k
            = this->parent()->get_rho_group_index(tmp1, *lidx_it, UNDEFINED);
        LIBSEMIGROUPS_ASSERT(k != UNDEFINED);
        LIBSEMIGROUPS_ASSERT(_rho_index_positions.find(k)
                             != _rho_index_positions.end());
        size_t rmult_pos = _rho_index_positions.at(k);
        Product()(
            this->to_external(tmp2),
            this->to_external_const(this->cbegin_right_mults()[rmult_pos]),
            this->to_external_const(tmp1));
        this->parent()->idem_in_H_class(tmp3, tmp2);

        _left_idem_reps.push_back(this->internal_copy(tmp3));
      }

      auto ridx_it = this->cbegin_right_indices();
      for (auto rmult_it = this->cbegin_right_mults();
           rmult_it < this->cend_right_mults();
           ++rmult_it, ++ridx_it) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(*rmult_it),
                  this->rep());
        lambda_orb_index_type k
            = this->parent()->get_lambda_group_index(tmp1, UNDEFINED, *ridx_it);
        LIBSEMIGROUPS_ASSERT(k != UNDEFINED);
        LIBSEMIGROUPS_ASSERT(_lambda_index_positions.find(k)
                             != _lambda_index_positions.end());
        size_t lmult_pos = _lambda_index_positions.at(k);
        Product()(
            this->to_external(tmp2),
            this->to_external(tmp1),
            this->to_external_const(this->cbegin_left_mults()[lmult_pos]));

        this->parent()->idem_in_H_class(tmp3, tmp2);

        _right_idem_reps.push_back(this->internal_copy(tmp3));
      }
      this->_idem_reps_computed = true;
    }

    // there should be some way of getting rid of this
    void compute_H_class() override {
      if (this->H_class_computed()) {
        return;
      }
      compute_H_gens();

      LIBSEMIGROUPS_ASSERT(_H_gens.begin() != _H_gens.end());

      this->internal_set().clear();

      for (auto it = _H_gens.begin(); it < _H_gens.end(); ++it) {
        this->internal_set().insert(*it);
        this->push_back_H_class(*it);
      }

      PoolGuard cg(this->parent()->element_pool());
      auto      tmp = cg.get();

      for (size_t i = 0; i < this->size_H_class_no_checks(); ++i) {
        for (internal_const_reference g : _H_gens) {
          Product()(this->to_external(tmp),
                    this->to_external_const(this->H_class_no_checks(i)),
                    this->to_external_const(g));
          if (this->internal_set().find(tmp) == this->internal_set().end()) {
            internal_element_type x = this->internal_copy(tmp);
            this->internal_set().insert(x);
            this->push_back_H_class(x);
          }
        }
      }
      this->set_H_class_computed(true);
    }

    void compute_frame() override {
      if (this->class_computed()) {
        return;
      }
      compute_left_indices();
      compute_right_indices();
      compute_mults();
      compute_reps();
      compute_idem_reps();
      compute_H_gens();
      compute_H_class();
      this->set_class_computed(true);
    }

    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - accessor member functions - private (friend NonRegular)
    ////////////////////////////////////////////////////////////////////////
    const_internal_iterator cbegin_left_idem_reps() const {
      LIBSEMIGROUPS_ASSERT(this->class_computed());
      return _left_idem_reps.cbegin();
    }

    const_internal_iterator cend_left_idem_reps() const {
      LIBSEMIGROUPS_ASSERT(this->class_computed());
      return _left_idem_reps.cend();
    }

    const_internal_iterator cbegin_right_idem_reps() const {
      LIBSEMIGROUPS_ASSERT(this->class_computed());
      return _right_idem_reps.cbegin();
    }

    const_internal_iterator cend_right_idem_reps() const {
      LIBSEMIGROUPS_ASSERT(this->class_computed());
      return _right_idem_reps.cend();
    }

    ////////////////////////////////////////////////////////////////////////
    // RegularDClass - data - private
    ////////////////////////////////////////////////////////////////////////
    std::vector<internal_element_type> _H_gens;
    bool                               _H_gens_computed;
    bool                               _idem_reps_computed;
    std::unordered_map<lambda_orb_index_type, left_indices_index_type>
                                       _lambda_index_positions;
    std::vector<internal_element_type> _left_idem_reps;
    bool                               _left_indices_computed;
    std::unordered_map<rho_orb_index_type, right_indices_index_type>
                                       _rho_index_positions;
    std::vector<internal_element_type> _right_idem_reps;
    bool                               _right_indices_computed;
  };

  /////////////////////////////////////////////////////////////////////////////
  // NonRegularDClass
  /////////////////////////////////////////////////////////////////////////////

  // Defined in `konieczny.hpp`.
  //
  // The nested class Konieczny::NonRegularDClass inherits from DClass
  // and represents a regular \f$\mathscr{D}\f$-class via a frame as
  // computed in %Konieczny's algorithm. See [here] for more details.
  //
  // A NonRegularDClass is defined by a pointer to the corresponding
  // Konieczny object, together with a representative of the
  // \f$\mathscr{D}\f$-class.
  //
  // \sa Konieczny, DClass, and RegularDClass
  //
  // [here]:  https://link.springer.com/article/10.1007/BF02573672
  template <typename Element, typename Traits>
  class Konieczny<Element, Traits>::NonRegularDClass
      : public Konieczny<Element, Traits>::DClass {
    // Konieczny is only a friend of NonRegularDClass so it can call the
    // private constructor
    friend class Konieczny<Element, Traits>;

   private:
    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - aliases - private
    ////////////////////////////////////////////////////////////////////////
    using left_indices_index_type =
        typename NonRegularDClass::konieczny_type::left_indices_index_type;
    using right_indices_index_type =
        typename NonRegularDClass::konieczny_type::right_indices_index_type;
    using internal_set_type = typename DClass::internal_set_type;

    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - constructors - private
    ////////////////////////////////////////////////////////////////////////

    NonRegularDClass()
        : Konieczny::DClass(),
          _H_set(),
          _idems_above_computed(false),
          _lambda_index_positions(),
          _left_idem_above(),
          _left_idem_class(),
          _left_idem_H_class(),
          _left_idem_left_reps(),
          _left_indices_computed(false),
          _rho_index_positions(),
          _right_idem_above(),
          _right_idem_class(),
          _right_idem_H_class(),
          _right_idem_right_reps(),
          _right_indices_computed(false) {}

    // Construct from a pointer to a Konieczny object and an element of the
    // semigroup represented by the Konieczny object.
    //
    // A NonRegularDClass cannot be constructed directly, but may be returned
    // by member functions of the parent semigroup.
    //
    // \param rep an element of the semigroup represented by \p parent.
    //
    // \throws LibsemigroupsException if \p rep is a regular element of the
    // semigroup represented by \p parent.
    NonRegularDClass(Konieczny* parent, internal_reference rep)
        : Konieczny::DClass(parent, rep),
          _H_set(),
          _idems_above_computed(false),
          _lambda_index_positions(),
          _left_idem_above(rep),
          _left_idem_class(),
          _left_idem_H_class(),
          _left_idem_left_reps(),
          _left_indices_computed(false),
          _rho_index_positions(),
          _right_idem_above(rep),
          _right_idem_class(),
          _right_idem_H_class(),
          _right_idem_right_reps(),
          _right_indices_computed(false) {
      if (parent->is_regular_element_no_checks(rep)) {
        LIBSEMIGROUPS_EXCEPTION("NonRegularDClass: the representative "
                                "given should not be idempotent");
      }
      compute_frame();
    }

    NonRegularDClass& operator=(NonRegularDClass const& that) {
      Konieczny<Element, Traits>::DClass::operator=(that);

      _idems_above_computed   = that._idems_above_computed;
      _lambda_index_positions = that._lambda_index_positions;
      _left_idem_class        = that._left_idem_class;
      _left_indices_computed  = that._left_indices_computed;
      _rho_index_positions    = that._rho_index_positions;
      _right_indices_computed = that._right_indices_computed;

      this->internal_free(_left_idem_above);
      this->internal_free(_right_idem_above);
      _left_idem_above  = this->parent()->internal_copy(that._left_idem_above);
      _right_idem_above = this->parent()->internal_copy(that._right_idem_above);

      InternalVecCopy()(that._left_idem_left_reps, _left_idem_left_reps);
      InternalVecCopy()(that._left_idem_H_class, _left_idem_H_class);
      InternalVecCopy()(that._right_idem_H_class, _right_idem_H_class);
      InternalVecCopy()(that._right_idem_right_reps, _right_idem_right_reps);

      construct_H_set();

      // Warning: if you are copying the DClass, there's a good chance you're
      // copying the classes above it too, in which case you will need
      // set_left_idem_class and set_right_idem_class
      _left_idem_class  = that._left_idem_class;
      _right_idem_class = that._right_idem_class;

      return *this;
    }

    NonRegularDClass(NonRegularDClass const& that) : NonRegularDClass() {
      *this = that;
    }

    NonRegularDClass(NonRegularDClass&&)            = delete;
    NonRegularDClass& operator=(NonRegularDClass&&) = delete;

   public:
    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - destructor - public
    ////////////////////////////////////////////////////////////////////////
    virtual ~NonRegularDClass() {
      InternalVecFree()(_left_idem_H_class);
      InternalVecFree()(_right_idem_H_class);
      InternalVecFree()(_left_idem_left_reps);
      InternalVecFree()(_right_idem_right_reps);
    }

    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - member functions - public
    ////////////////////////////////////////////////////////////////////////
    using DClass::contains;

    // \copydoc DClass::contains
    bool contains(const_reference       x,
                  lambda_orb_index_type lpos,
                  rho_orb_index_type    rpos) override {
      return contains_no_checks(this->to_internal_const(x), lpos, rpos);
    }

    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - member functions - private
    ////////////////////////////////////////////////////////////////////////
    using DClass::contains_no_checks;
    bool contains_no_checks(internal_const_reference x,
                            lambda_orb_index_type    lpos,
                            rho_orb_index_type       rpos) override {
      if (_lambda_index_positions.find(lpos) == _lambda_index_positions.end()) {
        return false;
      }
      if (_rho_index_positions.find(rpos) == _rho_index_positions.end()) {
        return false;
      }
      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      for (left_indices_index_type i : _lambda_index_positions[lpos]) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(x),
                  this->to_external_const(this->left_mults_inv(i)));
        for (right_indices_index_type j : _rho_index_positions[rpos]) {
          Product()(this->to_external(tmp2),
                    this->to_external_const(this->right_mults_inv(j)),
                    this->to_external(tmp1));
          if (_H_set.find(tmp2) != _H_set.end()) {
            return true;
          }
        }
      }
      return false;
    }

   private:
    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - initialisation member functions - private
    ////////////////////////////////////////////////////////////////////////
    void compute_frame() override {
      if (this->class_computed()) {
        return;
      }
      find_idems_above();
      compute_H_class();
      compute_mults();
      compute_left_indices();
      compute_right_indices();
      construct_H_set();
      this->set_class_computed(true);
    }

    void find_idems_above() {
      if (_idems_above_computed) {
        return;
      }
      // assumes that all D-classes above this have already been calculated!
      bool      left_found  = false;
      bool      right_found = false;
      PoolGuard cg(this->parent()->element_pool());
      auto      tmp = cg.get();

      for (auto it = this->parent()->_regular_D_classes.rbegin();

           (!left_found || !right_found)
           && it != this->parent()->_regular_D_classes.rend();
           it++) {
        RegularDClass* D = *it;
        if (!left_found) {
          for (auto idem_it = D->cbegin_left_idem_reps();
               idem_it < D->cend_left_idem_reps();
               idem_it++) {
            Product()(this->to_external(tmp),
                      this->rep(),
                      this->to_external_const(*idem_it));
            if (this->to_external(tmp) == this->rep()) {
              _left_idem_above = *idem_it;
              _left_idem_class = D;
              left_found       = true;
              break;
            }
          }
        }

        if (!right_found) {
          for (auto idem_it = D->cbegin_right_idem_reps();
               idem_it < D->cend_right_idem_reps();
               idem_it++) {
            Product()(this->to_external(tmp),
                      this->to_external_const(*idem_it),
                      this->rep());
            if (this->to_external(tmp) == this->rep()) {
              _right_idem_above = *idem_it;
              _right_idem_class = D;
              right_found       = true;
              break;
            }
          }
        }
      }
      LIBSEMIGROUPS_ASSERT(_left_idem_class != NULL);
      LIBSEMIGROUPS_ASSERT(_right_idem_class != NULL);
      _idems_above_computed = true;
#ifdef LIBSEMIGROUPS_DEBUG
      LIBSEMIGROUPS_ASSERT(
          _left_idem_class->contains_no_checks(_left_idem_above));
      LIBSEMIGROUPS_ASSERT(
          _right_idem_class->contains_no_checks(_right_idem_above));
      LIBSEMIGROUPS_ASSERT(left_found && right_found);
      Product()(this->to_external(tmp),
                this->rep(),
                this->to_external(_left_idem_above));
      LIBSEMIGROUPS_ASSERT(EqualTo()(this->to_external(tmp), this->rep()));
      Product()(this->to_external(tmp),
                this->to_external(_right_idem_above),
                this->rep());
      LIBSEMIGROUPS_ASSERT(EqualTo()(this->to_external(tmp), this->rep()));
#endif
    }

    void set_left_idem_class(RegularDClass* x) {
      _left_idem_class = x;
    }

    void set_right_idem_class(RegularDClass* x) {
      _right_idem_class = x;
    }

    void compute_H_class() override {
      if (this->H_class_computed()) {
        return;
      }
      find_idems_above();
      std::pair<lambda_orb_index_type, rho_orb_index_type> left_idem_indices
          = _left_idem_class->index_positions(
              this->to_external(_left_idem_above));
      internal_const_element_type left_idem_left_mult
          = _left_idem_class->cbegin_left_mults()[left_idem_indices.first];
      internal_const_element_type left_idem_right_mult
          = _left_idem_class->cbegin_right_mults()[left_idem_indices.second];

      std::pair<lambda_orb_index_type, rho_orb_index_type> right_idem_indices
          = _right_idem_class->index_positions(
              this->to_external(_right_idem_above));
      internal_const_element_type right_idem_left_mult
          = _right_idem_class->cbegin_left_mults()[right_idem_indices.first];
      internal_const_element_type right_idem_right_mult
          = _right_idem_class->cbegin_right_mults()[right_idem_indices.second];

      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      for (auto it = _left_idem_class->cbegin_H_class();
           it < _left_idem_class->cend_H_class();
           it++) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(left_idem_right_mult),
                  this->to_external_const(*it));
        Product()(this->to_external(tmp2),
                  this->to_external(tmp1),
                  this->to_external_const(left_idem_left_mult));
        _left_idem_H_class.push_back(this->internal_copy(tmp2));
      }

      for (auto it = _right_idem_class->cbegin_H_class();
           it < _right_idem_class->cend_H_class();
           it++) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(right_idem_right_mult),
                  this->to_external_const(*it));
        Product()(this->to_external(tmp2),
                  this->to_external(tmp1),
                  this->to_external_const(right_idem_left_mult));
        _right_idem_H_class.push_back(this->internal_copy(tmp2));
      }

      for (auto it = _left_idem_class->cbegin_left_mults();
           it < _left_idem_class->cend_left_mults();
           it++) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(left_idem_right_mult),
                  _left_idem_class->rep());
        Product()(this->to_external(tmp2),
                  this->to_external(tmp1),
                  this->to_external_const(*it));
        _left_idem_left_reps.push_back(this->internal_copy((tmp2)));
      }

      for (auto it = _right_idem_class->cbegin_right_mults();
           it < _right_idem_class->cend_right_mults();
           it++) {
        Product()(this->to_external(tmp1),
                  this->to_external_const(*it),
                  _right_idem_class->rep());
        Product()(this->to_external(tmp2),
                  this->to_external(tmp1),
                  this->to_external_const(right_idem_left_mult));
        _right_idem_right_reps.push_back(this->internal_copy(tmp2));
      }

      static std::vector<internal_element_type> Hex;
      static std::vector<internal_element_type> xHf;

      for (internal_const_reference s : _left_idem_H_class) {
        Product()(
            this->to_external(tmp1), this->rep(), this->to_external_const(s));
        xHf.push_back(this->internal_copy(tmp1));
      }

      for (internal_const_reference t : _right_idem_H_class) {
        Product()(
            this->to_external(tmp1), this->to_external_const(t), this->rep());
        Hex.push_back(this->internal_copy(tmp1));
      }

      static internal_set_type s;
      this->internal_set().clear();
      for (auto it = Hex.begin(); it < Hex.end(); ++it) {
        if (!this->internal_set().insert(*it).second) {
          this->internal_free(*it);
        }
      }
      Hex.clear();
      Hex.assign(this->internal_set().begin(), this->internal_set().end());

      this->internal_set().clear();
      for (auto it = xHf.begin(); it < xHf.end(); ++it) {
        if (!this->internal_set().insert(*it).second) {
          this->internal_free(*it);
        }
      }
      xHf.clear();
      xHf.assign(this->internal_set().begin(), this->internal_set().end());

      std::sort(Hex.begin(), Hex.end(), InternalLess());
      std::sort(xHf.begin(), xHf.end(), InternalLess());

      this->internal_vec().clear();
      std::set_intersection(Hex.begin(),
                            Hex.end(),
                            xHf.begin(),
                            xHf.end(),
                            std::back_inserter(this->internal_vec()),
                            InternalLess());

      for (auto it = this->internal_vec().cbegin();
           it < this->internal_vec().cend();
           ++it) {
        this->push_back_H_class(this->internal_copy(*it));
      }

      InternalVecFree()(xHf);
      InternalVecFree()(Hex);
      Hex.clear();
      xHf.clear();

      this->set_H_class_computed(true);
    }

    void compute_mults() {
      if (this->mults_computed()) {
        return;
      }
      find_idems_above();
      compute_H_class();
      std::pair<lambda_orb_index_type, rho_orb_index_type> left_idem_indices
          = _left_idem_class->index_positions(
              this->to_external(_left_idem_above));
      LIBSEMIGROUPS_ASSERT(left_idem_indices.first != UNDEFINED
                           && left_idem_indices.second != UNDEFINED);
      internal_const_element_type left_idem_left_mult
          = _left_idem_class->cbegin_left_mults()[left_idem_indices.first];

      std::pair<lambda_orb_index_type, rho_orb_index_type> right_idem_indices
          = _right_idem_class->index_positions(
              this->to_external(_right_idem_above));
      LIBSEMIGROUPS_ASSERT(right_idem_indices.first != UNDEFINED
                           && right_idem_indices.second != UNDEFINED);
      internal_const_element_type right_idem_right_mult
          = _right_idem_class->cbegin_right_mults()[right_idem_indices.second];

      static std::unordered_set<
          std::vector<internal_element_type>,
          Hash<std::vector<internal_element_type>, InternalHash>,
          InternalVecEqualTo>
          Hxhw_set;
      Hxhw_set.clear();

      static std::unordered_set<
          std::vector<internal_element_type>,
          Hash<std::vector<internal_element_type>, InternalHash>,
          InternalVecEqualTo>
          Hxh_set;
      Hxh_set.clear();

      static std::unordered_set<
          std::vector<internal_element_type>,
          Hash<std::vector<internal_element_type>, InternalHash>,
          InternalVecEqualTo>
          zhHx_set;
      zhHx_set.clear();

      static std::unordered_set<
          std::vector<internal_element_type>,
          Hash<std::vector<internal_element_type>, InternalHash>,
          InternalVecEqualTo>
          hHx_set;
      hHx_set.clear();

      PoolGuard cg1(this->parent()->element_pool());
      PoolGuard cg2(this->parent()->element_pool());
      PoolGuard cg3(this->parent()->element_pool());
      PoolGuard cg4(this->parent()->element_pool());
      auto      tmp1 = cg1.get();
      auto      tmp2 = cg2.get();
      auto      tmp3 = cg3.get();
      auto      tmp4 = cg4.get();
      for (internal_const_reference h : _left_idem_H_class) {
        static std::vector<internal_element_type> Hxh;
        LIBSEMIGROUPS_ASSERT(Hxh.empty());
        for (auto it = this->cbegin_H_class(); it < this->cend_H_class();
             ++it) {
          Product()(this->to_external(tmp1),
                    this->to_external_const(*it),
                    this->to_external_const(h));
          Hxh.push_back(this->internal_copy(tmp1));
        }

        std::sort(Hxh.begin(), Hxh.end(), InternalLess());
        if (Hxh_set.find(Hxh) == Hxh_set.end()) {
          for (size_t i = 0; i < _left_idem_left_reps.size(); ++i) {
            static std::vector<internal_element_type> Hxhw;
            Hxhw.clear();

            for (auto it = Hxh.cbegin(); it < Hxh.cend(); ++it) {
              Product()(this->to_external(tmp1),
                        this->to_external_const(*it),
                        this->to_external_const(_left_idem_left_reps[i]));
              Hxhw.push_back(this->internal_copy(tmp1));
            }
            std::sort(Hxhw.begin(), Hxhw.end(), InternalLess());
            if (Hxhw_set.find(Hxhw) == Hxhw_set.end()) {
              Hxhw_set.insert(std::move(Hxhw));

              Product()(this->to_external(tmp3),
                        this->to_external_const(h),
                        this->to_external_const(_left_idem_left_reps[i]));
              Product()(this->to_external(tmp4),
                        this->rep(),
                        this->to_external(tmp3));
              Lambda()(this->tmp_lambda_value(), this->to_external(tmp4));
              lambda_orb_index_type lpos = this->parent()->_lambda_orb.position(
                  this->tmp_lambda_value());
              if (_lambda_index_positions.find(lpos)
                  == _lambda_index_positions.end()) {
                _lambda_index_positions.emplace(
                    lpos, std::vector<left_indices_index_type>());
              }
              _lambda_index_positions[lpos].push_back(
                  this->number_of_left_reps_no_checks());

              // push_left_rep and push_left_mult use tmp_element and
              // tmp_element2
              this->push_left_rep(tmp4);
              this->push_left_mult(tmp3);

              Product()(
                  this->to_external(tmp1),
                  this->to_external_const(_left_idem_left_reps[i]),
                  this->to_external_const(_left_idem_class->left_mults_inv(i)));
              Product()(this->to_external(tmp2),
                        this->to_external(tmp1),
                        this->to_external_const(left_idem_left_mult));
              this->parent()->group_inverse(tmp3, _left_idem_above, tmp2);
              this->parent()->group_inverse(tmp4, _left_idem_above, h);
              Product()(this->to_external(tmp1),
                        this->to_external(tmp3),
                        this->to_external(tmp4));

              Product()(
                  this->to_external(tmp2),
                  this->to_external_const(_left_idem_class->left_mults_inv(i)),
                  this->to_external_const(left_idem_left_mult));
              Product()(this->to_external(tmp3),
                        this->to_external(tmp2),
                        this->to_external(tmp1));
              this->push_left_mult_inv(tmp3);
            } else {
              InternalVecFree()(Hxhw);
            }
          }
          Hxh_set.insert(std::move(Hxh));
        } else {
          InternalVecFree()(Hxh);
        }
        Hxh.clear();
      }

      for (auto it = Hxh_set.begin(); it != Hxh_set.end(); ++it) {
        InternalVecFree()(*it);
      }

      for (auto it = Hxhw_set.begin(); it != Hxhw_set.end(); ++it) {
        InternalVecFree()(*it);
      }

      for (internal_const_reference h : _right_idem_H_class) {
        static std::vector<internal_element_type> hHx;
        LIBSEMIGROUPS_ASSERT(hHx.empty());

        for (auto it = this->cbegin_H_class(); it < this->cend_H_class();
             ++it) {
          Product()(this->to_external(tmp1),
                    this->to_external_const(h),
                    this->to_external_const(*it));
          hHx.push_back(this->internal_copy(tmp1));
        }

        std::sort(hHx.begin(), hHx.end(), InternalLess());
        if (hHx_set.find(hHx) == hHx_set.end()) {
          for (size_t i = 0; i < _right_idem_right_reps.size(); ++i) {
            static std::vector<internal_element_type> zhHx;
            zhHx.clear();
            for (auto it = hHx.cbegin(); it < hHx.cend(); ++it) {
              Product()(this->to_external(tmp1),
                        this->to_external_const(_right_idem_right_reps[i]),
                        this->to_external_const(*it));
              zhHx.push_back(this->internal_copy(tmp1));
            }

            std::sort(zhHx.begin(), zhHx.end(), InternalLess());
            if (zhHx_set.find(zhHx) == zhHx_set.end()) {
              zhHx_set.insert(std::move(zhHx));
              // push_right_rep and push_right_mult use tmp_element and
              // tmp_element2
              Product()(this->to_external(tmp3),
                        this->to_external_const(_right_idem_right_reps[i]),
                        this->to_external_const(h));
              Product()(this->to_external(tmp4),
                        this->to_external(tmp3),
                        this->rep());

              Rho()(this->tmp_rho_value(), this->to_external(tmp4));
              rho_orb_index_type rpos
                  = this->parent()->_rho_orb.position(this->tmp_rho_value());
              if (_rho_index_positions.find(rpos)
                  == _rho_index_positions.end()) {
                _rho_index_positions.emplace(
                    rpos, std::vector<right_indices_index_type>());
              }
              _rho_index_positions[rpos].push_back(
                  this->number_of_right_reps_no_checks());
              this->push_right_rep(tmp4);
              this->push_right_mult(tmp3);

              Product()(this->to_external(tmp1),
                        this->to_external_const(right_idem_right_mult),
                        this->to_external_const(
                            _right_idem_class->right_mults_inv(i)));
              Product()(this->to_external(tmp2),
                        this->to_external(tmp1),
                        this->to_external_const(_right_idem_right_reps[i]));

              this->parent()->group_inverse(tmp3, _right_idem_above, h);
              this->parent()->group_inverse(tmp4, _right_idem_above, tmp2);
              Product()(this->to_external(tmp1),
                        this->to_external(tmp3),
                        this->to_external(tmp4));

              Product()(this->to_external(tmp2),
                        this->to_external_const(right_idem_right_mult),
                        this->to_external_const(
                            _right_idem_class->right_mults_inv(i)));
              Product()(this->to_external(tmp3),
                        this->to_external(tmp1),
                        this->to_external(tmp2));
              this->push_right_mult_inv(tmp3);
            } else {
              InternalVecFree()(zhHx);
            }
          }
          hHx_set.insert(std::move(hHx));
        } else {
          InternalVecFree()(hHx);
        }
        hHx.clear();
      }

      for (auto it = hHx_set.begin(); it != hHx_set.end(); ++it) {
        InternalVecFree()(*it);
      }

      for (auto it = zhHx_set.begin(); it != zhHx_set.end(); ++it) {
        InternalVecFree()(*it);
      }
      this->set_mults_computed(true);
    }

    void construct_H_set() {
      for (auto it = this->cbegin_H_class(); it < this->cend_H_class(); ++it) {
        _H_set.insert(*it);
      }
    }

    void compute_left_mults() override {
      compute_mults();
    }

    void compute_left_mults_inv() override {
      compute_mults();
    }

    void compute_left_reps() override {
      compute_mults();
    }

    void compute_right_mults() override {
      compute_mults();
    }

    void compute_right_mults_inv() override {
      compute_mults();
    }

    void compute_right_reps() override {
      compute_mults();
    }

    void compute_left_indices() override {
      if (_left_indices_computed) {
        return;
      }
      for (auto it = this->cbegin_left_reps(); it != this->cend_left_reps();
           ++it) {
        Lambda()(this->tmp_lambda_value(), this->to_external_const(*it));
        LIBSEMIGROUPS_ASSERT(
            this->parent()->_lambda_orb.position(this->tmp_lambda_value())
            != UNDEFINED);
        this->left_indices().push_back(
            this->parent()->_lambda_orb.position(this->tmp_lambda_value()));
      }
      _left_indices_computed = true;
    }

    void compute_right_indices() override {
      if (_right_indices_computed) {
        return;
      }
      for (auto it = this->cbegin_right_reps(); it != this->cend_right_reps();
           ++it) {
        Rho()(this->tmp_rho_value(), this->to_external_const(*it));
        LIBSEMIGROUPS_ASSERT(
            this->parent()->_rho_orb.position(this->tmp_rho_value())
            != UNDEFINED);
        this->right_indices().push_back(
            this->parent()->_rho_orb.position(this->tmp_rho_value()));
      }
      _right_indices_computed = true;
    }

    ////////////////////////////////////////////////////////////////////////
    // NonRegularDClass - data - private
    ////////////////////////////////////////////////////////////////////////
    internal_set_type _H_set;
    bool              _idems_above_computed;
    std::unordered_map<lambda_orb_index_type,
                       std::vector<left_indices_index_type>>
                                       _lambda_index_positions;
    internal_element_type              _left_idem_above;
    RegularDClass*                     _left_idem_class;
    std::vector<internal_element_type> _left_idem_H_class;
    std::vector<internal_element_type> _left_idem_left_reps;
    bool                               _left_indices_computed;
    std::unordered_map<rho_orb_index_type,
                       std::vector<right_indices_index_type>>
                                       _rho_index_positions;
    internal_element_type              _right_idem_above;
    RegularDClass*                     _right_idem_class;
    std::vector<internal_element_type> _right_idem_H_class;
    std::vector<internal_element_type> _right_idem_right_reps;
    bool                               _right_indices_computed;
  };

  template <typename Element, typename Traits>
  std::string to_human_readable_repr(Konieczny<Element, Traits> const& K) {
    return fmt::format("<{} enumerated Konieczny with {} generator{}, {} "
                       "element{} ({} regular) in {} D-class{} ({} regular)>",
                       K.finished() ? "fully" : "partially",
                       K.number_of_generators(),
                       K.number_of_generators() == 1 ? "" : "s",
                       K.current_size(),
                       K.current_size() == 1 ? "" : "s",
                       K.current_number_of_regular_elements(),
                       K.current_number_of_D_classes(),
                       K.current_numer_of_D_classes() == 1 ? "" : "es",
                       K.current_number_regular_D_classes());
  }

  template <typename Element, typename Traits>
  std::string
  to_human_readable_repr(typename Konieczny<Element, Traits>::DClass const& D) {
    return fmt::format("<{}Konieczny::DClass with representative {}, {} "
                       "L-classes, and {} R-classes, with total size {}>",
                       D.is_regular_D_class() ? "Regular " : "",
                       D.rep(),
                       D.number_of_L_classes(),
                       D.number_of_R_classes(),
                       D.size());
  }
}  // namespace libsemigroups
