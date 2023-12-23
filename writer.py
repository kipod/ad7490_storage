import random
import time

from qu import Queue, QData
from logger import log

queue = Queue()


def main():
    run = False
    counter = 0
    while True:
        if queue.is_active:
            qdata = QData(
                value1=random.randint(0, 4095),
                value2=random.randint(0, 4095),
                value3=random.randint(0, 4095),
                value4=random.randint(0, 4095),
                value5=random.randint(0, 4095),
                value6=random.randint(0, 4095),
                value7=random.randint(0, 4095),
                value8=random.randint(0, 4095),
                value9=random.randint(0, 4095),
                value10=random.randint(0, 4095),
                value11=random.randint(0, 4095),
                value12=random.randint(0, 4095),
                value13=random.randint(0, 4095),
                value14=random.randint(0, 4095),
                value15=random.randint(0, 4095),
                value16=random.randint(0, 4095),
            )
            if not run:
                log(log.INFO, "GO!!!")
            run = True

            execute = counter % 10000 == 0
            queue.push(qdata, execute)
        else:
            if run:
                log(log.INFO, "Stop!!! Wait for queue to be active.")
            run = False
            print(".", end="")
            time.sleep(1)




if __name__ == "__main__":
    try:
        main()
    finally:
        queue.stop()
