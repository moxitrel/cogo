#include "co.hpp"

class co_blocking_t {
    co_queue_t q;
    bool value = false;
public:
    void set()
    {
        value = true;
    }

    void unset()
    {
        value = false;
    }

    // used by co_blocking_wait()
    void _add(co_t &co)
    {
        q.push(co);
        co_t::tmp_blocking = &co;
    }
};

#define co_blocking_broadcast(BLOCKING)     \
    co_t::_q.join(BLOCKING);                \
    co_return()

// co_blocking_wait(co_blocking_t &)
#define co_blocking_wait(BLOCKING)          \
    (BLOCKING)._add(*this);                 \
    co_return()
