#include "xref.h"

#include <algorithm>
#include <string>

using namespace std;

// FIXME: should be in 'document' namespace!
static string tstring(const doc::Item &item) {
  return string(
      is<const doc::Function>(item) ? "function" 
    : is<const doc::Module>(item)   ? "module" 
    : is<const doc::Package>(item)  ? "package" 
    : is<const doc::Variable>(item) ? "variable" 
    : "UNKNOWN"
  );
}

void Index::add(Document &document) {
  // ItemMap format:
  // Key    ==> "function|module|variable <item name>"
  // Value  ==> unique_ptr<doc::Item>
  for(auto &item: document) {
    // toc Key ==> "<item name> (function|module|variable)"
    auto key  = item.second->name+" ("+tstring(*item.second)+')';
    insert(key,item.second);
  }
}

void Index::serialize(const doc::Writer &writer) const {
  writer.out() << writer.H("Table of Contents",1) << endl;
  // ToC format:
  // Key    ==> "<item name> (function|module|variable)"
  // Value  ==> unique_ptr<doc::Item>
  for(auto &item: _map) {
    // see https://www.markdownguide.org/extended-syntax/#heading-ids
    auto id = tstring(*item.second)+'-'+item.second->name;
    // replace(id.begin(), id.end(), ' ', '-');
    auto link = item.second->uri.string();
    auto title = item.first;
    writer.out() << "- [" << title << "](" << link << "#" << id << ")" << endl;
  }

}
