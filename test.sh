#!/usr/bin/env bash

if [ -n "$1" ]; then
    BUILD_DIR=$1
else
    BUILD_DIR="build"
fi

cd $(dirname $0)

function do_test {
    rm -f $BUILD_DIR/CMakeCache.txt &&
        cmake -S . -B $BUILD_DIR -DCOGO_USE_CASE=TRUE &&
        cmake --build $BUILD_DIR --clean-first &&
        ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random ||
        return $?

    rm -f $BUILD_DIR/CMakeCache.txt &&
        cmake -S . -B $BUILD_DIR -DCOGO_USE_LABEL_VALUE=TRUE &&
        cmake --build $BUILD_DIR --clean-first &&
        ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random ||
        return $?
}

export CC="clang"
export CFLAGS="-Oz -g3"
do_test || exit $?

export CC="gcc"
export CFLAGS="-Os -g3 -fkeep-inline-functions"
do_test || exit $?
