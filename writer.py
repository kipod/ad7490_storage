import random
import time

import redis

from qu import Queue, MAX_QUEUE_SIZE

r = redis.Redis(host="redis", port=6379, db=0)
queue = Queue(r)


def main():
    while True:
        time.sleep(0.01)
        fake_data = [
            f"{random.randint(0, 100)};{random.randint(0, 100)}"
            for _ in range(MAX_QUEUE_SIZE * 2)
        ]

        for vector in fake_data:
            number1, number2 = map(int, vector.split(";"))
            queue.push(number1)
            queue.push(number2)


if __name__ == "__main__":
    main()
