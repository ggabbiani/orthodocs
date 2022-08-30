
#include "annotations.h"
#include "listener.h"
#include "globals.h"
#include "utils.h"

#include <regex>

using namespace std;

namespace fs=std::filesystem;

namespace scad {

Listener::Listener(const char *pkg_name) : _package(pkg_name) {
}

void Listener::enterPkg(scad::SCADParser::PkgContext *ctx) {
  curr_item.push(make_unique<doc::Package>(_package));
}

void Listener::exitPkg(scad::SCADParser::PkgContext *ctx) {
  auto index    = "package "+_package;
  document[index]  = move(curr_item.top());
  curr_item.pop();
}

void Listener::enterFunction_def(scad::SCADParser::Function_defContext *ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = is<doc::Module>(*curr_item.top());
  curr_item.push(make_unique<doc::Function>(identifier,nested));
}

void Listener::exitFunction_def(scad::SCADParser::Function_defContext *ctx)  {
  auto name   = curr_item.top()->name;
  auto index  = string("function ")+name;
  if (!priv(name))
    document[index] = move(curr_item.top());
  curr_item.pop();
}

void Listener::enterModule_def(scad::SCADParser::Module_defContext * ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = is<doc::Module>(*curr_item.top());
  curr_item.push(make_unique<doc::Module>(identifier,nested));
}

void Listener::exitModule_def(scad::SCADParser::Module_defContext * ctx) {
  auto name   = curr_item.top()->name;
  auto index  = "module "+name;
  if (!curr_item.top()->nested && !priv(name))
    document[index] = move(curr_item.top());
  curr_item.pop();
}

void Listener::enterAnnotation(scad::SCADParser::AnnotationContext *ctx) {
  static doc::style::Factory factory;

  auto anno   = ctx->getText();
  auto style  = factory(anno);
  auto value  = style->manage(anno);

  if (option::admonitions)
    mk_admonitions(value);

  // FIXME: a sigle if with multiple OR sould be ok
  if (is<scad::SCADParser::ParameterContext>(*ctx->parent->parent->parent)) {   // parameter's annotation
    curr_parameter->_annotation = value;
  } else if (is<scad::SCADParser::Function_defContext>(*ctx->parent->parent)) { // function's annotation
    curr_item.top()->annotation = value;
  } else if (is<scad::SCADParser::Module_defContext>(*ctx->parent->parent)) {   // module's annotation
    curr_item.top()->annotation = value;
  } else if (is<scad::SCADParser::AssignmentContext>(*ctx->parent->parent)) {   // variable's annotation
    curr_variable.top()->annotation = value;
  } else if (is<scad::SCADParser::PkgContext>(*ctx->parent->parent))        {   // package's annotation
    curr_item.top()->annotation = value;
  }
}

void Listener::enterParameter(scad::SCADParser::ParameterContext *ctx) {
  curr_parameter  = make_unique<doc::Parameter>();
}

void Listener::exitParameter(scad::SCADParser::ParameterContext *ctx) {
  if (!curr_item.empty()) {
    curr_item.top()->parameters.push_back(move(curr_parameter));
  }
}

void Listener::enterLookup(scad::SCADParser::LookupContext *ctx) {
  auto value = ctx->ID()->getText();
  if (curr_parameter) {
    if (is<scad::SCADParser::ParameterContext>(*ctx->parent)) 
      curr_parameter->_name = value;
  }
}

void Listener::enterAssignment(scad::SCADParser::AssignmentContext *ctx) {
  auto id       = ctx->ID()->getText();
  auto defaults = ctx->expr()->getText();
  if (dynamic_cast<scad::SCADParser::StatContext*>(ctx->parent)) {
    auto nested             = is<doc::Module>(*curr_item.top());
    doc::Variable *variable = new doc::Variable(id,defaults,nested);
    curr_variable.push(doc::ItemPtr(variable));
  } else if (curr_parameter) {
    curr_parameter->_name      = id;
    curr_parameter->defaults  = defaults;
  }
}

void Listener::exitAssignment(scad::SCADParser::AssignmentContext *ctx) {
  if (dynamic_cast<scad::SCADParser::StatContext*>(ctx->parent)) {
    if (curr_variable.size()) {
      auto id     = curr_variable.top()->name;
      auto index  = "variable "+id;
      if (!curr_variable.top()->nested && !priv(id))
        document[index] = move(curr_variable.top());
      curr_variable.pop();
    }
  }
}

}
