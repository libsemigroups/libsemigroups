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

./bench-3-manifolds-to-json.sh NAME_OF_THIS_FILE

"""

import pytest

from low_index import permutation_reps


relators = [
    "ABAbaBabC",
    "ABCBcbCbc",
    "ABabacBAb",
    "ACaCAcacb",
    "ACacAcaBC",
    "AbABabacB",
    "AbaBabCAB",
    "AcaBCACac",
    "AcacbACaC",
    "BAbABabac",
    "BAbaBabCA",
    "BCACacAca",
    "BCBcbCbcA",
    "BCbcbaCBc",
    "BabCABAba",
    "BabacBAbA",
    "BcBCbcbaC",
    "BcbCbcABC",
    "CABAbaBab",
    "CACacAcaB",
    "CAcacbACa",
    "CBcBCbcba",
    "CBcbCbcAB",
    "CaCAcacbA",
    "CacAcaBCA",
    "CbcABCBcb",
    "CbcbaCBcB",
    "aBCACacAc",
    "aBabCABAb",
    "aCAcacbAC",
    "aCBcBCbcb",
    "abCABAbaB",
    "abacBAbAB",
    "acAcaBCAC",
    "acBAbABab",
    "acbACaCAc",
    "bABabacBA",
    "bACaCAcac",
    "bCABAbaBa",
    "bCbcABCBc",
    "baBabCABA",
    "baCBcBCbc",
    "bacBAbABa",
    "bcABCBcbC",
    "bcbaCBcBC",
    "cABCBcbCb",
    "cAcaBCACa",
    "cBAbABaba",
    "cBCbcbaCB",
    "caBCACacA",
    "cacbACaCA",
    "cbACaCAca",
    "cbCbcABCB",
    "cbaCBcBCb",
]


def test_correctness():
    assert (
        len(
            permutation_reps(
                rank=3,
                short_relators=relators,
                long_relators=[],
                max_degree=8,
                num_threads=1,
            )
        )
        == 3
    )


@pytest.mark.parametrize("threads", [1, 2, 4, 8])
def test_heineken_8(benchmark, threads):
    benchmark(
        permutation_reps,
        rank=3,
        short_relators=relators,
        long_relators=[],
        max_degree=8,
        num_threads=threads,
    )
