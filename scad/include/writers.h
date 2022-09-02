#pragma once

#include "document.h"
#include "utils.h"

#include <filesystem>
#include <map>
#include <ostream>

namespace doc {

extern size_t size(const Document &items,const std::type_info &type);

/*!
 * abstract output formatter
 */
class Writer {
public:
  /**
   * write a document in the concrete instance native format
   */
  virtual void document(const std::filesystem::path &source, const Document &document) = 0;
  /**
   * write a table of contents in the concrete instance native format
   */
  virtual void toc(const ToC &toc) = 0;
  /**
   * write a dependecy graph in the concrete instance native format
   */
  virtual void graph(const ToC &toc) = 0;
};

namespace writer {

/**
 * Markdown writer 
 */
class Mdown : public Writer {
public:
  using SubToc = std::multimap<const std::string&,doc::Item*>;

  void document(const std::filesystem::path &source, const Document &document) override;
  void toc(const ToC &toc) override;
  void graph(const ToC &toc) override;

private:
  void giraffe(const doc::Package &pkg, std::ostream &out);
  void subToc(SubToc &sub, std::ostream &out, char &current) const;
  void package(std::ostream &out,const doc::Package &pkg);
  void parameter(std::ostream &out,const doc::Parameter &param);
  void function(std::ostream &out,const doc::Function &func);
  void module(std::ostream &out,const doc::Module &mod);
  void variable(std::ostream &out,const doc::Variable &var);

  std::string BOLD(const std::string &text) const;
  std::string BR() const;
  std::string CODE(const std::string &text) const;
  std::string H(const std::string &text,int level=1) const;
  std::string H(char c,int level=1) const;
  std::string HRULE() const;

};

}

}
