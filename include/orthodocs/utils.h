#pragma once
/*
 * general utilities.
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

// TODO: implement as an Item's function member
inline bool priv(const std::string &s) {
  return s.find("__")==0 ? true : false;
}

using FileSet = std::set<std::filesystem::path>;

/*!
 * prints the explanatory string of an exception. If the exception is nested,
 * recurses to print the explanatory of the exception it holds
 */
extern void print_exception(const std::exception& e, int level =  0);

class cwd {
public:
  cwd(const std::filesystem::path &path);
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

/**
 * check if «sub» is a subdirectory of «base».
 * 
 * **NOTE:** for the function to work, both «sub» and «base» must be in the 
 * same format i.e both relative or both canonical.
 */
extern bool is_sub_of(const std::filesystem::path &sub, const std::filesystem::path &base);

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

struct Compare {
  using is_transparent = void; // enables heterogeneous lookup
  bool operator() (std::string_view lhs, std::string_view rhs) const {
    std::string str1(lhs.length(),' ');
    std::string str2(rhs.length(),' ');
    std::transform(lhs.begin(), lhs.end(), str1.begin(), ::tolower);
    std::transform(rhs.begin(), rhs.end(), str2.begin(), ::tolower);
    return  str1 < str2;
  }
};

}

// Factory for string labels with postfix operator
class IncLabel {
public:
  IncLabel(const char *base,int start = 0) : number(start),base(base) {}
  IncLabel operator++() {++number;return *this;}
  IncLabel operator++(int) {number++;return *this;}
  std::string string() const {return base+std::to_string(number);}
private:
  std::string base;
  int number = 0;
};

inline std::ostream & operator << (std::ostream& os,const IncLabel& label) {
  return os << label.string();
}
