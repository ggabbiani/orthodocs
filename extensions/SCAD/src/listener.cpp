/*
 * OpenSCAD listener
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
// private
#include "listener.h"
// public
#include <scad/document.h>
#include <SCADLexer.h>
// components
#include <commons/error_info.h>
#include <commons/utils.h>
// system
#include <regex>

using namespace std;
using StyleFactory = Singleton<scad::doc::style::Factory>;

namespace fs=std::filesystem;

namespace {

/**
 * return the first annotation on the left of the current token (but NOT THE ROOT).
 *
 * The algorithm is based on BufferedTokenStream::getHiddenTokensToLeft(),
 * returning a collection of off DEFAULT_TOKEN_CHANNEL (hidden) tokens left-side
 * of the current context and up to the first token found on DEFAULT_TOKEN_CHANNEL
 * or EOF.
 *
 * NOTE: we exclude from the result the root comment.
 */
string prevAnnoData(antlr4::BufferedTokenStream *stream, const antlr4::ParserRuleContext *ctx) {
  auto factory      = StyleFactory::instance();
  // start token in current context
  auto firstToken   = ctx->getStart();
  // index from which start searching
  auto startIndex   = firstToken->getTokenIndex();
  // list of tokens from the COMMENTS channel with an index value preceding the start index
  auto leftComments = stream->getHiddenTokensToLeft(startIndex, scad::SCADLexer::COMMENTS);
  // search for usable annotation FROM THE END (i.e. from the nearest to our first token)
  for(auto i=leftComments.rbegin(); i!=leftComments.rend(); ++i) {
    // we exclude the root token that is always used by the package item
    if (const auto *token = *i; token->getTokenIndex()>0) {
      // we use the style factory as a sentinel for distinguish a normal
      // comment from an annotation (a decorated comment)
      if (auto comment = token->getText(); factory(comment)) {
        // we return the nearest comment on the left with a known decoration
        return comment;
      }
    }
  }
  return {};
}

/**
 * The root annotation is the 0-indexed token text conforming the annotation decoration.
 * When not found or not conformed the empty string is returned.
 */
string rootAnnoData(const antlr4::BufferedTokenStream *stream) {
  auto factory  = StyleFactory::instance();
  // first token must belong to the COMMENTS channel
  if (const auto *token = stream->get(0); token && token->getChannel()==scad::SCADLexer::COMMENTS) {
    auto comment = token->getText();
    if (factory(comment))
      return comment;
  }
  return {};
}

/**
 * return the first annotation on the right of the current token.
 *
 * The algorithm is based on BufferedTokenStream::getHiddenTokensToRight(),
 * returning a collection of off DEFAULT_TOKEN_CHANNEL (hidden) tokens right-side
 * of the current context and up to the first token found on DEFAULT_TOKEN_CHANNEL.
 *
 * NOTE: in such conditions no need to check/exclude the root comment.
 */
string nextAnnoData(antlr4::BufferedTokenStream *stream, const antlr4::ParserRuleContext *ctx) {
  auto factory    = StyleFactory::instance();
  // ending token in current context
  auto lastToken  = ctx->getStop();
  // index from which start searching
  auto startIndex = lastToken->getTokenIndex();
  // list of tokens from the COMMENTS channel with an index value following the start index
  auto rightComments  = stream->getHiddenTokensToRight(startIndex,scad::SCADLexer::COMMENTS);
  // search for usable annotation FROM THE START (i.e. from the nearest to our last token)
  for(const auto *token:rightComments) {
    // we use the style factory as a sentinel for distinguish a normal
    // comment from an annotation (a decorated comment)
    if (auto comment = token->getText(); factory(comment)) {
      // we return the nearest comment on the right with a known decoration
      return comment;
    }
  }
  return {};
}

}

namespace scad {

Listener::Listener(const fs::path &pkg_source,antlr4::BufferedTokenStream *s)
: _tokens(s), _pkg_path(pkg_source), _document(make_unique<::Document>(pkg_source)) {
}

void Listener::enterPkg(Parser::PkgContext *ctx) {
  curr_package = new doc::Package(_pkg_path);
  curr_item.push(::doc::Item::Owner(curr_package));

  // annotate if any root comment
  if (auto data = rootAnnoData(_tokens); !data.empty())
    annotate(curr_package, data);
}

void Listener::exitPkg(Parser::PkgContext *ctx) {
  auto &pkg  = curr_item.top();
#ifndef NDEBUG
  auto key  = pkg->documentKey();
#endif // NDEBUG

  if (auto [i,success] = _document->index.emplace(move(pkg)); !success)
    throw std::domain_error(ERR_INFO+"Duplicate key «"+(*i)->documentKey()+"» in same document");
  curr_item.pop();
  curr_package  = nullptr;
}

void Listener::enterIncl(Parser::InclContext *ctx) {
  try {
    // change into directory of the current package
    cwd pwd(_pkg_path.parent_path());

    string      inc_file = ctx->FILE()->getText();
    fs::path    inc_path = inc_file.substr(1,inc_file.length()-2); // eliminates angular brackets
    error_code  error;  // we manage fs error: no need for exception here...
    auto        inc_canonical = fs::canonical(inc_path,error);

    if (!static_cast<bool>(error) && is_sub_of(inc_canonical,Option::sroot())) {
      auto requisite = fs::relative(inc_canonical,Option::sroot());
      curr_package->includes.emplace((requisite.parent_path()/requisite.stem()).string());
    }
  } catch(...) {
    throw_with_nested(runtime_error(ERR_CALL(ctx)));
  }
}

void Listener::enterUse(SCADParser::UseContext *ctx) {
  // change into directory of the current package
  cwd pwd(_pkg_path.parent_path());

  string      use_file = ctx->FILE()->getText();
  fs::path    use_path = use_file.substr(1,use_file.length()-2); // eliminates angular brackets
  error_code  error;  // we manage fs error: no need for exception here...
  auto        use_canonical = fs::canonical(use_path,error);

  if (!static_cast<bool>(error) && is_sub_of(use_canonical,Option::sroot())) {
    auto requisite = fs::relative(use_canonical,Option::sroot());
    curr_package->uses.emplace((requisite.parent_path()/requisite.stem()).string());
  }
}

void Listener::enterFunction_def(Parser::Function_defContext *ctx) {
  auto identifier = ctx->ID()->getText();
  bool nested     = dynamic_cast<doc::Module*>(curr_item.top().get())!=nullptr;
  auto item       = new doc::Function(identifier,nested);
  item->parent    = curr_package;
  assert(curr_package);
  curr_item.emplace(item);
}

void Listener::exitFunction_def(Parser::Function_defContext *ctx)  {
  auto &func = curr_item.top();
#ifndef NDEBUG
  auto key  = func->documentKey();
#endif // NDEBUG

  if (!func->nested && !func->privateId()) {
    // annotate if any comment just before function definition
    if (auto data  = prevAnnoData(_tokens, ctx); !data.empty())
      annotate(func.get(), data);
    if (auto [i,success] = _document->index.emplace(move(func)); !success)
      throw std::domain_error(ERR_INFO+"Duplicate key «"+(*i)->documentKey()+"» in same document");
  }
  curr_item.pop();
}

void Listener::enterModule_def(Parser::Module_defContext * ctx) {
  auto identifier = ctx->ID()->getText();
  bool nested     = dynamic_cast<doc::Module*>(curr_item.top().get());
  auto item       = make_unique<doc::Module>(identifier,nested);
  assert(curr_package);
  item->parent    = curr_package;
  curr_item.emplace(item.release());
}

void Listener::exitModule_def(Parser::Module_defContext * ctx) {
  auto &mod = curr_item.top();
#ifndef NDEBUG
  auto  key = mod->documentKey();
#endif // NDEBUG

  if (!mod->nested && !mod->privateId()) {
    // annotate if any comment just before function definition
    if (auto data  = prevAnnoData(_tokens, ctx); !data.empty())
      annotate(mod.get(), data);
    if (auto [i,success] = _document->index.emplace(move(mod)); !success)
      throw std::domain_error(ERR_INFO+"Key «"+(*i)->documentKey()+"» already present in document");
  }
  curr_item.pop();
}


void Listener::enterParameter(Parser::ParameterContext *ctx) {
  assert(curr_parameter.get()==nullptr);
  curr_parameter  = make_unique<::doc::Parameter>();
}

void Listener::exitParameter(Parser::ParameterContext *ctx) {
#ifndef NDEBUG
  auto PACKAGE  = curr_package->name;
  auto ITEM     = curr_item.top()->name;
  auto TYPE     = curr_item.top()->type;
  auto PARAM    = curr_parameter->name;
  auto LEFT     = prevAnnoData(_tokens, ctx);
  auto RIGHT    = nextAnnoData(_tokens, ctx);
#endif // NDEBUG
  if (!curr_item.empty()) {
    if (auto data = Option::orthodox() ? prevAnnoData(_tokens, ctx) : nextAnnoData(_tokens, ctx); !data.empty())
      annotate(curr_parameter.get(),data);
    curr_item.top()->parameters.push_back(move(curr_parameter));
  }
}

void Listener::enterLookup(Parser::LookupContext *ctx) {
  auto value = ctx->ID()->getText();
  if (curr_parameter) {
    if (dynamic_cast<Parser::ParameterContext*>(ctx->parent))
      curr_parameter->name = value;
  }
}

void Listener::enterAssignment(Parser::AssignmentContext *ctx) {
  auto id       = ctx->ID()->getText();
  auto defaults = ctx->expr()->getText();
  if (dynamic_cast<Parser::StatContext*>(ctx->parent)) {
    auto nested   = dynamic_cast<doc::Module*>(curr_item.top().get())!=nullptr;
    auto variable = new doc::Variable(id,defaults,nested);
    assert(curr_package);
    variable->parent        = curr_package;
    curr_variable.push(::doc::Item::Owner(variable));
  } else if (curr_parameter) {
    curr_parameter->name      = id;
    curr_parameter->defaults  = defaults;
  }
}

void Listener::exitAssignment(Parser::AssignmentContext *ctx) {
  if (dynamic_cast<Parser::StatContext*>(ctx->parent) && curr_variable.size()) {
    if (auto &var = curr_variable.top(); !var->nested && !var->privateId()) {
      if (auto data = prevAnnoData(_tokens, ctx); !data.empty())
        annotate(curr_variable.top().get(),data);
      if (auto [i,success] = _document->index.emplace(move(var)); !success)
        throw std::domain_error(ERR_INFO+"Key «"+(*i)->documentKey()+"» already present in document");
    }
    curr_variable.pop();
  }
}

}
