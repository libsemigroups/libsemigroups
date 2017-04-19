# Information for the maintainer

## Updating the conda package

The relevant files are:
- [meta.yaml](meta.yaml)
- [build.sh](build.sh)

To update the package:
- Update the version and md5sum in [meta.yaml](meta.yaml)

- Install conda and go into your conda environment.
  See the getting started section in the
  [Conda documentation](https://conda.io/docs/index.html) for details.

- Run:

    conda build .

- Try it with:

    conda install --use-local libsemigroups

- Publish it: See https://conda-forge.github.io/

References:

- [Conda's Tutorials on building packages](https://conda.io/docs/build_tutorials.html)
- A [repository of conda recipes for classical programs](https://github.com/conda/conda-recipes); nice source of inspiration
