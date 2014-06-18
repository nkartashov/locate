#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <memory>

namespace locate {
  class ThreadPool {
    public:
      using Task = std::function<void(void)>;
      template<class R, class... Args>
      using TaskPtr = std::shared_ptr<std::packaged_task<R(Args...)> >;
      using Lock = std::unique_lock<std::mutex>;

      explicit ThreadPool(size_t thread_number = std::thread::hardware_concurrency());
      ~ThreadPool();

      template<class R, class... Args>
      std::future<R> AddTask(std::function<R(Args...)> function, Args... arguments) {
        TaskPtr<R, Args...> new_task(new std::packaged_task<R(Args...)>(function));
        Task bound_task = std::bind([] (TaskPtr<R, Args...> new_task, Args... arguments) {
            new_task->operator()(arguments...);
          },
          new_task,
          arguments...);
        Lock lock(m_task_mutex);
        m_task_queue.push(bound_task);
        lock.unlock();
        m_worker_blocker.notify_one();
        return new_task->get_future();
      }

      void Done();

    private:
      ThreadPool(ThreadPool&);
      ThreadPool operator=(ThreadPool&);

      void SetupThreads(size_t thread_number);

      bool m_stop;
      std::mutex m_task_mutex;
      std::condition_variable m_worker_blocker;
      std::queue<Task> m_task_queue;
      std::vector<std::thread> m_workers;
  };
} // namespace locate

#endif /* end of include guard */
