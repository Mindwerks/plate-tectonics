#!/bin/bash
$PYCMD --version
sudo $PIP install -r dev-requirements.txt
sudo $PYCMD setup.py clean build sdist develop
sudo $PYCMD setup.py nosetests 
