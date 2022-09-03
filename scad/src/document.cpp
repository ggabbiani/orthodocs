#include "document.h"
#include "globals.h"
#include "utils.h"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <string.h>
#include <sstream> 

using namespace std;

namespace fs = std::filesystem;

namespace doc {

string Item::documentKey() const {
  return type()+' '+name;
}

string Item::indexKey() const {
  auto len  = option::prefix.length();
  string s  = option::prefix.empty() || !nocase::compare(name.substr(0,len),option::prefix) ? name : name.substr(len);
  auto res  = s+" ("+type()+')';
  return res;
}

string Package::indexKey(const string &s) {
  return fs::path(s).stem().string()+" (package)";
}

string Package::indexKey() const {
  auto    len   = option::prefix.length();
  auto    stem  = path.stem().string();
  string  s     = option::prefix.empty() || !nocase::compare(stem.substr(0,len),option::prefix) ? stem : stem.substr(len);
  auto    res   = s+" ("+type()+')';
  return res;
}

Signature Item::signature() const {
  ostringstream ss;
  ss << name << "(";
  for(auto i=parameters.begin();i!=parameters.end();i++) {
    if (i!=parameters.begin())
      ss << ",";
    auto parameter = i->get();
    ss << parameter->name();
    if (!parameter->defaults.empty())
      ss << "=" << parameter->defaults;
  }
  ss << ")";
  return ss.str();
}

namespace style {

static char        buffer[256];

// calculate the number of lines
static size_t lines(const string &text) {
  stringstream ss(text,ios_base::in);
  auto len = 0;
  while (ss.getline(buffer,sizeof buffer))
    ++len;
  return len;
}

const char *Single::id() {
  return Single::ID;
}

bool Single::check(const string &text) {
  return (lines(text)==1 && text.length()>=3);
}

string Single::manage(const string &text) {
  assert(lines(text)==1);
  assert(text.length()>=3);
  return text.length()==3 ? string() : text.substr(text.find_first_not_of(' ',3));
}

const char *Simple::decoration[2] = {"/*!","*/"};

bool Simple::check(const string &text) {
  auto len = lines(text);
  if (len<3)
    return false;
  
  stringstream ss(text,ios_base::in);
  auto i        = 1;
  bool goon     = true;
  this->column  = -1;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    const char *m = i==1 ? decoration[start] : i==len ? decoration[end] : nullptr;
    if (m) {
      auto p = strstr(buffer, m);
      goon = (i==1 ? p==buffer : p==buffer+strlen(buffer)-strlen(m));
    } else {
      this->column  = this->column==-1 ? strspn(buffer," ") : min((int)strspn(buffer," "),this->column);
    }
    ++i;
  }
  
  return goon;
}

string Simple::manage(const string &text) {
  auto len = lines(text);
  assert(len>2);
  
  string result;
  stringstream ss(text,ios_base::in);
  auto i    = 1;
  bool goon = true;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    if (i>1 && i<len) 
      result.append(string(buffer).substr(this->column)+'\n');
    ++i;
  }
  return result;
}

const char *Simple::id() {
  return Simple::ID;
}

const char *Fine::decoration[3] = {"/*!"," */"," *"};

  /**
   * @brief 
   * 
   * @param text 
   * @return true 
   * @return false 
   */
bool Fine::check(const string &text) {
  auto len = lines(text);
  if (len<3)
    return false;
  
  stringstream ss(text,ios_base::in);
  auto row        = 1;
  bool goon     = true;
  this->column  = -1;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    const char *m = row==1 ? decoration[start] : row==len ? decoration[end] : decoration[body];
    auto pos = strstr(buffer, m);
    if (row==1) {         // parser strip any trailing space on first row
      goon  = pos==buffer;
    } else if (row==2) {       // we take the column number of the '*'
      goon  = pos!=nullptr;
      if (goon)
        this->column  = pos-buffer+1;
    } else if (row<len) // check if '*' column is equal to the one we got in row 2
      goon  = (pos-buffer+1==this->column);
    ++row;
  }
  
  return goon;
}

string Fine::manage(const string &text) {
  string result;
  auto len = lines(text);
  assert(len>2);
  
  stringstream ss(text,ios_base::in);
  auto i    = 1;
  bool goon = true;
  while (goon && ss.getline(buffer,sizeof buffer)) {
    if (i>1 && i<len) {
      auto offset = this->column+1;
      result.append(string(buffer+offset+(offset<strlen(buffer)))+'\n');
    }
    ++i;
  }

  return result;
}

const char *Fine::id() {
  return Fine::ID;
}

AbstractStyle *Factory::operator()(const string &text) {
  static Single single;
  static Fine   fine;
  static Simple simple;

  if (fine.check(text))
    return static_cast<AbstractStyle*>(&fine);
  else if (simple.check(text))
    return static_cast<AbstractStyle*>(&simple);
  else if (single.check(text))
    return static_cast<AbstractStyle*>(&single);
  else
    throw runtime_error("**Unrecognized comment type**");
}

}

namespace toc {

SubToC filter(const std::filesystem::path &path,const ToC &toc, std::function<bool(const std::filesystem::path&,const Item*)> func) {
  SubToC result;
  for(auto &element: toc) {
    auto item = element.second.get();
    if (func(path,item))
      result.emplace(element.first,item);
  }
  return result;
}

}

}