#include "uart.hpp"
// #include <wiringPi.h>
#include <wiringSerial.h>

#include "config.hpp"

Config config;
const size_t SEND_PACKETS = config.TRANSMIT_SEND_PACKETS;
const size_t SEND_BYTES = SEND_PACKETS * sizeof(Uart::uart_data_t);

Uart::Uart(const char *uart_device, uint32_t baudrate)
    : m_buffer(new uart_data_t[SEND_PACKETS]),
      m_results(SEND_PACKETS),
      m_fd(-1)
{
    if (uart_device == nullptr)
    {
        uart_device = config.TRANSMIT_SERIAL_PORT.c_str();
    }
    if (baudrate == 0)
    {
        baudrate = config.TRANSMIT_SERIAL_BAUDRATE;
    }
    m_fd = ::serialOpen(uart_device, baudrate);
}

Uart::~Uart()
{
    ::serialClose(m_fd);
}

const std::vector<QData> &Uart::read()
{
    auto available_bytes = serialDataAvail(m_fd);
    while (available_bytes < SEND_BYTES)
    {
        // wait for 10 usec
        ::usleep(10);
        available_bytes = serialDataAvail(m_fd);
    }
    auto read_size = ::read(m_fd, m_buffer.get(), SEND_BYTES);
    assert(read_size == SEND_BYTES);
    for (size_t i = 0; i < SEND_PACKETS; i++)
    {
        auto &packet = m_buffer[i].packet;
        if (m_counter == 0 || packet.counter == 0)
        {
            m_counter = packet.counter;
        }
        else
        {
            assert(++m_counter == packet.counter);
        }
        m_results[i] = QData(
            packet.channel_0,
            packet.channel_1,
            packet.channel_2,
            packet.channel_3,
            packet.channel_4,
            packet.channel_5,
            packet.channel_6,
            packet.channel_7,
            packet.channel_8,
            packet.channel_9,
            packet.channel_10,
            packet.channel_11,
            packet.channel_12,
            packet.channel_13,
            packet.channel_14,
            packet.channel_15);
    }

    return m_results;
}