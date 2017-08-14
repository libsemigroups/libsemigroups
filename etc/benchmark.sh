#!/bin/bash
set -e

LOG_DIR=../../logs

mkdir -p $LOG_DIR
LOG_FILE=`date +%Y-%m-%d-%H-%M-%S`.log

echo "Logging to benchmark/logs/"$LOG_FILE" . . . "
LOG_FILE=$LOG_DIR/$LOG_FILE

FLAGS="--benchmark_min_time=0 --benchmark_report_aggregates_only=true --benchmark_format=console --benchmark_out="$LOG_FILE

if [ -d benchmark/bench/build ]; then
  cd benchmark/bench/build
  make 
  ./semigroups $FLAGS
else
  mkdir -p benchmark/bench/build
  cd benchmark/bench/build
  cmake -DCMAKE_BUILD_TYPE=Release  ..
  make
  ./semigroups $FLAGS 
fi
