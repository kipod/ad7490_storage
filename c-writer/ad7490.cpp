#include "ad7490.hpp"
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <time.h>
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <sys/stat.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

const static uint8_t default_tx[2] = {0xFF, 0xFF};
static uint8_t default_rx[2] = {0xFF, 0xFF};

static void pabort(const char *s)
{
    if (errno != 0)
        perror(s);
    else
        printf("%s\n", s);

    abort();
}

AD7490::AD7490(const char *spi_device, uint32_t spi_speed_hz, int power_mode, int dout, int rng, int coding)
    : m_spi_speed_hz(spi_speed_hz), m_power_mode(power_mode), m_dout(dout), m_rng(rng), m_coding(coding)
{
    spi_init(spi_device);
}

AD7490::~AD7490()
{
    ::close(m_fd);
}

int AD7490::spi_init(const char *spi_device)
{
    m_fd = ::open(spi_device, O_RDWR);
    if (m_fd < 0)
        pabort("can't open device");

    /*
     * spi mode
     */
    /* WR is make a request to assign 'mode' */
    auto request = m_mode;
    int ret = ioctl(m_fd, SPI_IOC_WR_MODE32, &m_mode);
    if (ret == -1)
        pabort("can't set spi mode");

    /* RD is read what mode the device actually is in */
    ret = ioctl(m_fd, SPI_IOC_RD_MODE32, &m_mode);
    if (ret == -1)
        pabort("can't get spi mode");
    /* Drivers can reject some mode bits without returning an error.
     * Read the current value to identify what mode it is in, and if it
     * differs from the requested mode, warn the user.
     */
    if (request != m_mode)
        printf("WARNING device does not support requested mode 0x%x\n",
               request);

    ret = ioctl(m_fd, SPI_IOC_WR_BITS_PER_WORD, &m_bits);
    if (ret == -1)
        pabort("can't set bits per word");

    ret = ioctl(m_fd, SPI_IOC_RD_BITS_PER_WORD, &m_bits);
    if (ret == -1)
        pabort("can't get bits per word");

    ret = ioctl(m_fd, SPI_IOC_WR_MAX_SPEED_HZ, &m_spi_speed_hz);
    if (ret == -1)
        pabort("can't set max speed hz");

    ret = ioctl(m_fd, SPI_IOC_RD_MAX_SPEED_HZ, &m_spi_speed_hz);
    if (ret == -1)
        pabort("can't get max speed hz");

    printf("spi mode: 0x%x\n", m_mode);
    printf("bits per word: %u\n", m_bits);
    printf("max speed: %u Hz (%u kHz) (%u MHz)\n", m_spi_speed_hz, m_spi_speed_hz / 1000, m_spi_speed_hz / 1000000);

    // send 3 times 0xFFFF
    uint8_t default_rx[2] = {0x00, 0x00};
    for (int i = 0; i < 6; i += 2)
    {
        spi_transfer(default_tx, default_rx, 2);
    }

    return 0;
}

void AD7490::spi_transfer(const uint8_t *tx, const uint8_t *rx, uint32_t len)
{
    spi_ioc_transfer tr = {0};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = len;
    // tr.speed_hz = m_spi_speed_hz;
    tr.delay_usecs = 0;
    tr.bits_per_word = m_bits;

    // printf("tx: %02X %02X - ", tx[0], tx[1]);
    int ret = ::ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
    // printf("rx: %02X %02X\n", rx[0], rx[1]);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }
}

QData AD7490::read()
{
    CW_t cw;
    cw.cw.write = 1;
    cw.cw.sec = 0;
    cw.cw.channel = 0;
    cw.cw.pm = m_power_mode;
    cw.cw.shadow = 0;
    cw.cw.weak_tri = 0;
    cw.cw.range = m_rng;
    cw.cw.coding = m_coding;
    cw.cw.align = 0;
    cw.cw.pm = m_power_mode;

    ChannelRX_t rx_data;
    uint16_t values[16] = {0};

    // NOTE:
    // each time when we put control word for read some channel
    // we get data from previous channel
    // so we need to do 16+1 spi transfers

    for (int channel = 0; channel < 16; channel++)
    {
        cw.cw.channel = channel;
        // printf("control world: %02X%02X\n", cw.bytes[0], cw.bytes[1]);
        spi_transfer(cw.bytes, default_rx, 2);
        rx_data.bytes[0] = default_rx[1];
        rx_data.bytes[1] = default_rx[0];
        // printf("channel: %d, data: %d\n", rx_data.rx.channel, rx_data.rx.data);
        values[rx_data.rx.channel] = rx_data.rx.data;
    }
    // read data for last channel
    spi_transfer(default_tx, default_rx, 2);
    rx_data.bytes[0] = default_rx[1];
    rx_data.bytes[1] = default_rx[0];
    // printf("channel: %d, data: %d\n", rx_data.rx.channel, rx_data.rx.data);
    values[rx_data.rx.channel] = rx_data.rx.data;

    return QData(values[0], values[1], values[2], values[3],
                 values[4], values[5], values[6], values[7],
                 values[8], values[9], values[10], values[11],
                 values[12], values[13], values[14], values[15]);
}