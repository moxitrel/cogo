#!/usr/bin/env sh
#
# gtags 默认检索:
#   asm : .s , .S
#   c   : .c , .h
#   yacc: .y
#   c++ : .cc , .cpp , .cxx , .c++ , .C , .hh , .hpp , .hxx , .H
#   java: .java
#   php : .php , .php3 , .phtml
#


GTAGS_FILE=""                   # 默认 gtags.files: 需要索引的所有文件列表, 通常使用 find 来生产
DB_DIR=""                       # 索引文件 保存的目录，默认当前目录


# 切换至 src 目录 (只索引 src)
cd $(dirname $0)/..                     # project root
cd src

cmd="gtags "                            # 默认索引当前目录下的文件
cmd="$cmd ${DB_DIR:+ $DB_DIR} "         # 索引文件 保存的目录，默认当前目录
#cmd="$cmd --gtagslabel "                #
cmd="$cmd ${GTAGS_FILE:+ --file $GTAGS_FILE} " # -f,  索引列表文件, 默认 gtags.files; 换行 分隔,
#cmd="$cmd --accept-dotfiles "           # 包含以 . 开头的 文件和目录，默认忽略
#cmd="$cmd --incremental "               # -i, 追加索引，默认替换
#cmd="$cmd --compact "                   # -c, 压缩输出的 GTAGS 文件

cmd="$cmd --warning "                   # -w, 打印 警告信息
#cmd="$cmd --verbose "                   # -v, 打印 详细信息
#cmd="$cmd --statistics "                # 打印 统计信息

$gtags $*                                #添加命令行额外的选项参数
