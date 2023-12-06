#!/usr/bin/env python3
"""
This module partially generates pybind11 bindings from the yml files in
libsemigroups/python/yml and from the doxygen output in docs/xml.
"""
import os
import re
import sys
import yaml
import argparse

from bs4 import BeautifulSoup

__DOXY_DICT = {}
__PARAMS_T_SUBSTITUTIONS = {"bool (*)": "bool(*)()"}
__EXCEPTIONAL_MEM_FN = {
    "run_for": "(void (%s::*)(std::chrono::nanoseconds))& Runner::run_for",
    "run_until": "(void (%s::*)(std::function<bool()> &)) & Runner::run_until",
    "report_every": "(void (%s::*)(std::chrono::nanoseconds)) & Runner::report_every",
}

# TODO
# 1) enums


def __accepts(*types):
    def check_accepts(f):
        assert len(types) == f.__code__.co_argcount

        def new_f(*args, **kwds):
            for a, t in zip(args, types):
                assert isinstance(a, t), "arg %r does not match %s" % (a, t)
            return f(*args, **kwds)

        new_f.__name__ = f.__name__
        return new_f

    return check_accepts


def __parse_args():
    parser = argparse.ArgumentParser(
        prog="generate_pybind11", usage="%(prog)s [options]"
    )
    parser.add_argument("files", nargs="+", help="the files to lint")
    return parser.parse_args()


@__accepts(str)
def doxygen_filename(class_n):
    """
    Returns the xml filename used by Doxygen for the class with name
    <class_n>.

    Arguments:
    class_n -- a string containing a fully qualified C++ class or struct name.
    """
    p = re.compile(r"::")
    class_n = p.sub("_1_1", class_n)
    p = re.compile(r"([A-Z])")
    class_n = p.sub(r"_\1", class_n).lower()
    if os.path.isfile("docs/xml/class" + class_n + ".xml"):
        return "docs/xml/class" + class_n + ".xml"
    elif os.path.isfile("xml/struct" + class_n + ".xml"):
        return "docs/xml/struct" + class_n + ".xml"
    raise FileNotFoundError("couldn't find the doxygen file for " + class_n)


@__accepts(str, str, str)
def get_xml(class_n, mem_fn=None, params_t=None):
    """
    Returns the xml entity of class_n::mem_fn(params_t).

    Arguments:
    class_n  -- a string containing a fully qualified C++ class or struct name.
    mem_fn   -- a string containing the unqualified name of a member function
                of <class_n>.
    params_t -- a string containing the types of the parameters of the
                overload of <class_n::mem_fn> required formatted as doxygen
                xml does:
                 - 'std::string const &,std::string const &',
                 - 'word_type const &,word_type const &',
                 - 'std::initializer_list< size_t >,std::initializer_list< size_t >',
                 - 'relation_type',
                 - 'rule_type'
    """
    global __DOXY_DICT
    if not class_n in __DOXY_DICT:
        with open(doxygen_filename(class_n), "r") as xml:
            xml = BeautifulSoup(xml, "xml")
            mem_fn_list = xml.find_all("memberdef")
            mem_fn_dict = {}
            for x in mem_fn_list:
                nm = x.find("name").text
                if nm not in mem_fn_dict:
                    mem_fn_dict[nm] = {}
                tparam = x.find("templateparamlist")
                if tparam is not None:
                    tparam = tparam.find_all("param")
                    tparam = [x.find("type").text.strip() for x in tparam]
                param = x.find_all("param")
                param = [x.find("type").text.strip() for x in param]
                if tparam is not None:
                    param = [x for x in param if x not in tparam]
                param = ",".join(param)
                mem_fn_dict[nm][param] = x
            __DOXY_DICT[class_n] = mem_fn_dict
    if mem_fn is not None:
        if params_t == "" and len(__DOXY_DICT[class_n][mem_fn]) == 1:
            return list(__DOXY_DICT[class_n][mem_fn].values())[0]
        elif params_t is not None:
            return __DOXY_DICT[class_n][mem_fn][params_t]
        else:
            return __DOXY_DICT[class_n][mem_fn]
    else:
        return __DOXY_DICT[class_n]


@__accepts(str, str, str)
def is_const_mem_fn(class_n, mem_fn, params_t):
    xml = get_xml(class_n, mem_fn, params_t)
    assert "const" in xml.attrs, "const not an attribute!"
    return xml["const"] == "yes"


@__accepts(str, str, str)
def is_mem_fn_template(class_n, mem_fn, params_t):
    xml = get_xml(class_n, mem_fn, params_t)
    return xml.find("templateparamlist") is not None


@__accepts(str, str, str)
def is_deleted_mem_fn(class_n, mem_fn, params_t):
    xml = get_xml(class_n, mem_fn, params_t)
    return xml.find("argsstring").text.find("=delete") != -1


@__accepts(str, str)
def is_constructor(class_n, mem_fn):
    return mem_fn.startswith(class_n.split("::")[-1])


@__accepts(str, str)
def is_overloaded(class_n, mem_fn):
    return len(get_xml(class_n, mem_fn)) > 1


@__accepts(str, str, str)
def skip_mem_fn(class_n, mem_fn, params_t):
    if mem_fn.startswith("operator"):
        return True
    try:
        get_xml(class_n, mem_fn, params_t)
    except KeyError:
        return True
    return is_deleted_mem_fn(class_n, mem_fn, params_t) or is_mem_fn_template(
        class_n, mem_fn, params_t
    )


@__accepts(str, str, str)
def param_names(class_n, mem_fn, params_t):
    xml = get_xml(class_n, mem_fn, params_t)
    params_n = xml.find("detaileddescription").find_all("parameterlist")
    params_n = [x for x in params_n if x.attrs["kind"] == "param"]
    return [y.text for x in params_n for y in x.find_all("parametername")]


@__accepts(str)
def normalize_params_t(params_t):
    if params_t in __PARAMS_T_SUBSTITUTIONS:
        return __PARAMS_T_SUBSTITUTIONS[params_t]
    params_t = params_t.strip()
    # replace more than 1 space by a single space
    params_t = re.sub("\s{2,}", " ", params_t)
    # Add space after < if it's a non-space
    params_t = re.sub("(?<=[<])(?=[^\s])", " ", params_t)
    # Add space before > if it's a non-space
    params_t = re.sub("(?<=[^\s])(?=[>])", " ", params_t)
    # Add space before & if it's a non-space and not &
    params_t = re.sub("(?<=[^\s\&])(?=[\&])", " ", params_t)
    # Add space after & if it's a non-space and not &
    params_t = re.sub("(?<=[\&])(?=[^\s\&])", " ", params_t)
    # remove whitespace around commas
    params_t = re.sub("\s*,\s*", ",", params_t)
    return params_t


@__accepts(str)
def extract_func_params_t(yml_entry):
    pos = yml_entry.find("(")
    if pos == -1:
        return yml_entry, ""
    else:
        params_t = normalize_params_t(yml_entry[pos + 1 : yml_entry.rfind(")")])
        return yml_entry[:pos], params_t


@__accepts(str, str)
def exceptional_mem_fn(class_n, mem_fn):
    return __EXCEPTIONAL_MEM_FN[mem_fn] % class_n


@__accepts(str, str, str)
def pybind11_doc(class_n, mem_fn, params_t):
    xml = get_xml(class_n, mem_fn, params_t)
    doc = xml.find("briefdescription").text.strip() + "\n"
    detailed = xml.find("detaileddescription")

    # get param text (if any)
    params = detailed.find_all("parameterlist")
    params = [x for x in params if x.attrs["kind"] == "param"]
    if is_overloaded(class_n, mem_fn) and len(params) > 0:
        doc += "\n               :Parameters: "
    for x in params:
        paramitem = x.find_all("parameteritem")
        for y in paramitem:
            nam = y.find("parametername").text
            des = y.find("parameterdescription").find("para")
            if des is not None:
                des = des.text
            else:
                des = ""
            if not is_overloaded(class_n, mem_fn):
                doc += "\n               :param %s: %s" % (nam, des)
                doc += "\n               :type %s: ??" % nam
            else:
                if len(paramitem) > 1:
                    doc += "- "
                doc += "**%s** (??) - %s" % (nam, des)
                doc += "\n                            "
    # get return text
    return_ = [
        x
        for x in detailed.find_all("simplesect")
        if x.attrs["kind"] == "return"
    ]
    if len(return_) > 0:
        if not is_overloaded(class_n, mem_fn):
            if len(params) > 0:
                doc += "\n"
            doc += "\n               :return: "
        else:
            if len(params) == 0:
                doc += "\n"
            doc += "\n               :Returns: "
        doc += return_[0].find("para").text
    return doc


@__accepts(str, str, str)
def pybind11_mem_fn_params_n(class_n, mem_fn, params_t):
    params_n = param_names(class_n, mem_fn, params_t)
    out = ['py::arg("%s")' % x for x in params_n]
    return ", ".join(out)


@__accepts(str)
def pybind11_class_n(class_n):
    if class_n.startswith("libsemigroups::"):
        shortname = class_n[len("libsemigroups::") :]
    else:
        shortname = class_n
    return 'py::class_<%s>(m, "%s")\n' % (class_n, shortname)


@__accepts(str, str, str)
def pybind11_mem_fn(class_n, mem_fn, params_t):
    if is_constructor(class_n, mem_fn):
        return ".def(py::init<%s>())\n" % params_t
    try:
        func = exceptional_mem_fn(class_n, mem_fn)
    except:
        if is_overloaded(class_n, mem_fn):
            func = "py::overload_cast<%s>(&%s::%s" % (params_t, class_n, mem_fn)
            if is_const_mem_fn(class_n, mem_fn, params_t):
                func += ", py::const_"
            func += ")"
        else:
            func = "&%s::%s" % (class_n, mem_fn)

    params_n = pybind11_mem_fn_params_n(class_n, mem_fn, params_t)
    if len(params_n) > 0:
        return """.def(\"%s\",
         %s,
         %s,
         R\"pbdoc(
               %s
               )pbdoc\")\n""" % (
            mem_fn,
            func,
            params_n,
            pybind11_doc(class_n, mem_fn, params_t),
        )
    else:
        return """.def(\"%s\",
         %s,
         R\"pbdoc(
               %s
               )pbdoc\")\n""" % (
            mem_fn,
            func,
            pybind11_doc(class_n, mem_fn, params_t),
        )


def generate(ymlfname):
    with open(ymlfname, "r") as f:
        ymldic = yaml.load(f, Loader=yaml.FullLoader)
        class_n = next(iter(ymldic))
        if ymldic[class_n] is None:
            return
        out = pybind11_class_n(class_n)
        for sectiondic in ymldic[class_n]:
            name = next(iter(sectiondic))
            if sectiondic[name] is not None:
                for x in sectiondic[name]:
                    if not isinstance(x, str):
                        continue
                    mem_fn, params_t = extract_func_params_t(x)
                    if skip_mem_fn(class_n, mem_fn, params_t):
                        continue  # ignore
                    out += pybind11_mem_fn(class_n, mem_fn, params_t)
        return out + ";"


def main():
    if sys.version_info[0] < 3:
        raise Exception("Python 3 is required")
    args = __parse_args()
    for fname in args.files:
        print(generate(fname))


if __name__ == "__main__":
    main()
