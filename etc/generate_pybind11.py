#!/usr/bin/env python3
"""
This module partially generates pybind11 bindings from the doxygen output in docs/xml.
"""
# pylint: disable=missing-docstring

# TODO(0):
# * enums
# * static mem_fns
# * repr
# * update the requirements.txt file

import os
import re
import sys
import argparse
import subprocess
from os.path import isfile, exists
from functools import cache

from accepts import accepts

import bs4
from bs4 import BeautifulSoup

__DOXY_DICT = {}
__EXCEPTIONAL_MEM_FN = {
    "run_for": "(void (%s::*)(std::chrono::nanoseconds))& Runner::run_for",
    "run_until": "(void (%s::*)(std::function<bool()> &)) & Runner::run_until",
    "report_every": "(void (%s::*)(std::chrono::nanoseconds)) & Runner::report_every",
}

__COPYRIGHT = """
//
// libsemigroups_pybind11
// Copyright (C) 20XX TODO
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
"""

__HEADERS = """
// C std headers....
// TODO complete or delete

// C++ stl headers....
// TODO complete or delete

// libsemigroups headers
// #include<libsemigroups/libsemigroups.hpp>
// TODO complete or delete

// pybind11....
// #include <pybind11/pybind11.h>
// TODO complete or delete

// libsemigroups_pybind11....
#include "main.hpp"  // for init_TODO

namespace py = pybind11;

namespace libsemigroups {
"""


def template_header(thing: str, template_p: list[str]) -> str:
    pack = ", ".join(template_p)
    alias = ", ".join([x.split(" ")[1] for x in template_p if x != "typename"])
    return f"""
namespace {{
    template <{pack}>
    void bind_TODO(py::module& m, std::string const& name) {{
    using {shortname(thing)}_ = {shortname(thing)}<{alias}>;
"""


@accepts()
def non_template_header() -> str:
    return """
namespace {
    void init_TODO(py::module& m) {
"""


__TEMPLATE_FOOTER = """
} // bind_TODO
} // namespace
   void init_TODO(py::module& m) {
     // One call to bind is required per list of types
     // bind_TODO<BMat8,
     //           BMat8,
     //           ImageRightAction<BMat8, BMat8>,
     //           ActionTraits<BMat8, BMat8>,
     //           side::right>(m, "RowOrbBMat8");
   }
"""

__NON_TEMPLATE_FOOTER = """
} // init_TODO
} // namespace
"""

__FOOTER = """
} // namespace libsemigroups
"""

########################################################################
# Internal stuff for this script
########################################################################


def __error(msg: str) -> None:
    sys.stderr.write(f"\033[0;31m{msg}\n\033[0m")


def __bold(msg: str) -> None:
    sys.stderr.write(f"\033[1m{msg}\n\033[0m")


def __parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="generate_pybind11", usage="%(prog)s [options]"
    )
    parser.add_argument(
        "things",
        nargs="+",
        help="the things (classes, structs, namespaces) to create bindings for",
    )
    return parser.parse_args()


########################################################################
# Stuff for extracting info from doxygen xml
########################################################################


@cache
@accepts(str)
def doxygen_filename(thing: str) -> str:
    """
    Returns the xml filename used by Doxygen for the class with thing
    <thing>.

    Arguments:
    thing -- a string containing the fully qualified thing of a C++ class, struct, or
    namespace.
    """
    orig = thing
    thing = re.sub("_", "__", thing)
    p = re.compile(r"::")
    thing = p.sub("_1_1", thing)
    p = re.compile(r"([A-Z])")
    thing = p.sub(r"_\1", thing).lower()
    for possible in ("class", "struct", "namespace"):
        fname = f"docs/xml/{possible}{thing}.xml"
        if exists(fname) and isfile(fname):
            return fname
    __error(f'Can\'t find the doxygen file for "{orig}" IGNORING!!!')
    return ""


# TODO doesn't seem to work for "Bipartition::at", this is because we cannot
# currently distinguish between const and non-const mem fns with the same
# parameters. This could be resolved by adding a 4th parameter here with is
# const/not const


@accepts(str, str, str)
def get_xml(
    thing: str, fn: str | None = None, params_t: str | None = None
) -> dict[str, bs4.element.Tag]:
    """
    Returns the xml entity of thing::fn(params_t).

    Arguments:
    thing  -- a string containing a fully qualified C++ class or struct name.
    fn   -- a string containing the unqualified name of a function
            in <thing>.
    params_t -- a string containing the types of the parameters of the
                overload of <thing::fn> required formatted as doxygen
                xml does:
                 - 'std::string const &,std::string const &',
                 - 'word_type const &,word_type const &',
                 - 'std::initializer_list< size_t >,std::initializer_list< size_t >',
                 - 'relation_type',
                 - 'rule_type'
    """
    if thing not in __DOXY_DICT:
        with open(doxygen_filename(thing), "r", encoding="utf-8") as xml:
            xml = BeautifulSoup(xml, "xml")
            fn_list = xml.find_all("memberdef")
            fn_dict = {}

            for x in fn_list:
                nm = x.find("name").text
                if nm not in fn_dict:
                    fn_dict[nm] = {}
                tparam = x.find("templateparamlist")
                if tparam is not None:
                    tparam = tparam.find_all("param")
                    tparam = [x.find("type").text.strip() for x in tparam]
                param = x.find_all("param")
                param = [x.find("type").text.strip() for x in param]
                if tparam is not None:
                    param = [x for x in param if x not in tparam]
                param = ",".join(param)

                fn_dict[nm][param] = x
            __DOXY_DICT[thing] = fn_dict
    if fn is not None:
        if params_t == "" and len(__DOXY_DICT[thing][fn]) == 1:
            return list(__DOXY_DICT[thing][fn].values())[0]
        if params_t is not None:
            return __DOXY_DICT[thing][fn][params_t]
        return __DOXY_DICT[thing][fn]
    return __DOXY_DICT[thing]


########################################################################
# Any function
########################################################################


@cache
@accepts(str, str, str)
def is_public_fn(thing: str, fn: str, params_t: str) -> bool:
    if is_namespace(thing):
        return True
    xml = get_xml(thing, fn, params_t)
    prot = xml.get("prot")
    if prot is not None:
        return prot == "public"
    return False


########################################################################
# Classes or structs
########################################################################


@cache
@accepts(str)
def class_template_params(thing: str) -> list[str]:
    result = []
    if is_namespace(thing):
        return result
    with open(doxygen_filename(thing), "r", encoding="utf-8") as xml:
        xml = BeautifulSoup(xml, "xml")
        for x in xml.doxygen.compounddef.children:
            if x.name == "templateparamlist":
                for y in x.find_all("param"):
                    result.append(y.find("type").text)
                    if y.find("declname") is not None:
                        result[-1] += " " + y.find("declname").text
    return result


@cache
@accepts(str)
def is_class_template(thing: str) -> bool:
    return len(class_template_params(thing)) != 0


@cache
@accepts(str, str)
def is_overloaded(thing: str, fn: str) -> bool:
    return len(get_xml(thing, fn)) > 1


@cache
@accepts(str)
def is_namespace(thing: str) -> bool:
    return "namespace" in doxygen_filename(thing)


@cache
@accepts(str, str, str)
def params_dict(thing: str, fn: str, params_t: str) -> dict[str, str]:
    xml = get_xml(thing, fn, params_t)
    result = {}
    for x in xml.find_all("param"):
        type_ = x.find("type").text
        # print(type_)
        # if type_.find("ref"):
        #     type_ = type_.find("ref").text + type_.text
        # else:
        #     type_ = type_.text
        if not type_.startswith("typename"):
            name = x.find("declname")
            if name:
                result[name.text] = type_
    return result


@cache
@accepts(str, str, str)
def return_type(thing: str, fn: str, params_t: str) -> str:
    xml = get_xml(thing, fn, params_t)
    return xml.find("type").text


@accepts(str, str, str)
def param_names(thing: str, fn: str, params_t: str) -> list[str]:
    return list(params_dict(thing, fn, params_t).keys())


@accepts(str, str, str)
def fn_sig(thing: str, fn: str, params_t: str) -> list[str]:
    params_d = params_dict(thing, fn, params_t)
    result = []
    for name, type_ in params_d.items():
        result.append(f"{type_} {name}")
    return result


@accepts(str)
def shortname(thing: str) -> str:
    if thing.startswith("libsemigroups::"):
        return thing[len("libsemigroups::") :]
    return thing


@accepts(str)
def shortname_(thing: str) -> str:
    name = shortname(thing)
    if is_class_template(thing):
        name += "_"
    return name


########################################################################
# Member functions
########################################################################


@cache
@accepts(str, str, str)
def is_const_mem_fn(thing: str, fn: str, params_t: str) -> bool:
    if is_namespace(thing):
        return False
    xml = get_xml(thing, fn, params_t)
    assert "const" in xml.attrs, "const not an attribute!"
    return xml["const"] == "yes"


@cache
@accepts(str, str, str)
def is_deleted_mem_fn(class_n: str, mem_fn: str, params_t: str) -> bool:
    xml = get_xml(class_n, mem_fn, params_t)
    if xml.find("argsstring") is None:
        return False
    return xml.find("argsstring").text.find("=delete") != -1


@cache
@accepts(str, str)
def is_constructor(class_n: str, mem_fn: str) -> bool:
    return mem_fn.startswith(class_n.split("::")[-1])


@cache
@accepts(str, str)
def is_operator(_: str, mem_fn: str) -> bool:
    return mem_fn.startswith("operator") or mem_fn == "at"


@accepts(str, str)
def try_rewrite_exceptional_mem_fn(class_n: str, mem_fn: str) -> str:
    try:
        return __EXCEPTIONAL_MEM_FN[mem_fn] % class_n
    except KeyError:
        return ""


# TODO combine with translate_cpp_to_py
def translate_cpp_operator_to_py(mem_fn: str) -> str:
    translator = {
        "operator==": "__eq__",
        "operator!=": "__ne__",
        "operator<": "__lt__",
        "operator>": "__gt__",
        "operator<=": "__le__",
        "operator>=": "__ge__",
        "operator+": "__add__",
        "operator*": "__mul__",
        "at": "__getitem__",
        "operator()": "__call__",
        "operator*=": "__imul__",
        "operator+=": "__iadd__",
    }
    if mem_fn in translator:
        return translator[mem_fn]

    __error(f"Unknown operator {mem_fn}")
    return mem_fn


########################################################################
# Documentation
########################################################################


@accepts(str)
def translate_cpp_to_py(type_: str) -> str:
    type_ = re.sub(r"<.*?>", "", type_)
    type_ = re.sub(r"\bconst\b", "", type_)
    type_ = re.sub(r"\&", "", type_)
    type_ = type_.strip()
    if type_ == "std::out_of_range":
        return "IndexError"
    if type_ == "std::bad_alloc":
        return "MemoryError"
    if type_ == "std::length_error":
        return "ValueError"
    if type_ == "LibsemigroupsException":
        return "LibsemigroupsError"
    if type_ in ("size_t", "uint32_t", "size_type"):
        return "int"
    if type_ in ("this", "*this"):
        return "self"
    if type_ == "true":
        return "True"
    if type_ == "false":
        return "False"
    if type_ == "void":
        return "None"
    return type_


@accepts(bs4.element.Tag, list)
def convert_to_rst(xml, context=[]):
    context.append(xml.name)
    if "kind" in xml.attrs and xml.attrs["kind"] == "enum":
        context.append(xml.attrs["kind"])

    result = ""
    if xml.name == "compounddef":
        try:
            t = next((x for x in xml if x.name == "templateparamlist"))
            xml = [t] + [x for x in xml if x.name != "templateparamlist"]
        except StopIteration:
            pass
    if (
        not isinstance(xml, list)
        and "kind" in xml.attrs
        and xml.attrs["kind"] == "enum"
    ):
        n = next((x for x in xml if x.name == "name"))
        bd = ""
        try:
            bd = next((x for x in xml if x.name == "briefdescription"))
        except StopIteration:
            pass
        xml = [n, bd] + [x for x in xml if x.name not in ("briefdescription", "name")]
    for x in xml:
        if isinstance(x, str):
            x = x.strip()
            result += " " if x != "." and x != "" and not x[0].isupper() else ""
            result += x
        elif "enum" in context and x.name == "name":
            result += x.text.strip()
        elif "enum" in context and x.name == "enumvalue":
            result += "\n\n.. py:enumerator:: "
            result += convert_to_rst(x, context)
        elif x.name == "briefdescription":
            result += "\n\n" + convert_to_rst(x, context)
        elif x.name == "detaileddescription":
            result += "\n" + convert_to_rst(x, context)
        elif x.name == "templateparamlist":
            params = []
            for y in x.find_all("param"):
                z = y.type.text
                if y.declname is not None:
                    z += " " + y.declname.text
                params.append(z)
            result += "template <" + ", ".join(params) + ">"
        elif x.name == "computeroutput":
            if len(x.text) != 0:
                result += f" ``{translate_cpp_to_py(x.text)}``"
        elif x.name == "formula":
            result += " :math:`" + x.text.replace("$", "") + "`"
        elif x.name == "title":
            result += f"\n\n:{x.text.lower()}: "
        elif x.name == "para":
            result += convert_to_rst(x, context)
        elif x.name == "simplesect" and x.attrs["kind"] == "par":
            result += convert_to_rst(x, context)
        elif x.name == "parameterlist" and x.attrs["kind"] == "exception":
            for y in x.find_all("parameteritem"):
                exception = y.find("parametername").text
                if exception != "(None)":
                    result += "\n\n"
                    result += f":raises {translate_cpp_to_py(exception)}: "
                    result += convert_to_rst(y.find("parameterdescription"), context)
        elif x.name == "simplesect" and x.attrs["kind"] == "see":
            result += "\n\n.. seealso:: " + convert_to_rst(x, context)
        elif x.name == "ref":
            result += f" :any:`{translate_cpp_to_py(x.text)}`"
        elif x.name == "emphasis":
            result += f" *{x.text}*"
        elif x.name == "bold":
            result += f"**{x.text}**"
        elif x.name == "compoundname":
            result += x.text[x.text.rfind("::") + 2 :]
        elif x.name == "ulink":
            result += f'`{x.text} <{x.attrs["url"]}>`_'
        elif x.name == "itemizedlist":
            result += "\n" + convert_to_rst(x, context)
        elif x.name == "listitem":
            result += "\n* " + convert_to_rst(x, context)
        elif x.name == "programlisting":
            result += "\n\n.. code-block::\n" + convert_to_rst(x)
        elif x.name == "codeline":
            result += "\n" + convert_to_rst(x)
        elif x.name == "highlight":
            result += convert_to_rst(x)
        elif x.name == "sp":
            result += " "

    if len(context) > 0 and context[-1] == "enum":
        context.pop()
    if context.pop() == "itemizedlist":
        result += "\n\n"

    return result


########################################################################
# Formatting output doc
########################################################################


@accepts(str)
def rst_fmt(doc: str) -> str:
    with open("tmp.rst", "w", encoding="utf-8") as f:
        f.write(doc)
    try:
        subprocess.check_output(
            "rstfmt tmp.rst",
            shell=True,
            stderr=subprocess.STDOUT,
        )
        with open("tmp.rst", "r", encoding="utf-8") as f:
            doc = f.read()
    except subprocess.CalledProcessError:
        pass
    os.remove("tmp.rst")
    return doc


########################################################################
# Output pybind11 stuff
########################################################################


@accepts(str)
def pybind11_stub(thing):
    if not is_namespace(thing):
        if not is_class_template(thing):
            return f'py::class_<{shortname_(thing)}>(m, "{shortname(thing)}")\n'
        return f"py::class_<{shortname_(thing)}>(m, name.c_str())\n"
    return "m"


@accepts(str, str, str)
def pybind11_fn_params(thing, fn, params_t):
    params_n = param_names(thing, fn, params_t)
    out = [f'py::arg("{x}")' for x in params_n]
    return ", ".join(out)


@accepts(str, str, str)
def pybind11_doc(thing, fn, params_t):
    xml = get_xml(thing, fn, params_t)
    brief = xml.find("briefdescription").text.strip()
    detailed = xml.find("detaileddescription")
    doc = ""
    if brief:
        doc += brief + "\n"

    # get param text (if any)
    params = detailed.find_all("parameterlist")
    params = [x for x in params if x.attrs["kind"] == "param"]

    params_d = params_dict(thing, fn, params_t)
    for x in params:
        paramitem = x.find_all("parameteritem")
        for y in paramitem:
            nam = y.find("parametername").text
            des = y.find("parameterdescription").find("para")
            if des is not None:
                des = des.text
            else:
                des = ""
            doc += f"\n:param {nam}: {des}"
            try:
                doc += f"\n:type {nam}: {translate_cpp_to_py(params_d[nam])}\n"
            except KeyError:
                __error(
                    f'Can\'t find the parameter "{nam}" for "{thing}::{fn}({params_t})" IGNORING!!!'
                )

    return_ = [
        x for x in detailed.find_all("simplesect") if x.attrs["kind"] == "return"
    ]
    doc += convert_to_rst(detailed)
    if len(return_) > 0:
        if len(params) > 0:
            doc += "\n"
        doc += f"\n\n:returns: {convert_to_rst(return_[0])}"
        doc += f"\n\n:rtype: {translate_cpp_to_py(return_type(thing, fn, params_t))}\n"
    return rst_fmt(doc)


def pybind11_operator(thing: str, fn: str, params_t: str) -> str:
    assert is_operator(thing, fn)
    op = fn[len("operator") :]
    if op not in ("[]"):
        return f"py::self {op} py::self"
    return f'"{translate_cpp_operator_to_py(fn)}", &{shortname_(thing)}::{fn}, py::is_operator()'


@accepts(str, str, str)
def pybind11_mem_fn(thing: str, fn: str, params_t: str) -> str:
    if is_constructor(thing, fn):
        if "&&" in params_t:
            return ""
        if is_class_template(thing):
            params_t = re.sub(shortname(thing), shortname_(thing), params_t)
        return f".def(py::init<{params_t}>())\n"
    func = try_rewrite_exceptional_mem_fn(thing, fn)
    short_mem_fn = f'"{shortname(fn)}", '
    if not func:
        # Use lambdas not overload_cast
        if is_overloaded(thing, fn):
            sig = fn_sig(thing, fn, params_t)
            if not is_namespace(thing):
                if is_const_mem_fn(thing, fn, params_t):
                    sig = ", ".join([f"{shortname_(thing)} const& self"] + sig)
                else:
                    sig = ", ".join([f"{shortname_(thing)}& self"] + sig)
            else:
                sig = ", ".join(sig)
            func = f"""[]({sig}) {{
                    return self.{fn}({", ".join(param_names(thing, fn, params_t))});
                    }}"""
        else:
            if is_operator(thing, fn):
                func = pybind11_operator(thing, fn, params_t)
                short_mem_fn = ""
            else:
                func = f"&{shortname_(thing)}::{fn}"

    params_n = pybind11_fn_params(thing, fn, params_t)
    if len(params_n) > 0:
        return f""".def({short_mem_fn}
{func},
{params_n},
R"pbdoc(
{pybind11_doc(thing, fn, params_t)}
)pbdoc")\n"""
    return f""".def({short_mem_fn}
{func},
R"pbdoc(
{pybind11_doc(thing, fn, params_t)}
)pbdoc")\n"""


########################################################################
# The main event
########################################################################


@accepts(str, str, str)
def skip_fn(thing: str, fn: str, params_t: str) -> bool:
    if (
        fn.endswith("_type")  # type definition
        or "iterator" in fn  # type definition
        or fn.endswith("_no_checks")  # don't offer no_checks functions by default
        or "initializer_list" in params_t  # no python analogue of initializer_list
        or fn.startswith("cbegin")  # use `make_iterator` instead (TODO add this)
        or fn.startswith("cend")
        or fn.startswith("begin")
        or fn.startswith("end")
        # copy/move assignment op no python analogue, operator[] is no checks
        # so skip it
        or fn in ("operator=", "operator[]")
    ):
        return True
    try:
        get_xml(thing, fn, params_t)
    except KeyError:
        return True
    return is_deleted_mem_fn(thing, fn, params_t) or not is_public_fn(
        thing, fn, params_t
    )


@accepts(str)
def generate(thing: str) -> str:
    if len(doxygen_filename(thing)) == 0:
        return ""
    out = pybind11_stub(thing)
    fns = get_xml(thing)
    for fn, overloads in fns.items():
        for params in overloads:
            if not isinstance(fn, str) or skip_fn(thing, fn, params):
                continue  # ignore
            out += pybind11_mem_fn(thing, fn, params)
    return out + ";"


def main():
    if sys.version_info[0] < 3:
        raise Exception("Python 3 is required")
    args = __parse_args()
    print(__COPYRIGHT)
    print(__HEADERS)
    for thing in args.things:
        template_p = class_template_params(thing)
        if len(template_p) != 0:
            print(template_header(thing, template_p))
        else:
            print(non_template_header())

        print(generate(thing))
        if len(template_p) != 0:
            print(__TEMPLATE_FOOTER)
        else:
            print(__NON_TEMPLATE_FOOTER)
    print(__FOOTER)
    __bold(
        """
Current limitations:

* if a member function is overloaded because there is a const and a non-const
  version, then this is not detected, and the generated code will not compile.
  For example, Bipartition::at has const and non-const overloads with the same
  parameters.

Things to do to include the generated code in _libsemigroups_pybind11:

1. add the generated code to a cpp file in libsemigroups_pybind11/src

2. rename init_TODO and bind_TODO (if it exists) to more appropriate names

3. add your name in place of TODO in the copyright statement in line 3 of the
   generated file

4. declare the function init_TODO in libsemigroups_pybind11/src/main.hpp

5. call the function init_TODO in libsemigroups_pybind11/src/main.cpp

6. Check that the document is correct, and doesn't use C++ names/idioms but
   rather python such, see libsemigroups_pybind11/CONTRIBUTING.rst for details
"""
    )


if __name__ == "__main__":
    main()
