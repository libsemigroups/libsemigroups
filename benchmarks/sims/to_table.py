#!/usr/bin/env python3
"""
TODO update this description
This module expects to be used as:
./bench_sims "[partition_monoid_2_sided]" --reporter=xml | benchmarks/to_table.py
"""

import sys
import json
from math import floor, log10
from rich.console import Console
from rich.table import Table
from bs4 import BeautifulSoup
import re


def format_time(mean: float, sd: float) -> tuple[str, str]:
    e = floor(log10(sd))
    if str(sd)[1] == 1:
        mean = floor(mean / (10 ** (e - 1))) / 10
        sd = floor(sd / (10 ** (e - 1))) / 10
    else:
        mean = floor(mean / 10**e)
        sd = floor(sd / 10**e)

    f = floor(log10(mean))
    return (
        rf"{mean / 10**f: 9} \times 10 ^ {{{e + f}}}",
        rf"{sd / 10**f} \times 10 ^ {{{e + f}}}",
    )


def row_name(json: dict) -> str:
    if json["name"].startswith("test_"):
        match = re.search(r"\[(\d)\]", json["name"])
        suffix = ""
        if match:
            suffix = f"{match.group(1)} thread"
            if match.group(1) != "1":
                suffix += "s"

        return f"3-manifolds ({suffix})"
    return json["name"]


def read_json(filename: str) -> dict:
    try:
        with open(filename, "r") as f:
            return json.load(f)
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
    except json.JSONDecodeError as e:
        print(f"Error: Failed to parse JSON - {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


def read_xml(filename: str) -> BeautifulSoup:
    try:
        with open(filename, "r") as f:
            return BeautifulSoup(f.read(), "xml")
    except FileNotFoundError:
        print(f"Error: File '{filename}' not found.")
    except json.JSONDecodeError as e:  # TODO correct exception
        print(f"Error: Failed to parse XML - {e}")
    except Exception as e:
        print(f"An unexpected error occurred: {e}")


def add_rows_from_json(table: Table, filename: str) -> None:
    json = read_json(filename)
    for benchmark in json["benchmarks"]:
        table.add_row(
            row_name(benchmark),
            *format_time(benchmark["stats"]["mean"], benchmark["stats"]["stddev"]),
        )


def add_rows_from_xml(table: Table, filename: str) -> None:
    xml = read_xml(filename)
    table.title = xml.find_all("TestCase")[0]["name"]
    for benchmark in xml.find_all("BenchmarkResults"):
        mean = float(benchmark.find("mean")["value"]) / 10**9
        sd = float(benchmark.find("standardDeviation")["value"]) / 10**9
        table.add_row(benchmark["name"], *format_time(mean, sd))


def main(*args) -> None:
    table = Table(title="Benchmark")
    table.add_column("Name", justify="left", style="cyan", no_wrap=True)
    table.add_column("Mean", justify="left", style="magenta")
    table.add_column("Sd", justify="left", style="green")

    for filename in args:
        if filename.endswith(".json"):
            add_rows_from_json(table, filename)
        elif filename.endswith(".xml"):
            add_rows_from_xml(table, filename)
        else:
            print(
                f'expected file extension to be "json" or "xml" but found {filename}, skipping!'
            )

    Console().print(table)


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: expected at least one argument (filenames) found 0")
    else:
        main(*sys.argv[1:])
