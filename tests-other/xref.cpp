/*
 * reads an annotation from std input and search for a cross reference
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <scad/document.h>

#include <spdlog/spdlog.h>

#include <iostream>
#include <regex>
#include <string>

using namespace std;

template <typename E>
constexpr typename std::underlying_type<E>::type underlying(E e) noexcept {
  return static_cast<typename std::underlying_type<E>::type>(e);
}

class CrossReference {
public:
  enum class Type {
    function,
    module,
    variable,
    package
  };
  struct Analysis {
    using Results = map<size_t,Analysis>;
    int       rule;
    ptrdiff_t position;
    ptrdiff_t length;
    string    token;
    Type      type;
  };
  using Dictionary  = map<string,string>;

  CrossReference(const char *name,const char *regex) : name(name),regex(regex) {}

  /**
   * Verify the presence of a known cross-reference inside «s».
   * Return a 'ready' smatch in case of success, empty otherwise.
   */
  cmatch match(const char *s) const {
    cmatch result;
    return regex_search(s, result, regex) ? result : cmatch();
  }

  static string manage(string s) {
    Analysis::Results results;
    analize(s,results);
    return apply(results,s);
  }

  /**
   * Analize «s» searching for a known pattern. If any, register it in the cross-reference.
   */
  static void analize(const std::string &s,Analysis::Results &result) {
    static CrossReference rules[]={
      {"Function",  "([a-zA-Z_][a-zA-Z0-9_]*)\\(\\)"},
      {"Module",    "([a-zA-Z_][a-zA-Z0-9_]*)\\{\\}"},
      {"Variable",  "variable ([a-zA-Z_][a-zA-Z0-9_]*)"},
      {"Package",   "package ([a-zA-Z_][a-zA-Z0-9_]*)"},
    };
    // first phase: fetch references position/length
    for(auto i=0;i<4;++i) {
      const char *t = s.c_str();
      cmatch match = rules[i].match(t);
      while(match.ready()) {
        auto offset = (t-s.c_str());
        Analysis analysis{i,match.position(0)+offset,match.length(0),s.substr(match.position(i>underlying(Type::module)?1:0)+offset,match.length(1)),static_cast<Type>(i)};
        result.emplace(analysis.position,analysis);
        t += analysis.position+analysis.length;
        match = rules[i].match(t);
      }
    }
  }

  static string apply(const Analysis::Results &results, string s) {
    for_each(results.rbegin(), results.rend(),
      [&s] (const Analysis::Results::value_type &value) {
        const auto &res = value.second;
        if (auto i=dictionary.find(res.token); i!=dictionary.end()) {
          string ref = "["+res.token+"]("+i->second+")";
          s.replace(res.position,res.length,ref);
        } else {
          // FIXME: it would help to have also an indication of the item for which the warn was emitted
          spdlog::warn("item {} not present in dictionary",res.token);
        }
      }
    );
    return s;
  }

  const string  name;
  std::regex    regex;
  static Dictionary dictionary;
};

CrossReference::Dictionary CrossReference::dictionary = {
  {"function_1",        "path_a/package_a.md#function-function_1"},
  {"function_2",        "path_a/package_a.md#function-function_2"},
  {"function_3",        "path_a/package_b.md#function-function_3"},
  {"module_I",          "path_a/package_a.md#module-module_I"},
  {"module_II",         "path_a/package_b.md#module-module_II"},
  {"module_III",        "package_c.md#module-module_III"},
  {"variable_A",        "path_a/package_a.md#variable-variable_A"},
  {"variable_B",        "path_a/package_a.md#variable-variable_B"},
  {"variable_C",        "path_a/package_a.md#variable-variable_C"},
  {"path_a/package_a",  "path_a/package_a.md#package-package_a"},
  {"path_a/package_b",  "path_a/package_b.md#package-package_b"},
  {"path_b/package_a",  "path_b/package_a.md#package-package_a"},
  {"path_c/package_c",  "path_c/package_c.md#package-package_c"},
  {"package_c",         "package_c.md#package-package_c"},
};

const char *annotation=
"/*!\n"
"this is a reference to function_1() that\n"
"should be recognized.\n"
"This (module_II{}) is instead a reference to a module in package package_c.\n"
"Variables are referrable too (see variable variable_A).\n"
"There is also a last function_2() function.\n"
"*/";

int main() {
  try {
    scad::doc::style::Factory factory;
    auto style  = factory(annotation);
    string anno = style->manage(annotation);

    anno = CrossReference::manage(anno);

    return EXIT_SUCCESS;
  } catch(const exception &error) {
    print_exception(error);
    return  EXIT_FAILURE;
  }
}
