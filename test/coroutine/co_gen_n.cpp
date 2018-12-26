#include "../../src/coroutine/coroutine.h"

/*
 *
 * co_wait(lock):
 *   lock.locked == true  => lock.add(this)
 *                            delete-from-concurrent-queue
 *                           co_return()
 *
 *
 *
 * */
// A natural number generator

struct co_lock_t {
    bool locked = false;
    co_t *head = nullptr;
    co_t *tail = nullptr;

    void lock()
    {
        locked = true
    }

    void unlock()
    {
        locked = false
    }

    bool is_locked()
    {
        return locked
    }

    void add(co_t &co)
    {
        if (tail == nullptr) {
            head = tail = &co;
        } else {
            tail->q_next = &co;
            tail = &co;
        }
    }

    void clear()
    {
        // restore to queue
    }
};

#include <stdio.h>
int main()
{
    Nat nat;
    nat(); // nat.value: 0
    nat(); // nat.value: 1
    nat(); // nat.value: 2

    printf("gen_t.size: %zd\n", sizeof(gen_t));
    printf("fun_t.size: %zd\n", sizeof(fun_t));
    printf("co_t.size: %zd\n", sizeof(co_t));
}