/* Copyright (c) 2012-2017 The ANTLR Project. All rights reserved.
 * Use of this file is governed by the BSD 3-clause license that
 * can be found in the LICENSE.txt file in the project root.
 */

//
//  main.cpp
//  antlr4-cpp-demo
//
//  Created by Mike Lischke on 13.03.16.
//

#include <iostream>

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"

using namespace scad;
using namespace antlr4;

int main(int , const char **) {
  // ANTLRInputStream input(u8"🍴 = 🍐 + \"😎\";(((x * π))) * µ + ∰; a + (x * (y ? 0 : 1) + z);");
  ANTLRInputStream input("ident = 3;");
  SCADLexer lexer(&input);
  CommonTokenStream tokens(&lexer);

  tokens.fill();
  for (auto token : tokens.getTokens()) {
    std::cout << token->toString() << std::endl;
  }

  SCADParser parser(&tokens);
  tree::ParseTree* tree = parser.pkg();

  std::cout << tree->toStringTree(&parser) << std::endl << std::endl;

  return 0;
}
