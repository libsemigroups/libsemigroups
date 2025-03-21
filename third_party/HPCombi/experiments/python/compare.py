#!/usr/bin/env python2

"""
compare.py - versatile benchmark output compare tool
"""

import argparse
from argparse import ArgumentParser
import sys
import gbench
from gbench import util, report
# ~ from gbench.util import *


def check_inputs(in1, in2, flags):
    """
    Perform checking on the user provided inputs and diagnose any abnormalities
    """
    in1_kind, in1_err = classify_input_file(in1)
    in2_kind, in2_err = classify_input_file(in2)
    output_file = find_benchmark_flag('--benchmark_out=', flags)
    output_type = find_benchmark_flag('--benchmark_out_format=', flags)
    if in1_kind == IT_Executable and in2_kind == IT_Executable and output_file:
        print(("WARNING: '--benchmark_out=%s' will be passed to both "
               "benchmarks causing it to be overwritten") % output_file)
    if in1_kind == IT_JSON and in2_kind == IT_JSON and len(flags) > 0:
        print("WARNING: passing optional flags has no effect since both "
              "inputs are JSON")
    if output_type is not None and output_type != 'json':
        print(("ERROR: passing '--benchmark_out_format=%s' to 'compare.py`"
               " is not supported.") % output_type)
        sys.exit(1)


def create_parser():
    parser = ArgumentParser(
        description='versatile benchmark output compare tool')
    subparsers = parser.add_subparsers(
        help='This tool has multiple modes of operation:',
        dest='mode')

    parser_a = subparsers.add_parser(
        'benchmarks',
        help='The most simple use-case, compare all the output of these two benchmarks')
    baseline = parser_a.add_argument_group(
        'baseline', 'The benchmark baseline')
    baseline.add_argument(
        'test_baseline',
        metavar='test_baseline',
        type=argparse.FileType('r'),
        nargs=1,
        help='A benchmark executable or JSON output file')
    contender = parser_a.add_argument_group(
        'contender', 'The benchmark that will be compared against the baseline')
    contender.add_argument(
        'test_contender',
        metavar='test_contender',
        type=argparse.FileType('r'),
        nargs=1,
        help='A benchmark executable or JSON output file')
    parser_a.add_argument(
        'benchmark_options',
        metavar='benchmark_options',
        nargs=argparse.REMAINDER,
        help='Arguments to pass when running benchmark executables')

    parser_b = subparsers.add_parser(
        'filters', help='Compare filter one with the filter two of benchmark')
    baseline = parser_b.add_argument_group(
        'baseline', 'The benchmark baseline')
    baseline.add_argument(
        'test',
        metavar='test',
        type=argparse.FileType('r'),
        nargs=1,
        help='A benchmark executable or JSON output file')
    baseline.add_argument(
        'filter_baseline',
        metavar='filter_baseline',
        type=str,
        nargs=1,
        help='The first filter, that will be used as baseline')
    contender = parser_b.add_argument_group(
        'contender', 'The benchmark that will be compared against the baseline')
    contender.add_argument(
        'filter_contender',
        metavar='filter_contender',
        type=str,
        nargs=1,
        help='The second filter, that will be compared against the baseline')
    parser_b.add_argument(
        'benchmark_options',
        metavar='benchmark_options',
        nargs=argparse.REMAINDER,
        help='Arguments to pass when running benchmark executables')

    parser_c = subparsers.add_parser(
        'benchmarksfiltered',
        help='Compare filter one of first benchmark with filter two of the second benchmark')
    baseline = parser_c.add_argument_group(
        'baseline', 'The benchmark baseline')
    baseline.add_argument(
        'test_baseline',
        metavar='test_baseline',
        type=argparse.FileType('r'),
        nargs=1,
        help='A benchmark executable or JSON output file')
    baseline.add_argument(
        'filter_baseline',
        metavar='filter_baseline',
        type=str,
        nargs=1,
        help='The first filter, that will be used as baseline')
    contender = parser_c.add_argument_group(
        'contender', 'The benchmark that will be compared against the baseline')
    contender.add_argument(
        'test_contender',
        metavar='test_contender',
        type=argparse.FileType('r'),
        nargs=1,
        help='The second benchmark executable or JSON output file, that will be compared against the baseline')
    contender.add_argument(
        'filter_contender',
        metavar='filter_contender',
        type=str,
        nargs=1,
        help='The second filter, that will be compared against the baseline')
    parser_c.add_argument(
        'benchmark_options',
        metavar='benchmark_options',
        nargs=argparse.REMAINDER,
        help='Arguments to pass when running benchmark executables')
    parser_d = subparsers.add_parser(
        'hpcombi', help='Compare filter one with the filter two of benchmark')

    parser_d.add_argument(
        'tests',
        metavar='tests',
        type=str,
        nargs=1,
        help='Benchmark executable or JSON output files. They should be separeted by comas.\nExample: "file1,file2".\nThey will be interpreted as regex.')
    parser_d.add_argument(
        'comps',
        metavar='comps',
        type=str,
        nargs=1,
        help='A list of comparisons to do separeted by comas.\nExample: "param1/param2,param3/param4", meaning "param1" should be compared with "param2" and "param3" should be compared with "param4".\nParameters will be interpreted as regex.')
    parser_d.add_argument(
        'constant',
        metavar='constant',
        type=str,
        nargs=1,
        help='A list of constant parameters, meaning parameters that should be present but not compared, separated by comas.\nExample: "param1,param2".\nParameters will be interpreted as regex.')
    parser_d.add_argument(
        'benchmark_options',
        metavar='benchmark_options',
        nargs=argparse.REMAINDER,
        help='Arguments to pass when running benchmark executables')

    return parser


def main():
    # Parse the command line flags
    parser = create_parser()
    args, unknown_args = parser.parse_known_args()
    assert not unknown_args
    benchmark_options = args.benchmark_options
    if args.mode == 'benchmarks':
        tests_baseline = {args.test_baseline[0].name}
        tests_contender = {args.test_contender[0].name}
        filter_baseline = ''
        filter_contender = ''

        # NOTE: if test_baseline == test_contender, you are analyzing the stdev

        description = 'Comparing %s to %s' % \
                        ( \
                        ', '.join(tests_baseline), \
                        ', '.join(tests_contender) \
                        )
    elif args.mode == 'filters':
        tests_baseline = {args.test[0].name}
        tests_contender = {args.test[0].name}
        filter_baseline = args.filter_baseline[0]
        filter_contender = args.filter_contender[0]

        # NOTE: if filter_baseline == filter_contender, you are analyzing the
        # stdev

        description = 'Comparing %s to %s (from %s)' % \
                            ( \
                            filter_baseline, \
                            filter_contender, \
                            args.test[0].name \
                            )
            
    elif args.mode == 'hpcombi':
        tests_baseline = set( args.tests[0].split(',') )
        tests_contender = set( args.tests[0].split(',') )
        comps = set( args.comps[0].split(',') )
        constant = set( args.constant[0].split(',') )
        constant.discard('')
        filter_baseline = ''
        filter_contender = ''
        
        # Merges all file matching the regex in one file list.
        new_tests_baseline = set()
        for test in tests_baseline:
            new_tests_baseline |= gbench.util.get_files_set(test)
        tests_baseline = new_tests_baseline
        new_tests_contender = set()
        for test in tests_contender:
            new_tests_contender |= gbench.util.get_files_set(test)
        tests_contender = new_tests_contender
        
        description = 'Comparisons: %s\nConstant parameters: %s\nFiles: %s'% \
                                ( \
                                ', '.join( comps ), \
                                ', '.join( constant ),  \
                                ', '.join( (tests_baseline | tests_contender) ) \
                                )
                                 
    elif args.mode == 'benchmarksfiltered':
        tests_baseline = {args.test_baseline[0].name}
        tests_contender = {args.test_contender[0].name}
        filter_baseline = args.filter_baseline[0]
        filter_contender = args.filter_contender[0]

        # NOTE: if test_baseline == test_contender and
        # filter_baseline == filter_contender, you are analyzing the stdev

        description = 'Comparing %s (from %s) to %s (from %s)' % \
                                ( \
                                filter_baseline, \
                                ', '.join(tests_baseline), \
                                filter_contender, \
                                ', '.join(tests_contender) \
                                )
    else:
        # should never happen
        print("Unrecognized mode of operation: '%s'" % args.mode)
        exit(1)

    """
    In hpcombi mode all input combinations should be tested.
    As it can be time consuming, it has been commented temporarily.
    Uncomment the tree following lines to do all the tests.
    TODO: the check_inputs function should be modified 
    to handle the case where more than to file are given
    """
    # for testA in tests_baseline :
        # for testB in tests_contender :
            # check_inputs(testA, testB, benchmark_options)

    options_baseline = []
    options_contender = []
    if filter_baseline and filter_contender:
        options_baseline = ['--benchmark_filter=%s' % filter_baseline]
        options_contender = ['--benchmark_filter=%s' % filter_contender]
    
    json1_orig = {'benchmarks':[]}
    json2_orig = {'benchmarks':[]}

    if args.mode == 'benchmarks':
        # Run the benchmarks
        for testA in tests_baseline :
            json1_orig['benchmarks'] += gbench.util.run_or_load_benchmark(
                testA, benchmark_options + options_baseline)['benchmarks']
        for testB in tests_contender :
            json2_orig['benchmarks'] += gbench.util.run_or_load_benchmark(
                testB, benchmark_options + options_contender)['benchmarks']
                
        json1 = json1_orig
        json2 = json2_orig
    else :
        json1 = {'benchmarks':[]}
        json2 = {'benchmarks':[]}
        # Run the benchmarks
        for testA in tests_baseline :
            json1_orig['benchmarks'] += gbench.util.run_or_load_benchmark(
                testA, benchmark_options + options_baseline)['benchmarks']

        # Get the comparisons filter and the constant parameters filters
        if args.mode == 'hpcombi':
            nameSets = gbench.util.get_name_sets(json1_orig)
            constant = gbench.util.get_constant_set(constant, nameSets)
            expFilter = gbench.util.get_regex(constant, nameSets)
            comps = gbench.util.get_comp_set(json1_orig, comps, constant, nameSets)
        else:
            comps = [filter_baseline + '/' + filter_contender]
            expFilter = ''

        json1, json2 = gbench.report.filter_benchmark(json1_orig, comps, expFilter)
        
    # Diff and output
    output_lines = gbench.report.generate_difference_report(json1, json2)
    print(description)
    for ln in output_lines:
        print(ln)


import unittest


class TestParser(unittest.TestCase):
    def setUp(self):
        self.parser = create_parser()
        testInputs = os.path.join(
            os.path.dirname(
                os.path.realpath(__file__)),
            'gbench',
            'Inputs')
        self.testInput0 = os.path.join(testInputs, 'test_baseline_run1.json')
        self.testInput1 = os.path.join(testInputs, 'test_baseline_run2.json')

    def test_benchmarks_basic(self):
        parsed = self.parser.parse_args(
            ['benchmarks', self.testInput0, self.testInput1])
        self.assertEqual(parsed.mode, 'benchmarks')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertFalse(parsed.benchmark_options)

    def test_benchmarks_with_remainder(self):
        parsed = self.parser.parse_args(
            ['benchmarks', self.testInput0, self.testInput1, 'd'])
        self.assertEqual(parsed.mode, 'benchmarks')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertEqual(parsed.benchmark_options, ['d'])

    def test_benchmarks_with_remainder_after_doubleminus(self):
        parsed = self.parser.parse_args(
            ['benchmarks', self.testInput0, self.testInput1, '--', 'e'])
        self.assertEqual(parsed.mode, 'benchmarks')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertEqual(parsed.benchmark_options, ['e'])

    def test_filters_basic(self):
        parsed = self.parser.parse_args(
            ['filters', self.testInput0, 'c', 'd'])
        self.assertEqual(parsed.mode, 'filters')
        self.assertEqual(parsed.test[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.filter_contender[0], 'd')
        self.assertFalse(parsed.benchmark_options)

    def test_filters_with_remainder(self):
        parsed = self.parser.parse_args(
            ['filters', self.testInput0, 'c', 'd', 'e'])
        self.assertEqual(parsed.mode, 'filters')
        self.assertEqual(parsed.test[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.filter_contender[0], 'd')
        self.assertEqual(parsed.benchmark_options, ['e'])

    def test_filters_with_remainder_after_doubleminus(self):
        parsed = self.parser.parse_args(
            ['filters', self.testInput0, 'c', 'd', '--', 'f'])
        self.assertEqual(parsed.mode, 'filters')
        self.assertEqual(parsed.test[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.filter_contender[0], 'd')
        self.assertEqual(parsed.benchmark_options, ['f'])

    def test_benchmarksfiltered_basic(self):
        parsed = self.parser.parse_args(
            ['benchmarksfiltered', self.testInput0, 'c', self.testInput1, 'e'])
        self.assertEqual(parsed.mode, 'benchmarksfiltered')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertEqual(parsed.filter_contender[0], 'e')
        self.assertFalse(parsed.benchmark_options)

    def test_benchmarksfiltered_with_remainder(self):
        parsed = self.parser.parse_args(
            ['benchmarksfiltered', self.testInput0, 'c', self.testInput1, 'e', 'f'])
        self.assertEqual(parsed.mode, 'benchmarksfiltered')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertEqual(parsed.filter_contender[0], 'e')
        self.assertEqual(parsed.benchmark_options[0], 'f')

    def test_benchmarksfiltered_with_remainder_after_doubleminus(self):
        parsed = self.parser.parse_args(
            ['benchmarksfiltered', self.testInput0, 'c', self.testInput1, 'e', '--', 'g'])
        self.assertEqual(parsed.mode, 'benchmarksfiltered')
        self.assertEqual(parsed.test_baseline[0].name, self.testInput0)
        self.assertEqual(parsed.filter_baseline[0], 'c')
        self.assertEqual(parsed.test_contender[0].name, self.testInput1)
        self.assertEqual(parsed.filter_contender[0], 'e')
        self.assertEqual(parsed.benchmark_options[0], 'g')


if __name__ == '__main__':
    # unittest.main()
    main()

# vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
# kate: tab-width: 4; replace-tabs on; indent-width 4; tab-indents: off;
# kate: indent-mode python; remove-trailing-spaces modified;
