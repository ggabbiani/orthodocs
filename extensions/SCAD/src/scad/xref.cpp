#include "scad/document.h"
#include "scad/xref.h"

#include <cassert>
#include <exception>

using namespace std;

namespace scad {
    
auto XRef::key(orthodocs::doc::Item *item) const -> Key {
  auto parent = dynamic_cast<scad::doc::Package*>(item->parent);
  if (auto variable = dynamic_cast<scad::doc::Variable*>(item)) {
    assert(parent);
    return parent->name+'/'+variable->name;
  } else if (auto function = dynamic_cast<scad::doc::Function*>(item)) {
    assert(parent);
    return parent->name+'/'+function->name+"()";
  } else if (auto module = dynamic_cast<scad::doc::Module*>(item)) {
    assert(parent);
    return parent->name+'/'+module->name+"()";
  } else if (auto package = dynamic_cast<scad::doc::Package*>(item)) {
    assert(!parent);
    return package->name;
  } else {
    throw runtime_error("type "+item->type()+" not suitable for xref");
  }
}

} // namespace scad
