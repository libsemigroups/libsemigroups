#!/usr/bin/env python3

import os
import re
import sys

import matplotlib
from bs4 import BeautifulSoup
from matplotlib import pyplot as plt

matplotlib.rcParams["mathtext.fontset"] = "stix"
matplotlib.rcParams["font.family"] = "STIXGeneral"

# Filenames should be: name.something.xml -> name.png


def normalize_xml(xml_fnam):
    with open(xml_fnam, "r") as f:
        xml = f.read()
        xml = re.sub("&lt;", "<", xml)
        xml = re.sub("{{", "[", xml)
        xml = re.sub("}}", "]", xml)
    with open(xml_fnam, "w") as f:
        f.write(xml)


def xml_stdout_get(xml, name):
    try:
        return xml.find("StdOut").find(name)["value"]
    except (KeyError, TypeError, NameError, AttributeError):
        print("No label {} in StdOut element, skipping . . .".format(name))
        return None


def time_unit(Y):
    time_units = ("nanoseconds", "microseconds", "milliseconds", "seconds")
    index = 0

    while all(y > 1000 for y in Y) and index < len(time_units):
        index += 1
        Y = [y / 1000 for y in Y]
    return time_units[index], Y


def add_plot(xml_fnam):
    print("Reading {} . . .".format(xml_fnam))
    normalize_xml(xml_fnam)

    xml = BeautifulSoup(open(xml_fnam, "r"), "xml")
    results = xml.find_all("BenchmarkResults")

    # Benchmark labels must be the value that is the x-axis

    title = xml_stdout_get(xml, "Title")
    xlabel = xml_stdout_get(xml, "XLabel")
    ylabel = xml_stdout_get(xml, "YLabel")
    label = xml_stdout_get(xml, "Label")

    data = xml_stdout_get(xml, "Data")
    if data is None:
        yfunc = lambda x, y: y
    else:
        data = eval(data)
        if isinstance(data, list):
            yfunc = lambda x, y: y / data[x]
        elif isinstance(data, int):
            yfunc = lambda x, y: y / data
        else:
            raise RuntimeError(
                "The <Data> tag should contain a list or an integer"
            )

    X = [int(x["name"]) for x in results]
    Y = []
    for x, xml in enumerate(results):
        if xml.find("mean") is not None:
            Y.append(yfunc(x, float(xml.find("mean")["value"])))
    X = X[: len(Y)]
    # times in nanoseconds
    t, Y = time_unit(Y)

    plt.plot(X, Y, "x", label=label)
    if title is not None:
        plt.title(title)
    if xlabel is not None:
        plt.xlabel(xlabel)
    if ylabel is not None:
        plt.ylabel(ylabel + " " + t)
    else:
        plt.ylabel("Time in {}".format(t))
    if label is not None:
        plt.legend(loc="lower right")


from sys import argv

args = sys.argv[1:]
# TODO arg checks
for x in args:
    add_plot(x)
xml_fnam = args[0]
png_fnam = "".join(xml_fnam.split(".")[:-2]) + ".png"
print("Writing {} . . .".format(png_fnam))
plt.savefig(png_fnam, format="png", dpi=300)
sys.exit(0)
