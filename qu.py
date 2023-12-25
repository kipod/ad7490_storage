import enum
import struct
import time
from typing import Self

import redis
from pydantic import BaseModel, Field

from config import Settings, get_settings

SETTINGS: Settings = get_settings()


def timestamp() -> int:
    """returns timestamp in microseconds

    Returns:
        int: timestamp in microseconds
    """
    return int(time.time() * 1e6)


class QData(BaseModel):
    ts: int = Field(default_factory=timestamp)
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
            self.ts,
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

    @classmethod
    def unpack(cls, data: bytes) -> Self:
        timestamp, *values = struct.unpack("Q16H", data)
        return cls(
            ts=timestamp,
            **{f"value{n[0]+1}": n[1] for n in enumerate(values)},
        )


class Queue:
    class Status(enum.IntEnum):
        WRITE = 0
        WAIT = 1

    def __init__(self):
        self.r = redis.Redis(
            host=SETTINGS.REDIS_HOST,
            port=SETTINGS.REDIS_PORT,
            db=SETTINGS.REDIS_DB,
        )
        self.pipe = self.r.pipeline()
        self.pipe_size = 0
        self.r.config_set("maxmemory", "200MB")

    @property
    def size(self) -> int:
        return self._llen(SETTINGS.QUEUE_NAME)

    def _llen(self, name: str) -> int:
        try:
            return self.r.llen(name)  # type: ignore
        except redis.exceptions.ResponseError:
            return 0

    def _rpop(self, name: str, count: int=1) -> list[bytes]:
        try:
            return self.r.rpop(name, count)  # type: ignore
        except redis.exceptions.ResponseError:
            return []

    def _lrange(self, name: str, start: int, end: int) -> list[bytes]:
        try:
            return self.r.lrange(name, start, end)  # type: ignore
        except redis.exceptions.ResponseError:
            return []

    def push(self, data: QData):
        self.pipe.lpush(SETTINGS.QUEUE_NAME, data.pack())
        self.pipe_size+=1
        if self.pipe_size >= SETTINGS.PIPE_SIZE:
            self.pipe.execute()
            self.pipe_size = 0
            over = self.size - SETTINGS.MAX_QUEUE_SIZE
            if over > 0:
                self.r.rpop(SETTINGS.QUEUE_NAME, over)

    def pop(self, count: int = 1) -> list[QData]:
        count = min(count, self.size)
        data = self._rpop(SETTINGS.QUEUE_NAME, count)

        return [QData.unpack(d) for d in data]

    def range(self, start: int = 0, end: int = -1) -> list[QData]:
        if end == -1:
            end = self.size - 1

        return [QData.unpack(d) for d in self._lrange(SETTINGS.QUEUE_NAME, start, end)]

    def clear(self):
        self.r.delete(SETTINGS.QUEUE_NAME)

    def start(self):
        self.status = Queue.Status.WRITE

    def stop(self):
        self.status = Queue.Status.WAIT

    @property
    def is_active(self) -> bool:
        return self.status == Queue.Status.WRITE

    @property
    def status(self) -> Status:
        status_value = int(self.r.get(SETTINGS.STATUS_NAME))  # type: ignore
        if status_value is None:
            self.r.set(SETTINGS.STATUS_NAME, Queue.Status.WRITE.value)
            return Queue.Status.WRITE
        return Queue.Status(status_value)

    @status.setter
    def status(self, value: Status):
        self.r.set(SETTINGS.STATUS_NAME, value.value)

    def __len__(self) -> int:
        return self.size

    def __repr__(self) -> str:
        return f"Queue({self.size})"

    @property
    def speed(self) -> int:
        range = self.range(0, 10000)
        if not range:
            return 0
        seconds = (range[0].ts - range[-1].ts) / 1e6
        return int(len(range) / seconds)
