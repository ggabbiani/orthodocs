#!/bin/bash
#
# Image builder entry-point for Fedora
#
# This file is part of the 'OrthoDocs' (ODOX) project.
#
# Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

set -e # exit immediately in case of error

if [[ "$#" -ne "3" ]]; then
  echo "expected: $0 «source directory» «build directory» «build type»"
  echo "got     : $0 $@"
  exit 1
fi

SRC_D=$1
BLD_D=$2
TYPE=$3

git config --global --add safe.directory '*'
cmake -GNinja -S $SRC_D -B $BLD_D -DCMAKE_BUILD_TYPE=$TYPE -DODOX_TESTS=ON
cmake --build $BLD_D --config $TYPE
cd $BLD_D
ctest
cpack
