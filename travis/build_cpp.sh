#!/bin/bash
$CXX --version
cmake . -DCMAKE_BUILD_TYPE=Release
make
cd test
make
./PlateTectonicsTests
