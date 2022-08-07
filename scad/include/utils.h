#pragma once

#include <string>

inline std::string trim(std::string str) {
  const char* ws = " \t\n\r\f\v";
  str.erase(str.find_last_not_of(ws) + 1);
  str.erase(0,str.find_first_not_of(ws));
  return str;
}

inline bool priv(const std::string &s) {
  return s.find("__")==0 ? true : false;
}

