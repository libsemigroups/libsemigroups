#!/usr/bin/env python3
import sys
import argparse
from math import isqrt

import matplotlib
from bs4 import BeautifulSoup
from matplotlib import pyplot as plt

# This file should be from libsemigroups/etc

matplotlib.rcParams["mathtext.fontset"] = "stix"
matplotlib.rcParams["font.family"] = "STIXGeneral"

colors = [
    (238 / 255, 20 / 255, 135 / 255),
    (0 / 255, 221 / 255, 164 / 255),
    (86 / 255, 151 / 255, 209 / 255),
    (249 / 255, 185 / 255, 131 / 255),
    (150 / 255, 114 / 255, 196 / 255),
]

# Filenames should be: name.something.xml -> name.png


def determine_subplot_layout(nr_plots: int) -> tuple[int, int]:
    """Determine the number of rows and columns from number of plots."""
    nr_plot_rows = isqrt(nr_plots)
    nr_plot_cols = nr_plot_rows
    if nr_plot_rows * nr_plot_cols < nr_plots:
        nr_plot_cols += 1
    while nr_plot_rows * nr_plot_cols < nr_plots:
        nr_plot_rows += 1
    return nr_plot_rows, nr_plot_cols


def process_result(result_soup) -> tuple[str, float]:
    """Extract data from a single xml result entry.

    Returns
    -------
    result_name: str
        The test case name
    result_time: float
        The test case time in nanoseconds
    """
    result_name = result_soup["name"]
    if "name" not in result_soup.attrs:
        raise ValueError(
            f"Malformed benchmark file, result record does not contain 'name': {result_soup}"
        )
    result_mean_soup = result_soup.find("mean")
    if result_mean_soup is None:
        raise ValueError(
            f"Malformed benchmark file, result record does not contain 'mean': {result_soup}"
        )
    if "value" not in result_mean_soup.attrs:
        raise ValueError(
            f"Malformed benchmark file, result 'mean' record does not contain 'value': {result_mean_soup}"
        )
    result_time = float(result_mean_soup["value"]) / 1  # time in nanoseconds
    return result_name, result_time


def make_ax(ax, test_case_soup, plot_speedup_type: str):
    if "name" not in test_case_soup.attrs:
        raise ValueError(
            f"Malformed benchmark file, test_case record does not contain 'name': {test_case_soup}"
        )
    results = test_case_soup.find_all("BenchmarkResults")
    result_names, result_times = zip(*map(process_result, reversed(results)))
    bars = ax.barh(
        result_names,
        result_times,
        align="center",
        color=[colors[i % len(colors)] for i in range(len(result_names))],
    )
    if plot_speedup_type == "slowest":
        reference_time = max(result_times)
    else:
        # This is the first element, since we reverse due to horizontal plot
        reference_time = result_times[-1]
    # TODO: fix type issue
    result_speedups = [reference_time / result_time for result_time in result_times]
    ax.bar_label(
        bars,
        list(map("{:.1f}".format, result_speedups)),
        padding=5,
        fontsize="6",
    )
    test_name = test_case_soup["name"]
    ax.set_title(f'Benchmark "{test_name}" runtime')
    ax.set_xlabel(f"time, ns")
    return ax


def make_fig(
    benchmark_soup,
    plot_width_inches: float = 7.5,
    plot_height_inches: float = 5,
    plot_title: None | str = None,
    plot_speedup_type: str = "slowest",
):
    assert plot_speedup_type is not None
    test_cases = benchmark_soup.find_all("TestCase")
    nr_plots = len(test_cases)
    nr_plot_rows, nr_plot_cols = determine_subplot_layout(nr_plots)
    fig, axs = plt.subplots(
        nr_plot_rows,
        nr_plot_cols,
        figsize=(plot_width_inches * nr_plot_cols, plot_height_inches * nr_plot_rows),
    )
    for test_case_soup, ax in zip(test_cases, axs.flat):
        ax = make_ax(ax, test_case_soup, plot_speedup_type)
    for ax in axs.flat[nr_plots:]:
        fig.delaxes(ax)

    if plot_title is not None:
        fig.suptitle(args.title, fontsize=16, weight="bold")
    return fig


def check_filename(xml_fnam):
    if len(xml_fnam.split(".")) < 2:
        raise ValueError(f"expected filename of form x.xml found {xml_fnam}")


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="A tool for plotting HPCombi benchmark data",
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
    )
    parser.add_argument(
        "xml_filenames",
        metavar="file",
        type=str,
        nargs="+",
        help="The names of xml file(s) to be processed. If multiple files are provided, then plots each (with same title)",
    )
    parser.add_argument(
        "-o",
        "--output",
        type=str,
        help="Output file name. If not provided then a custom naming method is used.",
    )
    parser.add_argument(
        "--title",
        type=str,
        help="The title of the plot.",
    )
    parser.add_argument(
        "--speedup",
        type=str,
        choices=["slowest", "first"],
        default="slowest",
        help="Speedup display type. 'slowest' compares to slowest benchmark. 'first' compares to first benchmark.",
    )
    parser.add_argument(
        "--width",
        type=float,
        default=7.5,
        help="Single subplot width in inches.",
    )
    parser.add_argument(
        "--height",
        type=float,
        default=5.0,
        help="Single subplot height in inches.",
    )
    args = parser.parse_args()
    print(args.title)

    for x in args.xml_filenames:
        check_filename(x)
        # TODO more arg checks

    for x in args.xml_filenames:
        with open(x, "r") as in_file:
            xml_text = in_file.read()
        soup = BeautifulSoup(xml_text, "xml")
        fig = make_fig(
            soup,
            plot_width_inches=args.width,
            plot_height_inches=args.height,
            plot_title=args.title,
            plot_speedup_type=args.speedup,
        )
        fig.tight_layout()

        xml_fnam = x
        png_fnam = args.output
        if png_fnam is None:
            png_fnam = "".join(xml_fnam.split(".")[:-1]) + ".png"
        print("Writing {} . . .".format(png_fnam))
        fig.savefig(png_fnam, format="png", dpi=300)
    sys.exit(0)
