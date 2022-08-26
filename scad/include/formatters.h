#pragma once

#include "document.h"
#include "utils.h"

#include <filesystem>
#include <fstream>

namespace doc {

extern size_t size(const ItemMap &items,const std::type_info &type);

/*!
 * abstract output formatter
 */
class Formatter {
protected:
  Formatter(const std::filesystem::path &droot,const std::filesystem::path &source);

  virtual void variable(const doc::Variable &var)   = 0;
  virtual void function(const doc::Function &func)  = 0;
  virtual void module(const doc::Module &mod)       = 0;
  virtual void package(const doc::Package &pkg)     = 0;
  virtual void format(const doc::ItemMap &items)    = 0;
  virtual void parameter(const doc::Parameter &p)   = 0;

  virtual std::string BOLD(const std::string &text)           = 0;
  virtual std::string BR()                                    = 0;
  virtual std::string CODE(const std::string &text)           = 0;
  virtual std::string H(const std::string &text,int level=1)  = 0;
  virtual std::string HRULE()                                 = 0;

  //! source file path relative to source root (and so to doc root either)
  const std::filesystem::path source;
private:
  cwd doc_root;
};

namespace formatter {

class Mdown : public Formatter {
public:
  Mdown(const std::filesystem::path &droot,const std::filesystem::path &source);
  void format(const doc::ItemMap &items) override;

private:
  void package(const doc::Package &pkg)     override;
  void parameter(const doc::Parameter &p)   override;
  void function(const doc::Function &func)  override;
  void module(const doc::Module &mod)       override;
  void variable(const doc::Variable &var)   override;

  std::string BOLD(const std::string &text)           override;
  std::string BR()                                    override;
  std::string CODE(const std::string &text)           override;
  std::string H(const std::string &text,int level=1)  override;
  std::string HRULE()                                 override;

  //! document file path relative to doc root
  const std::filesystem::path  document;
  std::ofstream   out;
};

}

}
