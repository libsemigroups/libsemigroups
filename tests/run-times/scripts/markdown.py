#!/usr/bin/env python3
"""Compare the runtime of libsemigroups tests and output with markdown"""

import argparse
from runtime_comparison.load_data import to_mean_time_comparison_df
from runtime_comparison.process_data import significantly_different_times

###########################################################
# CLI arguments
###########################################################

parser = argparse.ArgumentParser(
    description="A tool for comparing the runtime of libsemigroups tests"
)

parser.add_argument(
    "base_file",
    type=str,
    help="file path to the test time data against which all other test times will be compared",
)

parser.add_argument(
    "comparison_files",
    type=str,
    nargs="+",
    help="file paths to the test time data to compare against base_file",
)

parser.add_argument(
    "--factor",
    type=int,
    default=2,
    help="the number of multiples of the standard deviation away from the mean "
    "that is considered significant (default: 2)",
)

args = parser.parse_args()

###########################################################
# Main
###########################################################


def main():
    if args.base_file in args.comparison_files:
        raise ValueError(
            "Expected the base_file and the list of comparison files to be disjoint. Found the "
            f"file {args.base_file} in both. "
        )

    df = to_mean_time_comparison_df(args.base_file, *args.comparison_files)
    print("All tests:")
    print(df.drop("stddev", axis="columns").to_markdown())
    print("\nSignificantly different tests:")
    print(
        significantly_different_times(df, factor=args.factor)
        .drop("stddev", axis="columns")
        .to_markdown()
    )


if __name__ == "__main__":
    main()
