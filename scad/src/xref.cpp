#include "xref.h"

#include <algorithm>
#include <string>

using namespace std;

void Index::add(doc::ItemMap &items) {
  for(auto &i: items)
    insert(i.first,i.second);
}

void Index::serialize(const doc::Writer &writer) const {
  writer.out() << writer.H("Table of Contents",1) << endl;
  for(auto &i: _map) {
    auto id = i.first;
    auto document = i.second->document.string();
    replace(id.begin(), id.end(), ' ', '-');
    writer.out() << "- [" << i.second->name << "](" << document << "#" << id << ")" << endl;
  }

}
