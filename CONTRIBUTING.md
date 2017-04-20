# Information for developers

## Debugging and valgrinding after switching to autohell

To run `lldb` after switching to autohell we have to do 

    libtool --mode=execute lldb lstest

because of the way the executable is linked. `lstest` is the name of the
check program produced by `make check`. Similarly to run valgrind you have
to do:

    libtool --mode=execute valgrind ... 

## Making a release

### Definitions

A ***bugfix release*** is one of the form `x.y.z -> x.y.z+1`, and                
a ***non-bugfix release*** is one of the form `x.y.z -> x+1.y.z` or `x.y+1.z`. 

###Complete the following steps to make a release

1. For a bugfix release be in the `stable-x.y` branch, and for a non-bugfix be in the `master` branch

2. Do the following just to be on the safe side:
 
        make distclean; ./autogen.sh; ./configure
    
    just to be on the safe side.

2. Make sure that `make check-standard` runs ok and that all changes are committed. 

3. Check code coverage by running:

        etc/coverage.sh
add tests to improve the coverage (and start again if necessary).
    
4. Resolve any warnings, or errors produced by (and start again if necessary):

        make format; make lint; make doc
    
5. Update the version numbers in `README.md`, and in `configure.ac`
   and run:
    
        make distclean ; ./autogen.sh ; ./configure
    
6. Commit the changed version numbers: `git commit -a -m "Update version numbers"` 

7. Push to `origin/stable-x.y` (bugfix release) or `origin/master` (non-bugfix release)

8. Run valgrind:

        etc/debug.sh ; make check; libtool --mode=execute valgrind --leak-check=full ./lstest -d yes [quick],[standard] 2>&1 | tee --append valgrind.txt

9. Wait for the continuous integration and valgrind to complete. If something goes wrong, then go back to the start of the process.
    
10. Tag the release:

        git tag vx.y.z ; git push origin --tags
    
11. Update gh-pages:

        make doc ; cp -r html/* gh-pages
    
12. Push to gh-pages

        cd gh-pages ; git commit -a -m "Version x.y.z" ; git push 

13. Make a release archive (note that `make distcheck` is currently done by travis so no need to do it again here):

        make dist 

14. Go to github and make a release announcement and add the archive produced at step 14

15. If doing a bugfix release then merge `stable-x.y` into `master`, and if doing a non-bugfix release make a new `stable-x.y` branch push it github and delete the old stable branch.


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

    ```conda build .```

- Try it with:

    conda install --use-local libsemigroups

- Publish it: See https://conda-forge.github.io/

References:

- [Conda's Tutorials on building packages](https://conda.io/docs/build_tutorials.html)
- A [repository of conda recipes for classical programs](https://github.com/conda/conda-recipes); nice source of inspiration
