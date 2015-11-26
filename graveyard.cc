
/*******************************************************************************
 * Class for containing a C++ semigroup and accessing its methods
*******************************************************************************/

/*
    // in the case that we are taking the closure of the semigroup of
    // <old> with some new generators, these new generators are stored in
    // the <gens> component of <data>. I.e. the meaning of the <gens> component
    // of the <data> is different if we are taking the closure than if we are
    // not. 
    Interface (Obj data, Converter* converter, SemigroupBase* old) 
      : _converter(converter) {
    }*/

/*// method for Semigroup class
size_t simple_size () {
  T x(_degree, _gens.at(0)); 
  size_t report = 0;
  while (_pos < _nr) {
    for (size_t j = 0; j < _nrgens; j++) {
      x.redefine(_elements->at(_pos), _gens.at(j)); 
      auto it = _map.find(x); 
      if (it == _map.end()) {
        _elements->push_back(static_cast<T*>(x.copy()));
        _map.insert(std::make_pair(*_elements->back(), _nr));
        _nr++;
      }
    }
    _pos++;
    if (_nr > report + 10000) {
      report = _nr;
      std::cout << "found " << _nr << " elements so far\n";
    }
  }
  x.delete_data();
  return _nr;
}*/
    /*std::vector<Relation>* relations () {
      enumerate(-1);
      std::vector<Relation>* relations = new std::vector<Relation>();
      int nr = (int) _nrrules;
      
      size_t tmp = 0;

      for (size_t i = 1; i < _gens.size(); i++) {
        if (_genslookup.at(i) <= _genslookup.at(i - 1)) {
          nr--;
          relations->push_back(make_relation(i, _genslookup.at(i)));
        }
      }
      std::cout << "nr of relations = " << relations->size() - tmp << "\n";
      tmp = relations->size();
      
      size_t i;
      for (i = 0; i < _lenindex.at(1); i++) {
        for (size_t j = 0; j < _reduced.nrcols(); j++) {
          if (!_reduced.get(i, j)) {
            nr--;
            relations->push_back(make_relation(i, j));
          }
        }
      }
      std::cout << "nr of relations = " << relations->size() - tmp << "\n";
      tmp = relations->size();
      
      for (; i < _reduced.nrrows(); i++) {
        for (size_t j = 0; j < _reduced.nrcols(); j++) {
          if (_reduced.get(_suffix.at(i), j) && !_reduced.get(i, j)) {
            nr--;
            relations->push_back(make_relation(i, j));
          }
        }
      }
      std::cout << "nr of relations = " << relations->size() - tmp << "\n";
      
      std::cout << "_nrrules = " << _nrrules << "\n";
      assert(nr == 0);
      return relations;
    }*/

/*    // TODO split into 2 methods! 
    Semigroup (const Semigroup& copy, const std::vector<T*>& coll = std::vector<T*>())
      // initialize as if <coll> is not empty . . .
      : _batch_size    (8192),
        _degree        (copy._degree),    // copy for comparison in add_generators
        _duplicate_gens(copy._duplicate_gens), 
        _elements      (new std::vector<T*>()),
        _final         (copy._final),     // copy for assignment to specific positions in add_generators
        _first         (copy._first),     // copy for assignment to specific positions in add_generators
        _found_one     (copy._found_one), // copy in case degree doesn't change in add_generators
        _genslookup    (copy._genslookup),
        _index         (),
        _lenindex      (),
        _length        (copy._length),    // copy for assignment to specific positions in add_generators
        _map           (),
        _nr            (copy._nr),
        _nrgens        (copy._nrgens),
        _nr_idempotents(0),
        _nrrules       (0),
        _pos           (copy._pos),
        _pos_one       (copy._pos_one),   // copy in case degree doesn't change in add_generators
        _prefix        (copy._prefix),    // copy for assignment to specific positions in add_generators
        _relation_pos  (-1),
        _relation_gen  (0),
        _suffix        (copy._suffix),    // copy for assignment to specific positions in add_generators
        _tmp_product   (new T(copy.degree(), copy.gens().at(0))), // this is assigned for clean copy
        _wordlen       (0) 
    {
      _elements->reserve(copy._nr);
      _map.reserve(copy._nr);
      
      std::unordered_set<T*> new_gens;

      // check which of <coll> belong to <copy>
      for (T* x: coll) {
        if (copy._map.find(*x) == copy._map.end()) { 
          new_gens.insert(x);
        }
      }

      assert(new_gens.empty() || (*new_gens.begin())->degree() >= copy.degree());
      size_t deg_plus = (new_gens.empty() ? 0 : (*new_gens.begin())->degree() - copy.degree());

      if (new_gens.empty()) {// straight copy
        _found_one = copy._found_one;
        _index = copy._index;
        _lenindex = copy._lenindex;
        _nr_idempotents = copy._nr_idempotents;
        _nrrules = copy._nrrules;
        _left = new RecVec<size_t>(*copy._left);
        _pos_one = copy._pos_one;
        _reduced = copy._reduced;
        _relation_gen = copy._relation_gen;
        _relation_pos = copy._relation_pos;
        _right = new RecVec<size_t>(*copy._right);
        _wordlen = copy._wordlen;
      } else {
        if (deg_plus != 0) { 
          _degree = copy.degree() + deg_plus;
          _found_one = false;
          _pos_one = 0;
        } 
        _lenindex.push_back(0);
        _lenindex.push_back(copy._lenindex.at(1));
        _index.reserve(copy._nr);
        _left = new RecVec<size_t>(*copy._left, new_gens.size());
        _right = new RecVec<size_t>(*copy._right, new_gens.size());
        _reduced = RecVec<bool>(_nrgens + coll.size(), _nr);
        
        // add the distinct old generators to new _index
        for (size_t i = 0; i < copy._lenindex.at(1); i++) {
          _index.push_back(copy._index.at(i));
          _length.push_back(1);
        }
      }

      for (size_t i = 0; i < copy.nrgens(); i++) {
        _gens.push_back(static_cast<T*>(copy._gens.at(i)->copy(deg_plus)));
      }
      
      _id = static_cast<T*>(copy._id->copy(deg_plus));
      _tmp_product = new T(_degree, _gens.at(0));
      
      for (size_t i = 0; i < copy._elements->size(); i++) {
        _elements->push_back(static_cast<T*>(copy._elements->at(i)->T::copy(deg_plus)));
        is_one(_elements->back(), i);
        _map.insert(std::make_pair(*_elements->back(), i));
      }
      
      add_generators(new_gens, false);
    }*/
