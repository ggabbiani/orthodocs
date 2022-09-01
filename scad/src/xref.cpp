#include "globals.h"
#include "xref.h"

#include <algorithm>
#include <string>

using namespace std;

void Index::add(Document &document) {
  for(auto &item: document) 
    insert(item.second);
}

std::string Index::title(const doc::Item &item) {
  auto package = dynamic_cast<const doc::Package*>(&item);
  return (package ? (package->path.parent_path() / package->path.stem()).string() : item.name)+" ("+item.type()+')';
}

void Index::insert(Map::mapped_type &item) {
  _map.emplace(item->indexKey(),std::move(item));
}
