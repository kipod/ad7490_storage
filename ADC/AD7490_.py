import time
import spidev

MICROSECOND = 1/1000000.0

class ADC():
    def __init__(self, spi_bus, spi_device, spi_speed_hz=1000000, power_mode=3, dout=0, rng=0, coding=1):
        self.spi = spidev.SpiDev()
        # Connects to the specified SPI device, opening /dev/spidev<bus>.<device>
        self.spi.open(spi_bus, spi_device)
        self.spi.max_speed_hz = spi_speed_hz

        self.power_mode = -1
        self.dout = dout & 0b1       # set DOUT to three state at end of transfer
        self.rng = rng & 0b1         # set range to double Vref
        self.coding = coding & 0b1   # set coding to straight binary
        self.setPowerMode(power_mode & 0b11) # set power mode to normal

    def __decodeReadResult(self, val):
        if len(val) != 2:
            return -1, None
        return val[0] >> 4, ((val[0] & 0b1111) << 8) | val[1]

    def __encodeControlBytes(self, channel):
        byte1 = 0b10000000 # write = 1
        byte1 |=  ((channel & 0b1111) << 2) | (self.power_mode)
        byte2 = (self.dout << 6) | (self.rng << 5) | (self.coding << 4)
        return byte1, byte2

    def dummyReads(self, cnt):
        for i in range(cnt):
            self.readChannel(i & 0b1111)

    def readAllChannels(self) -> list[int]:
        actual_power_mode = self.power_mode
        self.setPowerMode(3)
        try:
            res = [self.readChannel(x) for x in range(16)]
        finally:
            self.setPowerMode(actual_power_mode)
        return res

    def readChannel(self, channel) -> int:
        actual_power_mode = self.power_mode
        self.setPowerMode(3)
        try:
            self.spi.writebytes(self.__encodeControlBytes(channel))
            chn, meas = self.__decodeReadResult(self.spi.readbytes(2))
        finally:
            self.setPowerMode(actual_power_mode)
        if chn != channel:
            raise Exception("Something strange happened, expected channel %d, got %d" % (channel, chn))
        return meas

    def setPowerMode(self, val):
        val &= 0b11
        if self.power_mode == val:
            return
        self.power_mode = val
        self.spi.writebytes(self.__encodeControlBytes(0))
        if self.power_mode == 3:
            # wake up from sleep or something
            time.sleep(MICROSECOND)
            self.dummyReads(2)

    def setDOUT(self, val):
        self.dout = val & 0b1

    def setRange(self, val):
        self.rng = val & 0b1

    def setCoding(self, val):
        self.coding = val & 0b1

    def setSpiSpeed(self, val):
        self.spi.max_speed_hz = val
