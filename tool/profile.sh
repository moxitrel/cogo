#!/usr/bin/env sh

clang

  -ftrapv            #catch integer overflow errors. abort when it happens
  -fsanitize=address #memory error detector, slowdown 2x
  -fsanitize=memory  #detect uninitialized reads, slowdown 3x.


Valgrind: 内存, Any Language, compile with debug info, -fno-inline
fix errors in the order that they are reported
valgrind [options] cmd …
  --tool = memcheck #default
         | …
  --gen-suppressions = yes #print out a suppression for each reported error
  —trace-children=no #sub-processes initiated via the exec system call
  —log-fd=2
  —log-file=<filename>
  —log-socket=<ip[:1500]>

gperftools: from google
Compile with debugging enabled and link gperftools profiler.so

gprof:
gcc -pg
采样估算，只能分析用户时间, 没有内核时间