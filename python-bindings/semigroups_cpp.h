
namespace semigroupsplusplus {

/*
class PythonElement: public Element {
 public:
  explicit PythonElement(int value);
};
*/

class PythonElement: public Element {
    private:
    int _value;
    public:
    explicit PythonElement(int value) : Element(), _value(value) {
        // This would be the place to call PyIncRef
    }

    bool operator==(Element const& that) const override {
      return static_cast<PythonElement const &>(that)._value == this->_value;
    }

    bool operator<(Element const& that) const override {
      return static_cast<PythonElement const &>(that)._value < this->_value;
    }

    size_t complexity() const override {
        return 1;
    }

    size_t degree() const override {
        return 0;
    }

    void cache_hash_value() const override {
        this->_hash_value = _value;
    }

    Element* identity() const override {
        return new PythonElement(1);
    }

    Element* really_copy(size_t increase_deg_by = 0) const override {
        return new PythonElement(_value);
    }

    void really_delete() override {
        // Call pydecref
        return;
    }

    void redefine(Element const* x, Element const* y) {
        // Call pydecref?
        _value = static_cast<const PythonElement *>(x)->_value * static_cast<const PythonElement *>(y)->_value;
        // Call pyincref?
        reset_hash_value();
    }

};

};

