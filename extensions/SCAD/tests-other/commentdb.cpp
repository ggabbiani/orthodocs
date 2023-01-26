#include <scad/extension.h>
#include <SCADLexer.h>

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace std;
namespace fs = std::filesystem;

int main(int argc, const char *argv[]) {
  if (argc<2) {
    cerr << "Misssing SCAD source" << endl;
    return EXIT_FAILURE;
  }
  ifstream                  is(argv[1]);
  antlr4::ANTLRInputStream  in(is);
  scad::SCADLexer           lexer(&in);
  antlr4::CommonTokenStream tokens(&lexer);
  // we have to fill the stream 'manually' since we do not schedule any consuming parsing
  tokens.fill();

  auto world = tokens.get(0,tokens.size());
  for(const auto *token: world) {
    cout << token->getChannel() << "[" << token->getTokenIndex() << "]:\n" << token->getText() << endl;
  }

  return EXIT_SUCCESS;
}