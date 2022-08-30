#pragma once

#include <algorithm>
#include <cctype>
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

namespace nocase {

template <class InputIt1,class InputIt2>
InputIt1 find(InputIt1 first1, InputIt1 last1, InputIt2 first2, InputIt2 last2) {
  auto it = std::search(
    first1, last1, 
    first2, last2,
    [](char ch1, char ch2) {return std::toupper(ch1) == std::toupper(ch2);}
  );
  return it;
}

inline size_t find(const std::string &str, const std::string &sub, size_t pos=0) {
  auto it = find(str.begin()+pos,str.end(),sub.begin(),sub.end());
  return it!=str.end() ? it-str.begin() : std::string::npos;
}

inline size_t rfind(const std::string &str, const std::string &sub, size_t pos=0) {
  auto it = find(str.rbegin()+pos,str.rend(),sub.rbegin(),sub.rend());
  return it!=str.rend() ? it-str.rbegin() : std::string::npos;
}

inline bool compare(std::string str1,std::string str2) {
  return (str1.size() == str2.size()) && std::equal(str1.begin(), str1.end(), str2.begin(), [](char & c1, char & c2) {return (std::toupper(c1) == std::toupper(c2));})
  ;
}
}
