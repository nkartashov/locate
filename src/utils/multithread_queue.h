#ifndef MULTITHREAD_QUEUE
#define MULTITHREAD_QUEUE

#include <queue>
#include <mutex>

namespace locate {
  template<typename T>
  class MultithreadQueue {
    public:
      using Lock = std::unique_lock<std::mutex>;

      MultithreadQueue() {}

      void Push(T const& object) {
        Lock lock(m_queue_mutex);
        m_queue.push(object);
      }

      bool PopIfNotEmpty(T& result) {
        Lock lock(m_queue_mutex);
        if (m_queue.empty()) {
          return false;
        }
        result = m_queue.front();
        m_queue.pop();
        return true;
      }

      std::queue<T>& Queue() {return m_queue;}

    private:
      std::queue<T> m_queue;
      std::mutex m_queue_mutex;
  };
} // namespace locate

#endif /* end of include guard */
