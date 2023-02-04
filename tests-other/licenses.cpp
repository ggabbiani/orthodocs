#include <scad/document.h>

#include <iostream>
#include <regex>
#include <string>

using namespace std;

class License {
public:
  License(const char *name,const char *regex) : name(name),_regex(regex) {}
  const std::string name;
  const std::regex &regex() const {
    return _regex;
  }
  /**
   * Verify the presence of a known license inside «s».
   * Return a 'ready' smatch in case of success, an empty one otherwise.
   */
  std::smatch match(const std::string &s) {
    smatch result;
    return regex_search(s, result, _regex) ? result : smatch();
  }
  /**
   * Analyze «s» searching for a known license match. If any, removes it from
   * «s» and returns the found license or nullptr otherwise.
   */
  static const License *remove(std::string &s) {
    static License licenses[]={
      {"GNU General Public License v3", "Copyright © (.|\\n)*GNU General Public License(.|\\n)*version 3 of the License(.|\\n)*If not, see <http:\\/\\/www\\.gnu\\.org\\/licenses\\/>\\.[\\n]*"},
      {"Apache v2.0",                   "Copyright (.|\\n)?http://www\\.apache\\.org/licenses/LICENSE-2\\.0(.|\\n)*limitations under the License\\.[\\n]*"},
    };

    for(auto lic = std::begin(licenses);lic!=std::end(licenses);++lic) {
      // search pattern regexp in the annotation string
      smatch match = lic->match(s);
      if (match.ready()) {
        // save the license text
        string license=match[0];
        // change the annotation
        s = s.substr(0,match.position(0))+s.substr(match.position(0)+match.length(0));
        return &(*lic);
      }
    }
    return nullptr;
  }
private:
  std::regex _regex;
};

int main() {
  // annotation from stdin
  string anno;
  std::string line;
  while (std::getline(std::cin, line))
    anno += line + '\n';
  cout << "Got annotation:\n"
      << '\'' <<  anno << '\'' << endl;

  scad::doc::style::Factory factory;
  auto style  = factory(anno);
  anno  = style->manage(anno);
  cout << "Style managed annotation:\n"
      << '\'' <<  anno << '\'' << endl;

  auto lic = License::remove(anno);
  if (lic)
    cout  << "Found '" << lic->name << "'\nAnnotation now is:\n"
          << '\'' <<  anno << '\'' << endl;
  return EXIT_SUCCESS;
}
