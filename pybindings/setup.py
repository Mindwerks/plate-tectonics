from setuptools import setup, Extension, Command
from setuptools.command.sdist import sdist
from setuptools.command.build_ext import build_ext
import os
import shutil

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

def prepare_cpp_sources():
  """Copy C++ sources from ../src to cpp_src if needed"""
  cpp_src_dir = "cpp_src"
  if os.path.exists("../src"):
    ensure_clean_dir(cpp_src_dir)
    copy_dir_contents("../src", cpp_src_dir)
  return cpp_src_dir

class CustomSdist(sdist):
  """Custom sdist that ensures cpp_src is populated before creating source dist"""
  def run(self):
    prepare_cpp_sources()
    sdist.run(self)

class CustomBuildExt(build_ext):
  """Custom build_ext that ensures cpp_src is populated before building"""
  def run(self):
    prepare_cpp_sources()
    build_ext.run(self)

# Prepare C++ sources
cpp_src_dir = prepare_cpp_sources()

# We add all .cpp files to the sources 
sources = [ 'platec_src/platecmodule.cpp']
for f in os.listdir(cpp_src_dir):
  if f.endswith(".cpp"):
    sources.append("%s/%s" % (cpp_src_dir, f))

import sys

# Platform-specific compiler flags
extra_compile_args = []
if sys.platform == 'win32':
    # MSVC uses /std:c++17
    extra_compile_args = ['/std:c++17']
else:
    # GCC/Clang use -std=c++17
    extra_compile_args = ['-std=c++17']

pyplatec = Extension(
    'platec',
    sources=sources,
    language='c++',
    include_dirs=[cpp_src_dir, 'platec_src'],
    extra_compile_args=extra_compile_args,
    extra_link_args=[]
)

setup (name = 'PyPlatec',
       version = '1.4.2',
       author = "Federico Tomassetti, Bret Curtis",
       author_email = 'f.tomassetti@gmail.com, psi29a@gmail.com',
       url = "https://github.com/Mindwerks/pyplatec",
       description = 'Plates simulation library',
       ext_modules = [pyplatec],
       include_package_data=True,
       include_dirs = [cpp_src_dir, 'platec_src'],
       python_requires='>=3.9',
       cmdclass={
           'sdist': CustomSdist,
           'build_ext': CustomBuildExt,
       },
       classifiers=[
        'Development Status :: 5 - Production/Stable',
        'Intended Audience :: Developers',
        'Natural Language :: English',
        'Operating System :: OS Independent',
        'Programming Language :: Python',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.9',
        'Programming Language :: Python :: 3.10',
        'Programming Language :: Python :: 3.11',
        'Programming Language :: Python :: 3.12',
        'Programming Language :: Python :: 3.13',
        'Programming Language :: Python :: 3.14',
        'Programming Language :: Python :: 3 :: Only',
        'Topic :: Software Development :: Libraries :: Python Modules',
      ]
)
