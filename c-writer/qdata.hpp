#pragma once
#include <sw/redis++/redis++.h>

using namespace sw::redis;

uint64_t getTimestamp();

#pragma pack(push, 1)
struct QData
{
    QData(
        uint16_t value1 = 0, uint16_t value2 = 0, uint16_t value3 = 0, uint16_t value4 = 0,
        uint16_t value5 = 0, uint16_t value6 = 0, uint16_t value7 = 0, uint16_t value8 = 0,
        uint16_t value9 = 0, uint16_t value10 = 0, uint16_t value11 = 0, uint16_t value12 = 0,
        uint16_t value13 = 0, uint16_t value14 = 0, uint16_t value15 = 0, uint16_t value16 = 0)
        : ts(getTimestamp()),
          value1(value1), value2(value2), value3(value3), value4(value4),
          value5(value5), value6(value6), value7(value7), value8(value8),
          value9(value9), value10(value10), value11(value11), value12(value12),
          value13(value13), value14(value14), value15(value15), value16(value16)
    {
    }
    QData(const StringView &sv);
    // QData(const char *data);
    // default copy constructor
    QData(const QData &) = default;
    // default move constructor
    QData(QData &&) = default;
    // compare  operators
    bool operator==(const QData &rhs) const;
    // operator casting in bool
    operator bool() const
    {
        return ts > 0;
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
    void unpack(const char *data);
};
#pragma pack(pop)