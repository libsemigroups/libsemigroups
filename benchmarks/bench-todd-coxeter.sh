#!/bin/bash
set -e

for monoid in orientation_preserving_monoid \
              orientation_preserving_reversing_monoid \
              partition_monoid \
              dual_symmetric_inverse_monoid \
              uniform_block_bijection_monoid \
              temperley_lieb_monoid \
              singular_brauer_monoid \
              stylic_monoid \
              stellar_monoid; do
  ./bench_todd_coxeter "[$monoid]" --reporter=xml | tee $monoid.xml
done 
