#pragma once
/*
 * OpenSCAD listener
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "annotations.h"

#include <scad/document.h>
#include <commons/globals.h>
#include <commons/singleton.h>
#include <SCADParserBaseListener.h>

#include <memory>

namespace scad {

/**
 * This listener is triggered during the parse-tree walk. The resulting synthetic
 * document ownership is then passed to the calling environment through the
 * documentRelease() call.
 */
class Listener : public scad::SCADParserBaseListener, protected ::doc::Annotation::Setter {
public:
  using BufferedTokenStream   = antlr4::BufferedTokenStream;
  using Function_defContext   = SCADParser::Function_defContext;
  using Item                  = ::doc::Item;
  using Package               = doc::Package;
  using Parameter             = ::doc::Parameter;
  using Parser                = scad::SCADParser;
  using ParserRuleContext     = antlr4::ParserRuleContext;
  using StyleFactory          = Singleton<scad::doc::style::Factory>;
  using Token                 = antlr4::Token;
  using path                  = std::filesystem::path;

  Listener(const path &pkg_source,antlr4::BufferedTokenStream *s, Document::Id doc_id);

  // return the Document's ownership to the calling environment
  Document::Owner releaseDocument() {return std::move(_document);};

  void enterAssignment(Parser::AssignmentContext *ctx)      override;
  void enterFunction_def(Parser::Function_defContext *ctx)  override;
  void enterIncl(Parser::InclContext *ctx)                  override;
  void enterLookup(Parser::LookupContext *ctx)              override;
  void enterModule_def(Parser::Module_defContext * ctx)     override;
  void enterParameter(Parser::ParameterContext *ctx)        override;
  void enterPkg(Parser::PkgContext *ctx)                    override;
  void enterUse(Parser::UseContext *ctx)                    override;
  void exitAssignment(Parser::AssignmentContext *ctx)       override;
  void exitFunction_def(Parser::Function_defContext *ctx)   override;
  void exitModule_def(Parser::Module_defContext * ctx)      override;
  void exitParameter(Parser::ParameterContext *ctx)         override;
  void exitPkg(Parser::PkgContext *ctx)                     override;

private:
  /*
   * Enrich the item with annotation data (a valid decorated comment)
   */
  template <class ANNOTABLE>
  void annotate(ANNOTABLE *item, const std::string &comment) const {
    // comment has been already checked, so style is !=nullptr
    auto data = StyleFactory::instance()(comment)->manage(comment);
    if (cli::admonitions())
      mk_admonitions(data);
    set(item->annotation, data);
  }

  antlr4::BufferedTokenStream *_tokens;
  // the following members represents the actual status variables for the parser
  Package          *curr_package;
  Item::OwnerStack  curr_item;
  Item::OwnerStack  curr_variable;
  Parameter::Owner  curr_parameter;
  /*
   * this is a kludge: is there an official way for retrieving the current
   * language source file from antlr4?
   */
  const std::filesystem::path &_pkg_path;
  Document::Owner   _document;
};

}
