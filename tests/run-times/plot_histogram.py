#!/usr/bin/env python
# /// script
# requires-python = ">=3.10"
# dependencies = [
#     "matplotlib",
#     "pyqt6",
#     "numpy",
# ]
# ///

"""This program shows `hyperfine` benchmark results as a histogram."""

import argparse
import json

import matplotlib.pyplot as plt
import numpy as np

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument("file", help="JSON file with benchmark results")
parser.add_argument("--title", help="Plot title")
parser.add_argument("--bins", help="Number of bins (default: auto)")
parser.add_argument(
    "--type", help="Type of histogram (*bar*, barstacked, step, stepfilled)"
)
parser.add_argument("-o", "--output", help="Save image to the given filename.")
parser.add_argument(
    "--t-min", metavar="T", help="Minimum time to be displayed (seconds)"
)
parser.add_argument(
    "--t-max", metavar="T", help="Maximum time to be displayed (seconds)"
)
parser.add_argument(
    "--log-count",
    help="Use a logarithmic y-axis for the event count",
    action="store_true",
)

args = parser.parse_args()

with open(args.file) as f:
    results = json.load(f)["results"]

all_times = [b["mean"] for b in results]

t_min = float(args.t_min) if args.t_min else min(all_times)
t_max = float(args.t_max) if args.t_max else max(list(map(np.max, all_times)))

bins = int(args.bins) if args.bins else "auto"
histtype = args.type if args.type else "bar"

plt.figure(figsize=(10, 5))
plt.hist(
    all_times,
    bins=bins,
    histtype=histtype,
    range=(t_min, t_max),
)

plt.xlabel("Time [s]")
if args.title:
    plt.title(args.title)

if args.log_count:
    plt.yscale("log")
else:
    plt.ylim(0, None)

if args.output:
    plt.savefig(args.output, dpi=600)
else:
    plt.show()
