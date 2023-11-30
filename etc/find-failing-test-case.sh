#!/bin/bash
set -e

exclude="[exclude:standard][exclude:extreme][exclude:fail][exclude:no-valgrind][exclude:no-coverage]"

for i in $(seq -f "%03g" 0 117); do  
  ./test_todd_coxeter "[$i]$exclude"
done
