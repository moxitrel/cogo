#!/usr/bin/env bash

ROOT=$(dirname "$0")/..
BUILD_DIR="$ROOT/build"

for cc in clang gcc; do
  export CC="$cc"

  cmake -S $ROOT -B $BUILD_DIR \
    --fresh \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install \
    -DCMAKE_EXPORT_PACKAGE_REGISTRY=ON \
    $* &&
    cmake --build $BUILD_DIR --clean-first -j2 &&
    cmake --install $BUILD_DIR &&
    env -C $BUILD_DIR cpack -G ZIP ||
    exit $?
done
