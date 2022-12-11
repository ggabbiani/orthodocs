#include <fstream>

#include <nlohmann/json.hpp>

#include <filesystem>
#include <iostream>

using json = nlohmann::json;

namespace fs=std::filesystem;
using namespace std;


int main(int argc, const char *argv[]) {
  try {
    if (argc<2) {
      throw runtime_error("List file name expected");
    }

    fs::path  fname(argv[1]);
    if (!fs::exists(fname)) {
      throw runtime_error("List file "+fname.string()+" not existing");
    }
    ifstream file(fname);
    json data = json::parse(file);
    cout  << "SPDX license list version " << data["licenseListVersion"] << '\n'
          << "successfully read " << data["licenses"].size() << " licenses." << endl;

    auto licenses = data["licenses"];
    for(auto license: licenses) {
      cout << "license id " << license["licenseId"] << " (" << license["name"] << ")\n";
    }
    cout << endl;
  } catch(const exception &error) {
    cerr << error.what() << endl;
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}
