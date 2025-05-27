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

./bench-3-manifolds-to-json.sh THIS_FILES_NAME

produces a ".json" file in the cwd.
"""

from low_index import permutation_reps
import pytest

relators = ["abaBAB", "bcbCBC", "cdcDCD", "acAC", "adAD", "bdBD"]


def test_correctness():
    assert (
        len(
            permutation_reps(
                rank=4,
                short_relators=relators,
                long_relators=[],
                max_degree=12,
                num_threads=1,
            )
        )
        == 21
    )


@pytest.mark.parametrize("threads", [1, 2, 4, 8])
def test_braid_5_12(benchmark, threads):
    benchmark(
        permutation_reps,
        rank=4,
        short_relators=relators,
        long_relators=[],
        max_degree=12,
        num_threads=threads,
    )
