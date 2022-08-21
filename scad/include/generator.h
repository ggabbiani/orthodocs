#pragma once

#include "document.h"

#include "SCADBaseListener.h"

#include <filesystem>

namespace fs=std::filesystem;

namespace scad {

class Generator : public scad::SCADBaseListener {
public:
  doc::ItemMap              items;
  std::stack<doc::ItemPtr>  curr_item;
  std::stack<doc::ItemPtr>  curr_variable;
  doc::ParameterPtr         curr_parameter;
  std::string               _package;

  Generator(const fs::path &source);

  void enterAnnotation(scad::SCADParser::AnnotationContext *ctx)      override;
  void enterAssignment(scad::SCADParser::AssignmentContext *ctx)      override;
  void enterFunction_def(scad::SCADParser::Function_defContext *ctx)  override;
  void enterLookup(scad::SCADParser::LookupContext *ctx)              override;
  void enterModule_def(scad::SCADParser::Module_defContext * ctx)     override;
  void enterParameter(scad::SCADParser::ParameterContext *ctx)        override;
  void enterPkg(scad::SCADParser::PkgContext *ctx)                    override;

  void exitAssignment(scad::SCADParser::AssignmentContext *ctx)       override;
  void exitFunction_def(scad::SCADParser::Function_defContext *ctx)   override;
  void exitModule_def(scad::SCADParser::Module_defContext * ctx)      override;
  void exitParameter(scad::SCADParser::ParameterContext *ctx)         override;
  void exitPkg(scad::SCADParser::PkgContext *ctx)                     override;
};

}
