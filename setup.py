from setuptools import setup, Extension

module = Extension(
    'fastmathart.cbindings',
    sources=['fastmathart/cbindings.cpp', 'fastmathart/render.cpp', 'fastmathart/utils/pixelUtils.cpp'],
    language='c++',
    extra_compile_args=['-std=c++2b'],
    extra_link_args=['-lfmt']
)

setup(
    name='fastmathart',
    version='1.0',
    description='Fast math art library',
    ext_modules=[module],
)

