#pragma once
/**
 * language extension declarations
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <commons/extensions.h>

#include <array>
#include <regex>

namespace scad {

class Extension : public language::Extension, private ::doc::Annotation::Setter {
public:
  using Annotation = ::doc::Annotation;

  static constexpr const char *ID = "scad";

  static language::Extension *builder(std::string_view language_id);
  Extension();

  Document::Owner parse(const std::filesystem::path &source) const override;
  void analyze(Annotation &anno) const override;

  const char *sourcePostfix() const override;

  enum Type {
    TYPE_FUNCTION,
    TYPE_MODULE,
    TYPE_PACKAGE,
    TYPE_VARIABLE
  };

  struct Slot {
    std::string type;
    std::string (*dictionaryKey)(const ::doc::Item*);
    std::regex  regularExpression;
  };

  static const std::array<Slot,4> slot;

};

} // namespace scad
