import serial
from invoke import task
from storage.config import get_settings

SETTINGS = get_settings()

BREAK_LINES = ("waiting for download",)


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
                line = line.strip()
                print(line)
                if any([break_line in line for break_line in BREAK_LINES]):
                    print("Got break line! Bye...")
                    break

    except KeyboardInterrupt:
        print("Bye...")
        ser.close()
