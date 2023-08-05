#!/usr/bin/env bash

ROOT=$(dirname "$0")/..

BUILD_DIR="$ROOT/build"
for cc in clang gcc; do
  for YIELD_TYPE in COGO_USE_CASE COGO_USE_LABEL_VALUE; do
    export CC="$cc"

    cmake -S $ROOT -B $BUILD_DIR -D${YIELD_TYPE}=TRUE -DCMAKE_BUILD_TYPE=MinSizeRel --fresh &&
      cmake --build $BUILD_DIR --clean-first -j2 &&
      ctest --test-dir $BUILD_DIR --output-on-failure --schedule-random -j2 &&
      cmake --install $BUILD_DIR --prefix $BUILD_DIR/install &&
      env -C $BUILD_DIR cpack -G ZIP ||
      exit $?
  done
done
