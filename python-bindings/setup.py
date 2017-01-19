from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

setup(
    name = "libsemigroups-python-bindings",
    ext_modules = cythonize([
        Extension("semigroups",
                      sources=["semigroups.pyx", "semigroups_cpp.cpp"],
                      libraries=["libsemigroups"],
                      language="c++",             # generate C++ code
                      extra_compile_args=["-std=c++11"],
                      )])
)
