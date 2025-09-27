#!/usr/bin/env python3
"""Plot the runtime of libsemigroups tests"""

import seaborn as sns
import matplotlib.pyplot as plt
import argparse
from runtime_comparison.load_data import to_mean_time_comparison_df, to_all_times_df
from runtime_comparison.process_data import significantly_different_times

###########################################################
# Helper functions
###########################################################


def float_or_none(input: str):
    if input.lower() == "none":
        return None
    else:
        return float(input)


###########################################################
# CLI arguments
###########################################################

parser = argparse.ArgumentParser(
    description="A tool for plotting runtime comparisons for libsemigroups tests"
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
    type=float,
    default=2,
    help="the number of multiples of the standard deviation away from the mean "
    "that is considered significant (default: 2)",
)

parser.add_argument("--title", type=str, help="the title of the plot")

parser.add_argument(
    "--kind",
    type=str,
    default="bar",
    help="the kind of plot. See https://seaborn.pydata.org/generated/seaborn.catplot.html for the "
    "full list of options (default: 'bar')",
)
parser.add_argument("--output", type=str, help="the filename to save the plot to")

parser.add_argument(
    "--log",
    help="whether to use a logarithmic y-axis for the times",
    action="store_true",
)
parser.add_argument(
    "--y-min",
    type=float_or_none,
    default=0.0,
    help="the minimum y-value of the plot (default: 0.0). This is ignored if --log-count is set",
)

parser.add_argument(
    "--y-max",
    type=float_or_none,
    default=None,
    help="the maximum y-value of the plot (default: None). This is ignored if --log-count is set",
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

    # Find the tests with significantly different runtimes
    df = to_mean_time_comparison_df(args.base_file, *args.comparison_files)
    sig_tests = significantly_different_times(df, args.factor).index

    # Get all of the times (rather than just the means) of those tests
    all_times_dfs = to_all_times_df(
        args.base_file, *args.comparison_files, restrict=sig_tests
    )

    # Plot the results
    sns.set_theme()
    sns.catplot(
        data=all_times_dfs,
        x="command",
        y="time",
        hue="runner",
        kind=args.kind,
        legend_out=False,
    )

    plt.ylabel("Time [s]")
    if args.title:
        plt.title(args.title)

    if args.log:
        plt.yscale("log")
    else:
        plt.ylim(args.y_min, args.y_max)

    if args.output:
        plt.savefig(args.output, dpi=600)
    else:
        plt.show()


if __name__ == "__main__":
    main()
