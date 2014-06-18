#ifndef DIRECTORY_TRAVERSER_H_
#define DIRECTORY_TRAVERSER_H_

#include <boost/filesystem.hpp>
#include <stdexcept>
#include <vector>

#include "thread_pool.h"
#include "multithread_queue.h"

namespace fs = boost::filesystem;

namespace locate {
  class DirectoryTraverser {
    public:
      using Path = fs::path;
      using Iterator = fs::directory_iterator;
      using ResultQueue = MultithreadQueue<Path>;

      DirectoryTraverser(ThreadPool& pool, std::string root):
        m_pool(pool),
        m_root(root) {}

      void Traverse() {
        Path root(m_root);
        fs::path root_path(root);
        if (!fs::exists(root)) {
            throw std::runtime_error(root.string() + " does not exists");
        }
        if (!fs::is_directory(root)) {
          m_results.Push(root);
          return;
        }
        PerformTraverse(&m_pool, &m_results, root);
      }

      ResultQueue& Results() {return m_results;}
    private:
      static void PerformTraverse(ThreadPool* pool, ResultQueue* results, Path root) {
        Iterator traverser(root);
        std::vector<std::future<void> > futures;
        while(traverser != Iterator()) {
          auto path = traverser->path();
          if (!fs::is_symlink(traverser->symlink_status())) {
            if (fs::is_directory(path) || fs::is_regular_file(path)) {
              results->Push(path);
            }
            if (fs::is_directory(path)) {
              ThreadPool::Task task = std::bind(&DirectoryTraverser::PerformTraverse, pool, results, path);
              futures.push_back(pool->AddTask(task));
            }
          }
          traverser++;
        }
        for (auto&& f : futures) {
          f.get();
        }
      }

      ResultQueue m_results;
      ThreadPool& m_pool;
      std::string m_root;
  };
} // namespace locate

#endif /* end of include guard */

