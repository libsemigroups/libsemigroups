#!/bin/bash
set -e

bold() {
  printf "\033[1m%s\033[0m\n" "$*"
}

if [[ $# -ne 1 ]]; then
  bold "error expected 1 argument, got $#!"
  exit 1
elif [[ ! -x $1 ]]; then 
  bold "Error, expected an executable, $1 is not executable!" 
  exit 1
fi

./$1 -l | perl -ne 'print if s/^.*(\d\d\d):.*$/\1/' > $1.txt

FNAME="$1" python - <<END
import os, sys
f = open(os.environ['FNAME'] + '.txt', 'r')
l = sorted([int(x.strip()) for x in f.readlines()])
for i in range(1, l[-1]):
  if not i in l:
    print('The first available test case in %s is: %s' % (os.environ['FNAME'], str(i).zfill(3)))
    sys.exit(0)
print('The first available test case in %s is: %s' % (os.environ['FNAME'], str(l[-1] + 1).zfill(3)))
END

rm -f $1.txt
