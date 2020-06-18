#!/bin/sh
set -e

# This script generates a version string for Libsemigroups based on the
# VERSION file (if any) and the output of `git describe` (if inside a git
# working tree).
#
# The result is printed into the file VERSION, but only if the version
# string changed. This way, we do not trigger unnecessary rebuilds.

# This script is based on GAP's cnf/GAP-VERSION-GEN script,
# which is based on git.git's GIT-VERSION-GEN script

DEF_VER=v1.dev

LF='
'

# First see if there is a VERSION file (included in release tarballs),
# then try git-describe, then default.
if test -f .VERSION
then
	VN=$(cat .VERSION) || VN="$DEF_VER"
elif test -d ${GIT_DIR:-.git} -o -f .git &&
	VN=$(git describe --tags --match "v[0-9]*" --abbrev=7 HEAD 2>/dev/null) &&
	case "$VN" in
	*$LF*) (exit 1) ;;
	v[0-9]*)
		git update-index -q --refresh
		test -z "$(git diff-index --name-only HEAD --)" ||
		VN="$VN-dirty" ;;
	esac
then
	VN=$(echo "$VN");
else
	VN="$DEF_VER"
fi

echo $(expr "$VN" : v*'\(.*\)')
