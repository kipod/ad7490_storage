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

void pabort(const char *s)
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
    init_cw_buf();
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

void AD7490::init_cw_buf()
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

    for (int i = 0; i < 34; i += 2)
    {
        cw.cw.channel = (i / 2) & 0x0F;
        m_cw_buf[i] = cw.bytes[0];
        m_cw_buf[i + 1] = cw.bytes[1];
    }
}

void AD7490::spi_transfer(const uint8_t *tx, const uint8_t *rx, uint32_t len, uint16_t delay)
{
    spi_ioc_transfer tr = {0};
    tr.tx_buf = (unsigned long)tx;
    tr.rx_buf = (unsigned long)rx;
    tr.len = len;
    // tr.speed_hz = m_spi_speed_hz;
    tr.delay_usecs = delay;
    tr.bits_per_word = m_bits;
    tr.rx_nbits = 0;
    tr.tx_nbits = 0;

    // printf("tx: %02X %02X - ", tx[0], tx[1]);
    int ret = ::ioctl(m_fd, SPI_IOC_MESSAGE(1), &tr);
    // printf("rx: %02X %02X\n", rx[0], rx[1]);
    if (ret < 1)
    {
        pabort("can't send spi message");
    }
}

void AD7490::spi_read_channels()
{
    spi_ioc_transfer trs[17] = {0};
    uint8_t *cw_buf = m_cw_buf;
    uint8_t *rx_buf = m_rx_buf;
    for (spi_ioc_transfer &tr : trs)
    {
        tr.tx_buf = (unsigned long)cw_buf;
        tr.rx_buf = (unsigned long)rx_buf;
        tr.len = 2;
        // tr.speed_hz = m_spi_speed_hz;
        tr.delay_usecs = 0;
        tr.bits_per_word = m_bits;
        tr.cs_change = 1;
        tr.rx_nbits = 0;
        tr.tx_nbits = 0;
        tr.word_delay_usecs = 0;

        cw_buf += sizeof(CW_t);
        rx_buf += sizeof(ChannelRX_t);
    }

    // printf("tx:");
    // for (spi_ioc_transfer &tr : trs)
    // {
    //     printf(" %02x:%02X%02X", ((uint8_t *)tr.tx_buf) - m_cw_buf, ((uint8_t *)tr.tx_buf)[0], ((uint8_t *)tr.tx_buf)[1]);
    // }
    // printf("\n");
    // printf("rx:");
    // for (spi_ioc_transfer &tr : trs)
    // {
    //     printf(" %02x:%02X%02X", ((uint8_t *)tr.rx_buf) - m_rx_buf, ((uint8_t *)tr.rx_buf)[0], ((uint8_t *)tr.rx_buf)[1]);
    // }
    // printf("\n");

    int ret = ::ioctl(m_fd, SPI_IOC_MESSAGE(17), trs);
}

QData AD7490::read()
{

    // NOTE:
    // each time when we put control word for read some channel
    // we get data from previous channel
    // so we need to do 16+1 spi transfers

    // printf("control words: ");
    // for (int i = 0; i < 32; i++)
    //     printf(" %02X", m_cw_buf[i]);
    // printf("\n");
    // printf("rx_buf:        ");
    // for (int i = 0; i < 32; i++)
    //     printf(" %02X", m_rx_buf[i]);
    // printf("\n");

    spi_read_channels();

    // printf("control words: ");
    // for (int i = 0; i < 34; i++)
    //     printf(" %02X", m_cw_buf[i]);
    // printf("\n");
    // printf("rx_buf: ");
    // for (int i = 0; i < 34; i++)
    //     printf(" %02X", m_rx_buf[i]);
    // printf("\n");

    auto rx_buf = m_rx_buf;
    auto cw_buf = m_cw_buf;

    int channel = 0;
    bool got_cannel[16] = {false};
    for (int i = 0; i < 17; i++)
    {
        // printf("channel: %d. rx_offset: %d cw_offset: %d\n", channel, rx_buf - m_rx_buf, cw_buf - m_cw_buf);
        // printf("control world: %02X%02X\n", cw_buf[0], cw_buf[1]);
        if (m_bits == 8)
        {
            m_rx_data.bytes[0] = rx_buf[1];
            m_rx_data.bytes[1] = rx_buf[0];
        }
        else
        {
            ::memcpy(m_rx_data.bytes, rx_buf, 2);
        }
        // printf("channel: %d(%d), data: %d\n", m_rx_data.rx.channel, channel, m_rx_data.rx.data);

        got_cannel[m_rx_data.rx.channel] = true;
        m_values[m_rx_data.rx.channel] = m_rx_data.rx.data;

        rx_buf += sizeof(ChannelRX_t);
        cw_buf += sizeof(CW_t);
        channel++;
    }

    for (int i = 0; i < 16; i++)
    {
        if (!got_cannel[i])
        {
            printf("!!! >did not read data for channel %d\n", i);
            pabort("unexpected not got channel");
        }
    }

    return QData(m_values[0], m_values[1], m_values[2], m_values[3],
                 m_values[4], m_values[5], m_values[6], m_values[7],
                 m_values[8], m_values[9], m_values[10], m_values[11],
                 m_values[12], m_values[13], m_values[14], m_values[15]);
}