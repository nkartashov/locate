#ifndef SUFFIX_ARRAY_H_
#define SUFFIX_ARRAY_H_

#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <tbb/parallel_sort.h>
#include <vector>
#include <string>
#include <iostream>
#include <set>

#include "unused.h"
#include "multithread_queue.h"

namespace locate {
  namespace fs = boost::filesystem;
  using Path = fs::path;

  class SuffixArray {
    public:
      friend class boost::serialization::access;
      using Strings = std::vector<std::string>;
      using Indices = std::vector<size_t>;
      using SuffixReference = std::pair<std::string, Indices>;
      using References = std::vector<SuffixReference>;
      using Matches = std::set<std::string>;

      SuffixArray() {}

      SuffixArray(MultithreadQueue<Path>& files) {
        std::queue<Path> file_queue = files.Queue();
        while(!file_queue.empty()) {
          Path file = file_queue.front();
          file_queue.pop();
          file = fs::canonical(file);
          m_paths.push_back(file.string());
          std::string word = file.filename().string();
          AddReferences(word, m_paths.size() - 1);
        }
        tbb::parallel_sort(m_references.begin(), m_references.end());
        RemoveDuplicates();
      }

      template<class Archive>
      void serialize(Archive& ar, const unsigned int version)
      {
        PARAMETER_UNUSED(version, "Compiler warning otherwise");
        ar & m_references;
        ar & m_paths;
      }

      Matches Search(std::string const& pattern) const {
        SuffixReference reference {pattern, {0}};
        References::const_iterator match = std::lower_bound(m_references.cbegin(), m_references.cend(), reference);
        Matches result;
        while (match != m_references.cend() && boost::starts_with(match->first, pattern)) {
          for (size_t index : match->second) {
            result.insert(m_paths[index]);
          }
          match++;
        }
        return result;
      }

    private:
      void RemoveDuplicates() {
        References references;
        SuffixReference current_reference;
        for (auto&& reference : m_references) {
          if (current_reference.first == reference.first) {
            current_reference.second.push_back(reference.second.front());
          } else {
            references.push_back(current_reference);
            current_reference = reference;
          }
        }
        if (current_reference != SuffixReference()) {
          references.push_back(current_reference);
        }
        m_references = references;
      }

      void AddReferences(std::string& word, size_t index) {
        for (size_t i = 0; i < word.size() - 1; i++) {
          Indices indices;
          indices.push_back(index);
          m_references.push_back(std::make_pair(word.substr(i), indices));
        }
      }

      References m_references;
      Strings m_paths;
  };

} // namespace locate

#endif /* end of include guard */
