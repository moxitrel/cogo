#!/bin/sh
#
# cppcheck
#

STD="c++17"

# clang [options] filename a|
cmd=" clang "
cmd="$cmd ${STD:+ -std=$STD} "   # language standard
cmd=" $cmd -fsyntax-only "      # preprocess, parse and type checking

#cmd=" $cmd -Wpadded"            # warnings about structure padding

$cmd $*                         # 添加命令行额外的选项参数

#gcc   -Wall         #enable all warnings
