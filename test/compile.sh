#!/bin/sh
g++ -c ../src/heightmap.cpp
g++ -c ../src/plate.cpp
g++ -c ../src/platecapi.cpp
g++ -c ../src/lithosphere.cpp
gcc -std=c99 -c ../src/sqrdmd.c
g++ -c test_heightmap.cpp -I ../src
g++ -c test_plate.cpp -I ../src
g++ -c test_acceptance.cpp -I ../src
g++ sqrdmd.o lithosphere.o plate.o test_plate.o heightmap.o test_heightmap.o platecapi.o test_acceptance.o -l UnitTest++ -o tests_runner
