#pragma once

#include "document.h"
#include "utils.h"

#include <filesystem>
#include <ostream>

namespace doc {

extern size_t size(const Document &items,const std::type_info &type);

/*!
 * abstract output formatter
 */
class Writer {
public:
  virtual void operator () (const std::filesystem::path &source, const std::filesystem::path &droot, const Document &document) = 0;
  virtual void operator () (const std::filesystem::path &droot, const Index &index) = 0;
};

namespace writer {

/**
 * Markdown writer 
 */
class Mdown : public Writer {
public:
  void operator () (const std::filesystem::path &source, const std::filesystem::path &droot, const Document &document) override;
private:
  void operator () (const std::filesystem::path &droot, const Index &index) override;

private:
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
