#!/bin/bash
$PYCMD setup.py sdist
sudo $PIP install dist `ls dist/PyPlatec*.tar.gz`
