#pragma once

#include "orthodocs/document.h"

#include <map>
#include <string>

namespace orthodocs {

/**
 * Abstract xref, concrete instances are defined in the language extensions.
 */
class AbstractXRef {
public:
  using Key=std::string;
  virtual ~AbstractXRef() = default;
  /**
   * the implementation is language dependant
   */
  virtual Key key(orthodocs::doc::Item *item) const = 0;
private:
  std::map< Key,orthodocs::doc::Item*,std::less<> > _index;
};

} // namespace orthodocs 
