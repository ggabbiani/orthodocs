#include "globals.h"

using namespace std;

namespace fs=std::filesystem;

namespace option {

fs::path droot;
fs::path sroot;

bool admonitions    = false;
bool toc            = false;
bool graph          = false;

string prefix;

string pkg_deps("text");
}
