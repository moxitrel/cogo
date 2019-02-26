#include <assert.h>
#include "../src/await.hpp"
#include <memory>
#include <iostream>

// Fibonacci number
class Fibonacci : public await_t {
    unsigned int n;                     // arg
    unsigned int v;                     // return value
    std::unique_ptr<Fibonacci> fib_n1;  // local variable, f(n-1)
    std::unique_ptr<Fibonacci> fib_n2;  // local variable, f(n-2)

    void operator()()
    {
        co_begin();

        if (n == 0) {           // f(0) = 0
            v = 0;
        } else if (n == 1) {    // f(1) = 1
            v = 1;
        } else {                // f(n) = f(n-1) + f(n-2)
            fib_n1 = std::make_unique<Fibonacci>(n - 1);
            fib_n2 = std::make_unique<Fibonacci>(n - 2);
            co_await(*fib_n1);  // wait f(n-1) to finish
            co_await(*fib_n2);  // wait f(n-2) to finish
            v = fib_n1->value() + fib_n2->value();
        }

        co_end();
    }

public:
    Fibonacci(unsigned int n)
        : n(n)
    {}

    // get return value
    unsigned int value()
    {
        return v;
    }
};

int main()
{
    Fibonacci f = Fibonacci(9);
    f.run();
    std::cout << f.value() << "\n";

    return 0;
}