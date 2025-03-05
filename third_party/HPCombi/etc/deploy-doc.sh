#!/bin/bash
set -e

if ! [[ -d "gh-pages" ]] ; then
    echo -e "Error, the \"gh-pages\" directory is not present:"
    echo -e "git clone --branch=gh-pages git@github.com:libsemigroups/hpcombi.git gh-pages"
    exit 1 
fi

printf "\033[0;32mDeploying updates to GitHub...\033[0m\n"

mkdir -p build
cd build
if [[ -f Makefile ]] ; then
  make clean
fi
cmake ..
make doc
cd ..
cp -r build/doc/html/* gh-pages
cd gh-pages
git add .
msg="rebuilding site $(date)"
if [ -n "$*" ]; then
	msg="$*"
fi
git commit -m "$msg"
git push origin gh-pages
