import struct
import asyncio
from asyncio import StreamReader, StreamWriter
from datetime import datetime

from storage.qu import Queue, QData
from storage.config import Settings, get_settings
from storage.logger import log

SETTINGS: Settings = get_settings()
BUFF_SIZE = 36

queue = Queue()


async def handle_client(reader: StreamReader, writer: StreamWriter):
    log(log.INFO, "TCP Client connected")

    DELTA_TIME = 5  # seconds
    prev_counter = 0
    start_counter = 0
    start = datetime.now()
    while True:
        data_byte: bytes = await reader.read(BUFF_SIZE)
        if not data_byte:
            break
        assert len(data_byte) == BUFF_SIZE
        counter, *values = struct.unpack("I16H", data_byte)
        if prev_counter:
            assert counter - prev_counter == 1
        prev_counter = counter
        now = datetime.now()
        time_delta = now - start
        if time_delta.seconds >= DELTA_TIME:
            start = now
            log(log.INFO, "speed: %d", (counter - start_counter) / DELTA_TIME)
            start_counter = counter

        # write data to redis
        # log(log.INFO, "counter: %d", counter)
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
        # log(log.INFO, "data: %s", qdata)

        queue.push(qdata)

    log(log.INFO, "TCP Client disconnected")
    writer.close()


async def main():
    server = await asyncio.start_server(handle_client, "0.0.0.0", SETTINGS.TCP_SERVER_PORT)

    addr = server.sockets[0].getsockname()
    log(log.INFO, "Server started on [%s]", addr)

    async with server:
        await server.serve_forever()


if __name__ == "__main__":
    asyncio.run(main())
