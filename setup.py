#!/usr/bin/env python3

import codecs
import os
from distutils.core import setup, Extension

module1 = Extension('_sfp',
                    sources = [
                        'src/sfpmodule.cpp',
                        'src/libsfp/src/serial_framing_protocol.cpp',
                        ],
                    include_dirs = [
                        'src/libsfp/include',
                        'include',
                        'src/cxx-util/include',
                        ],
                    extra_compile_args = [
                        '-std=c++11',
                        '-g',
                        ],
                    )

here = os.path.abspath(os.path.dirname(__file__))
README = codecs.open(os.path.join(here, 'README'), encoding='utf8').read()
setup (name = 'PySfp',
       version = '0.1.4',
       author = 'David Ko',
       author_email = 'david@barobo.com',
       url = 'http://github.com/BaroboRobotics/libsfp',
       description = 'This is a Python binding for Barobo\'s Serial Framing Protocol.',
       long_description = README,
       ext_modules = [module1],
       ext_package = 'sfp',
       packages = ['sfp'],
       )
