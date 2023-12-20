import time

import redis

from qu import Queue


r = redis.Redis(host="redis", port=6379, db=0)


queue = Queue(r)

while True:
    time.sleep(0.0001)
    print(queue.range(0, 0))
    # print(queue.pop())
