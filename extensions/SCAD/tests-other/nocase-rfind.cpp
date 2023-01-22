/*
 * test case for a case insensitive find/rfind algo
 *
 * This file is part of the 'OrthoDocs' (ODOX) project.
 *
 * Copyright © 2022, Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * SPDX-License-Identifier: GPL-3.0-or-later */

#include <algorithm>
#include <iostream>
#include <string>

using namespace std;

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

size_t find(const string &str, const string &sub, size_t pos=0) {
  auto it = find(str.begin()+pos,str.end(),sub.begin(),sub.end());
  return it!=str.end() ? it-str.begin() : string::npos;
}

size_t rfind(const string &str, const string &sub, size_t pos=0) {
  auto it = find(str.rbegin()+pos,str.rend(),sub.rbegin(),sub.rend());
  return it!=str.rend() ? it-str.rbegin() : string::npos;
}

}

int main(int argc, const char *argv[]) {
  string s =
    "blah blab albh sag òkg òqwg sa a **NOTE:** jsd lkajhvjh\n"
    "qhal  **note:** ksjb vlhjveqih vqhjelhhal";
  string sub = "**NotE:**";

  auto r1 = nocase::find(s,sub,49);
  cout << r1 << '(' << s.length() << ')' << ':' << s.substr(r1,sub.length()) << endl;

  auto r2 = nocase::find(s.rbegin(),s.rend(),sub.rbegin(),sub.rend());
  cout << distance(s.rbegin(),r2) << '(' << s.length() << ')' << ':' << s.substr(s.length()-distance(s.rbegin(),r2)-sub.length(),sub.length()) << endl;

  auto r3 = nocase::rfind(s,sub);
  cout << r3 << '(' << s.length() << ')' << ':' << s.substr(s.length()-r3-sub.length(),sub.length()) << endl;

  string change = ":bulb: ";
  auto pos = 0;
  while ((pos = nocase::rfind(s,sub,pos)) != std::string::npos) {
      std::cout << pos << ':' << s.substr(s.length()-pos-sub.length(),sub.length()) << std::endl;
      s.insert(s.length()-pos-sub.length(),change);
      pos += change.length();
  }
  cout << s << endl;
  return 0;
}
