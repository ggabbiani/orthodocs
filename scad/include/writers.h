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
class Writer {
public:
  virtual void operator () (const std::filesystem::path &droot, const std::filesystem::path &parent, const doc::ItemMap &document) = 0;
  virtual void operator () (const std::filesystem::path &droot, const Index &index) = 0;
  virtual std::ostream &out() const = 0;

  virtual void variable(const doc::Variable &var)   = 0;
  virtual void function(const doc::Function &func)  = 0;
  virtual void module(const doc::Module &mod)       = 0;
  virtual void package(const doc::Package &pkg)     = 0;
  virtual void parameter(const doc::Parameter &p)   = 0;

  virtual std::string BOLD(const std::string &text) const = 0;
  virtual std::string BR() const = 0;
  virtual std::string CODE(const std::string &text) const = 0;
  virtual std::string H(const std::string &text,int level=1) const  = 0;
  virtual std::string HRULE() const = 0;
};

namespace writer {

/**
 * write the document in Markdown format 
 */
class Mdown : public Writer {
public:
  void operator () (const std::filesystem::path &source, const std::filesystem::path &droot, const doc::ItemMap &document) override;
  void operator () (const std::filesystem::path &droot, const Index &index) override;
  std::ostream &out() const override {return *_out;}

private:
  void package(const doc::Package &pkg)     override;
  void parameter(const doc::Parameter &p)   override;
  void function(const doc::Function &func)  override;
  void module(const doc::Module &mod)       override;
  void variable(const doc::Variable &var)   override;

  std::string BOLD(const std::string &text) const override;
  std::string BR() const override;
  std::string CODE(const std::string &text) const override;
  std::string H(const std::string &text,int level=1) const override;
  std::string HRULE() const override;

  std::ofstream   *_out = nullptr;
};

}

}
