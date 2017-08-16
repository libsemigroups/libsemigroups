#!/bin/bash
set -e

LOG_DIR=$(pwd)/benchmark/logs
mkdir -p $LOG_DIR
LOG_FILE_SUFFIX=`date +%Y%m%d-%H-%M-%S`.json

FLAGS="--benchmark_min_time=0 --benchmark_report_aggregates_only=true --benchmark_format=console"

if [ -d benchmark/bench/build ]; then
  cd benchmark/bench/build
else
  mkdir -p benchmark/bench/build
  cd benchmark/bench/build
  cmake -DCMAKE_BUILD_TYPE=Release  ..
fi
make

nr_files=$(ls -1 ../*.cpp | wc -l)
i=0
for cpp_file in ../*.cpp
do
    i=$((i+1))
    exe_file=$(echo $cpp_file | sed 's/^\.\.\///g; s/\.cpp$//g')
    LOG_FILE=$LOG_DIR/$exe_file-$LOG_FILE_SUFFIX
    echo
    echo Executing file $i of $nr_files: $exe_file . . .
    echo
    echo Logging to $LOG_FILE . . .
    ./$exe_file $FLAGS --benchmark_out=$LOG_FILE
done
