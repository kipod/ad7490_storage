#pragma once
#include <stdint.h>
#include <unistd.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>
#include "qdata.hpp"

#pragma pack(push, 1)
union CW_t
{
    struct AD7490_ControlWord
    {
        uint8_t pm : 2;
        uint8_t channel : 4;
        uint8_t sec : 1;
        uint8_t write : 1;
        uint8_t align : 4;
        uint8_t coding : 1;
        uint8_t range : 1;
        uint8_t weak_tri : 1;
        uint8_t shadow : 1;
    } cw;
    uint8_t bytes[2];
};

union ChannelRX_t
{
    struct AD7490_ChannelRX
    {
        uint16_t data : 12;
        uint8_t channel : 4;
    } rx;
    uint8_t bytes[2] = {0x00, 0x00};
};
#pragma pack(pop)

class AD7490
{
private:
    int m_fd;
    uint32_t m_spi_speed_hz;
    uint8_t m_bits = 16;                        // 8 bits per word
    uint32_t m_mode = SPI_MODE_0 & SPI_CS_WORD; // spi mode
    int m_power_mode;
    int m_dout;
    int m_rng;
    int m_coding;
    uint8_t m_cw_buf[34] = {0}; // control word buffer. 16 channels * 2 bytes per channel
    ChannelRX_t m_rx_data = {0};
    uint16_t m_values[16] = {0};
    uint8_t m_rx_buf[34] = {
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
        0xBB,
    }; // rx buffer for receive data from spi

    int spi_init(const char *spi_device);
    void init_cw_buf();
    void spi_transfer(const uint8_t *tx, const uint8_t *rx, uint32_t len = 2, uint16_t delay = 0);
    void spi_read_channels();

public:
    AD7490(const char *spi_device = "/dev/spidev4.1", uint32_t spi_speed_hz = 1000000, int power_mode = 3, int dout = 0, int rng = 0, int coding = 1);
    ~AD7490();
    QData read();
};
