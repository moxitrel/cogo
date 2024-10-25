// Copyright (c) 2018-2024 Moxi Color
//
// Use of this source code is governed by a MIT-style license
// that can be found in the LICENSE file or at https://opensource.org/licenses/MIT

#ifndef COGO_YIELD_GOTO_H_
#define COGO_YIELD_GOTO_H_

#ifndef COGO_ON_BEGIN
#define COGO_ON_BEGIN(THIS)
#endif

#ifndef COGO_ON_YIELD
#define COGO_ON_YIELD(THIS)
#endif

#ifndef COGO_ON_RESUME
#define COGO_ON_RESUME(THIS)
#endif

#ifndef COGO_ON_RETURN
#define COGO_ON_RETURN(THIS)
#endif

#ifndef COGO_ON_END
#define COGO_ON_END(THIS)
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef intptr_t cogo_pc_t;

#define COGO_T cogo_yield_t
typedef struct cogo_yield {
  cogo_pc_t protected_pc;
} cogo_yield_t;

#define COGO_PC_BEGIN 0
#define COGO_PC_END   (-1)
#define COGO_PC(THIS) ((THIS)->protected_pc)

#define COGO_BEGIN(THIS)                                                                          \
  switch (COGO_PC(THIS)) {                                                                        \
    case COGO_PC_BEGIN:                                                                           \
      COGO_ON_BEGIN((&*(THIS)));                                                                  \
      goto cogo_begin;                                                                            \
      /* eliminate warning of unused label */                                                     \
      goto cogo_return;                                                                           \
      /* eliminate clang error: indirect goto in function with no address-of-label expressions */ \
      COGO_PC(THIS) = (cogo_pc_t)(&&cogo_begin);                                                  \
    case COGO_PC_END:                                                                             \
      goto cogo_end;                                                                              \
    default:                                                                                      \
      goto*(void*)COGO_PC(THIS);                                                                  \
  }                                                                                               \
  cogo_begin

#define COGO_YIELD(THIS)                       \
  do {                                         \
    COGO_ON_YIELD((&*(THIS)));                 \
    COGO_PC(THIS) = (cogo_pc_t)(&&COGO_LABEL); \
    goto cogo_end;                             \
  COGO_LABEL:                                  \
    COGO_ON_RESUME((&*(THIS)));                \
  } while (0)

#define COGO_RETURN(THIS)       \
  do {                          \
    COGO_ON_RETURN((&*(THIS))); \
    goto cogo_return;           \
  } while (0)

#define COGO_END(THIS)         \
  cogo_return:                 \
  COGO_ON_END((&*(THIS)));     \
  COGO_PC(THIS) = COGO_PC_END; \
  cogo_end

#define COGO_LABEL       COGO_LABEL1(__LINE__)
#define COGO_LABEL1(...) COGO_LABEL2(__VA_ARGS__)
#define COGO_LABEL2(N)   cogo_yield_##N

#define COGO_THIS        cogo_this
#define CO_BEGIN         COGO_BEGIN(COGO_THIS)
#define CO_END           COGO_END(COGO_THIS)
#define CO_YIELD         COGO_YIELD(COGO_THIS)
#define CO_RETURN        COGO_RETURN(COGO_THIS)

#ifdef __cplusplus
}
#endif
#endif  // COGO_YIELD_GOTO_H_
