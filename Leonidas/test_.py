from AD7490_ import ADC
import time

adc = ADC(4,1,1000000)

print("Reading channels with default settings:")
for i in range(16):
    x = adc.readChannel(i)
    print("Channel %d: %d"%(i, x))

print("Reading channels with range = 1:")
adc.setRange(1)
for i in range(16):
    x = adc.readChannel(i)
    print("Channel %d: %d"%(i, x))
adc.setRange(0)

print("Reading channels with coding = 0:")
adc.setCoding(0)
for i in range(16):
    x = adc.readChannel(i)
    print("Channel %d: %d"%(i, x))
adc.setCoding(1)

print("Reading channels with power mode = 01:")
adc.setPowerMode(1)
for i in range(16):
    x = adc.readChannel(i)
    print("Channel %d: %d"%(i, x))
adc.setPowerMode(3)

print("Reading channels with spi speed = 400000:")
adc.setSpiSpeed(400000)
for i in range(16):
    x = adc.readChannel(i)
    print("Channel %d: %d"%(i, x))
adc.setSpiSpeed(1000000)

print("Reading channels in infinite loop:")
while 1:
    time.sleep(1)
    for i, x in enumerate(adc.readAllChannels()):
        print("Channel %d: %d"%(i, x))
    print("use Ctrl+C to terminate")
