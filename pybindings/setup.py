from setuptools import setup, Extension, Command
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

pyplatec = Extension('platec',                    
                     sources = sources,
                     language='c++')

setup (name = 'PyPlatec',
       version = '1.3.1.post1',
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
      ]
)
