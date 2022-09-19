#pragma once

#include <regex>
#include <string>

namespace scad {
  
namespace doc {

class License {
public:
  License(const char *name,const char *regex) : name(name),_regex(regex) {}
  const std::string name;
  const std::regex &regex() const {return _regex;}
  /**
   * Verify the presence of a known license inside «s».
   * Return a 'ready' smatch in case of success, an empty one otherwise.
   */
  std::smatch match(const std::string &s);
  /**
   * Analize «s» searching for a known license match. If any, removes it from 
   * «s» and returns the found license or nullprt otherwise.
   * «s» is unchanged in case of no match;
   */
  static const License *remove(std::string &s);
private:
  std::regex _regex;
};

} // namespace doc

} // namespace othodocs
