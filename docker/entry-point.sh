#!/bin/bash

git config --global --add safe.directory '*'
cmake -GNinja -S /in -B /out -DCMAKE_BUILD_TYPE=Release -DODOX_TESTS=ON
cmake --build /out --config Release
ctest
cpack
cp *.rpm /out
