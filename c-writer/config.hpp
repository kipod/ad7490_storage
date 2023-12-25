#pragma once
#include <string>

// const std::string QUEUE_NAME = "queue";
// const std::string STATUS_NAME = "status";
// const int MAX_QUEUE_SIZE = 500000;
// const int WRITE_BATCH_SIZE = 10000;
// const int PIPE_SIZE = 100;

// const std::string REDIS_HOST = "localhost";
// const int REDIS_PORT = 6379;
// const int REDIS_DB = 0;

struct Config
{
    Config();
    std::string QUEUE_NAME;
    std::string STATUS_NAME;
    int MAX_QUEUE_SIZE;
    int WRITE_BATCH_SIZE;
    int PIPE_SIZE;

    std::string REDIS_HOST;
    int REDIS_PORT;
    int REDIS_DB;
};
