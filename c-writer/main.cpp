#include <iostream>
#include <sw/redis++/redis++.h>
#include <thread>
#include <signal.h>
#include "qu.hpp"
#include "test_q.hpp"
#include "config.hpp"

const Config SETTINGS;

using namespace sw::redis;

Queue q;

void sig_handler(int s)
{
    // printf("Caught signal %d\n", s);
    std::cout << "Caught signal " << s << std::endl;
    q.stop();
    exit(1);
}

int main(int, char **)
{
    // test_queue();

    struct sigaction sigIntHandler;

    sigIntHandler.sa_handler = sig_handler;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;

    sigaction(SIGINT, &sigIntHandler, nullptr);

    std::cout << "start c-writer" << std::endl;
    bool run = false;
    size_t counter = 0;
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
                    QData data(counter++);
                    q.push(data);
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
