#!/usr/bin/env python3

from distutils.core import setup, Extension

module1 = Extension('sfp',
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
                        ],
                    )

setup (name = 'PySfp',
       version = '1.0',
       description = 'This is a Python binding for Barobo\'s Serial Framing Protocol.',
       ext_modules = [module1])
