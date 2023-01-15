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
#include <codecvt>
#include <filesystem>
#include <string>
#include <utility>
#include <vector>

#if (__cplusplus > 202002L && __cplusplus < __cpp_lib_to_underlying) || (__cplusplus < 202102L)
template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
  return static_cast<typename std::underlying_type<E>::type>(e);
}
#endif  // (__cplusplus > 202002L && __cplusplus < __cpp_lib_to_underlying) || (__cplusplus < 202102L)

/**
 * @brief removes spaces from string head / tail 
 */
inline std::string trim(std::string str) {
  const char* ws = " \t\n\r\f\v";
  return str.erase(str.find_last_not_of(ws) + 1).erase(0,str.find_first_not_of(ws));
}

inline std::string utf32_to_utf8(std::u32string const& utf32) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cnv;
    std::string utf8 = cnv.to_bytes(utf32);
    if(cnv.converted() < utf32.size())
        throw std::runtime_error("incomplete conversion");
    return utf8;
}

inline std::u32string utf8_to_utf32(std::string const& utf8) {
    std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cnv;
    std::u32string utf32 = cnv.from_bytes(utf8);
    if(cnv.converted() < utf8.size())
        throw std::runtime_error("incomplete conversion");
    return utf32;
}

// TODO: move elsewhere
using FileSet = std::vector<std::filesystem::path>;

/*!
 * prints the explanatory string of an exception. If the exception is nested,
 * recurses to print the explanatory of the exception it holds
 */
extern void print_exception(const std::exception& e, int level =  0);

class cwd {
public:
  explicit cwd(const std::filesystem::path &path);
  ~cwd() {
    std::filesystem::current_path(_old);
  }
  explicit cwd(const cwd &) = delete;
  cwd operator = (const cwd&) = delete;
private:
  std::filesystem::path _old = std::filesystem::current_path();
};

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

inline size_t find(std::string_view str, std::string_view sub, size_t pos=0) {
  auto it = find(str.begin()+pos,str.end(),sub.begin(),sub.end());
  return it!=str.end() ? it-str.begin() : std::string::npos;
}

inline size_t rfind(std::string_view str, std::string_view sub, size_t pos=0) {
  auto it = find(str.rbegin()+pos,str.rend(),sub.rbegin(),sub.rend());
  return it!=str.rend() ? it-str.rbegin() : std::string::npos;
}

inline bool equal(std::string str1,std::string str2) {
  return (str1.size() == str2.size()) 
      && std::equal(
        str1.begin(), str1.end(), 
        str2.begin(), [](const char & c1,const char & c2) {
          return (std::toupper(c1) == std::toupper(c2));
        }
      );
}

/**
 * case insensitive lesser to be used with STL containers 
 */
struct Less {
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
  IncLabel(const char *base,int start = 0) : base(base),number(start) {}
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
