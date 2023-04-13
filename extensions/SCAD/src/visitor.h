#pragma once
/*
 * OpenSCAD language visitor
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// private
#include "annotations.h"

// public
#include <scad/document.h>
#include <SCADParserBaseVisitor.h>
#include <SCADLexer.h>

// components
#include <commons/globals.h>

// system
#include <filesystem>

namespace scad {

class Visitor : public SCADParserBaseVisitor, private ::doc::Annotation::Setter {
  using InclContext       = SCADParser::InclContext;
  using Item              = ::doc::Item;
  using path              = std::filesystem::path;
  using Package           = scad::doc::Package;
  using ParserRuleContext = antlr4::ParserRuleContext;
  using PkgContext        = SCADParser::PkgContext;
  using StatInclContext   = SCADParser::StatInclContext;
  using StatUseContext    = SCADParser::StatUseContext;
  using Token             = antlr4::Token;
  using UseContext        = SCADParser::UseContext;
public:
  explicit Visitor(const path &pkg_source, antlr4::BufferedTokenStream *s);

  std::any visitPkg(PkgContext *ctx) override;
  std::any visitIncl(InclContext *ctx) override;
  std::any visitUse(UseContext *ctx) override;

  // return the Document's ownership to the calling environment
  Document::Owner releaseDocument() {return std::move(_document);};

private:

  template <class NOTABLE>
  void annotate(NOTABLE *item, const Token *comment) const {
    static doc::style::Factory factory;
    auto anno = comment->getText();
    if (auto style = factory(anno)) {
      auto value  = style->manage(anno);
      if (cli::admonitions())
        mk_admonitions(value);
      set(item->annotation, value);
    }
  }

  const Token *prevComment(const ParserRuleContext *ctx) const;
  const Token *nextComment(const ParserRuleContext *ctx) const;

  antlr4::BufferedTokenStream *_tokens;
  /*
   * this is a kludge: is there an official way for retrieving the current
   * language source file from antlr4?
   */
  const path       &_pkg_path;
  Document::Owner   _document;
  Package          *_curr_package;
  Item::OwnerStack  _curr_item;
};

} // namespace scad
