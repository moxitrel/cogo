#include "_list.h"
#include <setjmp.h>

#define ID(X) X;
#define CO_FUNC(F, ...)       \
    typedef struct {          \
        int     yield;        \
        jmp_buf jmp_buf;      \
                              \
        MAP(ID, __VA_ARGS__); \
    } F##_ctx_t;              \
                              \
    void F(ctx_t *ctx)

#define CO_ARG(F, X) &((F##_ctx_t *)ctx)->X

CO_FUNC(f, int v)
{
    int *v = CO_ARG(f, v);

    // CO_BEGIN()
    static f_ctx_t *ctx_;
    ctx_ = (f_ctx_t *)ctx;

    if (ctx->yield > 0) {
        longjmp(ctx_->jmp, 1);
    } else if (ctx->yield < 0) {
        return;
    }

    *CO_ARG(f, v) = 0;
    // YIELD()
    if (setjmp(*ctx_->jmp) == 0) {
        return;
    }

    *CO_ARG(f, v) = 1;
    *CO_ARG(f, v) = 2;
}
