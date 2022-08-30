#include "globals.h"
#include "xref.h"

#include <algorithm>
#include <string>

using namespace std;

void Index::add(Document &document) {
  for(auto &item: document) 
    insert(item.second);
}

string Index::key(const doc::Item &item) {
  auto len    = option::prefix.length();
  string name = option::prefix.empty() || !nocase::compare(item.name.substr(0,len),option::prefix) ? item.name : item.name.substr(len);
  auto res = name+" ("+item.type()+')';
  return res;
}

std::string Index::title(const doc::Item &item) {
  return (is<const doc::Package>(item) ? dynamic_cast<const doc::Package&>(item).path.string() : item.name)+" ("+item.type()+')';
}

void Index::insert(Map::mapped_type &item) {
  _map.emplace(key(*item),std::move(item));
}
