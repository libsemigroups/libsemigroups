#!/usr/bin/env python

from accepts import accepts
import os
import shlex
import subprocess
import sys

nr = 0
assert_str = "LIBSEMIGROUPS_ASSERT"
undefine_asserts = """
#ifdef LIBSEMIGROUPS_ASSERT
#undef LIBSEMIGROUPS_ASSERT
#define LIBSEMIGROUPS_ASSERT(x)
#endif
"""
undefine_debug = """
#ifdef LIBSEMIGROUPS_DEBUG
#undef LIBSEMIGROUPS_DEBUG
#endif
"""


@accepts(str, bool)
def bash(args, show=False):
    child = subprocess.Popen(shlex.split(args), stdout=subprocess.PIPE)
    while show:
        line = child.stdout.readline()
        if not line:
            break
        else:
            print(line.decode("utf-8").rstrip())
        sys.stdout.flush()
    child.wait()
    return child.returncode


@accepts(str)
def is_valid_file(f):
    if not (f.endswith(".hpp") or f.endswith(".cpp")):
        return False
    if f.endswith('libsemigroups-config.hpp'):
        return False
    return not f.endswith("libsemigroups-debug.hpp")


@accepts(str)
def renumber_asserts(f):
    global nr
    if not os.path.isfile(f) or not is_valid_file(f):
        return
    with open(f, "r") as ff:
        lines = ff.readlines()
        for i in range(len(lines)):
            if lines[i].find(assert_str) != -1:
                nr += 1
                lines[i] = lines[i].replace(
                    assert_str, assert_str + "_" + str(nr).zfill(3)
                )
    with open(f, "w") as ff:
        ff.writelines(lines)


@accepts(str)
def disable_asserts_file(f):
    if not os.path.isfile(f) or not is_valid_file(f):
        return False
    with open(f, "r") as ff:
        lines = ff.read()
        if lines.find("LIBSEMIGROUPS_ASSERT") == -1:
            return False
        lines = undefine_asserts + lines
    with open(f, "w") as ff:
        ff.writelines(lines)
    return True


@accepts(str)
def disable_debug_file(f):
    if not os.path.isfile(f) or not is_valid_file(f):
        return False
    with open(f, "r") as ff:
        lines = ff.read()
        if lines.find("LIBSEMIGROUPS_DEBUG") == -1:
            return False
        lines = undefine_debug + lines
    with open(f, "w") as ff:
        ff.write(lines)
    return True


def function_dir(d, func, msg):
    if not os.path.isdir(d):
        return
    for f in sorted(os.listdir(d)):
        f = os.path.join(d, f)
        if func(f):
            print("Disabling " + msg + " in " + f + " . . .")
            bash("make test_cong_intf -j6")
            if bash("./test_cong_intf [004]", True) == 0:
                print(msg + " in file " + f + " is the culprit!!!!")
                sys.exit(0)
            else:
                bash("git checkout " + f)


@accepts(str)
def disable_asserts_dir(d):
    function_dir(d, disable_asserts_file, "assertions")


@accepts(str)
def disable_debug_dir(d):
    function_dir(d, disable_debug_file, "debug mode")


bash("git add etc/bad-assert.py")
bash("git commit --amend --no-edit")
bash("git reset --hard")

# disable_debug_dir("include")
# disable_debug_dir("src")
# disable_asserts_dir("include")
disable_asserts_dir("src")

sys.exit(0)


files("include/")
files("src/")

for exclude in range(0, nr + 1):
    print("Trying with " + str(exclude).zfill(3) + " excluded . . .")
    bash(
        'ag --ignore log.txt "LIBSEMIGROUPS_ASSERT_'
        + str(exclude).zfill(3)
        + '"'
    )

    rep = []
    for j in range(1, nr + 1):
        rep.append("#define " + assert_str + "_" + str(j).zfill(3) + "(x)")
        if j != exclude:
            rep[-1] += " assert(x)"
        rep[-1] += "\n"

    with open("include/libsemigroups-debug.hpp", "r") as f:
        lines = f.readlines()
        for i in range(len(lines)):
            if lines[i] == "#define LIBSEMIGROUPS_ASSERT(x) assert(x)\n":
                lines = lines[0:i] + rep + lines[i + 1 :]

    with open("include/libsemigroups-debug.hpp", "w") as f:
        f.writelines(lines)

    bash("make test_cong_intf -j6")
    if bash("./test_cong_intf [004]") == 0:
        print("Assertion number " + str(exclude) + " is the culprit!!!!")
        break
    else:
        bash("git checkout include/libsemigroups-debug.hpp")

# os.popen('git reset --hard')
