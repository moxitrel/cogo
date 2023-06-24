#!/usr/bin/env bash

if [ -n "$1" ]; then
    BUILD_DIR=$PWD/$1
else
    BUILD_DIR="build"
fi

cd $(dirname $0)

for CC in clang gcc; do
    for YIELD_TYPE in COGO_USE_CASE COGO_USE_LABEL_VALUE; do
        export CC="${CC}" CFLAGS="-Os -g3"
        cmake -S . -B $BUILD_DIR -D${YIELD_TYPE}=TRUE --fresh &&
        cmake --build $BUILD_DIR --clean-first -j &&
        ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random -j2 &&
        cmake --install $BUILD_DIR --prefix $BUILD_DIR/install &&
        env -C $BUILD_DIR cpack -G ZIP ||
        exit $?
    done
done
