#include "generator.h"
#include "utils.h"

using namespace std;

namespace fs=boost::filesystem;

namespace doc {

Generator::Generator(const fs::path &source) : _package(source.filename().stem().string()) {
  items[_package] = make_unique<doc::Item>(_package,Item::package);
}

void Generator::enterFunction_def(scad::SCADParser::Function_defContext *ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = !curr_item.empty();
  curr_item.push(make_unique<doc::Item>(identifier,Item::function,nested));
}

void Generator::exitFunction_def(scad::SCADParser::Function_defContext *ctx)  {
  auto name = curr_item.top()->name;
  if (!priv(name))
    items[name] = move(curr_item.top());
  curr_item.pop();
}

void Generator::enterModule_def(scad::SCADParser::Module_defContext * ctx) {
  auto identifier = ctx->ID()->getText();
  auto nested     = !curr_item.empty();
  curr_item.push(make_unique<doc::Item>(identifier,Item::module,nested));
}

void Generator::exitModule_def(scad::SCADParser::Module_defContext * ctx) {
  auto name = curr_item.top()->name;
  if (!curr_item.top()->nested || !priv(name))
    items[name] = move(curr_item.top());
  curr_item.pop();
}

void Generator::enterAnnotation(scad::SCADParser::AnnotationContext *ctx) {
  // remove comment start
  auto value  = ctx->getText().substr(4);
  // remove comment end
  auto pos = value.find_last_of("*/");
  if (pos!=string::npos)
    value = value.erase(pos-1,string::npos);
  // trim leading/trailing white spaces
  value = trim(value);

  if (dynamic_cast<scad::SCADParser::ParameterContext*>(ctx->parent->parent->parent)) {   // parameter's annotation
    curr_parameter->annotation = value;
  } else if (dynamic_cast<scad::SCADParser::Function_defContext*>(ctx->parent->parent)) { // function's annotation
    curr_item.top()->annotation = value;
  } else if (dynamic_cast<scad::SCADParser::Module_defContext*>(ctx->parent->parent)) {   // module's annotation
    curr_item.top()->annotation = value;
  } else if (dynamic_cast<scad::SCADParser::AssignmentContext*>(ctx->parent->parent)) {   // variable's annotation
    curr_variable.top()->annotation = value;
  }
}

void Generator::enterParameter(scad::SCADParser::ParameterContext *ctx) {
  curr_parameter  = make_unique<doc::Parameter>();
}

void Generator::exitParameter(scad::SCADParser::ParameterContext *ctx) {
  if (!curr_item.empty()) {
    curr_item.top()->parameters.push_back(move(curr_parameter));
  }
}

void Generator::enterLookup(scad::SCADParser::LookupContext *ctx) {
  auto value = ctx->ID()->getText();
  if (curr_parameter) {
    if (dynamic_cast<scad::SCADParser::ParameterContext*>(ctx->parent)) 
      curr_parameter->name = value;
  }
}

void Generator::enterAssignment(scad::SCADParser::AssignmentContext *ctx) {
  auto id = ctx->ID()->getText();
  if (dynamic_cast<scad::SCADParser::StatContext*>(ctx->parent)) {
    auto nested     = !curr_item.empty();
    Item *variable  = new Item(id,Item::variable,nested);
    variable->defaults = ctx->expr()->getText();
    curr_variable.push(ItemPtr(variable));
  } else if (curr_parameter) {
    curr_parameter->name = id;
  }
}

void Generator::exitAssignment(scad::SCADParser::AssignmentContext *ctx) {
  if (dynamic_cast<scad::SCADParser::StatContext*>(ctx->parent)) {
    if (curr_variable.size()) {
      auto id = curr_variable.top()->name;
      if (!curr_variable.top()->nested && !priv(id))
        items[id] = move(curr_variable.top());
      curr_variable.pop();
    }
  }
}

}
