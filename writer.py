import random

from qu import Queue, QData

queue = Queue()


def main():
    while queue.is_writing:
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
        print(qdata)
        queue.push(qdata)


if __name__ == "__main__":
    main()
