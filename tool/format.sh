#!/usr/bin/env bash
#
# Format Code: clang-format | astyle | indent
#
# astyle <file> …
#  --style=allman    #default
#         |kr
#  --dry-run         #without change
#  --recursive,-r,-R #process subdirectories recursively
#
cd $(dirname $0)/..                     # 切换至 project root 目录


STYLE="file"                            # coding style: file, LLVM, Google, Chromium, Mozilla, WebKit
                                        #   file: .clang-format in one of the parent directories of the source file directory


cd src

cmd="clang-format "                     #
cmd="$cmd ${STYLE:+ -style=$STYLE} "    # -style,          coding style
#cmd="$cmd -fallback-style=Google "      # -fallback-style, in case .clang-format not exist
cmd="$cmd -sort-includes "              # -sort-includes,  sort touched include lines
cmd="$cmd -i "                          # -i,              edit files in-place
cmd="$cmd -verbose "                    # -verbose,        shows the list of processed files

$cmd $*                     # 添加命令行额外的选项参数