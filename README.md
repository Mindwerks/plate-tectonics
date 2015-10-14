Plate-tectonics
===============
Travis
[![Build Status](https://travis-ci.org/Mindwerks/plate-tectonics.svg?branch=master)](https://travis-ci.org/Mindweks/plate-tectonics)
AppVeyor
[![Build status](https://ci.appveyor.com/api/projects/status/siss20palcy6rbif?svg=true)](https://ci.appveyor.com/project/ftomassetti/plate-tectonics)


This is a library to simulate plate tectonics.
It is written in C++ and it has Python bindings (as part of this project), as well as Haskell bindings ([hplatec](http://github.com/ftomassetti/hplatec)).

How can I use it?
=================

Being a library you want probably to use it inside some larger program. From example [WorldEngine](https://github.com/Mindwerks/worldengine) (a world generator) is based on plate-tectonics.

You can also use the examples to just run the code of this library and generate a few maps. However the examples do not unleash the full power of this library. For running the examples check section _Running the examples (C++)_.

How it looks like
=================

The library offers an API to generate heightmaps and some other data about the world resulting from the simulation. The example permits also to generate maps like this one:

![](https://raw.githubusercontent.com/Mindwerks/plate-tectonics/master/screenshots/map_grayscale.png)

![](https://raw.githubusercontent.com/Mindwerks/plate-tectonics/master/screenshots/map_colors.png)

You can see a video of simulation based on an old version of this library: http://www.youtube.com/watch?v=bi4b45tMEPE#t=0

How to build plate-tectonics (C++)
==================================

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

Running the examples (C++)
==========================

To run also the examples you need to install the library libpng.

From the root directory run:

```bash
cmake -DWITH_EXAMPLE=TRUE -G "Unix Makefiles"
make
cd examples
./simulation_rect
```

How to run tests (C++)
======================

To run tests you need to install DevIL

We use googletest (which is included in the project). After building the library:

```
cd test
make
./PlateTectonicsTest
```

Currently the test coverage is still poor (but improving!_, tests are present only for new code and tiny portion of the old code that were refactored.

## Python bindings

Supported versions:
* Python 2.6 (currently not supported on AppVeyor)
* Python 2.7
* Python 3.2 (currently not supported on AppVeyor)
* Python 3.3
* Python 3.4

To use it in your program you can either add a dependency use pip (_pip install PyPlatec_) or build and install it from source code.

### Compile (Python)

```
python setup.py build
```

### Usage (Python)

The library is quite simple:

```python    
    import platec

    p = platec.create(seed=3)
    while platec.is_finished(p) == 0:
        platec.step(p)
    hm = platec.get_heightmap(p)
    platec.destroy(p)
```    


Or if you want more control:

```python
    import platec
    
    p = platec.create(seed=3, width=1000, height=800,
                      sea_level=0.65,erosion_period=60,
                      folding_ratio=0.02,aggr_overlap_abs=1000000,
                      aggr_overlap_rel=0.33,cycle_count=2,num_plates=10)
```

Plans for the future
====================

* Improve the quality of the code and add some tests
* Support Google protocol buffer

Projects using plate-tectonics
==============================

[WorldEngine](http://github.com/Mindwerks/worldengine), a world generator

Original project
================

A fork of platec http://sourceforge.net/projects/platec/ .
That project is part of a Bachelor of Engineering thesis in Metropolia University of Applied Sciences, Helsinki, Finland. The thesis is freely downloadable from http://urn.fi/URN:NBN:fi:amk-201204023993 .

Kudos to the original author: Lauri Viitanen!

