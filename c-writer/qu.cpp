#include "qu.hpp"
#include "config.hpp"

const Config SETTINGS;

std::string get_redis_url()
{
    return "tcp://" + SETTINGS.REDIS_HOST + ":" + std::to_string(SETTINGS.REDIS_PORT);
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
        int over = size() - SETTINGS.MAX_QUEUE_SIZE;
        if (over > 0)
        {
            for (int i = 0; i < over; i++)
            {
                m_pipe.rpop(SETTINGS.QUEUE_NAME);
            }
            m_pipe.exec();
        }
    }
}

QData Queue::pop()
{
    auto r = m_r.rpop(SETTINGS.QUEUE_NAME);
    QData data;
    if (!r)
    {
        data.ts = 0;
        return data;
    }
    data.unpack(r.value());
    return data;
}

std::vector<QData> Queue::range(int start, int end)
{
    std::vector<std::string> result;
    m_r.lrange(SETTINGS.QUEUE_NAME, start, end, std::back_inserter(result));
    std::vector<QData> ret;
    for (auto &sv : result)
    {
        ret.push_back(QData(sv));
    }

    return ret;
}

void Queue::clear()
{
    m_r.del(SETTINGS.QUEUE_NAME);
}

Queue::Status Queue::getStatus()
{
    auto r = m_r.get(SETTINGS.STATUS_NAME);
    if (!r)
    {
        setStatus(Queue::Status::WRITE);
        return Queue::Status::WRITE;
    }
    return static_cast<Status>(std::stoi(r.value()));
}

void Queue::setStatus(Queue::Status status)
{
    m_r.set(SETTINGS.STATUS_NAME, std::to_string(static_cast<int>(status)));
}

int Queue::speed()
{
    auto r = range(0, 10000);
    if (r.empty())
    {
        return 0;
    }
    auto seconds = (r[0].ts - r[r.size() - 1].ts) / 1e6;
    return r.size() / seconds;
}
