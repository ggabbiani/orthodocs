#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

int main(int argc, const char *argv[]) {
  std::ifstream f("licenses.json");
  json data = json::parse(f);
}