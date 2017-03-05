from distutils.core import setup, Extension
import sys

lib = 'rasch'
if sys.platform == 'win32':
    lib = 'librasch'

module1 = Extension('RASCH_if',
                    define_macros = [('MAJOR_VERSION', '0'),
                                     ('MINOR_VERSION', '8')],
                    include_dirs = ['../../include'],
                    libraries = [lib],
                    library_dirs = ['../../'],
                    sources = ['ra_python.c'])

setup (name = 'RASCH4Python',
       version = '0.8.12',
       description = 'Interface to use libRASCH with Python',
       author = 'Raphael Schneider',
       author_email = 'librasch@gmail.org',
       url = 'http://www.librasch.org',
       long_description = '''
       This package provide an interface to access
       libRASCH from Python.
       ''',
       ext_modules = [module1],
       py_modules = ['RASCH'])

