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
  using Annotation  = orthodocs::doc::Annotation;
  using Dictionary  = orthodocs::doc::xref::Dictionary;
  using Document    = orthodocs::Document;
  using Function    = scad::doc::Function;
  using Item        = orthodocs::doc::Item;
  using Module      = scad::doc::Module;
  using Package     = scad::doc::Package;
  using Parameter   = orthodocs::doc::Parameter;
  using SubToC      = orthodocs::doc::SubToC;
  using ToC         = orthodocs::doc::ToC;
  using Variable    = scad::doc::Variable;

  static constexpr const char * const ID = "markdown";

  static writer::Extension *builder(std::string_view writer_id,Dictionary &dict,const language::Extension *lang);
  explicit Extension(Dictionary &dict,const language::Extension *lang) : writer::Extension(ID,dict,lang) {}

  void save(const Document &doc) override;
  void save(const ToC &toc) override;
  void graphs(const ToC &toc, const FileSet &dirs) override;
  /**
   * write an internal reference when document is null, internal or external otherwise
   */
  std::string reference(const Item *item, const std::filesystem::path *document_source=nullptr) const override;

private:
  void graph(const Package &pkg, std::ostream &out) const;
  void subToc(const std::filesystem::path &document_source, const SubToC &sub, std::ostream &out, char current) const;

  void write(const Document &document, const Package    *pkg,   std::ostream &out) const;
  void write(const Document &document, const Parameter  *param, std::ostream &out) const;
  void write(const Document &document, const Function   *func,  std::ostream &out) const;
  void write(const Document &document, const Module     *mod,   std::ostream &out) const;
  void write(const Document &document, const Variable   *var,   std::ostream &out) const;

  /*
   * Annotation(s) need a specific method for the cross-reference expansion
   */
  void write(const Document &document, const Annotation &annotation, std::ostream &out) const;

  template <class T>
  void write(const Document &document, const Document::Topic<T> &topic, std::ostream &out) const {
    if (topic.items.size()) {
      out << H(topic.title,2)
          << std::endl;
      std::for_each(
        topic.items.begin(),
        topic.items.end(),
        [this, &document, &out] (const typename decltype(topic.items)::value_type &value) {
          write(document,value,out);
        }
      );
    }
  }

  std::string BOLD(const std::string &text) const;
  std::string BR() const;
  std::string CODE(const std::string &text) const;
  std::string H(const std::string &text,int level=1) const;
  std::string H(char c,int level=1) const;
  std::string HRULE() const;
  std::string ITALIC(const std::string &text) const;
};

} // namespace markdown
