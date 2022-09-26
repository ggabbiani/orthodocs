#pragma once
/*
 * OpenSCAD listener
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * ODOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ODOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ODOX.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "scad/document.h"

#include "SCADParserBaseListener.h"

#include <memory>

namespace scad {

class Listener : public scad::SCADParserBaseListener {
public:
  using Parser = scad::SCADParser;

  std::unique_ptr<orthodocs::Document> document;
  
  doc::Package      *curr_package;  // only one package during the parsing of a source
  orthodocs::doc::ItemPtrStack  curr_item;
  orthodocs::doc::ItemPtrStack  curr_variable;
  orthodocs::doc::ParameterPtr  curr_parameter;

  explicit Listener(const std::filesystem::path &pkg_source);

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
  std::filesystem::path _pkg_path;
};

}
