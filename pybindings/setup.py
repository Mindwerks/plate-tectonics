from setuptools import setup, Extension, Command
from distutils.command.build_ext import build_ext

import os
import shutil
import sys
import re

def ensure_clean_dir(f):
  if os.path.exists(f):
    shutil.rmtree(f)
  os.makedirs(f)

def copy_dir_contents(src, dst):
  src_files = os.listdir(src)
  for file_name in src_files:
      full_file_name = os.path.join(src, file_name)
      if (os.path.isfile(full_file_name)):
          shutil.copy(full_file_name, dst)

# If we are compiling from inside the tree we need to move the C++ source code
# to cpp_src, otherwise if building from a source directory the C++ source code
# should be already there
cpp_src_dir = "cpp_src"

if os.path.exists("../src"):
  ensure_clean_dir(cpp_src_dir)
  copy_dir_contents("../src", cpp_src_dir)

# We add all .cpp files to the sources 
sources = [ 'platec_src/platecmodule.cpp']
for f in os.listdir(cpp_src_dir):
  if f.endswith(".cpp"):
    sources.append("%s/%s" % (cpp_src_dir, f))


class build_ext_subclass( build_ext ):
    def build_extensions(self):
        compileArgs = ""
        regexClang = re.compile('clang*')
        regexGCC = re.compile('g*')
        regexMSVC = re.compile('ms*')
        if re.match(regexClang, self.compiler.compiler_type) is not None:
            for e in self.extensions:
                e.extra_compile_args = ['-stdlib=libc++', '-std=c++14']
        if re.match(regexGCC, self.compiler.compiler_type) is not None:
            for e in self.extensions:
                e.extra_compile_args = ['-std=c++14']
        if re.match(regexMSVC, self.compiler.compiler_type) is not None:
            for e in self.extensions:
                e.extra_compile_args = ['/std:c++14']
        self.extensions.extra_compile_args = compileArgs
        build_ext.build_extensions(self)


pyplatec = Extension('platec',                    
                     sources = sources,
                    language='c++')


setup (name = 'PyPlatec',
       version = '1.4.0',
       author = "Federico Tomassetti, Bret Curtis",
       author_email = 'f.tomassetti@gmail.com, psi29a@gmail.com',
       url = "https://github.com/Mindwerks/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec],
       include_package_data=True,
       include_dirs = [cpp_src_dir, 'platec_src'],
       cmdclass = {'build_ext': build_ext_subclass },
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
      ]
)
