#!/bin/bash
set -e

bold() {
  printf "\033[1m%s\033[0m\n" "$*"
}

if [[ $# -ne 1 ]]; then
  bold "error expected 1 argument, got $#!"
  exit 1
elif [[ ! -f $1 ]]; then 
  bold "Error, expected a file, $1 is not a file!" 
  exit 1
fi

FNAME="$1" python - <<END
import os, re, sys
p1 = re.compile(r'"\d\d\d",')
p2 = re.compile(r'LIBSEMIGROUPS_TEST_CASE')
lines = []
with open(os.environ['FNAME'], 'r') as f:
  lines = f.readlines()
  prev = ''
  nr = 0
  for i in range(len(lines)):
    if p1.search(lines[i]) and (p2.search(lines[i]) or p2.search(prev)):
       lines[i] = re.sub(p1, '"%s",' % str(nr).zfill(3), lines[i])
       nr += 1
    prev = lines[i]
with open(os.environ['FNAME'], 'w') as f:
  f.writelines(lines)
sys.exit(0)
END
