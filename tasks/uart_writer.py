import os
import struct
import time
from datetime import datetime

import serial
from invoke import task

from config import get_settings
from qu import Queue, QData
from logger import log

SETTINGS = get_settings()

PACKET_SIZE = 36


def reset_cotroller():
    """Reset controller"""
    # set mode for reset pin
    os.system("gpio mode 0 out")
    # set mode for boot pin
    os.system("gpio mode 1 out")
    # press reset
    os.system("gpio write 0 0")
    time.sleep(1)
    # release reset
    os.system("gpio write 0 1")
    log(log.INFO, "reset done")


@task
def uart_writer(ctx):
    """Read data from serial port"""

    queue = Queue()

    # press reset
    os.system("gpio write 0 1")

    ser = serial.Serial(
        port=SETTINGS.TRANSMIT_SERIAL_PORT,
        baudrate=SETTINGS.TRANSMIT_SERIAL_BAUDRATE,
        # timeout=1,
    )

    DELTA_TIME = 5  # seconds
    prev_counter = 0
    start_counter = 0
    start = datetime.now()

    try:
        BUFF_SIZE = PACKET_SIZE * SETTINGS.RECEIVE_PACKET_NUM
        while True:
            data: bytes = ser.read(BUFF_SIZE)
            if not data:
                continue
            if len(data) != BUFF_SIZE:
                log(log.WARNING, "wrong data size: %d, reset...", len(data))
                reset_cotroller()
                continue
            for i in range(0, BUFF_SIZE, PACKET_SIZE):
                packet = data[i : i + PACKET_SIZE]

                counter, *values = struct.unpack("I16H", packet)
                if prev_counter and counter:
                    assert counter - prev_counter == 1, f"{counter} - {prev_counter}"
                else:
                    start_counter = counter
                prev_counter = counter

                qdata = QData(
                    value1=values[0],
                    value2=values[1],
                    value3=values[2],
                    value4=values[3],
                    value5=values[4],
                    value6=values[5],
                    value7=values[6],
                    value8=values[7],
                    value9=values[8],
                    value10=values[9],
                    value11=values[10],
                    value12=values[11],
                    value13=values[12],
                    value14=values[13],
                    value15=values[14],
                    value16=values[15],
                )
                queue.push(qdata)

            now = datetime.now()
            time_delta = now - start
            if time_delta.seconds >= DELTA_TIME:
                start = now
                log(log.INFO, "speed: %d", (counter - start_counter) / DELTA_TIME)
                start_counter = counter

    except KeyboardInterrupt:
        print("Bye...")
        ser.close()
