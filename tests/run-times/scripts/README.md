# How to use the scripts

Presently, there are three main scripts of importance in this directory which
are described below. These scripts are written in such a way that it is possible
to use some of the functions in an interactive terminal, in the case that the
scripts don't offer precisely what is wanted.

## `generate-test-times.py`

This script (in its simplest invocation) takes a `libsemigroups` test executable
and some `Catch2` tags, and uses `hyperfine` to record the times of those tests.
Files are stored in the `json` format, where the file name records the date of
the tests, the current commit hash, the machine architecture and the command
used to run the test.

The full options are:

```
./tests/run-times/scripts/generate-test-times.py -h
usage: generate-test-times.py [-h] [--max MAX] test_executable tags

A script for timing libsemigroups tests.
The output of this script is written to results/<DATE>-<COMMIT_HASH>-<ARCHITECTURE>-<TEST_COMMAND>.json

positional arguments:
  test_executable  The path to the test executable that hyperfine should benchmark
  tags             The tags that dictate which tests should be run. If no tags are specified, then all tests are run.

options:
  -h, --help       show this help message and exit
  --max MAX        Perform at most MAX runs for each command. By default, there is no limit and hyperfine will decide.

example:
generate-test-times.py --max=5 ./test_knuth_bendix "[quick]"
```

## `markdown.py`

This script takes any number of `json` test-time files, and produces two
markdown tables. The first table contains the comparison of every test, and the
second table contains only the tests for which the difference in mean test times
is significantly different.

The full options are:

```
./tests/run-times/scripts/markdown.py -h
usage: markdown.py [-h] [--factor FACTOR] base_file comparison_files [comparison_files ...]

A tool for comparing the runtime of libsemigroups tests

positional arguments:
  base_file         file path to the test time data against which all other test times will be compared
  comparison_files  file paths to the test time data to compare against base_file

options:
  -h, --help        show this help message and exit
  --factor FACTOR   the number of multiples of the standard deviation away from the mean that is considered significant
                    (default: 2)
```

## `plots.py`

This script takes any number of `json` test-time files, and produces a plot that
compares the tests that are significantly different.

The full options are:

```
./tests/run-times/scripts/plots.py -h
usage: plots.py [-h] [--factor FACTOR] [--title TITLE] [--kind KIND] [--output OUTPUT] [--log] [--y-min Y_MIN] [--y-max Y_MAX]
                base_file comparison_files [comparison_files ...]

A tool for plotting runtime comparisons for libsemigroups tests

positional arguments:
  base_file         file path to the test time data against which all other test times will be compared
  comparison_files  file paths to the test time data to compare against base_file

options:
  -h, --help        show this help message and exit
  --factor FACTOR   the number of multiples of the standard deviation away from the mean that is considered significant
                    (default: 2)
  --title TITLE     the title of the plot
  --kind KIND       the kind of plot. See https://seaborn.pydata.org/generated/seaborn.catplot.html for the full list of
                    options (default: 'bar')
  --output OUTPUT   the filename to save the plot to
  --log             whether to use a logarithmic y-axis for the times
  --y-min Y_MIN     the minimum y-value of the plot (default: 0.0). This is ignored if --log-count is set
  --y-max Y_MAX     the maximum y-value of the plot (default: None). This is ignored if --log-count is set
```
