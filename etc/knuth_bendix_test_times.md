# Knuth-Bendix Test Times

## Quick tests
| Number | Name                                              | `RewriteFromLeft` | `RewriteTrie` (Improvement) | `RewriteTrie` all nodes (Improvement) |
| ------ | ------------------------------------------------- | ----------------- | --------------------------- | ------------------------------------- |
| 000    | (fpsemi) confluent fp semigroup 1 (infinite)      | 399μs             | 696μs (0.57x)               | 624μs (0.64x)                         |
| 001    | (fpsemi) confluent fp semigroup 2 (infinite)      | 206μs             | 253μs (0.81x)               | 245μs (0.84x)                         |
| 002    | (fpsemi) confluent fp semigroup 3 (infinite)      | 126μs             | 164μs (0.77x)               | 163μs (0.77x)                         |
| 003    | non-confluent fp semigroup from wikipedia ( [...] | 442μs             | 497μs (0.89x)               | 477μs (0.93x)                         |
| 004    | Example 5.1 in Sims (infinite)                    | 288μs             | 378μs (0.76x)               | 378μs (0.76x)                         |
| 005    | Example 5.1 in Sims (infinite)                    | 294μs             | 364μs (0.81x)               | 355μs (0.83x)                         |
| 006    | Example 5.3 in Sims                               | 273μs             | 328μs (0.83x)               | 314μs (0.87x)                         |
| 007    | Example 5.4 in Sims                               | 391μs             | 435μs (0.90x)               | 425μs (0.92x)                         |
| 008    | Example 6.4 in Sims (size 168)                    | 54ms              | 25ms (2.16x)                | 24ms (2.25x)                          |
| 009    | random example                                    | 170ms             | 121ms (1.40x)               | 127ms (1.34x)                         |
| 010    | SL(2, 7) from Chapter 3, Proposition 1.5 in [...] | 505ms             | 68ms (7.43x)                | 73ms (6.92x)                          |
| 011    | F(2, 5) - Chapter 9, Section 1 in NR (size 11)    | 1404μs            | 1594μs (0.88x)              | 1669μs (0.84x)                        |
| 012    | Reinis example 1                                  | 1973μs            | 1975μs (1.00x)              | 1957μs (1.01x)                        |
| 013    | redundant_rule (std::string)                      | 100ms             | 100ms (1.00x)               | 100ms (1.00x)                         |
| 014    | redundant_rule (word_type)                        | 10ms              | 10ms (1.00x)                | 10ms (1.00x)                          |
| 101    | constructors/init for finished                    | 478μs             | 671μs (0.71x)               | 706μs (0.68x)                         |
| 110    | constructors/init for partially run               | 30ms              | 33ms (0.91x)                | 32ms (0.94x)                          |
| 999    | non-trivial classes                               | 346μs             | 368μs (0.94x)               | 433μs (0.80x)                         |
| 997    | non-trivial classes                               | 2237μs            | 2003μs (1.12x)              | 2144μs (1.04x)                        |
| 996    | non-trivial classes                               | 250μs             | 295μs (0.85x)               | 303μs (0.83x)                         |
| 995    | non-trivial classes                               | 316μs             | 369μs (0.86x)               | 378μs (0.84x)                         |
| 112    | non-trivial congruence on an infinite fp se [...] | 535μs             | 527μs (1.02x)               | 544μs (0.98x)                         |
| 113    | non-trivial congruence on an infinite fp se [...] | 517μs             | 556μs (0.93x)               | 577μs (0.90x)                         |
| 114    | trivial congruence on a finite fp semigroup       | 817μs             | 620μs (1.32x)               | 569μs (1.44x)                         |
| 116    | universal congruence on a finite fp semigroup     | 644μs             | 605μs (1.06x)               | 593μs (1.09x)                         |
| 121    | finite fp semigroup, size 16                      | 1401μs            | 1113μs (1.26x)              | 1105μs (1.27x)                        |
| 126    | non_trivial_classes exceptions                    | 2919μs            | 2864μs (1.02x)              | 2929μs (1.00x)                        |
| 021    | (from kbmag/standalone/kb_data/f25monoid)         | 1467μs            | 1667μs (0.88x)              | 1696μs (0.86x)                        |
| 022    | (from kbmag/standalone/kb_data/degen4a)           | 115ms             | 160ms (0.72x)               | 170ms (0.68x)                         |
| 023    | (from kbmag/standalone/kb_data/torus)             | 1405μs            | 1612μs (0.87x)              | 1582μs (0.89x)                        |
| 024    | (from kbmag/standalone/kb_data/3a6)               | 1604ms            | 219ms (7.32x)               | 226ms (7.10x)                         |
| 025    | (from kbmag/standalone/kb_data/f2)                | 284μs             | 328μs (0.87x)               | 262μs (1.08x)                         |
| 026    | (from kbmag/standalone/kb_data/s16)               | 403ms             | 197ms (2.05x)               | 205ms (1.97x)                         |
| 027    | (from kbmag/standalone/kb_data/a4monoid)          | 215μs             | 274μs (0.78x)               | 248μs (0.87x)                         |
| 028    | (from kbmag/standalone/kb_data/degen3)            | 28μs              | 55μs (0.51x)                | 42μs (0.67x)                          |
| 029    | (from kbmag/standalone/kb_data/ab1)               | 32μs              | 59μs (0.54x)                | 48μs (0.67x)                          |
| 030    | (from kbmag/standalone/kb_data/degen2)            | 14μs              | 22μs (0.64x)                | 21μs (0.67x)                          |
| 031    | (from kbmag/standalone/kb_data/f25)               | 1463μs            | 1643μs (0.89x)              | 1714μs (0.85x)                        |
| 032    | (from kbmag/standalone/kb_data/237)               | 2156μs            | 1815μs (1.19x)              | 1826μs (1.18x)                        |
| 033    | (from kbmag/standalone/kb_data/c2)                | 17μs              | 31μs (0.55x)                | 32μs (0.53x)                          |
| 034    | (from kbmag/standalone/kb_data/cosets)            | 863μs             | 761μs (1.13x)               | 767μs (1.13x)                         |
| 035    | Example 5.1 in Sims                               | 110μs             | 161μs (0.68x)               | 167μs (0.66x)                         |
| 036    | (from kbmag/standalone/kb_data/nilp2)             | 36μs              | 64μs (0.56x)                | 71μs (0.51x)                          |
| 037    | Example 6.4 in Sims                               | 100ms             | 24ms (4.17x)                | 25ms (4.00x)                          |
| 038    | 071 again                                         | 2545μs            | 2147μs (1.19x)              | 2245μs (1.13x)                        |
| 039    | Example 5.4 in Sims (11 again)  [...]             | 624μs             | 602μs (1.04x)               | 671μs (0.93x)                         |
| 040    | Example 5.4 in Sims (11 again)  [...]             | 505μs             | 497μs (1.02x)               | 540μs (0.94x)                         |
| 041    | operator<<                                        | 66μs              | 88μs (0.75x)                | 101μs (0.65x)                         |
| 042    | confluence_interval                               | 14μs              | 30μs (0.47x)                | 29μs (0.48x)                          |
| 043    | max_overlap                                       | 15μs              | 27μs (0.56x)                | 27μs (0.56x)                          |
| 044    | (from kbmag/standalone/kb_data/d22) (2 / 3) [...] | 4523μs            | 3929μs (1.15x)              | 4208μs (1.07x)                        |
| 045    | (from kbmag/standalone/kb_data/d22) (3 / 3) [...] | 7907μs            | 7539μs (1.05x)              | 8068μs (0.98x)                        |
| 046    | small example                                     | 4691μs            | 2708μs (1.73x)              | 2826μs (1.66x)                        |
| 047    | code coverage                                     | 22μs              | 37μs (0.59x)                | 39μs (0.56x)                          |
| 048    | small overlap 1                                   | 110μs             | 128μs (0.86x)               | 126μs (0.87x)                         |
| 049    | (from kbmag/standalone/kb_data/s9)                | 7693μs            | 5180μs (1.49x)              | 5283μs (1.46x)                        |
| 019    | C(4) monoid                                       | 56μs              | 87μs (0.64x)                | 96μs (0.58x)                          |
| 129    | search for a monoid that might not exist          | 2092μs            | 2058μs (1.02x)              | 2073μs (1.01x)                        |
| 050    | Chapter 11, Lemma 1.8 (q = 6, r = 5) in NR  [...] | 2611μs            | 2342μs (1.11x)              | 2424μs (1.08x)                        |
| 051    | Chapter 11, Section 2 (q = 6, r = 2, alpha  [...] | 358μs             | 457μs (0.78x)               | 452μs (0.79x)                         |
| 052    | Chapter 8, Theorem 4.2 in NR (infinite)           | 1047μs            | 761μs (1.38x)               | 818μs (1.28x)                         |
| 053    | equal_to fp semigroup                             | 82μs              | 107μs (0.77x)               | 124μs (0.66x)                         |
| 054    | equal_to free semigroup                           | 166μs             | 178μs (0.93x)               | 167μs (0.99x)                         |
| 055    | from GAP smalloverlap gap/test.gi (infinite)      | 220μs             | 225μs (0.98x)               | 242μs (0.91x)                         |
| 056    | from GAP smalloverlap gap/test.gi:49 (infinite)   | 166μs             | 187μs (0.89x)               | 206μs (0.81x)                         |
| 057    | from GAP smalloverlap gap/test.gi:63 (infinite)   | 107μs             | 131μs (0.82x)               | 138μs (0.78x)                         |
| 058    | from GAP smalloverlap gap/test.gi:70 (infinite)   | 1854μs            | 1709μs (1.08x)              | 1848μs (1.00x)                        |
| 059    | from GAP smalloverlap gap/test.gi:77 (infinite)   | 373μs             | 441μs (0.85x)               | 425μs (0.88x)                         |
| 060    | from GAP smalloverlap gap/test.gi:85 (infinite)   | 64μs              | 84μs (0.76x)                | 112μs (0.57x)                         |
| 061    | Von Dyck (2,3,7) group (infinite)                 | 2017μs            | 1730μs (1.17x)              | 1835μs (1.10x)                        |
| 062    | Von Dyck (2,3,7) group - different presenta [...] | 24ms              | 19ms (1.26x)                | 20ms (1.20x)                          |
| 063    | rewriting system from another test                | 1646μs            | 1881μs (0.88x)              | 1860μs (0.88x)                        |
| 064    | rewriting system from Congruence 20               | 55μs              | 67μs (0.82x)                | 69μs (0.80x)                          |
| 065    | (from kbmag/standalone/kb_data/ab2)               | 210μs             | 250μs (0.84x)               | 263μs (0.80x)                         |
| 066    | (from kbmag/standalone/kb_data/d22) (1 / 3) [...] | 4715μs            | 4096μs (1.15x)              | 4283μs (1.10x)                        |
| 067    | (from kbmag/standalone/kb_data/degen1)            | 11μs              | 20μs (0.55x)                | 15μs (0.73x)                          |
| 068    | (from kbmag/standalone/kb_data/s4)                | 1145μs            | 1073μs (1.07x)              | 1059μs (1.08x)                        |
| 069    | fp semigroup (infinite)                           | 85μs              | 128μs (0.66x)               | 116μs (0.73x)                         |
| 070    | Chapter 11, Section 1 (q = 4, r = 3) in NR( [...] | 10ms              | 5040μs (1.98x)              | 5190μs (1.93x)                        |
| 071    | Chapter 11, Section 1 (q = 8, r = 5) in NR  [...] | 796ms             | 75ms (10.61x)               | 77ms (10.34x)                         |
| 072    | Chapter 7, Theorem 3.9 in NR (size 240)           | 24ms              | 10ms (2.40x)                | 11ms (2.18x)                          |
| 073    | F(2, 5) - Chapter 9, Section 1 in NR (size 11)    | 1458μs            | 1594μs (0.91x)              | 1676μs (0.87x)                        |
| 074    | F(2, 6) - Chapter 9, Section 1 in NR              | 1603μs            | 1769μs (0.91x)              | 1847μs (0.87x)                        |
| 075    | Chapter 10, Section 4 in NR (infinite)            | 4183μs            | 4364μs (0.96x)              | 4552μs (0.92x)                        |
| 076    | Sym(5) from Chapter 3, Proposition 1.1 in N [...] | 17ms              | 9769μs (1.74x)              | 9961μs (1.71x)                        |
| 077    | SL(2, 7) from Chapter 3, Proposition 1.5 in [...] | 503ms             | 68ms (7.40x)                | 72ms (6.99x)                          |
| 078    | bicyclic monoid (infinite)                        | 99μs              | 125μs (0.79x)               | 133μs (0.74x)                         |
| 079    | plactic monoid of degree 2 (infinite)             | 118μs             | 170μs (0.69x)               | 181μs (0.65x)                         |
| 080    | example before Chapter 7, Proposition 1.1 i [...] | 48μs              | 64μs (0.75x)                | 64μs (0.75x)                          |
| 081    | Chapter 7, Theorem 3.6 in NR (size 243)           | 4771μs            | 2759μs (1.73x)              | 2813μs (1.70x)                        |
| 082    | finite semigroup (size 99)                        | 1021μs            | 904μs (1.13x)               | 957μs (1.07x)                         |
| 096    | Sorouhesh                                         | 1724μs            | 1470μs (1.17x)              | 1547μs (1.11x)                        |
| 097    | transformation semigroup (size 4)                 | 393μs             | 419μs (0.94x)               | 360μs (1.09x)                         |
| 098    | transformation semigroup (size 9)                 | 187μs             | 238μs (0.79x)               | 214μs (0.87x)                         |
| 099    | transformation semigroup (size 88)                | 4145μs            | 1025μs (4.04x)              | 1066μs (3.89x)                        |
| 100    | internal_string_to_word                           | 73μs              | 77μs (0.95x)                | 89μs (0.82x)                          |
| 102    | internal_string_to_word                           | 4125μs            | 847μs (4.87x)               | 907μs (4.55x)                         |
| 122    | manual right congruence                           | 16ms              | 5603μs (2.86x)              | 5947μs (2.69x)                        |
| 123    | right congruence!!!                               | 12ms              | 5412μs (2.22x)              | 5539μs (2.17x)                        |
| 124    | manual left congruence!!!                         | Failed            | Failed                      | Failed                                |
| 125    | automatic left congruence!!!                      | Failed            | Failed                      | Failed                                |
| 103    | Presentation<word_type>                           | 214μs             | 247μs (0.87x)               | 209μs (1.02x)                         |
| 104    | free semigroup congruence (6 classes)             | 581μs             | 630μs (0.92x)               | 659μs (0.88x)                         |
| 105    | free semigroup congruence (16 classes)            | 794μs             | 465μs (1.71x)               | 466μs (1.70x)                         |
| 106    | free semigroup congruence (6 classes)             | 1239μs            | 856μs (1.45x)               | 892μs (1.39x)                         |
| 107    | free semigroup congruence (240 classes)           | 24ms              | 10ms (2.40x)                | 11ms (2.18x)                          |
| 108    | free semigroup congruence (240 classes)           | 24ms              | 10ms (2.40x)                | 10ms (2.40x)                          |
| 111    | constructors                                      | 1367ms            | 260ms (5.26x)               | 273ms (5.01x)                         |
| 115    | to_froidure_pin                                   | 570μs             | 802μs (0.71x)               | 794μs (0.72x)                         |
| 117    | number of classes when obv-inf                    | 116μs             | 142μs (0.82x)               | 147μs (0.79x)                         |
| 020    | Chinese monoid                                    | 448μs             | 514μs (0.87x)               | 515μs (0.87x)                         |
| 109    | constructors/init for finished                    | 534μs             | 740μs (0.72x)               | 740μs (0.72x)                         |

## Standard Tests
| Number | Name                                              | `RewriteFromLeft` | `RewriteTrie` (Improvement) | `RewriteTrie` all nodes (Improvement) |
| ------ | ------------------------------------------------- | ----------------- | --------------------------- | ------------------------------------- |
| 084    | Example 6.6 in Sims (with limited overlap l [...] | 16907ms           | 5269ms (3.21x)              | 5462ms (3.10)                         |
| 085    | (from kbmag/standalone/kb_data/funny3)            | 4917ms            | 17396ms (0.28x)             | 16667ms (0.30)                        |
| 083    | partial_transformation_monoid(4)                  | 2560ms            | 655ms (3.91x)               | 639ms (4.01)                          |

## Extreme Tests
| Number | Name                                              | `RewriteFromLeft` | `RewriteTrie` (Improvement) | `RewriteTrie` all nodes (Improvement) |
| ------ | ------------------------------------------------- | ----------------- | --------------------------- | ------------------------------------- |
| 133    | sigma sylvester monoid                            | 1m21s             | 22853ms (3.54x)             | 22599ms (3.58x)                       |
| 135    | sigma sylvester monoid                            | 258ms             | 233ms (1.11x)               | 236ms (1.09x)                         |
| 086    | (from kbmag/standalone/kb_data/f27) (finite [...] | 1m14s             | 1m42s (0.75x)               | 1m41s (0.75x)                         |
| 087    | (from kbmag/standalone/kb_data/m11)               | 11m11s            | 13478ms (49.78x)            | 14242ms (47.11x)                      |
| 088    | (from kbmag/standalone/kb_data/e8)                | 1m7s              | 21257ms (3.15x)             | 14242ms (4.70x)                       |
| 089    | (from kbmag/standalone/kb_data/degen4b)           | 49405ms           | 2m8s (0.39x)                | 2m13s (0.37x)                         |
| 090    | (from kbmag/standalone/kb_data/f27_2gen)          | 23460ms           | 1m20s (0.29x)               | 1m10s (0.34x)                         |
| 091    | Example 6.6 in Sims                               | 11m36s            | 8400ms (82.86x)             | 8835ms (78.78x)                       |
| 092    | (from kbmag/standalone/kb_data/f27) (infini [...] | 55726ms           | 1m27s (0.64x)               | 1m27s (0.64x)                         |
| 093    | (from kbmag/standalone/kb_data/l32ext)            | 11m28s            | 8394ms (81.96x)             | 8664ms (79.41x)                       |
| 118    | partial_transformation_monoid5                    | 10m35s            | 1m41s (6.29x)               | 1m40s (6.35x)                         |
| 119    | full_transformation_monoid Iwahori                | 10m35s            | 10585ms (59.99x)            | 10339ms (61.42x)                      |
