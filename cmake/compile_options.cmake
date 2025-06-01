# Copyright (c) 2023-2024 Moxi Color
#
# Use of this source code is governed by a MIT-style license
# that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

set(ZY_COMPILE_OPTIONS_DIAGNOSTIC
  -Wall
  -Wextra

  # Common subset of C and C++
  $<$<COMPILE_LANGUAGE:C>:-Wc++-compat>

  # No language extensions
  -Wpedantic

  # implicit conversions may alter a value
  -Wconversion

  # implicit conversions may change the sign of an integer value
  -Wsign-conversion

  # implicit conversions may change the result of arithmetic
  -Warith-conversion

  # select unsigned->signed promotion but not unsigned->unsigned in overload resolution
  $<$<COMPILE_LANGUAGE:CXX>:-Wsign-promo>

  # remove qualifier (const T* -> T*) or add unsafe qualifier (T** -> const T**)
  -Wcast-qual

  # pointee alignment increased regardless target machine (char* -> int*)
  -Wcast-align=strict

  # 1: least accurate , all pointer conversions between possibly incompatible types, even never dereferenced
  # 2: not too precise, when address is taken
  # 3: (default) only when the converted pointer is dereferenced
  -Wstrict-aliasing=1

  # 1: (default) incompatibilities between the type of the alias declaration and that of its target
  # 2: aliased declaration is more restrictive than target
  -Wattribute-alias=2

  # a type with ABI tag used in context without it
  $<$<COMPILE_LANGUAGE:CXX>:-Wabi-tag>

  -Wformat=2

  # unmatched signedness ("%u",-1)
  -Wformat-signedness

  # check buffer overflow by assuming max length of numeric (sprintf)
  -Wformat-overflow=2

  # check output truncation (snprintf)
  -Wformat-truncation=2

  # check dst buffer overflow by assuming min size (strcpy, memcpy)
  -Wstringop-overflow=4

  # float == float
  -Wfloat-equal

  -Wlogical-op

  # left-shifting 1 into the sign bit (< C++14)
  -Wshift-overflow=2

  # ALLOC(0) is implementation-defined (malloc, realloc)
  -Walloc-zero

  # use indeterminate pointers in equality expressions
  -Wuse-after-free=3

  -Wunsafe-loop-optimizations

  # Warn omitted enumeration code even exists `default`.
  -Wswitch-enum

  # 2: .*falls?[ \t-]*thr(ough|u).*
  # 5: C++17: [[fallthrough]]; C++11: [[gnu::fallthrough]]
  -Wimplicit-fallthrough=2

  # hide virtual function
  $<$<COMPILE_LANGUAGE:CXX>:-Woverloaded-virtual>

  -Wshadow

  -Wbidi-chars=any,ucn -Wnormalized=nfkc

  # 1: array[], array[0], array[1]
  # 2: array[], array[0],
  # 3: array[] (default)
  # struct __attribute__((strict-flex-arrays(1))) T {};

  # may need -z execstack
  -Wtrampolines

  #
  # Optimization
  #
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Werror>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-pedantic-errors>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wundef> # C standard: undefined identifier is replaced with 0 in #if
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wdate-time> # __TIME__, __DATE__ or __TIMESTAMP__ lead to non-deterministic build
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wmissing-declarations>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wredundant-decls>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=noreturn>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=malloc> # malloc-like function
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=malloc> # malloc-like function
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=format> # printf-like function checking
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=format> # printf-like function checking
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=returns_nonnull>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=returns_nonnull>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=pure>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=pure>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=const>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=const>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wsuggest-attribute=cold>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=suggest-attribute=cold>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wduplicated-cond>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wuseless-cast>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wpacked> # packed attribute has no effect on the layout of the structure
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wpadded>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=padded>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wdouble-promotion> # float in hardware, double emulated in software
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=double-promotion> # float in hardware, double emulated in software
  -Wduplicated-branches -Wno-error=duplicated-branches
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wunused>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wunused-macros>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=unused-macros>
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wdisabled-optimization> # code is too big/complex to optimize
  $<$<CONFIG:Release,RelWithDebInfo,MinSizeRel>:-Wno-error=disabled-optimization> # code is too big/complex to optimize
  -Winvalid-pch -Wno-error=invalid-pch # precompiled header in search path cannot be used
  -Wmissing-include-dirs -Wno-error=missing-include-dirs # -I<dir> not exist

  # -Walloca-larger-than=byte-size     # alloca(n), n > byte-size
  # -Walloc-size-larger-than=byte-size # [[gnu::alloc_size(N)]]: arg[N] > byte-size; [[gnu::alloc_size(N,M)]]: arg[N] * arg[M] > byte-size
  # -Wlarger-than=$byteSize            # gcc: objectSize > $byteSize
  # -Wvla-larger-than=byte-size        # variable length array size > byte-size
  # -Wstack-usage=byte-size            # function stack size > byte-size

  #
  # C
  #
  $<$<COMPILE_LANGUAGE:C>:-Wbad-function-cast>
  $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes>
  $<$<COMPILE_LANGUAGE:C>:-Wold-style-definition>
  $<$<COMPILE_LANGUAGE:C>:-Wno-declaration-after-statement> # c99

  #
  # C++
  #
  $<$<COMPILE_LANGUAGE:CXX>:-Wmismatched-tags> # e.g. `struct` not match `class`
  $<$<COMPILE_LANGUAGE:CXX>:-Wextra-semi>
  $<$<COMPILE_LANGUAGE:CXX>:-Wredundant-tags>
  $<$<COMPILE_LANGUAGE:CXX>:-Wstrict-null-sentinel>
  $<$<COMPILE_LANGUAGE:CXX>:-Wzero-as-null-pointer-constant>
  $<$<COMPILE_LANGUAGE:CXX>:-Wold-style-cast>
  $<$<COMPILE_LANGUAGE:CXX>:-Weffc++>
  $<$<COMPILE_LANGUAGE:CXX>:-Wsuggest-override>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wnrvo>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wnoexcept>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wsuggest-final-types>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Wsuggest-final-methods>
  $<$<AND:$<COMPILE_LANGUAGE:CXX>,$<CONFIG:Release,RelWithDebInfo,MinSizeRel>>:-Winterference-size> # (ABI compatible) --param destructive-interference-size

  #
  # GCC
  #
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-fanalyzer> # static analysis, expensive than other warnings
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-fanalyzer-transitivity> # transitivity of constraints within the analyzer
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-Wtrivial-auto-var-init>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-fstrict-flex-arrays>
  $<$<OR:$<CXX_COMPILER_ID:GNU>,$<C_COMPILER_ID:GNU>>:-Wstrict-flex-arrays>

  #
  # Clang
  #
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:Clang>>:-Weverything>
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<C_COMPILER_ID:Clang>>:-Wno-unknown-warning-option>
)
