import serial
from invoke import task
from config import get_settings

SETTINGS = get_settings()


@task
def uart_writer(ctx):
    """Read data from serial port"""

    ser = serial.Serial(
        SETTINGS.TRANSMIT_SERIAL_PORT,
        baudrate=SETTINGS.TRANSMIT_SERIAL_BAUDRATE,
        timeout=1,
    )
    try:
        while True:
            line = ser.readline().decode("utf-8")
            if line:
                line = line.strip()
                print(line)

    except KeyboardInterrupt:
        print("Bye...")
        ser.close()
