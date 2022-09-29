#pragma once

#include "orthodocs/xref.h"

namespace scad {

class XRef : public orthodocs::AbstractXRef {
public:
  ~XRef() = default;
  /**
   * the xref key of an item is calculated from its name and the name of the package in which it's defined
   */
  Key key(orthodocs::doc::Item *item) const override;
};

} // namespace scad
