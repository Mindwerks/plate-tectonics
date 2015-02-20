from setuptools import setup, Extension, Command

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
                        'platec_src/simplerandom.cpp',
                        'platec_src/sqrdmd.cpp',
                        'platec_src/utils.cpp',
                        'platec_src/noise.cpp'],
                     language='c++',
                     extra_compile_args=[extra_compile_args],
                    )

setup (name = 'PyPlatec',
       version = '1.2.9',
       author = "Federico Tomassetti",
       author_email = "f.tomassetti@gmail.com",
       url = "https://github.com/ftomassetti/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec],
      include_package_data=True,
       classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 2',
        'Programming Language :: Python :: 2.6',
        'Programming Language :: Python :: 2.7',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.3',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ])