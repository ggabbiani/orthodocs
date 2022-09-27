#pragma once
/*
 * markdown extension declaration
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

#include "orthodocs/extensions.h"
#include "scad/document.h"

namespace markdown {

class Extension : public writer::Extension {

public:
  static constexpr const char * const ID = "markdown";
  Extension() : writer::Extension(ID) {}

  void save(const orthodocs::Document &doc) override;
  void save(const orthodocs::doc::ToC &toc) override;
  void graphs(const orthodocs::doc::ToC &toc, const FileSet &dirs) override;

private:
  void graph(const scad::doc::Package &pkg, std::ostream &out) const;
  void subToc(const orthodocs::doc::SubToC &sub, std::ostream &out, char current) const;

  void write(const scad::doc::Package         *pkg,   std::ostream &out) const;
  void write(const orthodocs::doc::Parameter  *param, std::ostream &out) const;
  void write(const scad::doc::Function        *func,  std::ostream &out) const;
  void write(const scad::doc::Module          *mod,   std::ostream &out) const;
  void write(const scad::doc::Variable        *var,   std::ostream &out) const;

  std::string BOLD(const std::string &text) const;
  std::string BR() const;
  std::string CODE(const std::string &text) const;
  std::string H(const std::string &text,int level=1) const;
  std::string H(char c,int level=1) const;
  std::string HRULE() const;
  std::string ITALIC(const std::string &text) const;
};

} // namespace markdown
