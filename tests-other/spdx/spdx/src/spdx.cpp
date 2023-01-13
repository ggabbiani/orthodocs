/*
 * spdx implementation file
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2023, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "spdx.h"

#include "SPDXLexer.h"
#include "SPDXParser.h"

#include <filesystem>
#include <iostream>

using namespace std;
using namespace antlr4;

namespace fs = std::filesystem;

namespace {

void append(char c, string &to) {
  if (!to.empty())
    to.push_back(' ');
  to.push_back(c);
}

void append(const string_view &s, string &to) {
  if (!to.empty())
    to.push_back(' ');
  to.append(s);
}

void append(antlr4::tree::TerminalNode *node, string &to) {
  append(node->getText(),to);
}

}

namespace spdx {

namespace license {

const string& LabelSet::operator [] (LabelType type) const {
  static const array< string, 6> label = {
    "name",
    "licenseId",
    "licenses",
    "licenseListVersion",
    "releaseDate",
    "detailsUrl",
  };
  return label[to_underlying(type)]; 
}

}

namespace exception {

const string& LabelSet::operator [] (LabelType type) const {
  static const array< string, 6> label = {
    "name",
    "licenseExceptionId",
    "exceptions",
    "licenseListVersion",
    "releaseDate",
    "detailsUrl",
  };
  return label[to_underlying(type)]; 
}

}

void Listener::exitLicense_and_beyond(License_and_beyondContext *ctx) {
  auto id   = ctx->ID()->getText();
  auto data = make_unique<spdx::Data>();

  data->position  = ctx->ID()->getSymbol()->getStartIndex();
  data->length    = ctx->ID()->getSymbol()->getStopIndex() - data->position + 1;
  const spdx::LicenseList::Item *lic  = nullptr;
  if (ctx->PLUS()) {
    lic = _licenses.find(id+'+');
    if (lic)
      data->name  = lic->name();
    else {
      lic         = _licenses.find(id);
      data->name  = lic ? lic->name()+" or later" : id+" or later";
    }
  } else {
    lic         = _licenses.find(id);
    data->name  = lic ? lic->name() : id;
  }
  data->url         = lic ? fs::path(lic->url()).replace_extension(".html").string() : "";
  auto [i,success] = _analitics.try_emplace(data->position,move(data));
  assert(success);
}

void Listener::exitCompound_expression(Compound_expressionContext *ctx) {
  if (ctx->WITH()) {
    auto xid          = ctx->xid->getText();
    auto data         = make_unique<spdx::Data>();
    data->position    = ctx->xid->getStartIndex();
    data->length      = ctx->xid->getStopIndex() - data->position + 1;
    auto xcp          = _exceptions.find(xid);
    data->name        = (xcp ? xcp->name() : xid);
    data->url         = (xcp ? (fs::path("https://spdx.org/licenses") / fs::path(xcp->url()).filename()).string() : "");
    auto [i,success]  = _analitics.try_emplace(data->position,move(data));
    assert(success);
  }
}

void analize(Annotation &annotation,const LicenseList &licenses, const ExceptionList &exceptions) {
  ANTLRInputStream  in(annotation.data());
  SPDXLexer         lexer(&in);
  CommonTokenStream tokens(&lexer);
  SPDXParser        parser(&tokens);

  // parsing
  tree::ParseTree       *tree = parser.all();

  // parse tree depth-first traverse
  tree::ParseTreeWalker  walker;
  // source parse listener
  Listener listener(licenses,exceptions,annotation.analitics());
  // creation of the document
  walker.walk(&listener,tree);
}

}
