#!/usr/bin/env python3

"""Python script to fix up certain some automatically generated files in Doxygen."""

from typing import Callable
from pprint import pformat
from glob import glob
import re


def modify_in_place(
    filename: str, function: Callable[[str], tuple[str, bool]], warn: bool = True
) -> bool:
    """Run a function over the contents of a file, modifying it in place."""
    with open(filename, "r") as in_file:
        data = in_file.read()
    data, success = function(data)
    if not success:
        if warn:
            print(f"Warning: {function.__name__} failed to modify {filename}!")
        return False
    with open(filename, "w") as out_file:
        out_file.write(data)
    return True


def modify_in_place_all_files(
    filename_glob: str, function: Callable[[str], tuple[str, bool]], warn: bool = True
) -> bool:
    filenames = glob(filename_glob)
    if len(filenames) == 0:
        if warn:
            print(
                f"Warning: no files matched glob {filename_glob}, so {function.__name__} did not run!"
            )
        return False

    all_fail = True
    for filename in filenames:
        success = modify_in_place(filename, function, warn=False)
        if success:
            all_fail = False

    if all_fail and warn:
        print(
            f"Warning: {function.__name__} failed to modify any files matching {filename_glob}!"
        )
    return all_fail


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
    assert tree[0][1] == "index.html", "Malformed tree"
    result, count = pattern.subn(
        "const None = null;\nvar NAVTREE = \n" + pformat(tree[0][2]) + ";\n", data
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
    for del_page in {"pages.html", "index.html"}:
        if del_page in paths:
            del paths[del_page]

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


def fix_menu_4(data: str) -> tuple[str, bool]:
    """Add support for navmenu headings.

    By default the every entry in the navmenu is a link. We would like to add support for navmenu headings.

    This function fixes this issue by modifying the contents of `navtree.js`.

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
    Does some rather esoteric modifications on `navtree.js`.
    Should work for 1.7.2 <= Doxygen <= 1.10.0
    """

    regex = re.compile(
        r'(?s:(var|const)\s*a\s*=\s*document.createElement\(\s*"a"\s*\);(.*?)return\s*node;)'
    )
    match = regex.search(data)
    if match is None:
        return data, False
    inner_data = match.group(2)
    # Check if we are in older version of Doxygen
    if re.match(r"node.childrenUL\s*=\s*null;", inner_data) is None:
        # New version >= 1.10.0
        result, count = regex.subn(
            r"""if (link == "_HEADING_") {
  node.labelSpan.role = "heading";
  node.labelSpan.appendChild(node.label);
  po.getChildrenUL().appendChild(node.li);
} else {
  \1 a = document.createElement("a");
  \2
}
return node;""",
            data,
        )
    else:
        # Old version < 1.10.0
        regex = re.compile(
            r'(?s:(var|const)\s*a\s*=\s*document.createElement\(\s*"a"\s*\);(.*?)node.childrenUL\s*=\s*null;)'
        )
        result, count = regex.subn(
            r"""if (link == "_HEADING_") {
  node.labelSpan.role = "heading";
  node.labelSpan.appendChild(node.label);
} else {
  \1 a = document.createElement("a");
  \2
}
node.childrenUL = null;""",
            data,
        )

    return result, count > 0


def fix_pages_1(data: str) -> tuple[str, bool]:
    """Remove detailed description header.

    The "Detailed Description" header looks out of place. This function fixes
    the issue by modifying the contents of `*.html` files.

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
    Fixed by deleting the `<h2 class="groupheader">Detailed Description</h2>` element.
    """
    result, count = re.subn(
        r'(?s:<h2\s+class\s*=\s*"groupheader"\s*>\s*Detailed\s*Description\s*</h2>)',
        r"",
        data,
    )
    return result, count > 0


if __name__ == "__main__":
    modify_in_place("./html/navtree.js", fix_menu_1)
    modify_in_place("./html/navtree.js", fix_menu_2)
    modify_in_place("./html/navtreedata.js", fix_menu_3_part_1)
    modify_in_place("./html/navtreeindex0.js", fix_menu_3_part_2)
    modify_in_place("./html/navtree.js", fix_menu_3_part_3)
    modify_in_place("./html/navtree.js", fix_menu_4)
    modify_in_place_all_files("./html/*.html", fix_pages_1)
