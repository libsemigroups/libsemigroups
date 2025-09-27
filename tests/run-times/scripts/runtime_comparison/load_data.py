"""This file contains functions that can be used to generate a variety of
dataframes related to json timing data.
"""

import json
from pathlib import Path
import pandas as pd
from .process_data import join_means


def to_time_records(file_name, restrict=None):
    """Returns a list of tuples of the form (command, runner, time)

    Note that each command will likely have been run multiple times, so the same
    command will appear more than once
    """
    records = []
    with open(file_name, encoding="utf-8") as f:
        results = json.load(f)["results"]
    for record in results:
        current_command = record["command"]
        # Only store commands specified in restrict (provided restrict is specified)
        if restrict is not None and current_command not in restrict:
            continue
        for time in record["times"]:
            records.append((current_command, Path(file_name).stem, time))
    return records


def to_df(file_name):
    """Returns a DataFrame where each record corresponds to a test that was run,
    and the columns are the headers in the json file <file_name>
    """
    with open(file_name, encoding="utf-8") as f:
        records = json.load(f)["results"]
    return pd.DataFrame(records).set_index("command")


def to_all_times_df(*file_names, restrict=None):
    """Returns a DataFrame where each record has the form
    (run_name, command, time)
    """
    records = []
    for f in file_names:
        records += to_time_records(f, restrict=restrict)
    return pd.DataFrame(records, columns=["command", "runner", "time"]).set_index(
        "command"
    )


def to_mean_time_comparison_df(base_file_name, *comparison_file_names, columns=None):
    """Returns a DataFrame with columns
    (base_mean_time, base_stddev, comparison_mean_time_1, comparison_mean_time_2, ...),
    and each record corresponds to a test that was run in base_file
    """
    base_time_df = to_df(base_file_name)
    comparison_dfs = (to_df(f) for f in comparison_file_names)

    if columns is None:
        columns = [Path(f).stem for f in [base_file_name] + list(comparison_file_names)]
    return join_means(base_time_df, *comparison_dfs, names=columns)
