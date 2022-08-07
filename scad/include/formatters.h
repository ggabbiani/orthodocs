#pragma once

#include "generator.h"

#include <iostream>

namespace doc {

/**
 * abstract output formatter
 */
class Formatter {

protected:
  virtual void function(const doc::Item &f)      = 0;
  virtual void module(const doc::Item &m)        = 0;
  virtual void package(const doc::Item &m)       = 0;
  virtual void format(const doc::ItemMap &items) = 0;
  virtual void parameter(const doc::Parameter &p) = 0;

  void item(const doc::Item& i);
  std::string signature(const doc::Item &item);
};

namespace formatter {

class Mdown : public Formatter {
public:
  Mdown(std::ostream &out);
  void format(const doc::ItemMap &items) override;

private:
  void package(const doc::Item &pkg) override;
  void parameter(const doc::Parameter &p) override;
  void function(const doc::Item &func) override;
  void module(const doc::Item &mod) override ;

  std::ostream &out;
};

}

}
