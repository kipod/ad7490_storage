#include <iostream>
#include <sw/redis++/redis++.h>
#include <thread>
#include <mutex>
#include <signal.h>

#include "qu.hpp"
#include "test_q.hpp"
#include "config.hpp"
// #include "ad7490.hpp"
#include "uart.hpp"

#include <pthread.h>

const Config SETTINGS;

using namespace sw::redis;

std::deque<double> speed_tests;
std::deque<QData> spiReadQueue;
std::mutex quMutex;
QData first;
const size_t TEST_RANGE_SIZE = 10000;

Queue q;

void sig_handler(int s)
{
    std::cout << "Caught signal " << s << std::endl;
    q.stop();
    std::cout << "Stop queue" << std::endl;
    std::cout << "Wrote " << counter << " records" << std::endl;

    if (!speed_tests.empty())
    {
        std::cout << "Speed test: " << std::endl;
        double sum = 0;
        double min = 1000000;
        double max = 0;
        for (std::deque<double>::const_iterator it = speed_tests.begin(); it != speed_tests.end(); ++it)
        {
            double speed = *it;
            sum += speed;
            if (speed < min)
            {
                min = speed;
            }
            if (speed > max)
            {
                max = speed;
            }
        }
        double avg = sum / speed_tests.size();
        std::cout << "Min: " << min << std::endl;
        std::cout << "Max: " << max << std::endl;
        std::cout << "Avg: " << avg << std::endl;
        std::cout << "Cache size: " << spiReadQueue.size() << std::endl;
    }
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

void read_mic_data()
{
    // up_priority();

    Uart uart;

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
                    // QData data(counter);
                    auto dataSet = uart.read();
                    // q.push(data);
                    for (auto it = dataSet.begin(); it != dataSet.end(); ++it)
                    {
                        const QData &data = *it;
                        {
                            std::lock_guard<std::mutex> lock(quMutex);
                            spiReadQueue.push_back(data);
                        }
                        counter++;

                        if (counter % TEST_RANGE_SIZE == 1)
                        {
                            first = data;
                        }

                        if (counter % TEST_RANGE_SIZE == 0)
                        {
                            //     // q.stop();
                            //     // auto range = q.range(0, -1);
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

                            // get first and last element
                            {
                                const QData &last = data;
                                double speed = (double)(TEST_RANGE_SIZE) / ((last.ts - first.ts) / 1e6);
                                speed_tests.push_back(speed);
                                std::cout << "Speed: " << speed << " records/sec (" << TEST_RANGE_SIZE << ")" << std::endl;
                            }
                        }
                    }
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
}

void write_to_redis()
{
    for (;;)
    {
        if (!spiReadQueue.empty())
        {
            std::lock_guard<std::mutex> lock(quMutex);
            QData data = spiReadQueue.front();
            spiReadQueue.pop_front();
            q.push(data);
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

int main(int, char **)
{
    // test_queue();

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);
    sigaction(SIGABRT, &sigIntHandler, nullptr);

    //  read_spi();
    std::thread reader(read_mic_data);
    std::thread writer(write_to_redis);

    reader.join();
    writer.join();

    return 0;
}
