#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import os
import subprocess
import sphinx_rtd_theme

on_rtd = os.environ.get("READTHEDOCS", None) == "True"
if on_rtd:
    subprocess.call(
        "cd .. && mkdir -p build && doxygen && cd .. && etc/make-doc-yml.sh",
        shell=True,
    )

html_theme = "sphinx_rtd_theme"

html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]

html_theme_options = {
    # Toc options
    "includehidden": False,
}


def setup(app):
    app.add_css_file("custom.css")


extensions = [
    "breathe",
    "sphinx.ext.mathjax",
    "sphinx_copybutton",
    "sphinxcontrib.bibtex",
]

bibtex_bibfiles = ["libsemigroups.bib"]

breathe_projects = {"libsemigroups": "../build/xml"}
breathe_projects_source = {}
templates_path = ["_templates"]
html_static_path = ["_static"]
source_suffix = ".rst"
master_doc = "index"
project = "libsemigroups"
copyright = "2019-22, J. D. Mitchell"
author = "J. D. Mitchell"
cpp_index_common_prefix = ["libsemigroups::"]

html_logo = ""

exclude_patterns = []
highlight_language = "c++"
pygments_style = "sphinx"
todo_include_todos = False
htmlhelp_basename = "libsemigroups"
