// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://github.com/moxitrel/cogo/refs/heads/main/LICENSE

#ifndef COGO_PT_GOTO_H_
#define COGO_PT_GOTO_H_

#ifndef __GNUC__
  #error "Computed goto isn't supported."
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(COGO_DEBUG) && defined(assert)
  #define COGO_ASSERT(...) assert(__VA_ARGS__)
#else
  #define COGO_ASSERT(...)  // noop
#endif

typedef intptr_t cogo_pc_t;
#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)

typedef struct cogo_pt {
  cogo_pc_t pc;
} cogo_pt_t;

#define COGO_PT_V(PT) (PT)
#define COGO_PC(PT)   (COGO_PT_V(PT)->pc)

#define COGO_BEGIN(PT)                         \
  switch (COGO_PC(PT)) {                       \
    case COGO_PC_BEGIN:                        \
      COGO_ON_BEGIN(PT);                       \
      goto cogo_begin;                         \
      goto cogo_return;                        \
      COGO_PC(PT) = (cogo_pc_t)(&&cogo_begin); \
    case COGO_PC_END:                          \
      goto cogo_end;                           \
    default:                                   \
      goto*(void*)COGO_PC(PT);                 \
  }                                            \
  cogo_begin

#define COGO_DO_YIELD(PT)                    \
  do {                                       \
    COGO_PC(PT) = (cogo_pc_t)(&&COGO_LABEL); \
    goto cogo_end;                           \
  COGO_LABEL:;                               \
  } while (0)

#define COGO_YIELD(PT)  \
  do {                  \
    COGO_ON_YIELD(PT);  \
    COGO_DO_YIELD(PT);  \
    COGO_ON_RESUME(PT); \
  } while (0)

#define COGO_RETURN(PT) \
  do {                  \
    COGO_ON_RETURN(PT); \
    goto cogo_return;   \
  } while (0)

#define COGO_END(PT)         \
cogo_return:                 \
  COGO_ON_END(PT);           \
  COGO_PC(PT) = COGO_PC_END; \
  cogo_end

#define COGO_LABEL          COGO_DO1_LABEL(__LINE__)
#define COGO_DO1_LABEL(...) COGO_DO2_LABEL(__VA_ARGS__)
#define COGO_DO2_LABEL(N)   cogo_yield_##N

#ifndef COGO_ON_BEGIN
  #define COGO_ON_BEGIN(PT)
#endif

#ifndef COGO_ON_YIELD
  #define COGO_ON_YIELD(PT)
#endif

#ifndef COGO_ON_RESUME
  #define COGO_ON_RESUME(PT)
#endif

#ifndef COGO_ON_RETURN
  #define COGO_ON_RETURN(PT)
#endif

#ifndef COGO_ON_END
  #define COGO_ON_END(PT)
#endif

#ifndef COGO_T
  #define COGO_T cogo_pt_t
#endif

#define CO_BEGIN  COGO_BEGIN(cogo_this)
#define CO_END    COGO_END(cogo_this)
#define CO_YIELD  COGO_YIELD(cogo_this)
#define CO_RETURN COGO_RETURN(cogo_this)

#ifdef __cplusplus
}
#endif
#endif  // COGO_PT_GOTO_H_
