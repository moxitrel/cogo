#!/usr/bin/env bash

ROOT=$(dirname "$0")/..

BUILD_DIR="$ROOT/build"
for cc in clang gcc; do
  for COGO_NO_LABELS_AS_VALUES in ON OFF; do
    export CC="$cc"

    cmake -S $ROOT -B $BUILD_DIR -DCOGO_NO_LABELS_AS_VALUES=$COGO_NO_LABELS_AS_VALUES -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=$BUILD_DIR/install --fresh &&
      cmake --build $BUILD_DIR --clean-first -j2 &&
      ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random -T MemCheck -j2 &&
      cmake --install $BUILD_DIR &&
      env -C $BUILD_DIR cpack -G ZIP ||
      exit $?
  done
done
