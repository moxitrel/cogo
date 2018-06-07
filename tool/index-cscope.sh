#!/usr/bin/env sh
#
### Install
#
#if ! which -s cscope; then
#    brew install cscope     #osx
#fi
#
###
# cscope 默认检索:
#   c   : .c , .h , #include files
#   lex : .l
#   yacc: .y
#
#   c++ : 通过 cscope.files 支持
#   java: 通过 cscope.files 支持
#


HEADER_DIR_S=("")           # 额外搜索的头文件目录列表
SRC_DIR=""                  # 额外搜索的源文件目录,    被忽略 若命令行已指定额外的文件
INDEX_FILE=""               # 默认 cscope.files: 需要索引的 所有文件列表, 通常使用 find 来生产
OUT_FILE=""                 # 默认 cscope.out


# 切换至 src 目录 (只索引 src)
cd $(dirname $0)/..         # project root
cd src

cmd="cscope "               # 默认索引 *当前目录*
cmd="$cmd -b "              # -b, 只生成索引文件，不进入cscope的界面
cmd="$cmd -R "              # -R, recurse into subdirectories
cmd="$cmd -q "              # -q, 反向索引，加快查找: $OUT_FILE.in, $OUT_FILE.po;  默认: cscope.in.out, cscope.po.out
cmd="$cmd ${HEADER_DIR_S:+  ${HEADER_DIR_S[*]/#/-I }} "     # -I, 额外在 $INCLUDE_DIR 目录下搜索 头文件，可多个-I叠加
cmd="$cmd ${SRC_DIR:+       -s $SRC_DIR} "                  # -s, 额外在 $SRC_DIR     目录下搜索 源文件，被忽略 若命令行已指定额外的文件
cmd="$cmd ${INDEX_FILE:+    -i $INDEX_FILE} "               # -i, 索引文件, 默认 cscope.files; 空格, tab 或 换行 分隔,
cmd="$cmd ${OUT_FILE:+      -f $OUT_FILE} "                 # -f, 指定 生成的索引文件名, 默认 cscope.out
#cmd="$cmd -k "              # -k 内核模式, 不索引 默认include目录(/usr/include)

$cmd $*                     # 添加命令行额外的选项参数
