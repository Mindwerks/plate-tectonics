from setuptools import setup, Extension, Command
import os

sources = [ 'platec_src/platecmodule.cpp']
for f in os.listdir("src"):
  if f.endswith(".cpp"):
    sources.append("src/%s" % f)
print(sources)

pyplatec = Extension('platec',                    
                     sources = sources,
                     language='c++')

setup (name = 'PyPlatec',
       version = '1.3.1',
       author = "Federico Tomassetti",
       author_email = "f.tomassetti@gmail.com",
       url = "https://github.com/Mindwerks/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec],
       include_package_data=True,
       include_dirs = ['src', 'platec_src'],
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
        'Programming Language :: Python :: 3.2',
        'Programming Language :: Python :: 3.3',
        'Programming Language :: Python :: 3.4',
        'Topic :: Software Development :: Libraries :: Python Modules',
    ])