import time
from ADC import ADC

from qu import Queue, QData
from logger import log
from config import Settings, get_settings

SETTINGS: Settings = get_settings()

queue = Queue()
adc = ADC(4,1,SETTINGS.SPI_SPEED)

def main():
    run = False
    counter = 0
    while True:
        if queue.is_active:
            if not run:
                log(log.INFO, "GO!!!")
            run = True
            for _ in range(SETTINGS.WRITE_BATCH_SIZE):
                values = adc.readAllChannels()
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
                queue.push(qdata)
                counter += 1
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
