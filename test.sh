#!/usr/bin/env bash

if [ -n "$1" ]; then
    BUILD_DIR=$1
else
    BUILD_DIR="build"
fi

cd $(dirname $0)

function do_test {
    cmake -S . -B $BUILD_DIR -DCMAKE_C_COMPILER="$CC" -DCMAKE_C_FLAGS="$CFLAGS" &&
        cmake --build $BUILD_DIR &&
        ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random
}

export CC="clang"
export CFLAGS="-Oz -g3 \
    -Werror -Weverything \
    -Wno-declaration-after-statement \
    -Wno-extra-semi-stmt \
    -Wno-switch-enum \
    -Wno-gnu-zero-variadic-macro-arguments \
    -Wno-padded -Wno-missing-prototypes \
    -Wno-missing-field-initializers \
    -Wno-unreachable-code"
do_test || exit $?

export CC="gcc"
export CFLAGS="-Os -g3 -fkeep-inline-functions \
    -Wc++-compat -Werror -Wall -Wextra -Wconversion -fanalyzer"
do_test || exit $?
