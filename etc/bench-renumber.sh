#!/bin/bash
set -e

bold() {
  printf "\033[1m%s\033[0m\n" "$*"
}

re='^[0-9]+$'

if [[ $# -ne 1 && $# -ne 2 ]]; then
  bold "error expected 1 or 2 arguments, got $#!"
  exit 1
elif [[ ! -f $1 ]]; then 
  bold "Error, expected a file as first argument, $1 is not a file!" 
  exit 1
elif  [[ $# -eq 2 && ! $2 =~ $re ]]; then
  bold "Error, expected a positive integer as second argument, $2 is not a positive integer!" 
  exit 1
fi

if [[ $# -eq 2 ]]; then
  export START="$2"
else
  export START=0
fi

FNAME="$1" python - <<END
import os, re, sys
p1 = re.compile(r'\[\d\d\d\]"')
p2 = re.compile(r'TEST_CASE')
lines = []
with open(os.environ['FNAME'], 'r') as f:
  lines = f.readlines()
  prev1 = ''
  prev2 = ''
  nr = int(os.environ['START'])
  for i in range(len(lines)):
    if p1.search(lines[i]) and (p2.search(lines[i]) or p2.search(prev1) or p2.search(prev2)):
       lines[i] = re.sub(p1, '[%s]",' % str(nr).zfill(3), lines[i])
       nr += 1
    prev2 = prev1
    prev1 = lines[i]
  print("%s: %d benchmarks renumbered" % (os.environ['FNAME'], nr - int(os.environ['START'])))
  print("%s: next benchmark number is %d" % (os.environ['FNAME'], nr))

with open(os.environ['FNAME'], 'w') as f:
  f.writelines(lines)
sys.exit(0)
END
