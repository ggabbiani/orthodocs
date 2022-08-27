#pragma once

#include "document.h"
#include "writers.h"

#include <iostream>
#include <map>
#include <string>

class Index {
public:
  using KeyType     = doc::Name;
  using MappedType  = doc::ItemPtr;
  using Map         = std::map<KeyType,MappedType>;
  
  Index() {}
  // move items into index, contextually enriching item with source uri
  void add(doc::ItemMap &items);
  void serialize(const doc::Writer &writer) const;
  void deserialize(std::istream &is) {}
private:
  void insert(const doc::Name &index,MappedType &item) {_map.emplace(index,std::move(item));}
  Map _map;
};
