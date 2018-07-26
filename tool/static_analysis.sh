#!/usr/bin/env sh
#
# cppcheck
#
clang
 -fsyntax-only #preprocess, parse and type checking
 -Wpadded      #warnings about structure padding



gcc   -Wall         #enable all warnings