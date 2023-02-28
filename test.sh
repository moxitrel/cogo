#!/bin/sh

export CC="gcc"
export CFLAGS="-Wall -Wextra -Wc++-compat"

if [ -n "$1" ]
then
    BUILD_DIR=$1
else
    BUILD_DIR="build"
fi

cd `dirname $0`
cmake -S . -B $BUILD_DIR            \
&& cmake --build $BUILD_DIR         \
&& ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random
