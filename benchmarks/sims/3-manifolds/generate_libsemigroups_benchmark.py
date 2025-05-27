#!/usr/bin/env python3
"""
TODO
"""

from benchmark import examples


for example in examples:
    if len(example.values()) != 5:
        continue
    group, rank, short, long, index = example.values()
    group = group[0].lower() + group[1:]
    tag = group.replace(" ", "-") + f"-{index}"
    tag = tag.replace("_", "-")

    print(
        f"""
    TEST_CASE("{group} - index {index} - 1-sided",
              "[{tag}]") {{
        auto                      rg = ReportGuard(false);
        Presentation<std::string> p;
        p.contains_empty_word(true);"""
    )

    print("// Short rules")
    for rule in short:
        print(f'presentation::add_cyclic_conjugates_no_checks(p, "{rule}");')

    print("// Long rules")
    length_long_rules = 100000000
    for rule in long:
        print(f'presentation::add_rule(p, "{rule}", "");')
        length_long_rules = min(length_long_rules, len(rule))
    print(
        f"""
        // TODO presentation::add_inverse_rules(p, "ABCabc");

        presentation::reduce_complements(p);
        presentation::remove_duplicate_rules(p);
        presentation::remove_trivial_rules(p);
        presentation::sort_each_rule(p);
        presentation::sort_rules(p);
        // REQUIRE(S.presentation().rules == std::vector<std::string>());
        // REQUIRE(presentation::to_gap_string(p, "G") == "");

        Sims1 S;
        S.presentation(to<Presentation<word_type>>(p))
            .add_pruner(conj_pruner)
            .length_long_rules({length_long_rules});

        for (auto nr_threads : {{1, 2, 4, 8}}) {{
        BENCHMARK(fmt::format("libsemigroups ({{}} thread{{}})",
                                nr_threads,
                                nr_threads == 1 ? "" : "s")
                        .c_str()) {{
            REQUIRE(S.number_of_threads(nr_threads).number_of_congruences({index}) == 0);
        }};
        }}
    }}
    """
    )
