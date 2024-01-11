import serial
import pynmea2

ser = serial.Serial("/dev/ttyS0", baudrate=9600, timeout=1)

try:
    while True:
        data = ser.readline().decode("utf-8")

        if data.startswith("$GPGGA"):
            msg = pynmea2.parse(data)
            latitude = "{:.6f}".format(msg.latitude)
            longitude = "{:.6f}".format(msg.longitude)

            print(f"{latitude}, {longitude}")

except KeyboardInterrupt:
    ser.close()
