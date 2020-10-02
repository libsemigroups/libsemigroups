#!/usr/bin/env python3
"""
This module generates the documentation pages in docs/source/_generated from
the yml files in docs/yml.
"""
import datetime
import os
import re
import sys
import yaml
import argparse

from bs4 import BeautifulSoup

def _parse_args():
    parser = argparse.ArgumentParser(prog="generate-pybind11", usage="%(prog)s [options]")
    parser.add_argument('files', nargs='+', help='the files to lint')
    return parser.parse_args()


# classname must be fully qualified
def _get_doxygen_filename(classname):
    p = re.compile(r"::")
    classname = p.sub("_1_1", classname)
    p = re.compile(r"([A-Z])")
    classname = p.sub(r"_\1", classname).lower()
    if os.path.isfile("docs/build/xml/class" + classname + ".xml"):
        return "docs/build/xml/class" + classname + ".xml"
    elif os.path.isfile("build/xml/struct" + classname + ".xml"):
        return "docs/build/xml/struct" + classname + ".xml"

def _warn(fname, msg):
    assert isinstance(msg, str)
    sys.stderr.write("WARNING in %s: %s\n" % (fname, msg))


def pybind11_class_string(classname):
  return "py::class_<%s>(m, \"%s\")\n" % (classname, classname)


def get_doc(classname, func):
    try:
        xml = open(_get_doxygen_filename(classname), "r")
        mem_fn = BeautifulSoup(xml, "xml")
        mem_fn = mem_fn.find_all("memberdef")
        index = [x.find("name").text for x in mem_fn].index(func)
    except:
        #_warn("no Doxygen xml file found for:\n" + classname + "::" + func)
        return
    if index == -1:
        return ""
    doc = mem_fn[index].find("briefdescription").text.strip() + "\n"
    detailed = mem_fn[index].find("detaileddescription")

    # get param text (if any)
    params = detailed.find_all("parameterlist")
    params = [x for x in params if x.attrs['kind'] == 'param']
    for x in params:
        for y in x.find_all("parameteritem"):
            nam = y.find("parametername").text
            des = y.find("parameterdescription").find("para").text
            doc += "\n     :param %s: %s" % (nam, des)
            doc += "\n     :type %s: ??" % nam

    # get return text
    return_ = [x for x in detailed.find_all("simplesect") if x.attrs['kind'] ==
               'return']
    if len(return_) > 0:
        if len(params) > 0:
            doc += '\n'
        doc += '\n     :return: ' + return_[0].find("para").text
    return doc

def get_param_names(classname, func):
    try:
        xml = open(_get_doxygen_filename(classname), "r")
        mem_fn = BeautifulSoup(xml, "xml")
        mem_fn = mem_fn.find_all("memberdef")
        index = [x.find("name").text for x in mem_fn].index(func)
    except:
        #_warn("no Doxygen xml file found for:\n" + classname + "::" + func)
        return ""
    if index == -1:
        return ""
    mem_fn = mem_fn[index].find("detaileddescription").find_all("parameterlist")
    mem_fn = [x for x in mem_fn if x.attrs['kind'] == 'param']
    mem_fn = [y.text for x in mem_fn for y in x.find_all("parametername")]
    mem_fn = ["py::arg(\"%s\")" % x for x in mem_fn]
    return ", ".join(mem_fn)


def pybind11_mem_fn(classname, func):
    if func.startswith("operator"):
        # TODO later
        return ""
    pos = func.find("(")
    if pos != -1:
        # overload
        func, overload = func[:pos], func[pos + 1:-1]
        name = func
        func = "py::overload_cast<%s>(&%s::%s)" % (overload, classname, func)
    else:
        name = func
        func = "&%s::%s" % (classname, func)

    params = get_param_names(classname, name)
    if len(params) > 0:
        return """.def(\"%s\",
     %s,
     %s,
     R\"pbdoc(
     %s
     )pbdoc\")\n""" % (name, func, params, get_doc(classname, name))
    else:
        return """.def(\"%s\",
     %s,
     R\"pbdoc(
     %s
     )pbdoc\")\n""" % (name, func, get_doc(classname, name))


def generate(ymlfname):
    with open(ymlfname, "r") as f:
        ymldic = yaml.load(f, Loader=yaml.FullLoader)
        classname = next(iter(ymldic))
        if ymldic[classname] is None:
            return
        out = pybind11_class_string(classname)
        for sectiondic in ymldic[classname]:
            name = next(iter(sectiondic))
            if sectiondic[name] is not None:
                for func in sectiondic[name]:
                    if isinstance(func, list) or func.startswith(classname.split("::")[-1]):
                        continue # ignore
                    out += pybind11_mem_fn(classname, func)
        return out


def main():
    if sys.version_info[0] < 3:
        raise Exception("Python 3 is required")
    args = _parse_args()
    for fname in args.files:
        print(generate(fname))


if __name__ == "__main__":
    main()
