#!/bin/bash
set -e

echo -e "\nCloning HPCombi master branch . . ."
git clone -b master --depth=1 https://github.com/hivert/HPCombi.git extern/HPCombi
