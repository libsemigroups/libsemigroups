#!/bin/bash
set -e

if [[ -x "$(command -v unbuffer)" ]]; then
  UNBUF="unbuffer"
  UNBUF_P="unbuffer -p"
else 
  UNBUF=
  UNBUF_P=
fi

cd docs/
mkdir -p source/_static
$UNBUF make html-no-doxy 2>&1 | $UNBUF_P grep -v --color=always "WARNING: Duplicate declaration\|WARNING: Inline emphasis start-string without end-string.\|WARNING: Too many template argument lists compared to parameter lists\|^\s*libsemigroups\|Citation .* is not referenced"
echo "See: docs/build/html/index.html"

# Below is some old postprocessing of the doc
# Make sections bigger
# find docs/build/html/api/*.html -type f -exec \
#  perl -i -pe "s|<p class=\"breathe-sectiondef-title rubric\">(.+)</p>|<h1>\1</h1>|g" {} \;

# Remove superfluous template<> in aliase
# find docs/build/html/api/*.html -type f -exec gsed -i'' -e "s|template&lt;&gt;<br />||g" {} \;
# find docs/build/html/_generated/*.html -type f -exec gsed -i'' -e "s|libsemigroups::||g" {} \;

# Remove inheritance from detail classes
# find docs/build/html/api/todd-coxeter.html -type f -exec gsed -i'' -e "s/: <em class=\"property\">public<\/em> libsemigroups::detail::CongruenceInterface, <em class=\"property\">public<\/em> libsemigroups::detail::CosetManager//g" {} \;

# Template parameters on separate lines
# find docs/build/html/api/cong-pair.html -type f -exec gsed -i'' -r "s/typename <code class=\"sig-name descname\">\w+<\/code>,/&<br\/>/g" {} \;
#find docs/build/html/api/cong-pair.html -type f -exec \
#  perl -i -pe "s|(typename <code class=\"sig-name descname\">\w+</code> = .*?,)|\1<br/>|g" {} \;
#find docs/build/html/api/cong-pair.html -type f -exec gsed -i'' -r "s|<br/> typename|<br/> \&nbsp;\&nbsp;\&nbsp;\&nbsp; typename|g" {} \;

# Remove inheritance from detail classes
#find docs/build/html/api/cong-pair.html -type f -exec \
#  gsed -i'' -r "s|: <em class=\"property\">public</em> libsemigroups::detail::CongruenceInterface,<br/> <em class=\"property\">protected</em> libsemigroups::BruidhinnTraits\&lt;<a class=\"reference internal\" href=\"#_CPPv4I0000EN13libsemigroups10congruence17CongruenceByPairsE\" title=\"libsemigroups::CongruenceByPairs::TElementType\">TElementType</a>\&gt;||" {} \;

# Remove inheritance from detail classes in Congruence
# find docs/build/html/api/cong.html -type f -exec \
#  gsed -i'' -r "s|: <em class=\"property\">public</em> libsemigroups::detail::CongruenceInterface||" {} \;
