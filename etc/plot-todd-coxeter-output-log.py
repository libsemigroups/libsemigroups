#!/usr/bin/env python3
# import numpy as np
import argparse
import pathlib
import re
import sys
from collections import defaultdict
from copy import deepcopy
from dataclasses import dataclass

import matplotlib.pyplot as plt
import seaborn as sns
from matplotlib import animation

TIME_PATTERN = re.compile(
    r"""(?:(\d+)y)?
        (?:(\d+)mon)?
        (?:(\d+)w)?
        (?:(\d+)d)?
        (?:(\d+)h)?
        (?:(\d+)min)?
        (?:(\d+(?:\.\d+)?)s)? # seconds is the only one that can be a decimal
        (?:(\d+)ms)?
        (?:(\d+)µs)?
        (?:(\d+)ns)?""",
    re.VERBOSE,
)
RUNTIME_PATTERN = re.compile(r"\|\s+all runs = (.*?)\s+\|")
NODE_PATTERN = re.compile(r"nodes\s+\|\s+(\d+(,\d\d\d)*)\s+\|")
EDGE_PATTERN = re.compile(r"edges.*\|\s+(\d+(?:\.\d+)?)%")
TITLE_PATTERN = re.compile(r"(\[\d\d\d\]:.*?) - START")
PHASE_START_PATTERN = re.compile(r"(?:ToddCoxeter: )(.*)(?: \d+\.\d+ START)")
COLOUR_CODE_PATTERN = re.compile(r"\x1B(?:[@-Z\\-_]|\[[0-?]*[ -/]*[@-~])")
PHASE_PALETTE = sns.color_palette("muted")

###########################################################################
# Input processing
###########################################################################


@dataclass
class PlotEntry:
    complete: bool = False
    edge_percentage: float = 0
    num_nodes: int = 0
    phase: str = ""
    time: float = 0
    title: str = ""


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Plot the output of a ToddCoxeter log file",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        "file",
        type=pathlib.Path,
        help="The log file to plot",
    )

    parser.add_argument(
        "--multiplier",
        type=float,
        default=1.0,
        help="A multiplier to determine how fast the animation should run (default=1.0)",
    )

    args = parser.parse_args()

    return args


def parse_time(time_string: str) -> float:
    """Convert a string representing a time to a float"""
    m = re.fullmatch(TIME_PATTERN, time_string)
    if not m:
        raise ValueError(f"Invalid time string: {time_string!r}")

    (
        years,
        months,
        weeks,
        days,
        hours,
        minutes,
        seconds,
        milliseconds,
        microseconds,
        nanoseconds,
    ) = (float(x) if x is not None else 0 for x in m.groups())
    return (
        31556952 * years
        + 2629746 * months
        + 604800 * weeks
        + 86400 * days
        + 3600 * hours
        + 60 * minutes
        + seconds
        + 10**-3 * milliseconds
        + 10**-6 * microseconds
        + 10**-9 * nanoseconds
    )


def parse_nodes(node_string: str) -> int:
    """Convert a string of a number with comma separators into an int"""
    return int(node_string.replace(",", ""))


def parse_edge_percentage(edge_string: str) -> float:
    """Convert a string into a float"""
    return float(edge_string)


def parse_phase(phase_str: str) -> str:
    return COLOUR_CODE_PATTERN.sub("", phase_str)


def extract_line_info(line: str, plot_data: PlotEntry):
    """Extract plot data from a line in the documentation."""

    if plot_data.title != "":
        m = re.search(TITLE_PATTERN, line)
        if m:
            plot_data.title = m.group(1)
            return

    m = re.search(NODE_PATTERN, line)
    if m:
        plot_data.num_nodes = parse_nodes(m.group(1))
        return

    m = re.search(EDGE_PATTERN, line)
    if m:
        plot_data.edge_percentage = parse_edge_percentage(m.group(1))
        return

    m = re.search(PHASE_START_PATTERN, line)
    if m:
        plot_data.phase = parse_phase(m.group(1))
        return

    m = re.search(RUNTIME_PATTERN, line)
    if m:
        plot_data.time = parse_time(m.group(1))
        plot_data.complete = True
        return


def parse_file(file: pathlib.Path) -> list[PlotEntry]:
    plot_entries: list[PlotEntry] = []
    with open(file) as f:
        current_plot_entry = PlotEntry()
        for line in f:
            extract_line_info(line, current_plot_entry)
            if current_plot_entry.complete:
                plot_entries.append(current_plot_entry)
                current_plot_entry = deepcopy(current_plot_entry)
                current_plot_entry.complete = False
    return plot_entries


###########################################################################
# Plotting
###########################################################################


class PhaseSpans:
    "A class for managing vertical axis spans for phases of ToddCoxeter"

    def __init__(self, axes):
        self._spans = []
        self._colours = {}
        self._axes = axes
        self._current_phase = ""
        self._last_start = 0.0

    def __bool__(self):
        return len(self._spans) != 0

    def _get_colour(self, phase):
        if phase not in self._colours:
            self._colours[phase] = PHASE_PALETTE[
                len(self._colours) % len(PHASE_PALETTE)
            ]
        return self._colours[phase]

    def add(self, time, phase):
        """Add a new phase span to the axes. If a phase of this type already
        exists, the plot is not added to the legend"""
        if phase == "":
            return

        # Update old phase
        if self._spans:
            last_spans = self._spans[-1]
            for span in last_spans:
                span[1] = time
        if phase != self._current_phase:
            # Add new phase
            self._current_phase = phase
            self._last_start = time
            label = phase if phase not in self._colours else "_nolegend_"
            kwargs = {
                "alpha": 0.15,
                "color": self._get_colour(phase),
                "label": label,
                "visible": False,
            }
            self._spans.append(
                [[time, time, ax.axvspan(time, time, **kwargs)] for ax in self._axes]
            )

    def set_xlim(self, time):
        for span in self._spans:
            for true_start, true_end, rect in span:
                if true_end <= time:
                    rect.set_width(true_end - true_start)
                    rect.set(visible=True)
                elif true_start < time <= true_end:
                    rect.set_width(time - true_start)
                    rect.set(visible=True)
                else:
                    rect.set(visible=False)


def main():
    args = parse_args()
    data = parse_file(args.file)

    # Setup plot
    sns.set_theme()
    fig, (ax1, ax2) = plt.subplots(nrows=2, figsize=(12, 12))

    times = []
    nodes = []
    edge_percentages = []
    phase_spans = PhaseSpans((ax1, ax2))
    for entry in data:
        times.append(entry.time)
        nodes.append(entry.num_nodes)
        edge_percentages.append(entry.edge_percentage)
        phase_spans.add(entry.time, entry.phase)

    current_line_col = "darkorange"

    node_plot = ax1.plot([], [])[0]
    current_node_line = ax1.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=current_line_col
    )
    ax1.set(ylabel="Number of active nodes")
    ax1.legend()

    edge_plot = ax2.plot([], [])[0]
    current_edge_line = ax2.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=current_line_col
    )
    ax2.set(ylabel="Edge completion (%)", xlabel="Time (s)")
    ax2.set_ylim(-5, 105)
    ax2.legend()

    if data[-1].title:
        fig.suptitle(data[-1].title, family="monospace")
    else:
        print("plot warning: no title found", flush=True)

    def draw_frame(frame):
        current_time = times[frame]

        phase_spans.set_xlim(current_time)

        node_plot.set_xdata(times[: frame + 1])
        node_plot.set_ydata(nodes[: frame + 1])
        current_node_line.set_xdata([times[0], current_time])
        current_node_line.set_ydata([nodes[frame], nodes[frame]])
        current_node_line.set(label=f"{nodes[frame]:.3g}")
        ax1.legend()
        ax1.relim()
        ax1.autoscale_view()

        edge_plot.set_xdata(times[: frame + 1])
        edge_plot.set_ydata(edge_percentages[: frame + 1])
        current_edge_line.set_xdata([times[0], current_time])
        current_edge_line.set_ydata([edge_percentages[frame], edge_percentages[frame]])
        current_edge_line.set(label=f"{edge_percentages[frame]}%")
        ax2.legend()
        ax2.relim()
        ax2.autoscale_view()

        fig.canvas.draw_idle()

    frame = 0

    def tick():
        nonlocal frame
        draw_frame(frame)

        frame += 1
        if frame >= len(times):
            return

        delta_seconds = times[frame] - times[frame - 1]
        delay_ms = max(delta_seconds * 1000 / args.multiplier, 1)

        timer = fig.canvas.new_timer(interval=delay_ms)
        timer.single_shot = True
        timer.add_callback(tick)
        timer.start()

    draw_frame(0)
    first_delay = ((times[1] - times[0]) * 1000) / args.multiplier
    timer = fig.canvas.new_timer(interval=first_delay)
    timer.single_shot = True
    timer.add_callback(tick)
    timer.start()
    plt.show()


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
