#include <Python.h>
#include <libsemigroups/semigroups.h>

namespace libsemigroups {

  /*
  class PythonElement: public Element {
   public:
    explicit PythonElement(int value);
  };
  */

  class PythonElement : public Element {
   private:
    PyObject* _value;  // TODO: make it private and provide a getter?

   public:
    explicit PythonElement(PyObject* value) : Element(), _value(value) {
      Py_INCREF(value);
    }

    PyObject* get_value() {
      return _value;
    }

    bool operator==(Element const& that) const override {
      return PyObject_RichCompareBool(
          static_cast<PythonElement const&>(that)._value, this->_value, Py_EQ);
    }

    bool operator<(Element const& that) const override {
      return PyObject_RichCompareBool(
          static_cast<PythonElement const&>(that)._value, this->_value, Py_LT);
    }

    size_t complexity() const override {
      return 1;  // TODO
    }

    size_t degree() const override {
      return 0;
    }

    void cache_hash_value() const override {
      this->_hash_value = PyObject_Hash(_value);
    }

    Element* identity() const override {
      return new PythonElement(Py_None);
    }

    Element* really_copy(size_t increase_deg_by = 0) const override {
      return new PythonElement(_value);
    }

    void copy(Element const* x) override {
      _value = static_cast<PythonElement const*>(x)->_value;
      this->reset_hash_value();
    }

    void really_delete() override {
      Py_DECREF(_value);
      return;
    }

    void redefine(Element const* x, Element const* y) override {
      PyObject* product
          = PyNumber_Multiply(static_cast<const PythonElement*>(x)->_value,
                              static_cast<const PythonElement*>(y)->_value);
      Py_DECREF(_value);
      _value = product;
      Py_INCREF(_value);
      reset_hash_value();
    }
  };

};  // namespace libsemigroups
