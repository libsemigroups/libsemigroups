n = 8;

lip = Partitions(n).list()
cards = [p.conjugacy_class_size() for p in lip]

lp = [SetPartitions(n)(SetPartitions(n, p).first()) for p in Partitions(n).list()]

def successors(p):
    return [np for np in p.coarsenings() if len(np) == len(p) - 1]

dct = [(p, successors(p)) for p in lp]

def part2mat(p):
    n = p.size()
    res = matrix(n,n)
    for pp in p:
        for i in pp:
            for j in pp:
                res[i-1,j-1]=1
    res.set_immutable()
    return res

def print_mat(m):
    return "BMat8({{" + "}, {".join(",".join(str(i) for i in r) for r in m) + "}})"

def print_pairs((p, sl)):
    res = "make_pair("
    res += print_mat(part2mat(p));
    res += ", vector<BMat8>({\n    " + ",\n    ".join(print_mat(part2mat(m)) for m in sl)
    return res + "\n}))"

def write_file():
    with open("from_sage", "w") as f:
        f.write(    ",\n".join(print_pairs(i) for i in dct))

