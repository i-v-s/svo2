from setuptools import setup, Extension
from torch.utils import cpp_extension

setup(name='vilib',
      ext_modules=[cpp_extension.CppExtension('vilib_cpp', ['src/vilib.cpp'])],
      cmdclass={'build_ext': cpp_extension.BuildExtension})
