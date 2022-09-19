#pragma once

#include "orthodocs/document.h"

#include <map>
#include <string>

namespace orthodocs {

class XRef {
public:
  using Key=std::string;
  // returns the item's key
  Key key() const;
private:
  std::map<Key,orthodocs::doc::Item*> _index;
};

} // namespace orthodocs 
