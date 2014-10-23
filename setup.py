from distutils.core import setup, Extension

extra_compile_args = "-std=c99"

pyplatec = Extension('platec',                    
                    sources = ['platec_src/platecmodule.c',
                        'platec_src/platecapi.cpp',
                        'platec_src/plate.cpp',
                        'platec_src/lithosphere.cpp',
                        'platec_src/sqrdmd.c'],
                    extra_compile_args=[extra_compile_args],
                    )

setup (name = 'PyPlatec',
       version = '1.1',
       author = "Federico Tomassetti",
       author_email = "f.tomassetti@gmail.com",
       url = "https://github.com/ftomassetti/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec])