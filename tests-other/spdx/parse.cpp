#include "spdx.h"

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
    
    const spdx::db<spdx::LicenseList>   licenses{SPDX_LICENSES_JSON};
    const spdx::db<spdx::ExceptionList> exceptions{SPDX_EXCEPTIONS_JSON};
    
    Annotation anno(annotation);
    cout  << licenses.size() << " SPDX Licenses:\n"
          << "version: " << licenses.version() << '\n'
          << "date   : " << std::put_time(licenses.date(),"%Y/%m/%d") << '\n'
          << endl;

    cout  << exceptions.size() << " SPDX Exceptions:\n"
          << "version: " << exceptions.version() << '\n'
          << "date   : " << std::put_time(exceptions.date(),"%Y/%m/%d") << '\n'
          << endl;

    spdx::analize(anno,licenses,exceptions);
    
    for(const auto &[size,data]: anno.analitics()) {
      auto sdata = dynamic_cast<spdx::Data*>(data.get());
      assert(sdata);
      cout  << "token     : " << anno.token(sdata) << '\n'
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
