#pragma once

#include <mutex>
#include <thread>
#include <unordered_map>
#include <list>
#include <atomic>

// Min number of threads
#define MIN_THREADS 3

// Multi-threading watchdog
class Watchdog
{
  public:
    using Interval_t = std::chrono::duration<std::chrono::seconds>;
    using Thread_id = std::thread::id;
    using Time_t = std::chrono::time_point<std::chrono::system_clock>;

    Watchdog(size_t max_threads, double interval);
    ~Watchdog();
    void add_thread(const std::string& name, const Thread_id& = std::this_thread::get_id());
    bool kick(const std::string& name, const Thread_id& = std::this_thread::get_id());
    bool done(const std::string& name, const Thread_id& = std::this_thread::get_id());

  private:
    enum ThreadInfo
    {
        NAME = 0,
        ID,
        TIME,
    };

    std::mutex m_mutex;

    static const int m_unresponsive = 3;
    size_t m_max_threads = 0;
    size_t m_current_threads = 0;
    std::list<std::tuple<std::string, Thread_id, Time_t>> m_list;
    std::unordered_map<std::string, decltype(m_list)::iterator> m_map;
    std::chrono::duration<double> m_interval;
    std::thread m_watchdog;
    std::unordered_map<std::string, int> m_failed_checks;
    bool m_watchdog_enable = true;

    bool m_is_expired(std::string& name, Thread_id& id);
    Time_t get_next_expire_time();
    void start();
};
