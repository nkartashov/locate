#include <fstream>
#include <iostream>
#include <boost/program_options.hpp>

#include "utils/suffix_array.h"

namespace po = boost::program_options;
using namespace locate;

void Help() {
  std::cout
    << "Usage: locate --db-file <file-with-database> --query <query>"
    << std::endl;
}

int main(int argc, char** argv) {
  po::options_description desc("Options");
  desc.add_options()
      ("db-file", po::value<std::string>(), "Database filename")
      ("query", po::value<std::string>(), "Query");
  try {
    po::variables_map args;
    po::store(po::command_line_parser(argc, argv)
                .options(desc).run(), args);
    po::notify(args);

    if (args.size() != 2) {
      Help();
      return 0;
    }
    SuffixArray array;

    std::ifstream ifs(args["db-file"].as<std::string>());
    if (!ifs) {
      std::cerr << "Unable to open the file " + args["db-file"].as<std::string>() + " for reading" << std::endl;
      return 2;
    }

    boost::archive::text_iarchive ia(ifs);
    ia >> array;
    auto matches = array.Search(args["query"].as<std::string>());
    for (auto&& match : matches) {
      std::cout<< match << std::endl;
    }
  } catch (std::exception &e) {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  return 0;
}

