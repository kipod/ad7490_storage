#include "qu.hpp"
#include "config.hpp"

uint64_t getTimestamp()
{
    // get timestamp in microseconds
    return std::chrono::duration_cast<std::chrono::microseconds>(
               std::chrono::system_clock::now().time_since_epoch())
        .count();
}

const Config SETTINGS;

std::string get_redis_url()
{
    return "tcp://" + SETTINGS.REDIS_HOST + ":" + std::to_string(SETTINGS.REDIS_PORT);
}

StringView QData::pack() const
{
    auto data = reinterpret_cast<const char *>(this);
    StringView ret = StringView(data, sizeof(*this));
    return ret;
}

void QData::unpack(const StringView &sv)
{
    auto data = sv.data();
    ::memcpy(reinterpret_cast<char *>(this), sv.data(), sizeof(*this));
}

Queue::Queue()
    : m_r(get_redis_url()),
      m_pipe(m_r.pipeline()),
      pipe_size(0)
{
    m_r.command<OptionalString>("config", "set", "maxmemory", "200MB");
}

Queue::~Queue()
{
}

int Queue::size()
{
    return m_r.llen(SETTINGS.QUEUE_NAME);
}

void Queue::push(QData &data, bool force)
{
    // push data to queue
    m_pipe.lpush(SETTINGS.QUEUE_NAME, data.pack());
    pipe_size++;
    if (pipe_size >= SETTINGS.PIPE_SIZE || force)
    {
        m_pipe.exec();
        pipe_size = 0;
    }
}

QData Queue::pop()
{
    auto r = m_r.rpop(SETTINGS.QUEUE_NAME);
    QData data;
    data.unpack(r.value());
    return data;
}
