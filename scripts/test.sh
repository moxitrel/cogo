#!/usr/bin/env bash

ROOT=$(dirname "$0")/..

BUILD_DIR="$ROOT/build"
for cc in clang gcc; do
  for YIELD_TYPE in COGO_USE_SWITCH COGO_USE_LABELS_AS_VALUES; do
    export CC="$cc"

    cmake -S $ROOT -B $BUILD_DIR -D${YIELD_TYPE}=TRUE -DCMAKE_BUILD_TYPE=Debug --fresh &&
      cmake --build $BUILD_DIR --clean-first -j2 &&
      ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random -T MemCheck -j2 &&
      cmake --install $BUILD_DIR --prefix $BUILD_DIR/install &&
      env -C $BUILD_DIR cpack -G ZIP ||
      exit $?
  done
done
