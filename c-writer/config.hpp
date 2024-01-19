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
    size_t MAX_QUEUE_SIZE;
    size_t WRITE_BATCH_SIZE;
    size_t PIPE_SIZE;

    std::string REDIS_HOST;
    int REDIS_PORT;
    int REDIS_DB;

    int SPI_SPEED;
    std::string TRANSMIT_SERIAL_PORT;
    uint32_t TRANSMIT_SERIAL_BAUDRATE;
    size_t TRANSMIT_SEND_PACKETS;
};
