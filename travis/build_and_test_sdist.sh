#!/bin/bash
sudo $PYCMD setup.py sdist
sudo $PIP install `ls dist/PyPlatec*.tar.gz`
