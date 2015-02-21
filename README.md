plate-tectonics
===============
[![Build Status](https://travis-ci.org/Mindwerks/plate-tectonics.svg?branch=master)](https://travis-ci.org/Mindweks/plate-tectonics)

This is a library to simulate plate tectonics.

You can see a simulation using the library at work at: http://www.youtube.com/watch?v=bi4b45tMEPE#t=0

How to build plate-tectonics
============================

The project can be built using CMake (so you should install it, to build plate-tectonics).
It permits to define how to build the project in a platform independent
way. To build it on Linux you can run:

```
cmake .
make
```

This should produce a library (libPlateTectonics.a).

Not that this command will build the library in the same directory where the source files are hosted. Some prefer to build out of tree the library (i.e., in a separate dir). For example in this scenario:

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

To run also the examples you need to install the library DevIL.

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

Currently the test coverage is rather poor, tests are present only for new code and tiny portion of the old code that were refactored.

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

I am working on these bindings:
* Python [pyplatec](http://github.com/Mindwerks/pyplatec)
* Haskell [hplatec](http://github.com/ftomassetti/hplatec)

Projects using plate-tectonics
==============================

[WorldEngine](http://github.com/Mindwerks/worldengine), a world generator
