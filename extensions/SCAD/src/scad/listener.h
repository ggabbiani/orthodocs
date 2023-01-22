#pragma once
/*
 * OpenSCAD listener
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include "document.h"

#include <SCADParserBaseListener.h>

#include <memory>

namespace scad {

/**
 * This listener is triggered during the parse-tree walk. The resulting synthetic
 * document ownership is then passed to the calling environment through the
 * documentRelease() call.
 */
class Listener : public scad::SCADParserBaseListener, ::doc::Annotation::Setter {
public:
  using Item      = ::doc::Item;
  using Package   = doc::Package;
  using Parameter = ::doc::Parameter;
  using Parser    = scad::SCADParser;

  explicit Listener(const std::filesystem::path &pkg_source);

  // return the Document's ownership to the calling environment
  Document::Owner releaseDocument() {return std::move(_document);};

  void enterAnnotation(Parser::AnnotationContext *ctx)      override;
  void enterAssignment(Parser::AssignmentContext *ctx)      override;
  void enterFunction_def(Parser::Function_defContext *ctx)  override;
  void enterLookup(Parser::LookupContext *ctx)              override;
  void enterModule_def(Parser::Module_defContext * ctx)     override;
  void enterParameter(Parser::ParameterContext *ctx)        override;
  void enterPkg(Parser::PkgContext *ctx)                    override;
  void enterIncl(SCADParser::InclContext *ctx)              override;
  void enterUse(SCADParser::UseContext *ctx)                override;
  void exitAssignment(Parser::AssignmentContext *ctx)       override;
  void exitFunction_def(Parser::Function_defContext *ctx)   override;
  void exitModule_def(Parser::Module_defContext * ctx)      override;
  void exitParameter(Parser::ParameterContext *ctx)         override;
  void exitPkg(Parser::PkgContext *ctx)                     override;

private:
  // the following members represents the actual status variables for the parser
  Package          *curr_package;
  Item::OwnerStack  curr_item;
  Item::OwnerStack  curr_variable;
  Parameter::Ptr    curr_parameter;
  /*
   * this is a kludge: is there an official way for retrieving the current
   * language source file from antlr4?
   */
  const std::filesystem::path &_pkg_path;
  Document::Owner   _document;
};

}
