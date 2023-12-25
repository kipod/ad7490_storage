#include "qdata.hpp"

uint64_t getTimestamp()
{
    // get timestamp in microseconds
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

QData::QData(const StringView &sv)
{
    unpack(sv);
}

// QData::QData(const char *data)
// {
//     unpack(data);
// }

bool QData::operator==(const QData &rhs) const
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

StringView QData::pack() const
{
    auto data = reinterpret_cast<const char *>(this);
    StringView ret = StringView(data, sizeof(*this));
    return ret;
}

void QData::unpack(const StringView &sv)
{
    ::memcpy(reinterpret_cast<char *>(this), sv.data(), sizeof(*this));
}

// void QData::unpack(const char *data)
// {
//     unpack(StringView(data, sizeof(*this)));
// }
