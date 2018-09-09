#!/usr/bin/env sh
#
# cmake
#   -G <generator-name>     #build system
#   -D <var>:<type>=<value> #
#   -D <var>=<value>        #


# clang | gcc
#
clang [options]  <file> …
  -E         #Preprocess
  -S         #assembly
  -c         #compile
  -shared    #生成动态库
  -o <file>  #save output to <file>

  -I <dir>      #search <dir> for include files
  -L <dir>      #add <dir> to ld search path, LIBRARY_PATH
  -rpath <path> #hardcode runtime search path into exe, LD_LIBRARY_PATH
  -l <lib>      #link <lib>

  -masm=<asm>   #assembly syntax: intel, att
  -g            #generate debug info

  -O         #= O2
  -O0        #no optimization
  -O1
  -O2        #enable most optimizations
  -O3
  -Og        #like -O1, improve debuggability
  -Os        #like -O2, reduce code size
  -Oz        #like -Os, reduce code size further
  -Ofast     #further than -O3, may violate language standard

  -f[no-]exceptions         # disable exceptions
  -f[no-]rtti               # disable RTTI

  -finline-hint-functions   #Inline functions marked inline
  -finline-functions        #Inline suitable functions
  -fno-inline-functions

pkg-config [opts] <lib> | <path-to-pc>  #PKG_CONFIG_PATH
  --cflags          #print flags required to compile
  --libs            #print flags required to link
