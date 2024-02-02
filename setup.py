from setuptools import setup, Extension

module = Extension(
    'fastmathart.cbindings',
    sources=['fastmathart/cbindings.cpp', 'fastmathart/render.cpp'],
    language='c++',
    extra_compile_args=['-std=c++2b'],
)

setup(
    name='fastmathart',
    version='1.0',
    description='Fast math art library',
    ext_modules=[module],
)

