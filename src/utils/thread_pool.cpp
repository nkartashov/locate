#include "thread_pool.h"

namespace locate {
  ThreadPool::ThreadPool(size_t thread_number) : m_stop(false) {
    SetupThreads(thread_number);
  }

  ThreadPool::~ThreadPool() {
    Done();
  }

  void ThreadPool::Done() {
    if (m_stop) {
      return;
    }
    m_stop = true;
    m_worker_blocker.notify_all();
      for(auto& worker: m_workers) {
        worker.join();
    }
  }

  void ThreadPool::SetupThreads(size_t thread_number) {
    for (size_t i = 0; i < thread_number; i++) {
      m_workers.push_back(std::thread([&] () {
        while(true) {
          std::unique_lock<std::mutex> lock(m_task_mutex);
          while (!m_stop && m_task_queue.empty()) {
            m_worker_blocker.wait(lock);
          }
          if (m_stop) {
            return;
          }
          Task task = m_task_queue.front();
          m_task_queue.pop();
          lock.unlock();
          task();
        }
      }));
    }
  }
} // namespace locate
