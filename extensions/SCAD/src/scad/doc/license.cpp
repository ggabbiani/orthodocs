#include "license.h"

using namespace std;

namespace scad::doc {

smatch License::match(const string &s) const {
  smatch result;
  return regex_search(s, result, _regex) ? result : smatch();
}

const License *License::remove(std::string &s) {
  static License licenses[]={
    {"GNU General Public License v3", "Copyright © (.|\\n)*GNU General Public License(.|\\n)*version 3 of the License(.|\\n)*If not, see <http:\\/\\/www\\.gnu\\.org\\/licenses\\/>\\.[\\n]*"},
    {"Apache v2.0",                   "Copyright (.|\\n)*http://www\\.apache\\.org/licenses/LICENSE-2\\.0(.|\\n)*limitations under the License\\.[\\n]*"},
  };

  for(auto lic = std::begin(licenses);lic!=std::end(licenses);++lic) {
    // search pattern regexp in the annotation string 
    smatch match = lic->match(s);
    if (!match.empty()) {
      // save the license text
      string license=match[0];
      // change the annotation 
      s = s.substr(0,match.position(0))+s.substr(match.position(0)+match.length(0));
      return lic;
    }
  }
  return nullptr;
}

} // namespace scad::doc
