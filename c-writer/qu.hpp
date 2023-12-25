#pragma once
#include <sw/redis++/redis++.h>
#include <string>
#include "qdata.hpp"

using namespace sw::redis;

const std::string REDIS_HOST = "localhost";
const int REDIS_PORT = 6379;
const std::string QUEUE_NAME = "queue";
const std::string STATUS_NAME = "status";
const int MAX_QUEUE_SIZE = 10000;
const int PIPE_SIZE = 10;

class Queue
{
private:
    Redis m_r;
    Pipeline m_pipe;
    int pipe_size;

    // int llen(const std::string &key);

public:
    enum class Status : int
    {
        WRITE = 0,
        WAIT = 1,
    };
    Queue();
    ~Queue();

    int size();
    void push(QData &data, bool force = false);
    QData pop();
    std::vector<QData> range(int start = 0, int end = -1);
    void clear();
    Status getStatus();
    void setStatus(Status status);
    bool isActive()
    {
        return getStatus() == Status::WRITE;
    }
    void start()
    {
        setStatus(Status::WRITE);
    }
    void stop()
    {
        setStatus(Status::WAIT);
    }
    int speed();
};
