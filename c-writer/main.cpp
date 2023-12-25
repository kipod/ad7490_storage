#include <iostream>
#include <sw/redis++/redis++.h>
#include "qu.hpp"

using namespace sw::redis;

int main(int, char **)
{
    uint64_t ts = getTimestamp();
    std::cout << "Hello, from writer!" << std::endl;
    return 0;
}
