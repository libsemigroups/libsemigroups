# Information for developers

## Debugging and valgrinding after switching to autohell

To run `lldb` after switching to autohell we have to do 

    ./configure --enable-debug ; make check-am 
    libtool --mode=execute lldb lstest

because of the way the executable is linked. `lstest` is the name of the
check program produced by `make check`. Similarly to run valgrind you have
to do:

    ./configure --enable-debug ; make check-am 
    libtool --mode=execute valgrind --leak-check=full ./lstest -d yes [quick],[standard] 2>&1 | tee --append valgrind.txt

## Adding new test cases

Any new tests should be tagged with one of the following:

Tag | Runtime 
---------|:----------
quick         | < 200ms
standard      | < 3s
extreme       | > 3s

## Making a release

### Definitions

A ***bugfix release*** is one of the form `x.y.z -> x.y.z+1`, and                
a ***non-bugfix release*** is one of the form `x.y.z -> x+1.y.z` or `x.y+1.z`. 

###Complete the following steps to make a release

1. For a bugfix release be in the `stable-x.y` branch, and for a non-bugfix be
   in the `master` branch

2. Update the version number in the VERSION file

3. Do the following just to be on the safe side:
 
        make distclean && ./autogen.sh && ./configure
    
    just to be on the safe side.

4. Make sure that `make check` runs ok and that all changes are committed. 

5. Check code coverage by running:

        make clean && ./configure --enable-code-coverage && make check-code-coverage -j4

    add tests to improve the coverage (and start again if necessary). This
    requires lcov, gcov, and genhtml. Under linux, you might need to do

        ./configure CXXFLAGS='-O0 -g --coverage' LDFLAGS='-O0 -g --coverage' --enable-code-coverage
        make check-code-coverage

6. Run the benchmarks and check there is no performance regression:

        git checkout <last_release_version>
        make uninstall ; make distclean
        benchmark/benchmark.sh

        git checkout <stable-x.y or master>
        make uninstall ; make distclean
        benchmark/benchmark.sh

        benchmark/compare_bench.py <json_file1> <json_file2> 
    
7. Update the version numbers in `README.md` and run:
    
        make distclean && ./autogen.sh && ./configure
    
8. Commit the changed version numbers: 
   `git commit -am "Update version numbers"` 

9. Push to `origin/stable-x.y` (bugfix release) or `origin/master` (non-bugfix
   release)

10. Wait for the continuous integration to complete. If something
   goes wrong, then go back to the start of the process.
    
11. Tag the release:

        git tag vx.y.z ; git push origin --tags
    
12. Update gh-pages:

        make doc ; cp -r html/* gh-pages
    
13. Push to gh-pages

        cd gh-pages ; git add * ; git commit -am "Version x.y.z" ; git push 

14. Make a release archive (note that `make distcheck` is currently done by
    travis so no need to do it again here):

        make dist 

15. Go to github and make a release announcement and add the archive produced
    at step 12

16. If doing a bugfix release then merge `stable-x.y` into `master`, and if
    doing a non-bugfix release make a new `stable-x.y` branch push it github
    and delete the old stable branch.

17. Update the conda-forge feedstock:

    https://github.com/conda-forge/libsemigroups-feedstock

    ***in your own fork***, by modifying `recipe/meta.yml` to increment the
    version number and sha256 variables at the top of the file. To obtain the
    sha256 value, using the archive from step 12 (uploaded to github), do `sha2
    libsemigroups-x.y.z.tar.gz` at the command line. 
    
    Make sure to pull from 

    https://github.com/conda-forge/libsemigroups-feedstock.git
  
    Then commit and push these changes to your fork, then make a PR to
    conda-forge. 

References:

- [Conda's Tutorials on building packages](https://conda.io/docs/build_tutorials.html)
- A [repository of conda recipes for classical programs](https://github.com/conda/conda-recipes); nice source of inspiration
