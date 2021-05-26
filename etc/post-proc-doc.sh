#!/bin/bash
set -e

cd docs/

PREFIX=build/html/_generated/

# Action

FILES=( "libsemigroups__action__constructors.html" \
        "libsemigroups__action__initialization.html" \
        "libsemigroups__action__member_functions_inherited_from_runner.html" \
        "libsemigroups__action__member_types.html" \
        "libsemigroups__action__position__size__empty___.html" \
        "libsemigroups__action__strongly_connected_components.html" )
LINK="libsemigroups__action__member_functions_inherited_from_runner.html"

for FILE in "${FILES[@]}"; do
  echo "* fixing Runner links in: $FILE"
  sed -i'' -e "s:libsemigroups__runner__operators.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__state.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__reporting.html\#:$LINK\#:g" "$PREFIX/$FILE"
done

# CongruenceInterface

FILES=( "libsemigroups__congruenceinterface.html" \
        "libsemigroups__congruenceinterface__member_types.html" \
        "libsemigroups__congruenceinterface__constructors.html" \
        "libsemigroups__congruenceinterface__deleted_constructors.html" \
        "libsemigroups__congruenceinterface__handedness.html" \
        "libsemigroups__congruenceinterface__initialization.html" \
        "libsemigroups__congruenceinterface__iterators.html" \
        "libsemigroups__congruenceinterface__member_functions_inherited_from_runner.html" \
        "libsemigroups__congruenceinterface__member_types.html" \
        "libsemigroups__congruenceinterface__numbers_of_classes__and_comparisons.html" \
        "libsemigroups__congruenceinterface__related_semigroups.html" \
        "libsemigroups__congruenceinterface__words_and_class_indices.html" )

LINK="libsemigroups__congruenceinterface__member_functions_inherited_from_runner.html"

for FILE in "${FILES[@]}"; do
  echo "* fixing Runner links in: $FILE"
  sed -i'' -e "s:libsemigroups__runner__operators.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__state.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__reporting.html\#:$LINK\#:g" "$PREFIX/$FILE"
done

# This only works because the structure of the doc for
# KnuthBendixCongruenceByPairs and CongruenceInterface are the same.
for FILE in "${FILES[@]}"; do
  echo "* fixing KnuthBendixCongruenceByPairs links in: $FILE"
  sed -i'' -e 's:libsemigroups__knuthbendixcongruencebypairs:libsemigroups__congruenceinterface:g' "$PREFIX/$FILE"
done

# Congruence

FILES=( "libsemigroups__congruence__member_functions.html" \
        "libsemigroups__congruence__member_functions_inherited_from_congruenceinterface.html" \
        "libsemigroups__congruence__member_functions_inherited_from_runner.html" \
        "libsemigroups__congruence__member_types.html" )

# This only works because the structure of the doc for
# KnuthBendixCongruenceByPairs and Congruence are the same.
for FILE in "${FILES[@]}"; do
  echo "* fixing KnuthBendixCongruenceByPairs links in: $FILE"
  sed -i'' -e 's:libsemigroups__knuthbendixcongruencebypairs:libsemigroups__congruence:g' "$PREFIX/$FILE"
done

LINK="libsemigroups__congruence__member_functions_inherited_from_runner.html"
for FILE in "${FILES[@]}"; do
  echo "* fixing Runner links in: $FILE"
  sed -i'' -e "s:libsemigroups__runner__operators.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__state.html\#:$LINK\#:g" "$PREFIX/$FILE"
  sed -i'' -e "s:libsemigroups__runner__reporting.html\#:$LINK\#:g" "$PREFIX/$FILE"
done

#

