from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    name = "semigroupsplusplus-python-bindings",
    ext_modules = cythonize([
        Extension("semigroups",
                      sources=["semigroups.pyx"],
                      libraries=["semigroups"],
                      language="c++",             # generate C++ code
                      extra_compile_args=["-std=c++11"],
                      )])
)
