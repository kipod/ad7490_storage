from qu import Queue


q = Queue()


def main():
    while q.is_writing:
        print(q.pop())


def sample():
    while q.is_writing:
        for qdata in q.pop_gen():
            print(qdata)


if __name__ == "__main__":
    main()
