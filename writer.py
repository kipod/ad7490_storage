import random

from qu import Queue, QData

queue = Queue()


def main():
    while queue.is_writing:
        qdata = QData(
            value1=random.randint(0, 65535),
            value2=random.randint(0, 65535),
            value3=random.randint(0, 65535),
            value4=random.randint(0, 65535),
            value5=random.randint(0, 65535),
            value6=random.randint(0, 65535),
            value7=random.randint(0, 65535),
            value8=random.randint(0, 65535),
            value9=random.randint(0, 65535),
            value10=random.randint(0, 65535),
            value11=random.randint(0, 65535),
            value12=random.randint(0, 65535),
            value13=random.randint(0, 65535),
            value14=random.randint(0, 65535),
            value15=random.randint(0, 65535),
            value16=random.randint(0, 65535),
        )
        print(qdata)
        queue.push(qdata)


if __name__ == "__main__":
    main()
