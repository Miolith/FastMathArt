import sys
from setuptools import setup, Extension

debug = '--debug' in sys.argv
if debug:
    sys.argv.remove('--debug')

module = Extension(
    'fastmathart.cbindings',
    sources=['fastmathart/cbindings.cpp', 'fastmathart/render.cpp'],
    language='c++',
    extra_compile_args=['-g', '-fno-optimize-sibling-calls', '-fsanitize=address'] if debug else ['-O3'],

)

setup(
    name='fastmathart',
    version='1.0',
    description='Fast math art library',
    ext_modules=[module],
)