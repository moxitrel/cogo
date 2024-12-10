#!/usr/bin/env bash

ROOT=$(dirname "$0")/..

BUILD_DIR="$ROOT/build"
for cc in clang gcc; do
  for COGO_USE_COMPUTED_GOTO in ON OFF; do
    export CC="$cc"

    cmake -S $ROOT -B $BUILD_DIR -DCOGO_USE_COMPUTED_GOTO=$COGO_USE_COMPUTED_GOTO -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install -DCMAKE_EXPORT_PACKAGE_REGISTRY=ON --fresh &&
      cmake --build $BUILD_DIR --clean-first -j2 &&
      cmake --install $BUILD_DIR &&
      env -C $BUILD_DIR cpack -G ZIP ||
      exit $?
  done
done
