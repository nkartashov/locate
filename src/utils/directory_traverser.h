#ifndef DIRECTORY_TRAVERSER_H_
#define DIRECTORY_TRAVERSER_H_

#include <boost/filesystem.hpp>
#include <stdexcept>
#include <vector>
#include <iostream>

#include "thread_pool.h"
#include "suffix_array.h"
#include "unused.h"

namespace fs = boost::filesystem;

namespace locate {
  class DirectoryTraverser {
    public:
      using Path = fs::path;
      using Iterator = fs::recursive_directory_iterator;

      DirectoryTraverser(ThreadPool& pool, std::string root, SuffixArray& receiver):
        m_pool(pool),
        m_root(root),
        m_receiver(receiver) {}

      void Traverse() {
        Path root(m_root);
        fs::path root_path(root);
        if (!fs::exists(root)) {
            throw std::runtime_error(root.string() + " does not exists");
        }
        Iterator traverser(root);
        std::vector<std::future<void> > futures;
        while(traverser != Iterator()) {
          auto path = traverser->path();
          if (!fs::is_symlink(traverser->symlink_status())) {
            if (fs::is_directory(path) || fs::is_regular_file(path)) {
              ThreadPool::Task task = [this, path]() {
                m_receiver.MultithreadAddReferences(path);
              };
              futures.push_back(m_pool.AddTask(task));
            }
          }
          traverser++;
        }
        for (auto&& f : futures) {
          f.get();
        }
      }

    private:
      ThreadPool& m_pool;
      std::string m_root;
      SuffixArray& m_receiver;
  };
} // namespace locate

#endif /* end of include guard */

