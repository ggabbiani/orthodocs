#pragma once

#include "document.h"

#include "SCADBaseListener.h"

namespace scad {

class Generator : public scad::SCADBaseListener {
public:
  using Parser = scad::SCADParser;

  Document          document;
  doc::ItemPtrStack curr_item;
  doc::ItemPtrStack curr_variable;
  doc::ParameterPtr curr_parameter;

  Generator(const char *pkg_name);

  void enterAnnotation(Parser::AnnotationContext *ctx)      override;
  void enterAssignment(Parser::AssignmentContext *ctx)      override;
  void enterFunction_def(Parser::Function_defContext *ctx)  override;
  void enterLookup(Parser::LookupContext *ctx)              override;
  void enterModule_def(Parser::Module_defContext * ctx)     override;
  void enterParameter(Parser::ParameterContext *ctx)        override;
  void enterPkg(Parser::PkgContext *ctx)                    override;

  void exitAssignment(Parser::AssignmentContext *ctx)       override;
  void exitFunction_def(Parser::Function_defContext *ctx)   override;
  void exitModule_def(Parser::Module_defContext * ctx)      override;
  void exitParameter(Parser::ParameterContext *ctx)         override;
  void exitPkg(Parser::PkgContext *ctx)                     override;

protected:
  std::string       _package;
};

}
