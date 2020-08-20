#!/bin/bash
set -e

# The following are required to build all of the documentation
ci/travis-setup.sh
./configure

sudo -H pip3 install --upgrade pip
sudo -H pip3 install setuptools
sudo -H pip3 install -r docs/requirements.txt
sudo -H pip3 install --upgrade --ignore-installed pyyaml

echo "Doxygen version: "
doxygen --version

echo "Sphinx version: "
sphinx-build --version

etc/make-doc-yml.sh | tee make-doc-yml.log
etc/make-doc-sphinx.sh | tee make-doc-sphinx.log

echo
( ! grep "WARNING:" make-doc-sphinx.log)
