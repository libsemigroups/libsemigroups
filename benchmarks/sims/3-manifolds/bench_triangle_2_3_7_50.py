# -*- coding: utf-8 -*-

# Copyright (c) May 2025, J. D. Mitchell
#
# Distributed under the terms of the GPL license version 3.
#
# The full license is in the file LICENSE, distributed with this software.

"""
This module contains some benchmarks for comparing the implementation of
Sims1/2 in libsemigroups with

https://github.com/3-manifolds/low_index

Doing:

pytest -v --benchmark-json="triangle.json"
benchmarks/sims/bench_triangle_2_3_7_50.py

produces a "triangle.json" file in the cwd.
"""

import pytest
from low_index import permutation_reps

relators = ["aa", "bbb", "ababababababab"]


def test_correctness():
    assert (
        len(
            permutation_reps(
                rank=2,
                short_relators=relators,
                long_relators=[],
                max_degree=50,
                num_threads=1,
            )
        )
        == 1747
    )


@pytest.mark.parametrize("threads", [1, 2, 4, 8])
def test_triangle_2_3_7_50(benchmark, threads):
    benchmark(
        permutation_reps,
        rank=2,
        short_relators=relators,
        long_relators=[],
        max_degree=50,
        num_threads=threads,
    )
