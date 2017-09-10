#!/bin/bash
set -e

benchmark/build.sh

LOG_DIR=$(pwd)/benchmark/logs
LOG_FILE_SUFFIX=`date +%Y%m%d%H%M%S`.json
FLAGS="--benchmark_min_time=0 --benchmark_report_aggregates_only=true --benchmark_format=console"

mkdir -p $LOG_DIR
cd benchmark/src/build

nr_files=$(ls -1 ../*.cpp | wc -l)
i=0
for cpp_file in ../*.cpp
do
    i=$((i+1))
    exe_file=$(echo $cpp_file | sed 's/^\.\.\///g; s/\.bm.cpp$//g')
    LOG_FILE=$LOG_DIR/$exe_file-$LOG_FILE_SUFFIX
    echo
    echo Executing file $i of $nr_files: $exe_file . . .
    echo
    echo Logging to $LOG_FILE . . .
    ./$exe_file $FLAGS --benchmark_out=$LOG_FILE
done
