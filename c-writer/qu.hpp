#pragma once
#include <sw/redis++/redis++.h>
#include <string>

using namespace sw::redis;

const std::string REDIS_HOST = "localhost";
const int REDIS_PORT = 6379;
const std::string QUEUE_NAME = "queue";
const std::string STATUS_NAME = "status";
const int MAX_QUEUE_SIZE = 10000;
const int PIPE_SIZE = 10;

uint64_t getTimestamp();

#pragma pack(push, 1)
struct QData
{
    QData(
        uint16_t value1, uint16_t value2, uint16_t value3, uint16_t value4,
        uint16_t value5, uint16_t value6, uint16_t value7, uint16_t value8,
        uint16_t value9, uint16_t value10, uint16_t value11, uint16_t value12,
        uint16_t value13, uint16_t value14, uint16_t value15, uint16_t value16)
        : ts(getTimestamp()),
          value1(value1), value2(value2), value3(value3), value4(value4),
          value5(value5), value6(value6), value7(value7), value8(value8),
          value9(value9), value10(value10), value11(value11), value12(value12),
          value13(value13), value14(value14), value15(value15), value16(value16)
    {
    }
    QData() = default;
    // default copy constructor
    QData(const QData &) = default;
    // default move constructor
    QData(QData &&) = default;
    // default compare  operators
    bool operator==(const QData &rhs) const
    {
        return ts == rhs.ts &&
               value1 == rhs.value1 &&
               value2 == rhs.value2 &&
               value3 == rhs.value3 &&
               value4 == rhs.value4 &&
               value5 == rhs.value5 &&
               value6 == rhs.value6 &&
               value7 == rhs.value7 &&
               value8 == rhs.value8 &&
               value9 == rhs.value9 &&
               value10 == rhs.value10 &&
               value11 == rhs.value11 &&
               value12 == rhs.value12 &&
               value13 == rhs.value13 &&
               value14 == rhs.value14 &&
               value15 == rhs.value15 &&
               value16 == rhs.value16;
    }
    // timestamp in microseconds
    uint64_t ts;
    // 16 values
    uint16_t value1;
    uint16_t value2;
    uint16_t value3;
    uint16_t value4;
    uint16_t value5;
    uint16_t value6;
    uint16_t value7;
    uint16_t value8;
    uint16_t value9;
    uint16_t value10;
    uint16_t value11;
    uint16_t value12;
    uint16_t value13;
    uint16_t value14;
    uint16_t value15;
    uint16_t value16;

    StringView pack() const;
    void unpack(const StringView &sv);
};
#pragma pack(pop)

class Queue
{
private:
    Redis m_r;
    Pipeline m_pipe;
    int pipe_size;

    // int llen(const std::string &key);

public:
    Queue();
    ~Queue();

    int size();
    void push(QData &data, bool force = false);
    QData pop();
};
