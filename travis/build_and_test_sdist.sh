#!/bin/bash
sudo $PYCMD setup.py sdist
# we ensure PyPlatec is removed (if not installed it would fail but that is ok)
sudo $PIP uninstall PyPlatec -y -q || echo "ok"
sudo $PIP install `ls dist/PyPlatec*.tar.gz`
