#ifndef COGO_AWAIT_H
#define COGO_AWAIT_H

#include "gen.hpp"

class await_t;
class sch_await_t;

// await_t: add call stack support, behave like function
//  .state() -> int      : return the current running state.
//  .run  ()             : keep running until finish.
class await_t : public gen_t {
    friend sch_await_t;

    // the coroutine function
    virtual void operator()() = 0;

    // the lower call stack frame
    await_t *caller = nullptr;

    // scheduler
    sch_await_t *sch;   // inited by co_await() or .run()

protected:
    // co_await()
    void _await(await_t &callee);

public:
    virtual ~await_t() = 0;

    // run until finish
    void run();
};

// scheduler for await_t
class sch_await_t {
    friend await_t;

    // the coroutine run by scheduler
    await_t *stack_top; // inited by .run()

protected:
    // run until yield
    void step();

public:
    // run a coroutine until finish
    static void run(await_t &co);
    static void run(await_t &&co);
};


//
// await_t
//
inline void await_t::_await(await_t &callee)
{
    assert(sch);

    // stack push
    callee.caller = this;
    callee.sch = sch;
    callee.sch->stack_top = &callee;
}
// await_t::co_await(await_t &co);
// Call another coroutine.
#define co_await(CO)                        \
do {                                        \
    _await(CO);                             \
    co_yield();                             \
} while (0)

inline void await_t::run()
{
    sch_await_t::run(*this);
}



//
// sch_await_t
//
inline void sch_await_t::step()
{
    if (stack_top->state() < 0) {
        // stack pop
        stack_top = stack_top->caller;
    } else {
        stack_top->operator()();
    }
}

inline void sch_await_t::run(await_t &co)
{
    sch_await_t sch = {&co,};
    co.sch = &sch;

    for (;sch.stack_top;) {
        sch.step();
    }
}

inline void sch_await_t::run(await_t &&co)
{
    sch_await_t::run(co);
}

#endif // COGO_AWAIT_H
