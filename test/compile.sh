#!/bin/sh
export FLAGS="-Wall"
g++ $FLAGS -c ../src/heightmap.cpp
g++ $FLAGS -c ../src/plate.cpp
g++ $FLAGS -c ../src/platecapi.cpp
g++ $FLAGS -c ../src/lithosphere.cpp
gcc $FLAGS -std=c99 -c ../src/sqrdmd.c
g++ $FLAGS -c test_heightmap.cpp -I ../src
g++ $FLAGS -c test_plate.cpp -I ../src
g++ $FLAGS -c test_acceptance.cpp -I ../src
g++ sqrdmd.o lithosphere.o plate.o test_plate.o heightmap.o test_heightmap.o platecapi.o test_acceptance.o -l UnitTest++ -o tests_runner
