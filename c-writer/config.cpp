#include <string>
#include "config.hpp"

std::string get_str_env(const char *name, const char *default_value)
{
    const char *value = getenv(name);
    if (value == nullptr)
    {
        return std::string(default_value);
    }
    return std::string(value);
}

int get_int_env(const char *name, int default_value)
{
    const char *value = getenv(name);
    if (value == nullptr)
    {
        return default_value;
    }
    return atoi(value);
}

Config::Config()
{
    QUEUE_NAME = get_str_env("QUEUE_NAME", "queue");
    STATUS_NAME = get_str_env("STATUS_NAME", "status");
    MAX_QUEUE_SIZE = get_int_env("MAX_QUEUE_SIZE", 500000);
    WRITE_BATCH_SIZE = get_int_env("WRITE_BATCH_SIZE", 10000);
    PIPE_SIZE = get_int_env("PIPE_SIZE", 100);

    REDIS_HOST = get_str_env("REDIS_HOST", "localhost");
    REDIS_PORT = get_int_env("REDIS_PORT", 6379);
    REDIS_DB = get_int_env("REDIS_DB", 0);

    SPI_SPEED = get_int_env("SPI_SPEED", 16 * 1000 * 1000);
}
