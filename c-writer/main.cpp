#include <iostream>
#include <sw/redis++/redis++.h>
#include "qu.hpp"

using namespace sw::redis;

int main(int, char **)
{
    uint64_t ts = getTimestamp();
    // std::cout << "Hello, from writer!" << std::endl;
    Queue q;

    QData data(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    q.push(data, true);

    auto data2 = q.pop();

    assert(data == data2);

    return 0;
}
