#include <boost/program_options.hpp>
#include <stdexcept>
#include <iostream>

#include "utils/directory_traverser.h"
#include "utils/unused.h"
#include "utils/thread_pool.h"
#include "utils/suffix_array.h"

namespace po = boost::program_options;
using namespace locate;

void Help() {
  std::cout
    << "Usage: updatedb --file-root <dir-to-start-from> --db-root <dir-where-to-dump-results>"
    << std::endl;
}

int main(int argc, char** argv) {
  PARAMETER_UNUSED(argc, "lol");
  PARAMETER_UNUSED(argv, "foo");
  po::options_description desc("Options");
  desc.add_options()
      ("file-root", po::value<std::string>(), "Where to index files")
      ("db-root", po::value<std::string>(), "Where to put database");
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
    DirectoryTraverser traverser(pool, args["file-root"].as<std::string>());
    traverser.Traverse();
    pool.Done();
    auto& results = traverser.Results();

    SuffixArray array(results);
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}
