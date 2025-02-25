#!/bin/bash
set -e

bold() {
    printf "\033[1m%s\033[0m\n" "$*"
}

dim() {
    printf "\033[2m%s\033[0m\n" "$*"
}

if [[ $# -lt 2 ]]; then
  bold "error at least 2 arguments, got $(($#-1))!"
  exit 1
elif [[ -f $1 && ! -x $1 ]]; then
  bold "error expected an executable file, $1 is not one!"
  exit 1
fi

if [[ ! -f config.log ]]; then
  bold "error cannot locate config.log file!"
  exit 1
fi

args=$(echo $1 $2 | gsed 's/[^a-zA-Z0-9]/_/g' )
args=$(echo $args | gsed 's/_\+/_/g' )
result=`git rev-parse --short HEAD`-"$args".log

if [[ -f $result ]]; then
  bold "file $result exists!"
  exit 1
fi

echo "## Created:       " `date` >> "$result"
echo "## System info:   " `sysctl -n machdep.cpu.brand_string` >> "$result"
echo "## Computer name: " `scutil --get LocalHostName` >> "$result"
echo "## Configure:     " `head -7 config.log | tail -1` >> "$result"
echo "## Arguments:     ./$@" >> "$result"

echo ./"$@" | tee -a "$result"
./"$@" | tee -a "$result"
