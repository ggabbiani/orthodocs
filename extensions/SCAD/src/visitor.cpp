/*
 * insert a brief description here
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

// private
#include "visitor.h"

// components
#include <commons/globals.h>

using namespace std;
namespace fs  = std::filesystem;

namespace scad {

Visitor::Visitor(const path &pkg_source, antlr4::BufferedTokenStream *s)
  : _tokens(s),
    _pkg_path(pkg_source),
    _document(make_unique<::Document>(pkg_source)) {
}

any Visitor::visitPkg(PkgContext *ctx) {
  _curr_package = new Package(_pkg_path);
  _curr_item.push(Item::Owner(_curr_package));

  for(auto statement: ctx->stat()) {
    if (auto incl=dynamic_cast<StatInclContext*>(statement)) {
      visit(incl);
    }
    if (auto use=dynamic_cast<StatUseContext*>(statement)) {
      visit(use);
    }
  }

#ifndef NDEBUG
  auto KEY  = _curr_package->documentKey();
#endif // NDEBUG

  // annotate if any comment at source' beginning
  if (auto comment  = prevComment(ctx))
    annotate(_curr_package, comment);

  if (auto [i,success] = _document->index.emplace(move(_curr_item.top())); !success)
    throw std::domain_error(ERR_INFO+"Duplicate key «"+(*i)->documentKey()+"» in same document");
  _curr_item.pop();
  _curr_package  = nullptr;

  return any();
}

any Visitor::visitIncl(InclContext *ctx) {
  try {
    // change into directory of the current package
    cwd pwd(_pkg_path.parent_path());

    string      inc_file = ctx->FILE()->getText();
    path        inc_path = inc_file.substr(1,inc_file.length()-2); // eliminates angular brackets
    error_code  error;  // fs error can be managed here, no need for exceptions
    auto        inc_canonical = fs::canonical(inc_path,error);

    if (!static_cast<bool>(error) && is_sub_of(inc_canonical,Option::sroot())) {
      auto requisite = fs::relative(inc_canonical,Option::sroot());
      _curr_package->includes.emplace((requisite.parent_path()/requisite.stem()).string());
    }
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(ctx)));
  }
  return any();
}

any Visitor::visitUse(UseContext *ctx) {
  // change into directory of the current package
  cwd pwd(_pkg_path.parent_path());

  string      use_file = ctx->FILE()->getText();
  path        use_path = use_file.substr(1,use_file.length()-2); // eliminates angular brackets
  error_code  error;  // fs error can be managed here, no need for exceptions
  auto        use_canonical = fs::canonical(use_path,error);

  if (!static_cast<bool>(error) && is_sub_of(use_canonical,Option::sroot())) {
    auto requisite = fs::relative(use_canonical,Option::sroot());
    _curr_package->uses.emplace((requisite.parent_path()/requisite.stem()).string());
  }
  return any();
}

auto Visitor::prevComment(const ParserRuleContext *ctx) const -> const Token* {
  auto firstToken   = ctx->getStart();
  auto i            = firstToken->getTokenIndex();
  auto leftComments = _tokens->getHiddenTokensToLeft(i,scad::SCADLexer::COMMENTS);
  return leftComments.empty() ? nullptr : leftComments.back();
}

auto Visitor::nextComment(const ParserRuleContext *ctx) const -> const Token* {
  auto lastToken      = ctx->getStop();
  auto i              = lastToken->getTokenIndex();
  auto rightComments  = _tokens->getHiddenTokensToRight(i,scad::SCADLexer::COMMENTS);
  return rightComments.empty() ? nullptr : rightComments.front();
}

} // namespace scad
