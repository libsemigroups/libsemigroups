#!/usr/bin/env python3
# import numpy as np
import queue
import re
import sys
import threading


import matplotlib.animation as animation
import matplotlib.pyplot as plt
import seaborn as sns

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

plot_data = queue.SimpleQueue()
latest_nodes = None
latest_edge_percentage = None
title = ""
title_ready = threading.Event()


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
    ) = map(lambda x: float(x) if x is not None else 0, m.groups())
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


def extract_line_info(line: str) -> None:
    """Extract plot data from a line in the documentation"""
    global latest_nodes, latest_edge_percentage, title

    if not title_ready.is_set():
        m = re.search(TITLE_PATTERN, line)
        if m:
            title = m.group(1)
            print(title)
            title_ready.set()
            return

    m = re.search(RUNTIME_PATTERN, line)
    if m:
        time = parse_time(m.group(1))
        if latest_nodes is not None and latest_edge_percentage is not None:
            plot_data.put((time, latest_nodes, latest_edge_percentage))
        return

    m = re.search(NODE_PATTERN, line)
    if m:
        latest_nodes = parse_nodes(m.group(1))
        return

    m = re.search(EDGE_PATTERN, line)
    if m:
        latest_edge_percentage = parse_edge_percentage(m.group(1))
        return


def read_stdin():
    for line in sys.stdin:
        line = line.rstrip("\n")
        extract_line_info(line)
        print(line, flush=True)


def main():
    reader = threading.Thread(target=read_stdin, daemon=True)
    reader.start()

    # Setup plot

    sns.set_theme()
    fig, (ax1, ax2) = plt.subplots(nrows=2, figsize=(12, 12))
    times = []
    nodes = []
    edge_percentages = []

    node_plot = ax1.plot(times, nodes)[0]
    current_node_line = ax1.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color="darkorange"
    )
    ax1.set(ylabel="Number of active nodes")

    edge_plot = ax2.plot(times, nodes)[0]
    current_edge_line = ax2.axhline(
        y=0, xmin=0, xmax=0, label="0", linestyle="--", color="darkorange"
    )
    ax2.set(ylabel="Edge completion (%)", xlabel="Time (s)")
    ax2.set_ylim(0, 100)
    ax2.legend()

    if title_ready.wait(timeout=1):
        fig.suptitle(title, family="monospace")
    else:
        print("plot warning: no title found", flush=True)

    def update(frame):
        changed = False
        while True:
            try:
                time, num_nodes, edge_percentage = plot_data.get_nowait()
            except queue.Empty:
                break
            times.append(time)
            nodes.append(num_nodes)
            edge_percentages.append(edge_percentage)
            changed = True

        if not changed:
            return (node_plot, edge_plot, current_edge_line, current_edge_line)

        node_plot.set_xdata(times)
        node_plot.set_ydata(nodes)
        current_node_line.set_xdata([times[0], times[-1]])
        current_node_line.set_ydata([nodes[-1], nodes[-1]])
        current_node_line.set(label=f"{nodes[-1]:.3g}")
        ax1.legend()
        ax1.relim()
        ax1.autoscale_view()

        edge_plot.set_xdata(times)
        edge_plot.set_ydata(edge_percentages)
        current_edge_line.set_xdata([times[0], times[-1]])
        current_edge_line.set_ydata([edge_percentages[-1], edge_percentages[-1]])
        current_edge_line.set(label=f"{edge_percentages[-1]}%")
        ax2.legend()
        ax2.relim()
        ax2.autoscale_view()
        return (node_plot, edge_plot, current_edge_line, current_edge_line)

    ani = animation.FuncAnimation(fig=fig, func=update, interval=500, save_count=60)
    plt.show()
    return ani


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        sys.exit(0)
