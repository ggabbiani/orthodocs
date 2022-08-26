#pragma once

#include <filesystem>
#include <string>
#include <set>

inline std::string trim(std::string str) {
  const char* ws = " \t\n\r\f\v";
  str.erase(str.find_last_not_of(ws) + 1);
  str.erase(0,str.find_first_not_of(ws));
  return str;
}

inline bool priv(const std::string &s) {
  return s.find("__")==0 ? true : false;
}

using FileSet = std::set<std::filesystem::path>;

//! return a FileSet matching «extension»
extern void lookup(
  //! list of source directories/files
  const FileSet &sources,
  //! extension to filter out
  const char *extension, 
  //! list of source files matching «extension»
  FileSet *result
);

/*!
 * prints the explanatory string of an exception. If the exception is nested,
 * recurses to print the explanatory of the exception it holds
 */
extern void print_exception(const std::exception& e, int level =  0);

class cwd {
public:
  cwd(const std::filesystem::path &path) : _old(std::filesystem::current_path()) {
    try {
      std::filesystem::current_path(path);
    } catch(...) {
      std::throw_with_nested(std::runtime_error("error while setting current path: '"+path.string()+'\''));
    }
  }
  ~cwd() {
    std::filesystem::current_path(_old);
  }
private:
  std::filesystem::path _old;
};

template <class A,class B>
bool is(B &b) {
  return dynamic_cast<A*>(&b)!=nullptr;
}
