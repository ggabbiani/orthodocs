/*
 * OpenSCAD source file processor
 *
 * Copyright © 2022 Giampiero Gabbiani (giampiero@gabbiani.org)
 *
 * This file is part of the 'AutoDox' (ADOX) project.
 *
 * ADOX is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ADOX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ADOX.  If not, see <http: //www.gnu.org/licenses/>.
 */

#include <processors.h>
#include <generator.h>
#include <formatters.h>

#include "antlr4-runtime.h"
#include "SCADLexer.h"
#include "SCADParser.h"
#include <tree/ParseTreeWalker.h>

#include <iostream>

using namespace std;
using namespace antlr4;

namespace scad {

void Processor::operator () (const fs::path &sroot, fs::path file, const fs::path &droot) {
  assert(file.is_relative());

  cout << file << endl;
  fs::current_path(sroot);
  ifstream          is(file);
  ANTLRInputStream  in(is);
  SCADLexer         lexer(&in);
  CommonTokenStream tokens(&lexer);
  SCADParser        parser(&tokens);

  // error management
  parser.removeErrorListeners();
  parser.addErrorListener(&_handler);

  // source parsing
  Generator  generator(file.c_str());
  // parse tree depth-first traverse
  tree::ParseTreeWalker  walker;
  tree::ParseTree       *tree = parser.pkg();
  walker.walk(&generator,tree);

  // documentation generation
  if (!file.has_filename())
    throw runtime_error("'" + file.string() + "' has no file part");
  auto directory = file.parent_path();

  fs::current_path(droot);
  if (file.has_parent_path() && !fs::exists(file.parent_path()))
    fs::create_directory(file.parent_path());
  ofstream os(file.replace_extension("md"));
  doc::formatter::Mdown markdown(os);
  markdown.format(generator.items);
}

} // namespace scad
