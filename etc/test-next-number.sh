#!/bin/bash
set -e

bold() {
  printf "\033[1m%s\033[0m\n" "$*"
}

if [[ $# -ne 1 ]]; then
  bold "error expected 1 argument, got $#!"
  exit 1
elif [[ ! -x $1 ]]; then 
  export EXEC="test_all" 
  export TAG="$1"
else
  export EXEC=$1
  export TAG=
fi

./$EXEC $TAG -l | perl -ne 'print if s/^.*(\d\d\d):.*$/\1/' > tmp.txt

python3 - <<END
import os, sys
f = open('tmp.txt', 'r')
l = sorted([int(x.strip()) for x in f.readlines()])
for i in range(1, l[-1]):
  if not i in l:
    print('The first available test case in %s is: %s' % (os.environ['EXEC'], str(i).zfill(3)))
    sys.exit(0)
print('The first available test case in %s is: %s' % (os.environ['EXEC'], str(l[-1] + 1).zfill(3)))
END

rm -f tmp.txt
