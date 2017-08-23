#!/usr/bin/env python
#pylint: disable-all

import re
import os
import sys
from os import listdir
from os.path import isfile, join

_SOURCE_DIRS = ['src', 'src/cong']
_THRESHOLD = 95.0

_ERR_PREFIX = '\033[31mlcov-summary.py: error: '

def get_prefix(covpercent):
    covpercent = float(covpercent)
    if covpercent >= _THRESHOLD:
        return '\033[40;38;5;82m'
    elif covpercent >= _THRESHOLD - 5.0:
        return '\033[33m'
    elif covpercent >= _THRESHOLD - 15.0:
        return '\033[40;38;5;208m'
    else:
        return '\033[31m'

def cpp_files(folder):
    files = []
    for f in listdir(folder):
        if isfile(join(folder, f)) and (f.split('.')[-1] in ['cc', 'h']):
            files.append(f)
    return files

_SOURCES = map(cpp_files, _SOURCE_DIRS)

def find(name, path):
    name = name.split('/')
    if len(name) == 1:
        name = path + '/' + name[0]
        if not (os.path.exists(name) and os.path.isfile(name)):
            sys.exit(_ERR_PREFIX + name + ' does not exist!\033[0m')
        return name
    else:
        for root, dirs, files in os.walk(path):
            if name[0] in dirs:
                return find('/'.join(name[1:]), root + '/' + name[0])
        sys.exit(path + '/' + name[0] + ' does not exist!\033[0m')

# The directory containing the html output of lcov, i.e.
# libsemigroups/log/2017...

if len(sys.argv) != 2:
    sys.exit(_ERR_PREFIX + ' the should be exactly 1 argument\033[0m')

covdir = os.getcwd() + '/' + str(sys.argv[1])
if not (os.path.exists(covdir) and os.path.isdir(covdir)):
    sys.exit(_ERR_PREFIX + covdir + ' does not exist!\033[0m')

# Find the main coverage file
for i in xrange(len(_SOURCE_DIRS)):
    fname = _SOURCE_DIRS[i] + '/index.html'
    fname = find(fname, covdir)
    covfile = file(fname, 'r').read()
    for f in _SOURCES[i]:
        pat = re.compile(r'>' + f + r'.*?>(\d{1,3}.\d)&nbsp', re.DOTALL)
        m = pat.search(covfile)
        if m:
            print (get_prefix(m.group(1)) + m.group(1) + '%\033[0m\t' +
                   _SOURCE_DIRS[i] + '/' + f)
