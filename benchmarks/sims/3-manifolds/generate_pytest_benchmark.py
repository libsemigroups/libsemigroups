#!/usr/bin/env python3
"""
This script generates a pytest benchmark of the type required by libsemigroups,
from the 3-manifolds benchmarks file benchmark.py.
"""

from benchmark import examples

for example in examples:
    if len(example.values()) != 5:
        continue
    group, rank, short, long, index = example.values()
    group = group[0].lower() + group[1:]
    test_fn_name = group.replace(" ", "_")

    print(
        f"""
# -*- coding: utf-8 -*-

# Copyright (c) May 2025, J. D. Mitchell
#
# Distributed under the terms of the GPL license version 3.
#
# The full license is in the file LICENSE, distributed with this software.

\"\"\"
This module contains some benchmarks for comparing the implementation of
Sims1/2 in libsemigroups with

https://github.com/3-manifolds/low_index

Doing:

./bench-3-manifolds-to-json.sh NAME_OF_THIS_FILE

produces a ".json" file in the cwd.
\"\"\"

from low_index import permutation_reps
import pytest

short = {short}
long = {long}

def test_{test_fn_name}_correctness():
    assert (len( permutation_reps( rank={rank}, short_relators=short, long_relators=long, max_degree={index}, num_threads=1)) == 0)

@pytest.mark.parametrize("threads", [1, 2, 4, 8])
def test_{test_fn_name}(benchmark, threads):
    benchmark(
        permutation_reps,
        rank={rank},
        short_relators=short,
        long_relators=long,
        max_degree={index},
        num_threads=threads,
    )"""
    )
