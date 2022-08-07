#pragma once

#include "SCADBaseListener.h"

#include <boost/filesystem.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace doc {

struct Parameter {
  std::string name;
  std::string annotation;
  std::string defaults;
};
using ParameterPtr  = std::unique_ptr<Parameter>;
using ParameterVec  = std::vector<ParameterPtr>;

struct Item {
  Item(const std::string &name,const std::string &type,bool nested=false) : name(name),type(type),nested(nested) {}
  std::string name;
  std::string annotation;
  std::string type;
  ParameterVec parameters;
  bool nested;
};
using ItemPtr = std::unique_ptr<Item>;
using ItemMap = std::map<std::string,ItemPtr>;

struct Generator : public scad::SCADBaseListener {
  ItemMap             items;
  std::stack<ItemPtr> curr_item;
  ParameterPtr        curr_parameter;
  std::string         _package;

  Generator(const boost::filesystem::path &source);
  
  void enterFunction_def(scad::SCADParser::Function_defContext *ctx) override;
  void exitFunction_def(scad::SCADParser::Function_defContext *ctx) override;
  void enterModule_def(scad::SCADParser::Module_defContext * ctx) override;
  void exitModule_def(scad::SCADParser::Module_defContext * ctx) override;
  void enterAnnotation(scad::SCADParser::AnnotationContext *ctx) override;
  void enterParameter(scad::SCADParser::ParameterContext *ctx) override;
  void exitParameter(scad::SCADParser::ParameterContext *ctx) override;
  void enterLookup(scad::SCADParser::LookupContext *ctx) override;
  void enterAssignment(scad::SCADParser::AssignmentContext *ctx) override;
};

}
