#include <iostream>
#include <sw/redis++/redis++.h>
#include <thread>
#include <signal.h>
#include "qu.hpp"
#include "test_q.hpp"
#include "config.hpp"
#include "ad7490.hpp"
#include <pthread.h>

const Config SETTINGS;

using namespace sw::redis;

Queue q;

void sig_handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    q.stop();
    std::cout << "Stop queue" << std::endl;
    std::cout << "Wrote " << counter << " records" << std::endl;
    exit(1);
}

void up_priority()
{
    pthread_t thId = pthread_self();
    pthread_attr_t thAttr;
    int policy = 0;
    int max_prio_for_policy = 0;

    pthread_attr_init(&thAttr);
    pthread_attr_getschedpolicy(&thAttr, &policy);
    max_prio_for_policy = sched_get_priority_max(policy);

    pthread_setschedprio(thId, max_prio_for_policy);
    pthread_attr_destroy(&thAttr);
}

int main(int, char **)
{
    // test_queue();
    up_priority();

    AD7490 ad7490("/dev/spidev4.1", SETTINGS.SPI_SPEED);
    // std::list<QData> data_range;

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);
    sigaction(SIGABRT, &sigIntHandler, nullptr);

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
                    counter++;

                    // QData data(counter);

                    QData data = ad7490.read();

                    q.push(data);
                    // data_range.push_front(data);

                    // if (counter >= 10000)
                    // {
                    //     // q.stop();
                    //     // auto range = q.range(0, -1);
                    //     auto range = data_range;
                    //     // counter = 0;
                    //     // for (std::list<QData>::const_iterator it = range.begin(); it != range.end(); ++it)
                    //     // {
                    //     //     const QData &data = *it;
                    //     //     if (counter == 0)
                    //     //     {
                    //     //         printf("----------------------------------------------------------------------------------------\n");
                    //     //         printf("|  No  | 00 | 01 | 02 | 03 | 04 | 05 | 06 | 07 | 08 | 09 | 10 | 11 | 12 | 13 | 14 | 15 |\n");
                    //     //         printf("----------------------------------------------------------------------------------------\n");
                    //     //     }
                    //     //     printf("|%06d|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|%04X|\n",
                    //     //            counter,
                    //     //            data.value1, data.value2, data.value3, data.value4,
                    //     //            data.value5, data.value6, data.value7, data.value8,
                    //     //            data.value9, data.value10, data.value11, data.value12,
                    //     //            data.value13, data.value14, data.value15, data.value16);
                    //     //     printf("----------------------------------------------------------------------------------------\n");
                    //     //     counter++;
                    //     // }

                    //     // get first and last element
                    //     if (range.size() > 1)
                    //     {
                    //         auto first = range.front();
                    //         auto last = range.back();
                    //         double speed = (double)(range.size()) / ((first.ts - last.ts) / 1e6);
                    //         std::cout << "Speed: " << speed << " records/sec (" << range.size() << ")" << std::endl;
                    //     }

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
