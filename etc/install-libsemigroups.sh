#!/bin/bash
set -e

git clone https://github.com/libsemigroups/libsemigroups
cd libsemigroups/extern
curl -L -O https://github.com/fmtlib/fmt/releases/download/5.3.0/fmt-5.3.0.zip
tar -xzf fmt-5.3.0.zip && rm -f fmt-5.3.0.zip
curl -L -O https://github.com/hivert/HPCombi/archive/v0.0.3.zip
tar -xzf v0.0.3.zip && rm -f v0.0.3.zip && cd ..
