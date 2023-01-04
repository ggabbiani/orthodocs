#include "spdx.h"

#include "SPDXLexer.h"
#include "SPDXParser.h"

#include <iostream>

using namespace std;
using namespace antlr4;

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
  static const array< string, 5> label = {
    "name",
    "licenseId",
    "licenses",
    "licenseListVersion",
    "releaseDate",
  };
  return label[to_underlying(type)]; 
}

}

namespace exception {

const string& LabelSet::operator [] (LabelType type) const {
  static const array< string, 5> label = {
    "name",
    "licenseExceptionId",
    "exceptions",
    "licenseListVersion",
    "releaseDate",
  };
  return label[to_underlying(type)]; 
}

}

std::pair<std::string,std::string> filter(const string &annotation,const LicenseList &licenses, const ExceptionList &exceptions) {
  ANTLRInputStream  in(annotation);
  SPDXLexer         lexer(&in);
  CommonTokenStream tokens(&lexer);
  SPDXParser        parser(&tokens);

  // parsing
  tree::ParseTree       *tree = parser.all();

  // parse tree depth-first traverse
  tree::ParseTreeWalker  walker;
  // source parse listener
  Listener listener(licenses,exceptions);
  // creation of the document
  walker.walk(&listener,tree);

  stringstream  ss;
  auto result = tokens.getTokens();
  for(const auto *token: result)
    if (token->getChannel()==lexer.ANNOTATION)
      ss << token->getText();

  return make_pair(listener.license(),ss.str());
}

void Listener::exitLicense_and_beyond(License_and_beyondContext *ctx) {
  string text;
  auto id   = ctx->id->getText();
  if (ctx->PLUS()) {
    if (auto lic=_licenses.find(id+'+'); lic)
      text  = lic ? lic->name() : id;
    else {
      lic   = _licenses.find(id);
      text  = lic ? lic->name()+" or later": id+" or later";
    }
  } else {
    auto lic  = _licenses.find(id);
    text      = lic ? lic->name() : id;
  }
  append(text,_licensing);
  if (_op.size()) {
    append(_op.top(),_licensing);
    _op.pop();
  }
}

void Listener::enterCompound_expression(Compound_expressionContext *ctx) {
  if (ctx->LEFT_PAREN())
    append('(',_licensing);
  else if (ctx->OR())
    _op.emplace("or");
  else if (ctx->AND())
    _op.emplace("and");
}

void Listener::exitCompound_expression(Compound_expressionContext *ctx) {
  if (ctx->RIGHT_PAREN())
    append(')',_licensing);
  else if (ctx->WITH() && ctx->op) {
    auto xid  = ctx->xid->getText();
    auto xcp  = _exceptions.find(xid);
    auto text = "with " + (xcp ? xcp->name() : xid);
    append(text, _licensing);
  }
}

}
