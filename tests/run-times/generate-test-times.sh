#!/bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

############################################################
# Help                                                     #
############################################################
Help() {
    # Display Help
    cat <<-EOF
A script for timing libsemigroups tests.

${bold}Usage:${normal}
    $(basename $0) -h
    $(basename $0) <test-executable> [<tags>]

${bold}Arguments:${normal}
    <test-executable>
            The path to the test executable that hyperfine should benchmark
    <tags> (optional)
            The tags that dictate which tests should be run. If no tags are
            specified, then all tests are run.

${bold}Options:${normal}
    -h
            Display this message

${bold}Example:${normal}
    $(basename $0) ./test_knuth_bendix "[quick]"
EOF
}

############################################################
# Main program                                             #
############################################################

# Get the options
while getopts "h" option; do
    case $option in
    h) # display Help
        Help
        exit
        ;;
    \?) # Invalid option
        echo "Error: Invalid option"
        exit 1
        ;;
    esac
done

if [ $# -ne 2 ]; then
    echo "Expected two arguments, but found $#."
    echo "For more information, try '-h'."
    exit 1
fi

# Change file name to DATE+HASH+ARCHITECTURE+COMMAND
printf -v date "%(%Y_%m_%d_%H_%M_%S)T" -1
hash=$(git rev-parse HEAD)
output_file="tests/run-times/results/${date}_${hash}.json"

tags=$($1 $2 --list-tags | grep -Po --line-buffered "(?<=LIBSEMIGROUPS_TEST_NUM=)\d\d\d" | tr '\n' ',' | sed 's/.$//')

hyperfine --max-runs=3 -L tag $tags "$1 '[{tag}]'" --export-json $output_file
