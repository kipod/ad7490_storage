#include "qu.hpp"

uint64_t getTimestamp()
{
    // get timestamp in microseconds
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

Queue::Queue()
{
}