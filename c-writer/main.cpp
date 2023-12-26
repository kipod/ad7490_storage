#include <iostream>
#include <sw/redis++/redis++.h>
#include <thread>
#include <signal.h>
#include "qu.hpp"
#include "test_q.hpp"
#include "config.hpp"
#include "ad7490.hpp"

const Config SETTINGS;

using namespace sw::redis;

Queue q;
size_t counter = 0;

void sig_handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    q.stop();
    std::cout << "Stop queue" << std::endl;
    std::cout << "Wrote " << counter << " records" << std::endl;
    exit(1);
}

int main(int, char **)
{
    // test_queue();

    AD7490 ad7490("/dev/spidev4.1", 10000000);

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);

    std::cout << "start c-writer" << std::endl;
    bool run = false;
    try
    {
        for (;;)
        {
            if (q.isActive())
            {
                if (!run)
                {
                    std::cout << "GO!!" << std::endl;
                    run = true;
                }
                for (int i = 0; i < SETTINGS.WRITE_BATCH_SIZE; i++)
                {
                    // QData data(counter++);

                    QData data = ad7490.read();
                    // if (counter == 0)
                    // {
                    //     printf("----------------------------------------------------------------------------------------\n");
                    //     printf("|  No  | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 |\n");
                    //     printf("----------------------------------------------------------------------------------------\n");
                    // }
                    // printf("|%06d|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|\n",
                    //        counter,
                    //        data.value1, data.value2, data.value3, data.value4,
                    //        data.value5, data.value6, data.value7, data.value8,
                    //        data.value9, data.value10, data.value11, data.value12,
                    //        data.value13, data.value14, data.value15, data.value16);
                    // printf("----------------------------------------------------------------------------------------\n");
                    q.push(data);

                    counter++;

                    // if (counter >= 20000)
                    // {
                    //     q.stop();
                    //     exit(1);
                    // }
                }
            }
            else
            {
                if (run)
                {
                    std::cout << "Stop!!! Wait for queue to be active" << std::endl;
                    run = false;
                }
                // sleep 1 sec
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception" << std::endl;
    }
    q.stop();
    return 0;
}
