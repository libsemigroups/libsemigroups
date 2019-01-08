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

def successors(p):
    return [np for np in p.coarsenings() if len(np) == len(p) - 1]


def write_file(size):

    lip = Partitions(size).list()
    lp = [SetPartitions(size)(SetPartitions(size, p).first()) for p in lip]
    dct = [(p, successors(p)) for p in lp]

    with open("idemp%i.hpp"%n, "w") as f:
        f.write("const int N = %i;\n\n"%size)
        f.write("vector<pair<BMat8, vector<BMat8>>> input = {\n")
        f.write(",\n".join(print_pairs(i) for i in dct))
        f.write("\n};\n")

dsize = {1 : [1],
2 : [1, 1],
3 : [1, 3, 1],
4 : [1, 7, 9, 22, 1],
5 : [1,15,21,137,155,517,1],
6 : [1,31,45,676,49,762,7924,1044,7689,34341,1],
7 : [1,63,93,3047,105,3369,87640,3333,5091,79189,1190191,98391,997113,5527771,1],
8 : [1,127,189,13150,217,14252,857353,225,13668,22410,736920,28370891,24698,655317,919902,21105957,412556411,1398444,23471712,301181717,1955385808,1]
}

def card(n):
    return sum(dsize[n][i]*SetPartitions(n, l).cardinality()
               for i, l in enumerate(Partitions(n)))
