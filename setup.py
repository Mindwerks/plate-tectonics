from distutils.core import setup, Extension

extra_compile_args = "-std=c++11"

pyplatec = Extension('platec',                    
                    sources = [
                        'platec_src/platecmodule.cpp',
                        'platec_src/platecapi.cpp',
                        'platec_src/plate.cpp',
                        'platec_src/lithosphere.cpp',
                        'platec_src/heightmap.cpp',
                        'platec_src/rectangle.cpp',
                        'platec_src/simplexnoise.cpp',
                        'platec_src/sqrdmd.cpp'],
                     language='c++',
                     extra_compile_args=[extra_compile_args],
                    )

setup (name = 'PyPlatec',
       version = '1.2.5',
       author = "Federico Tomassetti",
       author_email = "f.tomassetti@gmail.com",
       url = "https://github.com/ftomassetti/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec])