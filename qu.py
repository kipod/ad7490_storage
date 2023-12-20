from typing import Self, Generator
import time
import struct

import redis
from pydantic import BaseModel, Field

QUEUE_NAME = "queue"
MAX_QUEUE_SIZE = 10000


def timestamp() -> int:
    """returns timestamp in microseconds

    Returns:
        int: timestamp in microseconds
    """
    return int(time.time() * 1e6)


class QData(BaseModel):
    timestamp: int = Field(default_factory=timestamp)
    value1: int = 0
    value2: int = 1
    value3: int = 2
    value4: int = 3
    value5: int = 4
    value6: int = 5
    value7: int = 6
    value8: int = 7
    value9: int = 8
    value10: int = 9
    value11: int = 10
    value12: int = 11
    value13: int = 12
    value14: int = 13
    value15: int = 14
    value16: int = 15

    def pack(self) -> bytes:
        return struct.pack(
            "Q16H",
            self.timestamp,
            self.value1,
            self.value2,
            self.value3,
            self.value4,
            self.value5,
            self.value6,
            self.value7,
            self.value8,
            self.value9,
            self.value10,
            self.value11,
            self.value12,
            self.value13,
            self.value14,
            self.value15,
            self.value16,
        )

    @staticmethod
    def unpack(data: bytes) -> Self:
        timestamp, *values = struct.unpack("Q16H", data)
        return QData(
            timestamp=timestamp,
            **{f"value{n[0]+1}": n[1] for n in enumerate(values)},
        )


class Queue:
    def __init__(self, r: redis.Redis):
        self.r = r
        self.current_size = r.llen(QUEUE_NAME)

    @property
    def size(self):
        return self.r.llen(QUEUE_NAME)

    def push(self, data: QData):
        if self.current_size >= MAX_QUEUE_SIZE:
            # self.pop(1)
            # using more efficient python without calling functions
            self.r.rpop(QUEUE_NAME)  # every 1000 element pop 1000 elements
            self.current_size -= 1

        self.r.lpush(QUEUE_NAME, data.pack())
        self.current_size += 1
        print(f"Pushed {data} to queue")

    def pop(self, num_data: int = 1) -> list[QData]:
        data = self.r.rpop(QUEUE_NAME, num_data)
        self.current_size -= num_data
        return [QData.unpack(d) for d in data]

    def pop_gen(self, num_data: int = 1) -> Generator[QData, None, None]:
        if num_data == -1:
            num_data = self.current_size

        for _ in range(num_data):
            data = self.r.rpop(QUEUE_NAME)
            self.current_size -= 1
            yield QData.unpack(data)

    def range(self, start: int = 0, end: int = -1) -> list[int]:
        if end == -1:
            end = self.current_size - 1

        return self.r.lrange(QUEUE_NAME, start, end)

    def clear(self):
        self.r.delete(QUEUE_NAME)
        self.current_size = 0

    def __len__(self) -> int:
        return self.current_size

    def __repr__(self) -> str:
        return f"Queue({self.current_size})"
