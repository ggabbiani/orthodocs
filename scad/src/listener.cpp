
#include "annotations.h"
#include "listener.h"
#include "globals.h"
#include "utils.h"

#include <regex>

using namespace std;

namespace fs=std::filesystem;

namespace scad {

Listener::Listener(const std::filesystem::path &pkg_source) : _pkg_path(pkg_source) {
}

void Listener::enterPkg(scad::SCADParser::PkgContext *ctx) {
  curr_package = new doc::Package(_pkg_path);
  curr_item.push(doc::ItemPtr(curr_package));
}

void Listener::exitPkg(scad::SCADParser::PkgContext *ctx) {
  auto &item  = curr_item.top();
  document[item->documentKey()] = move(item);
  curr_item.pop();
  curr_package  = nullptr;
}

void Listener::enterIncl(scad::SCADParser::InclContext *ctx) {
  // try {
  // change into directory of the current package
  cwd pwd(_pkg_path.parent_path());

  string      inc_file = ctx->FILE()->getText();
  fs::path    inc_path = inc_file.substr(1,inc_file.length()-2); // eliminates angular brackets
  error_code  error;  // we manage fs error: no need for exception here...
  auto        inc_canonical = fs::canonical(inc_path,error);

  if (!static_cast<bool>(error) && is_sub_of(inc_canonical,option::sroot)) {
    auto requisite = fs::relative(inc_canonical,option::sroot);
    curr_package->includes.emplace((requisite.parent_path()/requisite.stem()).string());
  }
  // } catch(const fs::filesystem_error &error) {
  //   cout << error.code().value() << ',' << error.code().category().name() << endl;
  //   throw;
  // }
}

void Listener::enterUse(SCADParser::UseContext *ctx) {
  // change into directory of the current package
  cwd pwd(_pkg_path.parent_path());

  string      use_file = ctx->FILE()->getText();
  fs::path    use_path = use_file.substr(1,use_file.length()-2); // eliminates angular brackets
  error_code  error;  // we manage fs error: no need for exception here...
  auto        use_canonical = fs::canonical(use_path,error);

  if (!static_cast<bool>(error) && is_sub_of(use_canonical,option::sroot)) {
    auto requisite = fs::relative(use_canonical,option::sroot);
    curr_package->uses.emplace((requisite.parent_path()/requisite.stem()).string());
  }
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
