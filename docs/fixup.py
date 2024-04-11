#!/usr/bin/env python3

"""Python script to fix up certain some automatically generated files in Doxygen."""

from typing import Callable
from pprint import pformat
import re


def modify_in_place(filename: str, function: Callable[[str], tuple[str, bool]]) -> None:
    """Run a function over the contents of a file, modifying it in place."""
    with open(filename, "r") as in_file:
        data = in_file.read()
    data, success = function(data)
    if not success:
        print(f"Warning: {function.__name__} failed to modify {filename}!")
    with open(filename, "w") as out_file:
        out_file.write(data)


def fix_menu_1(data: str) -> tuple[str, bool]:
    """Fix navmenu item expansion bug.

    For some unknown reason there is a check before doing a node expansion,
    which basically does some checking related to the root node of the page.
    Since we are in some sense circumventing the usual navigation layout, this
    check fails.

    Parameters
    ----------
    data: str
        The string corresponding to the source code to be modified.

    Returns
    -------
    str
        The modified source code.
    bool
        A boolean indicating if any changes were made.


    Notes
    -----
    Fixed by modifying a line which sets the `rootBase` variable. We will
    override it so that our `rootBase` is always `index`.

    Should work for 1.8.2 <= Doxygen <= 1.10.0
    """
    result, count = re.subn(
        r"(?s:rootBase\s*=([^=]|\s+).*?;)", r'rootBase = "index";', data
    )
    return result, count > 0


def fix_menu_2(data: str) -> tuple[str, bool]:
    """Fix navmenu item highlighting bug.

    Sometimes get wrong highlighting in navbar. This is caused by `cachedLink`
    function using something silly to figure out the previously used link and
    then this result being erroneously used later on. Seems to be fixed in
    Doxygen 1.10.0, but fails in 1.9.4.

    Parameters
    ----------
    data: str
        The string corresponding to the source code to be modified.

    Returns
    -------
    str
        The modified source code.
    bool
        A boolean indicating if any changes were made.


    Notes
    -----
    Fixed by making the `cachedLink` function a noop.

    Should work for 1.8.1 <= Doxygen <= 1.10.0
    """
    result, count = re.subn(
        r"(?s:cachedLink\s*=\s*function\s*\(\s*\)\s*\{[^{]*?\})",
        r'cachedLink = function() { return "" }',
        data,
    )
    return result, count > 0


def fix_menu_3_part_1(data: str) -> tuple[str, bool]:
    """Fix navmenu item hierarchy bug.

    By default the navmenu includes the `mainpage` as the top level root of the
    project. This means that all the sections in the main page appear on the
    same level as documentation describing different files. This is confusing
    and looks bad.

    This function fixes part of the issue by modifying the contents of `navtreedata.js`.

    Parameters
    ----------
    data: str
        The string corresponding to the source code to be modified.

    Returns
    -------
    str
        The modified source code.
    bool
        A boolean indicating if any changes were made.


    Notes
    -----
    Fixed by changing the nesting level of various components in `NAVTREE`.
    """
    pattern = re.compile(r"(?s:var\s+NAVTREE\s*=\s*(\[.*?\])\s*;)")
    match = pattern.search(data)
    if match is None:
        return data, False

    null = None  # Required for eval to work
    tree = eval(match.group(1))
    assert len(tree) > 0, "Malformed tree"
    assert len(tree[0]) == 3, "Malformed tree"
    index_children = tree[0][2]
    new_index_children = []
    for child in index_children:
        assert len(child) == 3, "Malformed tree"
        assert isinstance(child[1], str), "Malformed tree"
        if child[1].split("#")[0] == "index.html":
            new_index_children.append(child)
        else:
            tree.append(child)
    tree[0][2] = new_index_children

    result, count = pattern.subn(
        "const None = null;\nvar NAVTREE = \n" + pformat(tree) + ";\n", data
    )

    return result, count > 0


def fix_menu_3_part_2(data: str) -> tuple[str, bool]:
    """Fix navmenu item hierarchy bug.

    By default the navmenu includes the `mainpage` as the top level root of the
    project. This means that all the sections in the main page appear on the
    same level as documentation describing different files. This is confusing
    and looks bad.

    This function fixes part of the issue by modifying the contents of `navtreeindex0.js`.

    Parameters
    ----------
    data: str
        The string corresponding to the source code to be modified.

    Returns
    -------
    str
        The modified source code.
    bool
        A boolean indicating if any changes were made.


    Notes
    -----
    Fixed by changing the breadcrumb path of various components in `NAVTREEINDEX0`.
    """

    pattern = re.compile(r"(?s:var\s+NAVTREEINDEX0\s*=\s*(\{.*?\})\s*;)")
    match = pattern.search(data)
    if match is None:
        return data, False

    paths = eval(match.group(1))
    if "pages.html" in paths:
        # WARN: Not sure what the ramifications of deleting this are. Probably
        # not an issue.
        del paths["pages.html"]

    bad_indices = []
    for key, value in paths.items():
        assert isinstance(key, str), "Malformed navigation index"
        assert isinstance(value, list), "Malformed navigation index"
        if key.split("#")[0] == "index.html":
            if len(value) > 0:
                assert value[0] >= 0, "Malformed navigation index"
                bad_indices.append(value[0])
            value.insert(0, 0)
    bad_indices.sort()
    k = max(bad_indices)

    if k == 0:
        # Everything is already fine
        return data, True

    if bad_indices != list(range(k + 1)):
        # TODO: Implement this.
        raise NotImplementedError(
            "Can't handle case where index navtree indices are non contiguous",
            (data, bad_indices, k),
        )

    for key, value in paths.items():
        if key.split("#")[0] != "index.html":
            if len(value) > 0:
                assert value[0] > k, "Malformed navigation index"
                value[0] -= k

    result, count = pattern.subn(
        "var NAVTREEINDEX0 = \n" + pformat(paths) + ";\n", data
    )

    return result, count > 0


def fix_menu_3_part_3(data: str) -> tuple[str, bool]:
    """Fix navmenu item hierarchy bug.

    By default the navmenu includes the `mainpage` as the top level root of the
    project. This means that all the sections in the main page appear on the
    same level as documentation describing different files. This is confusing
    and looks bad.

    This function fixes part of the issue by modifying the contents of `navtree.js`.

    Parameters
    ----------
    data: str
        The string corresponding to the source code to be modified.

    Returns
    -------
    str
        The modified source code.
    bool
        A boolean indicating if any changes were made.


    Notes
    -----
    Fixed by removing a line which erroneously adds the `0` node as the root
    for all nodes of the navtree.
    """

    result, count = re.subn(
        r"o.breadcrumbs.unshift\(0\);",
        r"// o.breadcrumbs.unshift(0);",
        data,
    )
    return result, count > 0


if __name__ == "__main__":
    modify_in_place("./html/navtree.js", fix_menu_1)
    modify_in_place("./html/navtree.js", fix_menu_2)
    modify_in_place("./html/navtreedata.js", fix_menu_3_part_1)
    modify_in_place("./html/navtreeindex0.js", fix_menu_3_part_2)
    modify_in_place("./html/navtree.js", fix_menu_3_part_3)
