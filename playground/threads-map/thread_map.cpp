#include "thread_map.h"

ThreadMap::ThreadMap() : m_stop(false)
{
    // Start processor loop
    m_loop_thread = std::thread(&ThreadMap::processor, this);
};

ThreadMap::~ThreadMap()
{
    // Stop the processor loop
    m_stop = true;
    // NOTE Must notify loop thread to stop blocking on wait and exit
    m_cv_done.notify_one();
    m_loop_thread.join();

    // Join any possible missed threads before exiting
    for (auto& thread : m_threads_map)
        thread.second.join();
};

// Add worker thread to the map, while std::thread objects cannot be copied
// (default copy constructor would be ill formed). std::move is used in this
// context so that the processor() routine takes ownership of the thread object
// and original one that the caller supplied will become valid
void ThreadMap::add(std::thread& th, std::thread::id id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_threads_map[id] = std::move(th);
    // Sanity check on the size of the threads map
    if (m_threads_map.size() > 20)
    {
        // TODO log, raise warning on map size
        // TODO add control mechanism
    }
}

// Worker threads must call this method to notify exiting
void ThreadMap::notify_done(std::thread::id id)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_finished_threads.push_back(id);
    m_cv_done.notify_one();
}

// Processor routine joins and removes finished threads from the map
void ThreadMap::processor()
{
    while (1)
    {
        // Using if in infinite while loop to give ability of logging or
        // performing additional actions on stop
        if (m_stop)
            break;

        std::unique_lock<std::mutex> lock(m_mutex);
        m_cv_done.wait(lock, [this] {
            // On termination - stop blocking by setting predicate to true
            return m_stop || m_finished_threads.size();
        });

        for (auto const& id : m_finished_threads)
        {
            if (m_threads_map.find(id) != m_threads_map.end())
            {
                m_threads_map[id].join();
                m_threads_map.erase(id);
            }
            else
            {
                // TODO log notify that given id does not exist in the map
            }
        }
        // Clear vector of finished threads
        m_finished_threads.clear();
    }
}
