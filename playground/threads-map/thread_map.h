#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <vector>

// ThreadMap is an experimental class to manage worker threads that will be
// stored by their thread id unordered map.
// Usually in multi-thread (MT) design, there is a single, well defined waiter
// for a thread. This object is meant to be such a waiter.
//
// IMPORATNT: Current implementation of the add() method moves the guts of the
// passed thread as parameter into the map - That means object that the caller
// supplied becomes invalid!
//
// Exiting threads have to notify about their exitting
class ThreadMap
{
  public:
    ThreadMap();
    ~ThreadMap();

    // Add worker thread to the map using std::move
    void add(std::thread& th, std::thread::id id = std::this_thread::get_id());
    // Worker thread must call this method to notify exiting and be removed from the map
    void notify_done(std::thread::id id = std::this_thread::get_id());

  private:
    mutable std::mutex m_mutex;

    // ---- Below here are variables locked by m_ mutex -----

    // Cleanup routine will wait on m_cv_done for worker threads to nofity about exiting
    std::condition_variable m_cv_done;
    // Map stores threads by their unique id as keys
    std::unordered_map<std::thread::id, std::thread> m_threads_map;
    // Vector of finished threads to be removed from the map
    std::vector<std::thread::id> m_finished_threads;
    // Flag controls while loop of cleanup routine
    bool m_stop;

    // ---- End of variables locked by m_mutex -----

    // Thread running processor() loop
    std::thread m_loop_thread;

    // Processing loop that joins and removes finished threads from the map
    void processor();
};
