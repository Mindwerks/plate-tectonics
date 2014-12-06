#!/bin/sh
g++ -c ../src/plate.cpp
g++ -c test_plate.cpp -I ../src
g++ plate.o test_plate.o -l UnitTest++ -o tests_runner
