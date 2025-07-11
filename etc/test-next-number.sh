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

./$EXEC $TAG --list-tests > tmp.txt

python3 - <<END
import os, re, sys
with open('tmp.txt', 'r') as file:
  lines = file.read()
it = re.finditer(r"LIBSEMIGROUPS_TEST_NUM=\s*(\d\d\d)", lines, re.MULTILINE)
matches = sorted(list(set([match.group(1) for match in it])))
result = str(int(matches[-1]) + 1).zfill(3)
for i in range(1, int(matches[-1])):
  possible = str(i).zfill(3)
  if not possible in matches:
    result = possible
    break
print(f'The first available test case in "{os.environ['EXEC']}" is: {result}')
END

rm -f tmp.txt
