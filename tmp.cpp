py::class_<libsemigroups::ToWord>(m, "ToWord")
.def(py::init<>())
.def("init",
         py::overload_cast<>(&libsemigroups::ToWord::init),
         R"pbdoc(
               Initialize an existing ToWord object.


               :Returns: A reference to *this.
               )pbdoc")
.def(py::init<std::string const &>())
.def("init",
         py::overload_cast<std::string const &>(&libsemigroups::ToWord::init),
         py::arg("alphabet"),
         R"pbdoc(
               Initialize an existing ToWord object.

               :Parameters: **alphabet** (??) - the alphabet
                            
               :Returns: A reference to *this.
               )pbdoc")
.def(py::init<ToWord const &>())
.def("empty",
         &libsemigroups::ToWord::empty,
         R"pbdoc(
               Check if the alphabet is defined.

               :return: A value of type bool.
               )pbdoc")
;
