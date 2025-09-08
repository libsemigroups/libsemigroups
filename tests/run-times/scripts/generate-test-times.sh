#!/bin/bash

bold=$(tput bold)
normal=$(tput sgr0)

############################################################
# Help                                                     #
############################################################
Help() {
    # Display Help
    cat <<-EOF
A script for timing libsemigroups tests. The output of this script is written to
"results/<DATE>-<COMMIT_HASH>-<ARCHITECTURE>-<TEST_COMMAND>.json".

${bold}Usage:${normal}
    $(basename $0) -h
    $(basename $0) [-M] <test-executable> [<tags>]

${bold}Arguments:${normal}
    <test-executable>
            The path to the test executable that hyperfine should benchmark
    <tags> (optional)
            The tags that dictate which tests should be run. If no tags are
            specified, then all tests are run.

${bold}Options:${normal}
    -h
            Display this message
    -M <NUM>
            Perform at most NUM runs for each command. By default, there is no
            limit.

${bold}Example:${normal}
    $(basename $0) ./test_knuth_bendix "[quick]"
EOF
}

############################################################
# Main program                                             #
############################################################
arg_string=()

# Get the options
while getopts "hM:" option; do
    case $option in
    h) # display Help
        Help
        exit
        ;;
    M) # set the maximum number of runs
        M=$OPTARG
        arg_string+=("--max-runs=$M ")
        ;;
    \?) # Invalid option
        echo "Error: Invalid option"
        exit 1
        ;;
    esac
done

shift $((OPTIND-1))

# Validate number of arguments
if [ $# -eq 0 ] || [ $# -ge 4 ]; then
    echo "Expected 0, 1 or 2 arguments, but found $#."
    echo "For more information, try '-h'."
    exit 1
fi

# Construct the file path
SCRIPT_DIR=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" &>/dev/null && pwd)

printf -v DATE "%(%Y_%m_%d_%H_%M_%S)T" -1
HASH=$(git rev-parse --short HEAD)
ARCH=$(uname -m)
COMMAND=$(basename $1)

OUTPUT_FILE="${SCRIPT_DIR}/../results/${DATE}-${HASH}-${ARCH}-${COMMAND}.json"

# Find the the tags of the tests to run, wrap them in quotes, and separate by
# commas
TAGS=$($1 $2 --list-tags | grep -Po --line-buffered "(?<=LIBSEMIGROUPS_TEST_NUM=)\d\d\d" | tr '\n' ',' | sed 's/.$//')

# Run the specified tests and export to json file.
hyperfine $arg_string -L tag $TAGS "$1 '[{tag}]'" --export-json $OUTPUT_FILE --sort=command
echo Writing results to $OUTPUT_FILE . . .
