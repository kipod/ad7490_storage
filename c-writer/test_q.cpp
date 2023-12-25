#include <iostream>
#include "qu.hpp"
#include "config.hpp"

const Config SETTINGS;

const size_t TEST_SIZE_RANGE = 100000;

void test_queue()
{
    uint64_t ts = getTimestamp();
    assert(ts > 0);
    assert(sizeof(QData) == 40);
    Queue q;

    q.clear();
    assert(q.size() == 0);

    QData data(
        1, 2, 3, 4,
        5, 6, 7, 8,
        9, 10, 11, 12,
        13, 14, 15, 16);

    q.push(data, true);

    auto data2 = q.pop();

    assert(data == data2);

    auto data3 = q.pop();
    assert(!data3);

    for (int i = 0; i < TEST_SIZE_RANGE; i++)
    {
        QData data(i);
        q.push(data);
    }

    auto range = q.range();
    auto q_size = std::min(TEST_SIZE_RANGE, SETTINGS.MAX_QUEUE_SIZE);
    assert(range.size() == q_size);

    auto speed = q.speed();
    std::cout << "queue speed: " << speed << std::endl;
}