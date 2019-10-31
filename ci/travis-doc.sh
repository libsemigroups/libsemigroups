#!/bin/bash
set -e

# The following are required to build all of the documentation
ci/travis-setup.sh
ci/travis-setup-hpcombi.sh
./configure

sudo -H pip3 install --upgrade pip

sudo -H pip3 install sphinx 
sudo -H pip3 install bs4 
sudo -H pip3 install --upgrade --ignore-installed pyyaml
sudo -H pip3 install lxml 
sudo -H pip3 install breathe 
sudo -H pip3 install sphinx_rtd_theme 
sudo -H pip3 install sphinx-copybutton 
sudo -H pip3 install sphinxcontrib-bibtex

echo "Doxygen version: "
doxygen --version

echo "Sphinx version: "
sphinx-build --version

etc/make-doc-yml.sh | tee make-doc-yml.log
etc/make-doc-sphinx.sh | tee make-doc-sphinx.log

echo
( ! grep "WARNING:" make-doc-sphinx.log)
