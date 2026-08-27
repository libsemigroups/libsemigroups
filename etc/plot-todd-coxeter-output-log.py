#!/usr/bin/env python3
import argparse
import gzip
import pathlib
import re
import sys
from dataclasses import dataclass, replace

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import numpy as np
import seaborn as sns
from PIL import Image

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

TIME_MULTIPLIERS = (
    31556952,  # years
    2629746,  # months
    604800,  # weeks
    86400,  # days
    3600,  # hours
    60,  # minutes
    1.0,  # seconds
    1e-3,  # ms
    1e-6,  # µs
    1e-9,  # ns
)

MINIMUM_FRAME_DURATION = 20


###########################################################################
# Input processing
###########################################################################


@dataclass(slots=True)
class PlotEntry:
    complete: bool = False
    edge_percentage: float = 0.0
    num_nodes: int = 0
    phase: str = ""
    time: float = 0.0
    title: str = ""


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Plot the output of a ToddCoxeter log file",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )

    parser.add_argument(
        "file",
        type=pathlib.Path,
        help="Log file to plot",
    )

    parser.add_argument(
        "--multiplier",
        type=float,
        default=1.0,
        help="Speed multiplier for animation playback (default: 1.0)",
    )

    parser.add_argument(
        "--output",
        type=str,
        default="output.gif",
        help="Output file path (default: output.gif)",
    )

    parser.add_argument(
        "--repeat-delay",
        type=float,
        default=1.0,
        help="Delay at the end of the gif in seconds (default: 1.0)",
    )

    parser.add_argument(
        "--frame-increment",
        type=int,
        default=1,
        help="Number of data points to advance by each frame (default: 1)",
    )

    parser.add_argument(
        "--static-axes",
        action="store_true",
        help="Fix the axes to be their final size",
    )

    return parser.parse_args()


def parse_time(time_string: str) -> float:
    """Convert a string representing a time to a float"""
    m = re.fullmatch(TIME_PATTERN, time_string)
    if not m:
        raise ValueError(f"Invalid time string: {time_string!r}")

    return sum(
        float(val) * mult for val, mult in zip(m.groups(), TIME_MULTIPLIERS) if val
    )


def parse_nodes(node_string: str) -> int:
    """Convert a string of a number with comma separators into an int"""
    return int(node_string.replace(",", ""))


def parse_edge_percentage(edge_string: str) -> float:
    """Convert a string into a float"""
    return float(edge_string)


def parse_phase(phase_str: str) -> str:
    return COLOUR_CODE_PATTERN.sub("", phase_str)


def extract_line_info(line: str, plot_data: PlotEntry) -> None:
    """Extract plot data from a line in the documentation."""

    if plot_data.title == "":
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


def parse_file(file: pathlib.Path) -> list[PlotEntry]:
    plot_entries: list[PlotEntry] = []

    if file.suffix == ".gz":
        opener = gzip.open
    else:
        opener = open
    with opener(file, mode="rt") as f:
        current_plot_entry = PlotEntry()
        for line in f:
            extract_line_info(line, current_plot_entry)
            if current_plot_entry.complete:
                plot_entries.append(current_plot_entry)
                current_plot_entry = replace(current_plot_entry, complete=False)

    return plot_entries


###########################################################################
# Plotting
###########################################################################


@dataclass(slots=True)
class Span:
    start: float
    end: float
    rect: Rectangle
    fully_visible: bool


class PhaseSpans:
    "A class for managing vertical axis spans for phases of ToddCoxeter"

    def __init__(self, axes):
        self._spans: list[list[Span]] = []
        self._colours = {}
        self._axes = axes
        self._current_phase = ""

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
                span.end = time
        if phase != self._current_phase:
            # Add new phase
            self._current_phase = phase
            label = phase if phase not in self._colours else "_nolegend_"
            kwargs = {
                "alpha": 0.15,
                "color": self._get_colour(phase),
                "label": label,
            }
            self._spans.append(
                [
                    Span(
                        start=time,
                        end=time,
                        rect=ax.axvspan(time, time, **kwargs),
                        fully_visible=True,
                    )
                    for ax in self._axes
                ]
            )

    def set_xlim(self, time):
        for span_collection in self._spans:
            for span in span_collection:
                if span.fully_visible:
                    continue

                if span.end <= time:
                    span.rect.set_width(span.end - span.start)
                    span.rect.set_visible(True)
                    span.fully_visible = True
                elif span.start < time <= span.end:
                    span.rect.set_width(time - span.start)
                    span.rect.set_visible(True)
                else:
                    return

    def make_invisible(self):
        for span_collection in self._spans:
            for span in span_collection:
                span.rect.set_visible(False)
                span.fully_visible = False


def main():
    args = parse_args()
    print("Reading the input file ...", end=" ", flush=True)
    data = parse_file(args.file)
    if not data:
        print("Error: No plot data found in file.")
        return

    print("Done!")
    print("Constructing the plot ...", end=" ", flush=True)

    # Collect the data to be plotted
    num_data_points = len(data)
    times = np.fromiter((d.time for d in data), count=num_data_points, dtype=float)
    nodes = np.fromiter((d.num_nodes for d in data), count=num_data_points, dtype=int)
    edge_percentages = np.fromiter(
        (d.edge_percentage for d in data), count=num_data_points, dtype=float
    )

    # Setup plots
    sns.set_theme()
    fig, (ax1, ax2) = plt.subplots(nrows=2, figsize=(12, 12))
    ax1.set(ylabel="Number of active nodes")
    ax2.set(ylabel="Edge completion (%)", xlabel="Time (s)")
    ax2.set_ylim(-5, 105)
    if args.static_axes:
        max_time = max(times)
        max_node = max(nodes)
        ax1.set_xlim(-0.05 * max_time, 1.1 * max_time)
        ax1.set_ylim(-0.05 * max_node, 1.1 * max_node)
        ax2.set_xlim(-0.05 * max_time, 1.1 * max_time)

    if data[-1].title:
        fig.suptitle(data[-1].title, family="monospace")

    current_line_col = "darkorange"
    max_line_col = "green"

    current_node_line = ax1.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=current_line_col
    )
    current_edge_line = ax2.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=current_line_col
    )

    max_node = 0
    max_edge_percentage = 0

    max_node_line = ax1.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=max_line_col
    )
    max_edge_percentage_line = ax2.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color=max_line_col
    )

    (node_plot,) = ax1.plot([], [])
    (edge_percentage_plot,) = ax2.plot([], [])

    phase_spans = PhaseSpans((ax1, ax2))
    for entry in data:
        phase_spans.add(entry.time, entry.phase)

    # Construct legend after phases have been constructed
    node_legend = ax1.legend(loc="upper left")
    edge_percentage_legend = ax2.legend(loc="upper left")

    # Make phases invisible after the legend has been created
    phase_spans.make_invisible()

    def draw_frame(frame: int):
        nonlocal max_node, max_edge_percentage

        previous_frame = max(0, frame - args.frame_increment)
        current_time = times[frame]
        max_node = max(*nodes[previous_frame : frame + 1], max_node)
        max_edge_percentage = max(
            *edge_percentages[previous_frame : frame + 1], max_edge_percentage
        )
        phase_spans.set_xlim(current_time)

        # Update plot data
        node_plot.set_data(times[: frame + 1], nodes[: frame + 1])
        edge_percentage_plot.set_data(times[: frame + 1], edge_percentages[: frame + 1])

        # Update current node values
        current_node_line.set_xdata([times[0], current_time])
        current_node_line.set_ydata([nodes[frame], nodes[frame]])
        node_legend.get_texts()[0].set_text(f"currently: {nodes[frame]:.3g}")

        # Update max node values
        max_node_line.set_xdata([times[0], current_time])
        max_node_line.set_ydata([max_node, max_node])
        node_legend.get_texts()[1].set_text(f"max:        {max_node:.3g}")

        # Update current edge percentage
        current_edge_line.set_xdata([times[0], current_time])
        current_edge_line.set_ydata([edge_percentages[frame], edge_percentages[frame]])
        edge_percentage_legend.get_texts()[0].set_text(
            f"currently: {edge_percentages[frame]}%"
        )

        # Update max edge percentage values
        max_edge_percentage_line.set_xdata([times[0], current_time])
        max_edge_percentage_line.set_ydata([max_edge_percentage, max_edge_percentage])
        edge_percentage_legend.get_texts()[1].set_text(
            f"max:        {max_edge_percentage}%"
        )

        # Update axes
        if not args.static_axes:
            ax1.set_xlim(-0.05 * current_time, 1.1 * current_time)
            ax1.set_ylim(-0.05 * max_node, 1.1 * max_node)
            ax2.set_xlim(-0.05 * current_time, 1.1 * current_time)

    if len(times) > 1:
        deltas = (np.diff(times) * 1000) / args.multiplier
        durations = np.maximum(deltas, MINIMUM_FRAME_DURATION).tolist()
        durations.append(args.repeat_delay * 1000)
    else:
        durations = [200]

    frames = []
    for frame in range(0, len(times), args.frame_increment):
        draw_frame(frame)
        fig.canvas.draw()
        rgba_array = np.asarray(fig.canvas.buffer_rgba())
        frames.append(Image.fromarray(rgba_array).convert("RGB"))

    print("Done!")
    print("Writing to file ...", end=" ", flush=True)
    frames[0].save(
        args.output,
        save_all=True,
        append_images=frames[1:],
        duration=durations,
        loop=0,
    )
    print("Done!")

    plt.close(fig)


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
