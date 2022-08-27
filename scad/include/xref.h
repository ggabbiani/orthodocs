#pragma once

#include "document.h"
#include "writers.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <string>

class Index {
  friend doc::writer::Mdown;
public:
  struct NoCase {
    bool operator() (const std::string& s1, const std::string& s2) const {
      std::string str1(s1.length(),' ');
      std::string str2(s2.length(),' ');
      std::transform(s1.begin(), s1.end(), str1.begin(), ::tolower);
      std::transform(s2.begin(), s2.end(), str2.begin(), ::tolower);
      return  str1 < str2;
    }
  };

  using KeyType     = doc::Name;
  using MappedType  = doc::ItemPtr;
  using Map         = std::map<KeyType,MappedType,NoCase>;
  
  Index() {}
  // move Document items into index, contextually enriching with source uri
  void add(Document &items);
private:
  void insert(const doc::Name &index,MappedType &item) {_map.emplace(index,std::move(item));}
  Map _map;
};
