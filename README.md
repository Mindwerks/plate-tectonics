plate-tectonics
===============
Travis
[![Build Status](https://travis-ci.org/Mindwerks/plate-tectonics.svg?branch=master)](https://travis-ci.org/Mindweks/plate-tectonics)
AppVeyor
[![Build status](https://ci.appveyor.com/api/projects/status/siss20palcy6rbif?svg=true)](https://ci.appveyor.com/project/ftomassetti/plate-tectonics)


This is a library to simulate plate tectonics.

How it looks like
=================

The library offers an API to generate heightmaps and some other data about the world resulting from the simulation. The example permits also to generate maps like this one:

![](https://raw.githubusercontent.com/Mindwerks/plate-tectonics/master/screenshots/map_grayscale.png)

![](https://raw.githubusercontent.com/Mindwerks/plate-tectonics/master/screenshots/map_colors.png)

You can see a video of simulation based on an old version of this library: http://www.youtube.com/watch?v=bi4b45tMEPE#t=0

How to build plate-tectonics
============================

We use [CMake](http://www.cmake.org/). Install it and then run the folowing commands

### Linux

```
cmake . -G "Unix Makefiles"
make
```

### Mac OS-X

```
cmake .
make
```

This should produce a library (libPlateTectonics.a).

### Windows

```
cmake .
cmake --build .
```

If you want to build also the examples run:

```
# instead of cmake .
cmake . -DWITH_EXAMPLES=ON
```

Note that this command will build the library in the same directory where the source files are hosted. Some prefer to build out of tree the library (i.e., in a separate dir). For example in this scenario:

```
-- plate-tectonics
\- build-directory
```

You can enter build-directory and type:

```
cmake ../plate-tectonics
make
```

To compile on other platforms please run:

```
cmake --help
```

Running the examples
====================

To run also the examples you need to install the library libpng.

From the root directory run:

```bash
cmake -DWITH_EXAMPLE=TRUE -G "Unix Makefiles"
make
cd examples
./simulation_rect
```

How to run tests
================

To run tests you need to install DevIL

We use googletest (which is included in the project). After building the library:

```
cd test
make
./PlateTectonicsTest
```

Currently the test coverage is still poor (but improving!_, tests are present only for new code and tiny portion of the old code that were refactored.

Original project
================

A fork of platec http://sourceforge.net/projects/platec/ .
That project is part of a Bachelor of Engineering thesis in Metropolia University of Applied Sciences, Helsinki, Finland. The thesis is freely downloadable from http://urn.fi/URN:NBN:fi:amk-201204023993 .

Kudos to the original author: Lauri Viitanen!

License
=======

This work is (as the original work) released under the GNU LGPL.

Goals
=====

* Improve the quality of the code and add some tests
* Support Google protocol buffer

Bindings
========

We are working on these bindings:
* Python [pyplatec](http://github.com/Mindwerks/pyplatec)
* Haskell [hplatec](http://github.com/ftomassetti/hplatec)

Projects using plate-tectonics
==============================

[WorldEngine](http://github.com/Mindwerks/worldengine), a world generator

PyPlatec
========

_Travis_
[![Build Status](https://travis-ci.org/Mindwerks/pyplatec.svg?branch=master)](https://travis-ci.org/Mindwerks/pyplatec) _AppVeyor_ [![Build status](https://ci.appveyor.com/api/projects/status/3qh8txg5bhf41fxv?svg=true)](https://ci.appveyor.com/project/ftomassetti/pyplatec)


Plate simulation for python (wrapper of an existing C/C++ library).

Take a look at the simulation at work: http://www.youtube.com/watch?v=bi4b45tMEPE#t=0

The original project is here: http://sourceforge.net/projects/platec/ . That project is part of a Bachelor of Engineering thesis in Metropolia University of Applied Sciences, Helsinki, Finland. The thesis is freely downloadable from http://urn.fi/URN:NBN:fi:amk-201204023993 .

Kudos to the original author: Lauri Viitanen!

Requirements
============

Supported versions:
* Python 2.6 (currently not supported on AppVeyor)
* Python 2.7
* Python 3.2 (currently not supported on AppVeyor)
* Python 3.3
* Python 3.4

Clone
=====

This project contains git submodules. After cloning it do:

```
git submodule update --init --recursive
cd plate-tectonics
git checkout v1.3.1
```

Compile
=======

```
python setup.py build
```

Usage
=====

The library is quite simple:

    p = platec.create(seed=3)
    while platec.is_finished(p)==0:
        platec.step(p)
    hm = platec.get_heightmap(p)
    platec.destroy(p)


Or if you want more control:

    p = platec.create(seed=3, width=1000, height=800,
                      sea_level=0.65,erosion_period=60,
                      folding_ratio=0.02,aggr_overlap_abs=1000000,
                      aggr_overlap_rel=0.33,cycle_count=2,num_plates=10)

Enjoy!

Projects using it
=================

I created this wrapper for using this fantastic piece of code in [worldengine](http://github.com/Mindwerks/worldengine)

Supporting windows wheels
=========================

Through AppVeyor we build the binary distributions for windows (several python versions, both win32 and win64).
However they need to be downloaded from AppVeyor and uploaded to Pypi manually.
