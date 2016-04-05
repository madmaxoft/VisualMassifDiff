#!/usr/bin/env bash
echo "Building..."

set -e

echo "Running qmake..."
qmake "QMAKE_CXX=g++-5" "QMAKE_LINK=g++-5"

echo "Running make..."
make -j 3
