#!/bin/sh

# Project root
cd $(dirname $0)/.. || exit

find include src -type f \( \
  -iname '*.c' -or \
  -iname '*.h' \) \
  -exec \
  clang-format -style=file -fallback-style=Google -i -verbose '{}' \;
