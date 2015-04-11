from setuptools import setup, Extension, Command
import os


current_dir = os.path.dirname(__file__)
parent_dir = os.path.join(current_dir, os.pardir)
cpp_src_dir = os.path.abspath(os.path.join(parent_dir, "src"))

sources = [ 'platec_src/platecmodule.cpp']
for f in os.listdir(cpp_src_dir):
  if f.endswith(".cpp"):
    sources.append("%s/%s" % (cpp_src_dir, f))
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
       include_dirs = [cpp_src_dir, 'platec_src'],
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