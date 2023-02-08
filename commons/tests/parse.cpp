#include <commons/spdx.h>

#include <CLI/CLI.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs=std::filesystem;

using json=nlohmann::json;
using namespace std;

int main(int argc, const char *argv[]) {
  CLI::App  app{"spdx test"};
  auto      result = EXIT_SUCCESS;

  try {
    string annotation;

    app.add_option("annotation",annotation,"annotation to parse")
      ->required();
    app.parse(argc,argv);

    doc::Annotation anno(annotation);

    spdx::analyze(anno);

    for(const auto &[size,data]: anno.analytics()) {
      auto sdata = dynamic_cast<spdx::Data*>(data.get());
      assert(sdata);
      cout  << "token     : " << annotation.substr(sdata->position,sdata->length) << '\n'
            << "name      : " << sdata->name << '\n'
            << "position  : " << sdata->position << '\n'
            << "length    : " << sdata->length << '\n'
            << "url       : " << sdata->url << '\n'
            << endl;
    }
  } catch (const CLI::Error &error) {
    result  = app.exit(error);
  } catch(const std::exception &error) {
    cerr << error.what() << endl;
    result  = EXIT_FAILURE;
  }
  return result;
}
