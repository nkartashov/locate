#ifndef SUFFIX_ARRAY_H_
#define SUFFIX_ARRAY_H_

#include <boost/filesystem.hpp>
#include <tbb/parallel_sort.h>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include "multithread_queue.h"

namespace locate {
  namespace fs = boost::filesystem;
  using Path = fs::path;

  /*class SuffixReference {
    public:
      using WordsPtr = std::vector<std::string>*;

      SuffixReference(size_t start, size_t index, WordsPtr words):
        m_start(start), m_index(index), m_words(words) {}

      SuffixReference(SuffixReference const& right):
        m_start(right.m_start),
        m_index(right.m_index),
        m_words(right.m_words) {}

      static void AddReferences(
          WordsPtr words,
          size_t index,
          std::vector<SuffixReference>& references) {
        for (size_t i = 0; i != words[index].size() - 1; i++) {
          references.push_back(SuffixReference(i, index, words));
        }
      }

      bool operator< (SuffixReference const& right) const {
        return Compare(right) < 0;
      }

      int Compare(std::string const& right_string) const {
        return (*m_words)[Index()].compare(m_start, std::string::npos, right_string);
      }

      bool Compare(SuffixReference const& left, SuffixReference const& right) {
        return left < right;
      }

      char operator[](size_t index) const {
        return (*m_words)[m_index][m_start + index];
      }

      operator std::string() const {
        return (*m_words)[Index()].substr(m_start);
      }

      void swap(SuffixReference& left, SuffixReference& right) {
        std::swap(left.m_start, right.m_start);
        std::swap(left.m_index, right.m_index);
        std::swap(left.m_words, right.m_words);
      }

      std::string String() const {return *this;}

      size_t Index() const {return m_index;}

    private:
      size_t m_start;
      size_t m_index;
      WordsPtr m_words;
  };*/


  class SuffixArray {
    public:
      using Strings = std::vector<std::string>;
      using SuffixReference = std::pair<std::string, size_t>;

      Strings paths;
      SuffixArray(MultithreadQueue<Path>& files) {
        std::queue<Path> file_queue = files.Queue();
        while(!file_queue.empty()) {
          Path file = file_queue.front();
          file_queue.pop();
          file = fs::canonical(file);
          paths.push_back(file.string());
          std::string word = file.filename().string();
          m_words.push_back(word);
          for (size_t i = 0; i < word.size(); i++) {
            m_references.push_back(std::make_pair(word.substr(i), m_words.size() - 1));
          }
        }
        tbb::parallel_sort(m_references.begin(), m_references.end());
        for (auto ref : m_references) {
          std::cout << ref.first << std::endl;
        }
      }

    private:
      std::vector<SuffixReference> m_references;
      std::vector<std::string> m_words;
      std::vector<Strings> m_paths;
  };

} // namespace locate

#endif /* end of include guard */
