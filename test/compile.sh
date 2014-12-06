#!/bin/sh
g++ -c ../src/heightmap.cpp
g++ -c ../src/plate.cpp
g++ -c test_heightmap.cpp -I ../src
g++ -c test_plate.cpp -I ../src
g++ plate.o test_plate.o heightmap.o test_heightmap.o -l UnitTest++ -o tests_runner
