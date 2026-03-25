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

FNAME="$1" python3 - <<END
import os, re, sys
p1 = re.compile(r'\[\d\d\d\]')
p2 = re.compile(r'(TEST_CASE|TEST_AGREES)')
lines = []
fnam = os.environ['FNAME']
start = int(os.environ['START'])

with open(fnam, 'r') as f:
  lines = f.readlines()
  prev1 = ''
  prev2 = ''
  nr = start
  for i in range(len(lines)):
    if p1.search(lines[i]) and (p2.search(lines[i]) or p2.search(prev1) or p2.search(prev2)):
       lines[i] = re.sub(p1, f'[{str(nr).zfill(3)}]', lines[i])
       nr += 1
    prev2 = prev1
    prev1 = lines[i]
  print(f"{fnam}: {nr - start} tests renumbered")
  print(f"{fnam}: next test number is {str(nr).zfill(3)}")

with open(os.environ['FNAME'], 'w') as f:
  f.writelines(lines)
sys.exit(0)
END
