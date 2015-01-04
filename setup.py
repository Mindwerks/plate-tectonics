from distutils.core import setup, Extension

pyplatec = Extension('platec',                    
                    sources = ['platec_src/platecmodule.c',
                        'platec_src/platecapi.cpp',
                        'platec_src/plate.cpp',
                        'platec_src/lithosphere.cpp',
                        'platec_src/heightmap.cpp',
                        'platec_src/rectangle.cpp',
                        'platec_src/simplexnoise.cpp',
                        'platec_src/sqrdmd.cpp']
                    )

setup (name = 'PyPlatec',
       version = '1.2',
       author = "Federico Tomassetti",
       author_email = "f.tomassetti@gmail.com",
       url = "https://github.com/ftomassetti/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec])