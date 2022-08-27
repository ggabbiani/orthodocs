#include "xref.h"

#include <algorithm>
#include <string>

using namespace std;

void Index::add(Document &document) {
  // ItemMap format:
  // Key    ==> "function|module|variable <item name>"
  // Value  ==> unique_ptr<doc::Item>
  for(auto &item: document) {
    // toc Key ==> "<item name> (function|module|variable)"
    auto key  = item.second->name+" ("+item.second->type()+')';
    insert(key,item.second);
  }
}
