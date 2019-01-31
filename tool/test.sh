#!/bin/sh

# 切换至 project root 目录
cd $(dirname $0)/..

mkdir build
cd build

cmake ..
make

ctest
