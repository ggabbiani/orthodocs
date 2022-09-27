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

#include "error_info.h"
#include "scad/annotations.h"
#include "scad/listener.h"
#include "orthodocs/globals.h"
#include "orthodocs/utils.h"

#include <regex>

using namespace std;

namespace fs=std::filesystem;

namespace scad {

Listener::Listener(const fs::path &pkg_source) : document(make_unique<orthodocs::Document>(pkg_source)),_pkg_path(pkg_source) {
}

void Listener::enterPkg(scad::SCADParser::PkgContext *ctx) {
  curr_package = new doc::Package(_pkg_path);
  curr_item.push(orthodocs::doc::ItemPtr(curr_package));
}

void Listener::exitPkg(scad::SCADParser::PkgContext *ctx) {
  auto &item  = curr_item.top();
  document->index.emplace(item->documentKey(),move(item));
  curr_item.pop();
  curr_package  = nullptr;
}

void Listener::enterIncl(scad::SCADParser::InclContext *ctx) {
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

void Listener::enterFunction_def(scad::SCADParser::Function_defContext *ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = is<doc::Module>(*curr_item.top());
  auto item       = new doc::Function(identifier,nested);
  item->parent    = curr_package;
  assert(curr_package);
  // curr_item.push(make_unique<doc::Function>(identifier,nested));
  curr_item.emplace(item);
}

void Listener::exitFunction_def(scad::SCADParser::Function_defContext *ctx)  {
  auto &func = curr_item.top();
  auto key  = func->documentKey();
  if (!func->privateId())
    document->index.emplace(key,move(func));
  curr_item.pop();
}

void Listener::enterModule_def(scad::SCADParser::Module_defContext * ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = is<doc::Module>(*curr_item.top());
  auto item       = new doc::Module(identifier,nested);
  item->parent    = curr_package;
  assert(curr_package);
  curr_item.emplace(item);
  // curr_item.push(make_unique<doc::Module>(identifier,nested));
}

void Listener::exitModule_def(scad::SCADParser::Module_defContext * ctx) {
  // TODO: implement the whole piece of code as a Document function
  auto &module  = curr_item.top();
  auto key      = module->documentKey();
  if (!module->nested && !module->privateId())
    document->index.emplace(key,move(module));
  curr_item.pop();
}

void Listener::enterAnnotation(scad::SCADParser::AnnotationContext *ctx) {
  static doc::style::Factory factory;

  auto anno   = ctx->getText();
  auto style  = factory(anno);
  auto value  = style->manage(anno);

  if (Option::admonitions)
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
    auto package = dynamic_cast<scad::doc::Package *>(curr_item.top().get());
    // search and subsitute any - known - license note
    if (auto lic = doc::License::remove(value))
      package->license = lic->name;
    package->annotation = value;
  }
}

void Listener::enterParameter(scad::SCADParser::ParameterContext *ctx) {
  curr_parameter  = make_unique<orthodocs::doc::Parameter>();
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
    assert(curr_package);
    variable->parent        = curr_package;
    curr_variable.push(orthodocs::doc::ItemPtr(variable));
  } else if (curr_parameter) {
    curr_parameter->_name      = id;
    curr_parameter->defaults  = defaults;
  }
}

void Listener::exitAssignment(scad::SCADParser::AssignmentContext *ctx) {
  if (dynamic_cast<scad::SCADParser::StatContext*>(ctx->parent)) {
    if (curr_variable.size()) {
      auto &var   = curr_variable.top();
      auto key    = var->documentKey();
      if (!var->nested && !var->privateId())
        document->index.emplace(key, move(var));
      curr_variable.pop();
    }
  }
}

}
