#!/usr/bin/env python
# /// script
# requires-python = ">=3.10"
# dependencies = [
#     "matplotlib",
#     "pyqt6",
#     "numpy",
# ]
# ///

"""
This script shows `hyperfine` benchmark results as a lineplot.
Note all the input files must contain results for all commands.
"""

import argparse
import json
import pathlib

import seaborn as sns
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

# Define input

parser = argparse.ArgumentParser(description=__doc__)
parser.add_argument(
    "files",
    nargs="+",
    type=pathlib.Path,
    help="JSON files with benchmark results",
)
parser.add_argument("--title", help="Plot Title")
parser.add_argument(
    "--benchmark-names", nargs="+", help="Names of the benchmark groups"
)
parser.add_argument("-o", "--output", help="Save image to the given filename")

args = parser.parse_args()

# Read in data

commands = None
means = []
inputs = []

if args.benchmark_names:
    assert len(args.files) == len(
        args.benchmark_names
    ), "Number of benchmark names must match the number of input files."

for i, filename in enumerate(args.files):
    with open(filename) as f:
        results = json.load(f)["results"]
    benchmark_commands = [b["command"] for b in results]
    if commands is None:
        commands = benchmark_commands
    else:
        assert (
            commands == benchmark_commands
        ), f"Unexpected commands in {filename}: {benchmark_commands}, expected: {commands}"
    means.append([b["mean"] for b in results])
    if args.benchmark_names:
        inputs.append(args.benchmark_names[i])
    else:
        inputs.append(filename.stem)

# Analyse

# sns.set_theme(style="whitegrid")

data = pd.DataFrame(means, index=inputs, columns=commands)
sns.lineplot(data=data, markers=True, legend=False).set_yscale("log")

if args.output:
    plt.savefig(args.output)
else:
    plt.show()

sns.lineplot(data=data.diff(), markers=True, legend=False).set_yscale("log")
if args.output:
    plt.savefig(args.output)
else:
    plt.show()
