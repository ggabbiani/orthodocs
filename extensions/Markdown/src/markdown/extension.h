#pragma once
/*
 * markdown extension declaration
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <scad/document.h>
#include <commons/extensions.h>

namespace markdown {

class Extension : public writer::Extension {
public:
  using Annotation  = ::doc::Annotation;
  using Dictionary  = ::doc::xref::Dictionary;
  using Function    = scad::doc::Function;
  using Item        = ::doc::Item;
  using Module      = scad::doc::Module;
  using Package     = scad::doc::Package;
  using Parameter   = ::doc::Parameter;
  using SubToC      = ::doc::SubToC;
  using ToC         = ::doc::ToC;
  using Variable    = scad::doc::Variable;

  static constexpr const char * const ID = "markdown";

  static writer::Extension *builder(std::string_view writer_id,Dictionary &dict,const language::Extension *lang);
  Extension(Dictionary &dict,const language::Extension *lang);

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
