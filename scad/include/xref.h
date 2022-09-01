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
  // multimap with case insensitive sort 
  using Map = std::multimap<doc::Name,doc::ItemPtr,NoCase>;
  
  // move Document items into index, contextually enriching with source uri
  void add(Document &document);
  static std::string title(const doc::Item &item);
  const Map &toc() const {return _map;}
private:
  void insert(Map::mapped_type &item);
  Map _map;
};
