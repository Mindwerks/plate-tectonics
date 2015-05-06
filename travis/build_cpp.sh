#!/bin/bash
$CXX --version
cmake . -G 'Unix Makefiles'
make
cd test
make
./PlateTectonicsTests
