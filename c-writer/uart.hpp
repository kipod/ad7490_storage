#pragma once
#include <stdint.h>
#include <unistd.h>
#include <linux/types.h>

#include "qdata.hpp"

class Uart
{
public:
    Uart(const char *uart_device = nullptr, uint32_t baudrate = 0);
    ~Uart();

    const std::vector<QData> &read();

#pragma pack(push, 1)
    union uart_data_t
    {
        struct Packet
        {
            uint16_t counter;
            // 16 channels by 12 bits
            unsigned channel_0 : 12;
            unsigned channel_1 : 12;
            unsigned channel_2 : 12;
            unsigned channel_3 : 12;
            unsigned channel_4 : 12;
            unsigned channel_5 : 12;
            unsigned channel_6 : 12;
            unsigned channel_7 : 12;
            unsigned channel_8 : 12;
            unsigned channel_9 : 12;
            unsigned channel_10 : 12;
            unsigned channel_11 : 12;
            unsigned channel_12 : 12;
            unsigned channel_13 : 12;
            unsigned channel_14 : 12;
            unsigned channel_15 : 12;
        } packet;
        uint8_t bytes[sizeof(Packet)];
    };
#pragma pack(pop)

private:
    std::unique_ptr<uart_data_t[]> m_buffer;
    std::vector<QData> m_results;
    int m_fd;
    uint16_t m_counter = 0;
};