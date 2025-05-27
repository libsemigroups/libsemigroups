import os
import sys
import time
import subprocess
from tempfile import NamedTemporaryFile
import low_index
from low_index import permutation_reps
from low_index import benchmark_util

use_low_level = False
low_level_multi_threaded = False

examples = [
    {
        'group' : 'Fundamental group of K11n34',
        'rank': 3,
        'short relators': ["aaBcbbcAc"],
        'long relators': ["aacAbCBBaCAAbbcBc"],
        'index': 7,
    },
    {
        'group' : 'Fundamental group of K11n34',
        'rank': 3,
        'short relators': ["aaBcbbcAc"],
        'long relators':  ["aacAbCBBaCAAbbcBc"],
        'index': 8,
    },
    {
        'group' : 'Fundamental group of K15n12345',
        'rank': 3,
        'short relators': ["aBcACAcb"],
        'long relators': ["aBaCacBAcAbaBabaCAcAbaBaCacBAcAbaBabCAcAbABaCabABAbABaCabCAcAb"],
        'index': 7,
    },
    {
        'group' : 'Fundamental group of K15n12345',
        'rank': 3,
        'short relators': ["aBcACAcb"],
        'long relators':["aBaCacBAcAbaBabaCAcAbaBaCacBAcAbaBabCAcAbABaCabABAbABaCabCAcAb"],
        'index': 8,
    },
    {
        'group' : 'Fundamental group of o9_15405',
        'rank': 2,
        'short relators': ["aaaaabbbaabbbaaaaabbbaabbbaaaaaBBBBBBBB"],
        'long relators': [],
        'index': 9,
    },
    {
        'group' : 'Fundamental group of o9_15405',
        'rank': 2,
        'short relators': [],
        'long relators': ["aaaaabbbaabbbaaaaabbbaabbbaaaaaBBBBBBBB"],
        'index': 9,
    },
    {
        'group': 'K5_13(3,0)',
        'rank': 2,
        'short relators': ['bbb', 'abABaaBAbabABaaBAbabABAb'],
        'long relators': [],
        'index': 15
    },
    {
        'group': 'K15n145097(0,1)',
        'rank': 3,
        'short relators': ['aBCBabaBAbcbAccAbC'],  # Fastest was just one short rel
        'long relators': ['aBAbcbAcbAcAbAcAbAcAbCaBC',
                          'aBCBabABaaaaaaaaaaaaaaaaBAbcbAccbAcbAcAC'],
        'index': 7,
    },
    {
        'group': 'DodecahedralOrientableClosedCensus[3]',
        'rank': 4,
        'short relators': ['abbDDabacdc',
                           'acdAcDc',
                           'aBacddBBcD',
                           'acddCdCbAddB'],
        'long relators': [],
        'index': 6,
    },
    {
        'group': 'DodecahedralOrientableClosedCensus[8]',
        'rank': 4,
        'short relators': ['adADAddAbdbdbadaDD',
                           'adaDDbcccDAddAbdb',
                           'adaDDbccACACdaDDadCCBdAbcBdAbcBD',
                           'acacaDAddADcacaCCBdAbcBdAbcBdAbc'],
        'long relators': [],  # It didn't seem to help to make some rels long.
        'index': 5,
    },
    {
        'group': 'L14a26995(2,0)(2,0)(2,0)',
        'rank': 4,
        'short relators': ['bb',
                           'aa',
                           'cdcd',
                           'aCABacAddcDbABdCDDaCAbac',
                           'aBdcDbABddcDbaBdCDbABacAb',
                           'aCAbacACBDCbcaCAbacACBcdbcaCABacACBc'],
        'long relators': [], # Making rels long *slowed things down*.
        'index': 7,
    },
    {
        'group': 'ocube06_06795',
        'rank': 6,
        'short relators': ['bdbEED'],
        'long relators': ['aeecbcAfcbcF',
                          'bcedeeccDBec',
                          'aeecfedABaDBeFbcAb',
                          'aeecaeecfeBeFbcAbcAb'],
        'index': 4,

    },
    {
        'group': 'v3448~irr(1,0)(1,0)(1,0)',
        'spec': 'XLLvLPLvwLLvwzAzPvPwMLLzAPQAQQQQQkcdjhijoqnswwuBztxGIEAORLTKGPHUOHNSTSWLWRVQOVPQUVUWhssovshsksxtuxphrreckdlpqxreuaxejknrawarjnsjvrncbd_CbbadEBbBDab(1,0)(1,0)(1,0)',
        'rank': 5,
        'short relators': ['aaaabccccDDb'],
        'long relators': ['aaabcECBAAABAAABEEECBAAABaaaabcedAbaaabceCBAAABaaaabcbaaabcDbb',
            'aaabcdAbaaabceeeedAbaaabceCBAAABBddcDAAAABdAAAABEEECBAAABaaaabcbaaabcDbb',
            'aaabcdaaabcedAbaaabceeeebaaaaDbaaaadCDCBAAABCBAAAAbaaabcECBAAABaDEbaaabaaabcedAbaaabceeeebaaaadCCCCb',
            'aaabcdAbaaabceeeedAbaaabceCBAAABaaaabcbaaabcdaaabcedAbaaabceeeebaaaaDbaaaadCaaabcedAbaaabceeeebaaaaDbaaaadCaaabcedAbaaabceeeebaaaadCCCCb'],
        'index': 4,
    },
    {
        'group' : 'Symmetric Group S7',
        'rank' : 2,
        'short relators': [
            "aaaaaaa",
            "bb",
            "abababababab",
            "AbabAbabAbab",
            "AAbaabAAbaab",
            "AAAbaaabAAAbaaab"],
        'long relators': [],
        'index': 35,
    },
    # This example must go at the end to avoid an infinite loop when
    # using Magma for the benchmark.
    {
        'group' : 'Modular Group',
        'rank': 2,
        'short relators': ["aa", "bbb"],
        'long relators': [],
        'index': 25,
        'gap skip': True,
    },
]


def run_example(ex, num_threads=0):
    return len(
        permutation_reps(
            rank = ex['rank'],
            short_relators = ex['short relators'],
            long_relators = ex['long relators'],
            max_degree = ex['index'],
            strategy = ex.get('strategy', 'spin_short'),
            num_threads = num_threads))

def run_example_low_level(ex):
    short_relators = low_index.spin_short(
        [ low_index.parse_word(
            rank = ex['rank'], word = relator)
          for relator in ex['short relators'] ],
        max_degree = ex['index'])
    long_relators = [
        low_index.parse_word(
            rank = ex['rank'], word = relator)
          for relator in ex['long relators'] ]
    if low_level_multi_threaded:
        tree = low_index.SimsTreeMultiThreaded(
            rank = ex['rank'],
            max_degree = ex['index'],
            short_relators = short_relators,
            long_relators = long_relators,
            num_threads = low_index.hardware_concurrency())
    else:
        tree = low_index.SimsTree(
            rank = ex['rank'],
            max_degree = ex['index'],
            short_relators = short_relators,
            long_relators = long_relators)
    return len(tree.list())

def run(ex, num_threads = 0):
    print('%s; index = %d.'%(ex['group'], ex['index']))
    start = time.time()
    if use_low_level:
        n = run_example_low_level(ex)
    else:
        n = run_example(ex, num_threads = num_threads)
    elapsed = time.time() - start
    print('%d subgroups' % n)
    print('%.3fs'%elapsed)
    sys.stdout.flush()

def run_regina(ex):
    print('%s; index = %d.'%(ex['group'], ex['index']))
    if ex['index'] > 7:
        print("Skipping because regina requires index <= 7")
        sys.stdout.flush()
        return
    start = time.time()
    G = regina.GroupPresentation(
        ex['rank'], ex['short relators'] + ex['long relators'])
    n = 1
    for d in range(2, ex['index'] + 1):
        n += len(G.enumerateCovers(d))
    elapsed = time.time() - start
    print('%d subgroups' % n)
    print('%.3fs'%elapsed)
    sys.stdout.flush()

def translate_to_gap(ex, output):
    output.write('info := "%s; index=%d";\n'%(ex['group'], ex['index']))
    all_relators = ex['short relators'] + ex['long relators']
    gap_relators = [benchmark_util.expand_relator(r) for r in all_relators]
    letters = 'abcdefghijklmnopqrstuvwxyz'
    generators = letters[:ex['rank']]
    output.write('F := FreeGroup(')
    output.write(', '.join(['"%s"'%g for g in generators]))
    output.write(');\n')
    for n, gen in enumerate(generators):
        output.write('%s := F.%d;\n'%(gen, n + 1))
    output.write('G := F / [\n')
    for relator in gap_relators:
        output.write('%s,\n'%relator)
    output.write('];\n')
    output.write("""
PrintFormatted("{}\\n", info);
start := NanosecondsSinceEpoch();
ans := Length(LowIndexSubgroupsFpGroup(G,%d));
elapsed := Round(Float(NanosecondsSinceEpoch() - start) / 10000000.0)/100;
PrintFormatted("{} subgroups\\n", ans);
PrintFormatted("{} secs\\n", ViewString(elapsed));
"""%ex['index'])

def translate_to_magma(ex, output):
    output.write('"%s; index = %d";\n'%(ex['group'], ex['index']))
    all_relators = ex['short relators'] + ex['long relators']
    relators = [benchmark_util.expand_relator(r) for r in all_relators]
    letters = 'abcdefghijklmnopqrstuvwxyz'
    generators = letters[:ex['rank']]
    output.write('G := Group<\n')
    output.write(', '.join(['%s'%g for g in generators]))
    output.write(' | ')
    output.write(', '.join(['%s'%r for r in relators]))
    output.write('>;\n')
    output.write('T := Time();\n')
    output.write('sgps := LowIndexSubgroups(G, <1, %d>);\n'%ex['index'])
    output.write('T := Time(T);\n')
    output.write('count := #sgps;\n')
    output.write('printf "%o subgroups\\n", count;\n')
    output.write('printf "%o secs\\n", T;\n')

if __name__ == '__main__':
    print(benchmark_util.cpu_info(),
          'with',
          os.cpu_count(),
          'cores (reported by python)/',
          low_index.hardware_concurrency(),
          'cores (reported by C++)', file=sys.stderr)
    if '-gap' in sys.argv:
        with open('/tmp/benchmark.gap', 'w') as gap_script:
            for example in examples:
                if 'gap skip' not in example:
                    translate_to_gap(example, gap_script)
            gap_script.write("QUIT;\n")
        subprocess.run(['sage', '-gap', '/tmp/benchmark.gap'])
        os.unlink('/tmp/benchmark.gap')
    elif '-magma' in sys.argv:
        with open('/tmp/benchmark.magma', 'w') as magma_script:
            for example in examples:
                if 'magma skip' not in example:
                     translate_to_magma(example, magma_script)
            magma_script.write("\nquit;\n")
        subprocess.run(['magma', '/tmp/benchmark.magma'])
        os.unlink('/tmp/benchmark.magma')
    elif '-single' in sys.argv:
        print('Running single-threaded')
        for example in examples:
            run(example, num_threads = 1)
    elif '-regina' in sys.argv:
        import regina
        for example in examples:
            run_regina(example)
    else:
        for example in examples:
            run(example)
