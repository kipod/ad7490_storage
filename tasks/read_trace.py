import serial
from invoke import task
from config import get_settings

SETTINGS = get_settings()


@task
def read_trace(ctx):
    """Read trace from serial port"""

    ser = serial.Serial(
        SETTINGS.TRACE_SERIAL_PORT,
        baudrate=SETTINGS.TRACE_SERIAL_BAUDRATE,
        timeout=1,
    )
    try:
        while True:
            line = ser.readline().decode("utf-8")
            if line:
                print(line)

    except KeyboardInterrupt:
        print("Bye...")
        ser.close()
