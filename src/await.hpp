/*

* API
- co_await(await_t &)   :: call another coroutine.

- .run()                :: run the coroutine until finish.

*/
#ifndef COGO_AWAIT_H
#define COGO_AWAIT_H

#include "gen.hpp"

class await_t;
class await_sch_t;

// await_t: add call stack support, behave like function
//  .run(): keep running until finish.
class await_t : public gen_t {
    // the coroutine function
    virtual void operator()() = 0;

    // the lower call stack frame
    await_t *caller = nullptr;

    // scheduler, inited by co_await() or .run()
    await_sch_t *sch;

    friend await_sch_t;
protected:
    // co_await()
    void _await(await_t &callee) noexcept;
    void _await(await_t *callee) noexcept;

public:
    virtual ~await_t() = default;

    // run until finish
    void run();
};

// scheduler for await_t
class await_sch_t {
    // call stack top, the coroutine run by scheduler
    await_t *stack_top; // inited by .run()

    friend await_t;
protected:
    // run until yield
    void step();
};


inline void await_t::_await(await_t &callee) noexcept
{
    assert(this->sch);

    // call stack push
    callee.caller = this;
    callee.sch = this->sch;
    callee.sch->stack_top = &callee;    // set new stack top
}

inline void await_t::_await(await_t *callee) noexcept
{
    assert(callee);
    _await(*callee);
}

// await_t::co_await(await_t &co): call another coroutine.
#define co_await(CO)                        \
do {                                        \
    _await(CO);                             \
    co_yield();                             \
} while (0)


inline void await_sch_t::step()
{
    if (stack_top->state() < 0) {
        // call stack pop
        stack_top = stack_top->caller;
    } else {
        // run
        stack_top->operator()();
    }
}

inline void await_t::run()
{
    await_sch_t scheduler;

    scheduler.stack_top = this;
    sch = &scheduler;

    while (scheduler.stack_top) {
        scheduler.step();
    }
}

#endif // COGO_AWAIT_H
