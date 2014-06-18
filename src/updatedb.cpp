#include <boost/program_options.hpp>
#include <stdexcept>
#include <iostream>
#include <fstream>

#include "utils/directory_traverser.h"
#include "utils/unused.h"
#include "utils/thread_pool.h"
#include "utils/suffix_array.h"

namespace po = boost::program_options;
using namespace locate;

void Help() {
  std::cout
    << "Usage: updatedb --file-root <dir-to-start-from> --db-file <file-with-database>"
    << std::endl;
}

int main(int argc, char** argv) {
  po::options_description desc("Options");
  desc.add_options()
      ("file-root", po::value<std::string>(), "Where to index files")
      ("db-file", po::value<std::string>(), "Database filename");
  try {
    po::variables_map args;
    po::store(po::command_line_parser(argc, argv)
                .options(desc).run(), args);
    po::notify(args);

    if (args.size() != 2) {
      Help();
      return 0;
    }
    ThreadPool pool;
    SuffixArray array;
    DirectoryTraverser traverser(pool, args["file-root"].as<std::string>(), array);
    traverser.Traverse();
    pool.Done();
    array.FinishBuild();
    std::ofstream ofs(args["db-file"].as<std::string>());
    if (!ofs) {
      std::cerr << "Unable to open the file " + args["db-file"].as<std::string>() + " for writing" << std::endl;
      return 2;
    }

    boost::archive::text_oarchive oa(ofs);
    oa << array;
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
