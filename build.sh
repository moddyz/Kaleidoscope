#!/bin/bash

set -euxo pipefail

mkdir -p build && cd build
if [ $# -eq 0 ]
then
    echo "build.sh: Please specify installation path as the first argument."
    exit 1
else
    cmake -DCMAKE_INSTALL_PREFIX=$1 ..
fi
cmake --build . --target install
