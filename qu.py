import redis

QUEUE_NAME = "queue"
MAX_QUEUE_SIZE = 10000


class Queue:
    def __init__(self, r: redis.Redis):
        self.r = r
        self.current_size = r.llen(QUEUE_NAME)

    @property
    def size(self):
        return self.r.llen(QUEUE_NAME)

    def push(self, number: int):
        if type(number) is not int:
            raise TypeError("number must be integer")

        if self.current_size >= MAX_QUEUE_SIZE:
            # self.pop(1)
            # using more efficient python without calling functions
            self.r.rpop(QUEUE_NAME)  # every 1000 element pop 1000 elements
            self.current_size -= 1

        self.r.lpush(QUEUE_NAME, number)
        self.current_size += 1
        print(f"Pushed {number} to queue")

    def pop(self, num_data: int = 1) -> list[int]:
        data = self.r.rpop(QUEUE_NAME, num_data)
        self.current_size -= len(data)

        return data

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
