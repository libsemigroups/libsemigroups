#!/usr/bin/env python3
"""
This module expects to be used as:
./bench_sims "[partition_monoid_2_sided]" --reporter=xml | benchmarks/latex_table.py
"""

import sys
from bs4 import BeautifulSoup
from math import floor, log10
import re
from rich.console import Console
from rich.table import Table


def format_time(mean_str: str, sd_str: str) -> tuple[str, str]:
    mean, sd = float(mean_str), float(sd_str)
    e = floor(log10(sd))
    if str(sd)[1] == 1:
        mean = floor(mean / (10 ** (e - 1))) / 10
        sd = floor(sd / (10 ** (e - 1))) / 10
    else:
        mean = floor(mean / 10**e)
        sd = floor(sd / 10**e)

    e -= 9  # for nanoseconds
    f = floor(log10(mean))
    return (
        rf"{mean / 10**f: 9} \times 10 ^ {{{e + f}}}",
        rf"{sd / 10**f} \times 10 ^ {{{e + f}}}",
    )


def format_benchmark_result(table: Table, xml: BeautifulSoup):
    mean = xml.find("mean")["value"]
    sd = xml.find("standardDeviation")["value"]
    table.add_row(xml["name"], *format_time(mean, sd))


table = Table(title="Benchmark")
table.add_column("Name", justify="left", style="cyan", no_wrap=True)
table.add_column("Mean", justify="left", style="magenta")
table.add_column("Sd", justify="left", style="green")
xml_str = ""
num_benchmarks = 0
for line in sys.stdin:
    print(line, file=sys.stdout, flush=True, end="")
    xml_str += line

    if line.find(r"</BenchmarkResults>") != -1:
        xml = BeautifulSoup(xml_str, "xml")
        results = xml.find_all("BenchmarkResults")[num_benchmarks:]
        if len(results) > 0:
            if num_benchmarks == 0:
                table.title = xml.find("TestCase")["name"]
            num_benchmarks += 1
            format_benchmark_result(table, results[0])
        # print(results)
    if line.find(r"</Section>") != -1:
        table.add_section()
Console().print(table)
