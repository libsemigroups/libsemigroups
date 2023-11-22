py::class_<libsemigroups::Words>(m, "Words")
.def(py::init<>())
.def("init",
         &libsemigroups::Words::init,
         R"pbdoc(
               Initialize an existing Words object.

               :return: A reference to *this.
               )pbdoc")
.def(py::init<Words const &>())
.def("get",
         &libsemigroups::Words::get,
         R"pbdoc(
               Get the current value.

               :return: A value of type output_type.
               )pbdoc")
.def("next",
         &libsemigroups::Words::next,
         R"pbdoc(
               Advance to the next value.

               )pbdoc")
.def("at_end",
         &libsemigroups::Words::at_end,
         R"pbdoc(
               Check if the range object is exhausted.

               :return: A value of type bool.
               )pbdoc")
.def("size_hint",
         &libsemigroups::Words::size_hint,
         R"pbdoc(
               The possible size of the range.

               :return: A value of type size_t.
               )pbdoc")
.def("count",
         &libsemigroups::Words::count,
         R"pbdoc(
               The actual size of the range.

               :return: A value of type size_t.
               )pbdoc")
.def("number_of_letters",
         py::overload_cast<size_type>(&libsemigroups::Words::number_of_letters),
         py::arg("n"),
         R"pbdoc(
               Set the number of letters in the alphabet.

               :Parameters: **n** (??) - the number of letters.
                            
               :Returns: A reference to *this.
               )pbdoc")
.def("number_of_letters",
         py::overload_cast<>(&libsemigroups::Words::number_of_letters, py::const_),
         R"pbdoc(
               The current number of letters in the alphabet.


               :Returns: A value of type size_type.
               )pbdoc")
.def("first",
         py::overload_cast<word_type const &>(&libsemigroups::Words::first),
         py::arg("frst"),
         R"pbdoc(
               Set the first word in the range.

               :Parameters: **frst** (??) - the first word.
                            
               :Returns: A reference to *this.
               )pbdoc")
.def("first",
         py::overload_cast<>(&libsemigroups::Words::first, py::const_),
         R"pbdoc(
               The current first word in the range.


               :Returns: A const reference to a word_type.
               )pbdoc")
.def("last",
         py::overload_cast<word_type const &>(&libsemigroups::Words::last),
         py::arg("lst"),
         R"pbdoc(
               Set one past the last word in the range.

               :Parameters: **lst** (??) - the first word.
                            
               :Returns: A reference to *this.
               )pbdoc")
.def("last",
         py::overload_cast<>(&libsemigroups::Words::last, py::const_),
         R"pbdoc(
               The current one past the last word in the range.


               :Returns: A const reference to a word_type.
               )pbdoc")
.def("order",
         py::overload_cast<Order>(&libsemigroups::Words::order),
         py::arg("val"),
         R"pbdoc(
               Set the order of the words in the range.

               :Parameters: **val** (??) - the order.
                            
               :Returns: A reference to *this.
               )pbdoc")
.def("order",
         py::overload_cast<>(&libsemigroups::Words::order, py::const_),
         R"pbdoc(
               The current order of the words in the range.


               :Returns: A value of type Order.
               )pbdoc")
.def("upper_bound",
         py::overload_cast<size_type>(&libsemigroups::Words::upper_bound),
         py::arg("n"),
         R"pbdoc(
               Set an upper bound for the length of a word in the range.

               :Parameters: **n** (??) - the upper bound.
                            
               :Returns: A reference to *this.
               )pbdoc")
.def("upper_bound",
         py::overload_cast<>(&libsemigroups::Words::upper_bound, py::const_),
         R"pbdoc(
               The current upper bound on the length of a word in the range.


               :Returns: A value of type size_type.
               )pbdoc")
.def("min",
         &libsemigroups::Words::min,
         py::arg("val"),
         R"pbdoc(
               Set the first word in the range by length.

               :param val: the exponent.
               :type val: ??

               :return: A reference to *this.
               )pbdoc")
.def("max",
         &libsemigroups::Words::max,
         py::arg("val"),
         R"pbdoc(
               Set one past the last word in the range by length.

               :param val: the exponent.
               :type val: ??

               :return: A reference to *this.
               )pbdoc")
;
